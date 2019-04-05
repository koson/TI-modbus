/**
 * @file main.c
 * @brief Main routine
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.6
 **/

//Dependencies
#include <stdlib.h>
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo_144.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/stm32h7xx_eth_driver.h"
#include "drivers/phy/lan8742_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "coap/coap_client.h"
#include "rng/yarrow.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-07-43"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::743"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::743"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//CoAP server name
#define APP_SERVER_NAME "coap.me"

//CoAP server port
#define APP_SERVER_PORT 5683   //CoAP over UDP
//#define APP_SERVER_PORT 5684 //CoAP over DTLS

//Client's PSK identity
#define APP_CLIENT_PSK_ID ""

//Client's PSK
#define APP_CLIENT_PSK "12345678"

//Global variables
RNG_HandleTypeDef RNG_Handle;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
CoapClientContext coapClientContext;
YarrowContext yarrowContext;
uint8_t seed[32];


/**
 * @brief System clock configuration
 **/

void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct;
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_PeriphCLKInitTypeDef RCC_PeriphClkInitStruct;

   //Supply configuration update enable
   MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

   //Configure voltage scaling
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
   while((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);

   //Enable HSE oscillator and activate PLL with HSE as source
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 4;
   RCC_OscInitStruct.PLL.PLLN = 400;
   RCC_OscInitStruct.PLL.PLLP = 2;
   RCC_OscInitStruct.PLL.PLLR = 2;
   RCC_OscInitStruct.PLL.PLLQ = 4;
   RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
   RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;

   HAL_RCC_OscConfig(&RCC_OscInitStruct);

   //Select PLL as system clock source and configure bus clocks dividers
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1;

   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
   RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
   RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

   HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

   //Select clock source for RNG peripheral
   RCC_PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG;
   RCC_PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;

   HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInitStruct);

   //Enable CSI clock
   __HAL_RCC_CSI_ENABLE();
   //Enable SYSCFG clock
   __HAL_RCC_SYSCFG_CLK_ENABLE() ;

   //Enable the I/O compensation cell
   HAL_EnableCompensationCell();
}


/**
 * @brief MPU configuration
 **/

void MPU_Config(void)
{
   MPU_Region_InitTypeDef MPU_InitStruct;

   //Disable MPU
   HAL_MPU_Disable();

   //DTCM RAM
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER0;
   MPU_InitStruct.BaseAddress = 0x20000000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
   MPU_InitStruct.SubRegionDisable = 0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //AXI SRAM
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER1;
   MPU_InitStruct.BaseAddress = 0x24000000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
   MPU_InitStruct.SubRegionDisable = 0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //AHB SRAM
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER2;
   MPU_InitStruct.BaseAddress = 0x30000000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
   MPU_InitStruct.SubRegionDisable = 0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //AHB SRAM (no cache)
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER3;
   MPU_InitStruct.BaseAddress = 0x30040000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
   MPU_InitStruct.SubRegionDisable = 0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //Enable MPU
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


/**
 * @brief DTLS initialization callback
 * @param[in] context Pointer to the CoAP client context
 * @param[in] dtlsContext Pointer to the DTLS context
 * @return Error code
 **/

error_t coapTestDtlsInitCallback(CoapClientContext *context,
   TlsContext *dtlsContext)
{
   error_t error;

   //Debug message
   TRACE_INFO("DTLS initialization callback\r\n");

   //Set the PRNG algorithm to be used
   error = tlsSetPrng(dtlsContext, YARROW_PRNG_ALGO, &yarrowContext);
   //Any error to report?
   if(error)
      return error;

   //Set the PSK identity to be used by the client
   error = tlsSetPskIdentity(dtlsContext, APP_CLIENT_PSK_ID);
   //Any error to report?
   if(error)
      return error;

   //Set the pre-shared key to be used
   error = tlsSetPsk(dtlsContext, (uint8_t *) APP_CLIENT_PSK,
      strlen(APP_CLIENT_PSK));
   //Any error to report?
   if(error)
      return error;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief CoAP observe callback
 * @param[in] context Pointer to the CoAP client context
 * @param[in] request CoAP request handle
 * @param[in] status CoAP request status
 * @param[in] param Callback function parameter
 * @return Error code
 **/

error_t coapTestObserveCallback(CoapClientContext *context,
   CoapClientRequest *request, CoapRequestStatus status, void *param)
{
   error_t error;
   CoapMessage *message;
   CoapCode responseCode;
   size_t payloadLen;
   const uint8_t *payload;

   //Check request status
   if(status == COAP_REQUEST_STATUS_SUCCESS)
   {
      //Debug message
      TRACE_INFO("CoAP notification received...\r\n");

      //Point to the response message
      message = coapClientGetResponseMessage(request);

      //Retrieve response code
      error = coapClientGetResponseCode(message, &responseCode);

      //Check status code
      if(!error)
      {
         //Debug message
         TRACE_INFO("  Response Code: %" PRIu8 ".%02" PRIu8 "\r\n",
            responseCode / 32, responseCode & 31);
      }

      //Get payload data
      error = coapClientGetPayload(message, &payload, &payloadLen);

      //Check status code
      if(!error)
      {
         //Debug message
         TRACE_INFO("  Payload (%u bytes):\r\n%s\r\n", payloadLen, payload);
      }
   }
   else
   {
      //Debug message
      TRACE_INFO("CoAP observation canceled!\r\n");
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Observable resource registration
 * @param[in] context Pointer to the CoAP client context
 * @return Error code
 **/

error_t coapTestObserve(CoapClientContext *context)
{
   error_t error;
   CoapClientRequest *request;
   CoapMessage *message;

   //Create a new CoAP request
   request = coapClientCreateRequest(context);

   //Valid request handle?
   if(request != NULL)
   {
      //Set request timeout
      coapClientSetRequestTimeout(request, 10000);

      //Point to the request message
      message = coapClientGetRequestMessage(request);

      //Format request message
      coapClientSetType(message, COAP_TYPE_CON);
      coapClientSetMethodCode(message, COAP_CODE_GET);
      coapClientSetUriPath(message, "/obs");

      //A client registers its interest in a resource by issuing a GET
      //request with an Observe option set to 0
      coapClientSetUintOption(message, COAP_OPT_OBSERVE, 0,
         COAP_OBSERVE_REGISTER);

      //Send CoAP request
      error = coapClientSendRequest(request, coapTestObserveCallback, NULL);

      //Any error to report?
      if(error)
      {
         //Clean up side effects
         coapClientDeleteRequest(request);
      }
   }
   else
   {
      //Debug message
      TRACE_ERROR("Failed to create CoAP request!\r\n");
      //Report an error
      error = ERROR_FAILURE;
   }

   //Return status code
   return error;
}


/**
 * @brief CoAP request callback (asynchronous mode)
 * @param[in] context Pointer to the CoAP client context
 * @param[in] request CoAP request handle
 * @param[in] status CoAP request status
 * @param[in] param Callback function parameter
 * @return Error code
 **/

error_t coapTestRequestAsyncCallback(CoapClientContext *context,
   CoapClientRequest *request, CoapRequestStatus status, void *param)
{
   error_t error;
   CoapMessage *message;
   CoapCode responseCode;
   size_t payloadLen;
   const uint8_t *payload;

   //Check request status
   if(status == COAP_REQUEST_STATUS_SUCCESS)
   {
      //Debug message
      TRACE_INFO("CoAP response received...\r\n");

      //Point to the response message
      message = coapClientGetResponseMessage(request);

      //Retrieve response code
      error = coapClientGetResponseCode(message, &responseCode);

      //Check status code
      if(!error)
      {
         //Debug message
         TRACE_INFO("  Response Code: %" PRIu8 ".%02" PRIu8 "\r\n",
            responseCode / 32, responseCode & 31);
      }

      //Get payload data
      error = coapClientGetPayload(message, &payload, &payloadLen);

      //Check status code
      if(!error)
      {
         //Debug message
         TRACE_INFO("  Payload (%u bytes):\r\n%s\r\n", payloadLen, payload);
      }
   }
   else
   {
      //Debug message
      TRACE_INFO("CoAP request failed!\r\n");
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Send CoAP request (asynchronous mode)
 * @param[in] context Pointer to the CoAP client context
 * @return Error code
 **/

error_t coapTestRequestAsync(CoapClientContext *context)
{
   error_t error;
   CoapClientRequest *request;
   CoapMessage *message;

   //Create a new CoAP request
   request = coapClientCreateRequest(context);

   //Valid request handle?
   if(request != NULL)
   {
      //Set request timeout
      coapClientSetRequestTimeout(request, 10000);

      //Point to the request message
      message = coapClientGetRequestMessage(request);

      //Format request message
      coapClientSetType(message, COAP_TYPE_CON);
      coapClientSetMethodCode(message, COAP_CODE_GET);
      coapClientSetUriPath(message, "/separate");

      //Send CoAP request without blocking. The specified callback function
      //will be called upon request completion
      error = coapClientSendRequest(request, coapTestRequestAsyncCallback,
         NULL);

      //Any error to report?
      if(error)
      {
         //Clean up side effects
         coapClientDeleteRequest(request);
      }
   }
   else
   {
      //Debug message
      TRACE_ERROR("Failed to create CoAP request!\r\n");
      //Report an error
      error = ERROR_FAILURE;
   }

   //Return status code
   return error;
}


/**
 * @brief Send CoAP request (synchronous mode)
 * @param[in] context Pointer to the CoAP client context
 * @return Error code
 **/

error_t coapTestRequestSync(CoapClientContext *context)
{
   error_t error;
   CoapClientRequest *request;
   CoapMessage *message;
   CoapCode responseCode;
   size_t payloadLen;
   const uint8_t *payload;

   //Create a new CoAP request
   request = coapClientCreateRequest(context);

   //Valid request handle?
   if(request != NULL)
   {
      //Set request timeout
      coapClientSetRequestTimeout(request, 10000);

      //Point to the request message
      message = coapClientGetRequestMessage(request);

      //Format request message
      coapClientSetType(message, COAP_TYPE_CON);
      coapClientSetMethodCode(message, COAP_CODE_POST);
      coapClientSetUriPath(message, "/test");

      //Set Content-Format option
      coapClientSetUintOption(message, COAP_OPT_CONTENT_FORMAT, 0,
         COAP_CONTENT_FORMAT_TEXT_PLAIN);

      //Set request payload
      coapClientSetPayload(message, "Hello World!", 12);

      //Send CoAP request and wait for completion
      error = coapClientSendRequest(request, NULL, NULL);

      //Any response received?
      if(!error)
      {
         //Point to the response message
         message = coapClientGetResponseMessage(request);

         //Retrieve response code
         error = coapClientGetResponseCode(message, &responseCode);

         //Check status code
         if(!error)
         {
            //Debug message
            TRACE_INFO("  Response Code: %" PRIu8 ".%02" PRIu8 "\r\n",
               responseCode / 32, responseCode & 31);
         }

         //Get response payload
         error = coapClientGetPayload(message, &payload, &payloadLen);

         //Check status code
         if(!error)
         {
            //Debug message
            TRACE_INFO("  Payload (%u bytes):\r\n%s\r\n", payloadLen, payload);
         }
      }
      else
      {
         //Debug message
         TRACE_WARNING("No CoAP response received!\r\n");
      }

      //Release CoAP request
      coapClientDeleteRequest(request);
   }
   else
   {
      //Debug message
      TRACE_ERROR("Failed to create CoAP request!\r\n");
      //Report an error
      error = ERROR_FAILURE;
   }

   //Return status code
   return error;
}


/**
 * @brief Establish connection with a remote CoAP server
 * @param[in] context Pointer to the CoAP client context
 * @return Error code
 **/

error_t coapTestConnect(CoapClientContext *context)
{
   error_t error;
   IpAddr ipAddr;

   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");

   //Resolve CoAP server name
   error = getHostByName(NULL, APP_SERVER_NAME, &ipAddr, 0);
   //Any error to report?
   if(error)
      return error;

#if (APP_SERVER_PORT == 5683)
   //CoAP over UDP
   coapClientSetTransportProtocol(context, COAP_TRANSPORT_PROTOCOL_UDP);

#elif (APP_SERVER_PORT == 5684)
   //CoAP over DTLS
   coapClientSetTransportProtocol(context, COAP_TRANSPORT_PROTOCOL_DTLS);
   //Register DTLS initialization callback
   coapClientRegisterDtlsInitCallback(context, coapTestDtlsInitCallback);
#endif

   //Set default timeout
   coapClientSetTimeout(context, 20000);

   //Debug message
   TRACE_INFO("Connecting to CoAP server %s...\r\n",
      ipAddrToString(&ipAddr, NULL));

   //Establish connection with the CoAP server
   error = coapClientConnect(context, &ipAddr, APP_SERVER_PORT);
   //Return status code
   return error;
}


/**
 * @brief CoAP test task
 * @param[in] param Unused parameter
 **/

void coapTestTask(void *param)
{
   error_t error;
   bool_t connectionState;
   uint_t buttonState;
   uint_t prevButtonState;
   Ipv4Addr ipv4Addr;

   //Initialize variables
   connectionState = FALSE;
   prevButtonState = 0;

   //Initialize CoAP client context
   coapClientInit(&coapClientContext);

   //Endless loop
   while(1)
   {
      //Check connection state
      if(!connectionState)
      {
         //Make sure the link is up
         if(netGetLinkState(&netInterface[0]))
         {
            //Retrieve host address
            ipv4GetHostAddr(&netInterface[0], &ipv4Addr);

            //Valid IP address assigned to the interface?
            if(ipv4Addr != IPV4_UNSPECIFIED_ADDR)
            {
               //Connect to the CoAP server
               error = coapTestConnect(&coapClientContext);

               //Successful connection?
               if(!error)
               {
                  //The CoAP client is connected to the server
                  connectionState = TRUE;
                  //Observable resource registration
                  coapTestObserve(&coapClientContext);
               }
               else
               {
                  //Delay between subsequent connection attempts
                  osDelayTask(2000);
               }
            }
         }
      }
      else
      {
         //Initialize status code
         error = NO_ERROR;

         //Get user button state
         buttonState = BSP_PB_GetState(BUTTON_KEY);

         //User button pressed?
         if(buttonState && !prevButtonState)
         {
#if 1
            //Send CoAP request (synchronous mode)
            coapTestRequestSync(&coapClientContext);
#else
            //Send CoAP request (asynchronous mode)
            coapTestRequestAsync(&coapClientContext);
#endif
         }

         //Save current state
         prevButtonState = buttonState;

         //Process events
         error = coapClientTask(&coapClientContext, 100);

         //Connection to CoAP server lost?
         if(error)
         {
            //Close connection
            coapClientDisconnect(&coapClientContext);
            //Update connection state
            connectionState = FALSE;
            //Recovery delay
            osDelayTask(2000);
         }
      }
   }
}


/**
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   //Endless loop
   while(1)
   {
      BSP_LED_On(LED1);
      osDelayTask(100);
      BSP_LED_Off(LED1);
      osDelayTask(900);
   }
}


/**
 * @brief Main entry point
 * @return Unused value
 **/

int_t main(void)
{
   error_t error;
   uint_t i;
   uint32_t value;
   NetInterface *interface;
   OsTask *task;
   MacAddr macAddr;
#if (APP_USE_DHCP == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

   //MPU configuration
   MPU_Config();
   //HAL library initialization
   HAL_Init();
   //Configure the system clock
   SystemClock_Config();

   //Enable I-cache and D-cache
   SCB_EnableICache();
   SCB_EnableDCache();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("*** CycloneTCP CoAP Client Demo ***\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: STM32H743\r\n");
   TRACE_INFO("\r\n");

   //LED configuration
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED2);
   BSP_LED_Init(LED3);

   //Clear LEDs
   BSP_LED_Off(LED1);
   BSP_LED_Off(LED2);
   BSP_LED_Off(LED3);

   //Initialize user button
   BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

   //Enable RNG peripheral clock
   __HAL_RCC_RNG_CLK_ENABLE();
   //Initialize RNG
   RNG_Handle.Instance = RNG;
   HAL_RNG_Init(&RNG_Handle);

   //Generate a random seed
   for(i = 0; i < 32; i += 4)
   {
      //Get 32-bit random value
      HAL_RNG_GenerateRandomNumber(&RNG_Handle, &value);

      //Copy random value
      seed[i] = value & 0xFF;
      seed[i + 1] = (value >> 8) & 0xFF;
      seed[i + 2] = (value >> 16) & 0xFF;
      seed[i + 3] = (value >> 24) & 0xFF;
   }

   //PRNG initialization
   error = yarrowInit(&yarrowContext);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize PRNG!\r\n");
   }

   //Properly seed the PRNG
   error = yarrowSeed(&yarrowContext, seed, sizeof(seed));
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to seed PRNG!\r\n");
   }

   //TCP/IP stack initialization
   error = netInit();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
   }

   //Configure the first Ethernet interface
   interface = &netInterface[0];

   //Set interface name
   netSetInterfaceName(interface, "eth0");
   //Set host name
   netSetHostname(interface, "CoAPClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &stm32h7xxEthDriver);
   netSetPhyDriver(interface, &lan8742PhyDriver);
   //Set host MAC address
   macStringToAddr(APP_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
   }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_USE_DHCP == ENABLED)
   //Get default settings
   dhcpClientGetDefaultSettings(&dhcpClientSettings);
   //Set the network interface to be configured by DHCP
   dhcpClientSettings.interface = interface;
   //Disable rapid commit option
   dhcpClientSettings.rapidCommit = FALSE;

   //DHCP client initialization
   error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP client!\r\n");
   }

   //Start DHCP client
   error = dhcpClientStart(&dhcpClientContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP client!\r\n");
   }
#else
   //Set IPv4 host address
   ipv4StringToAddr(APP_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_USE_SLAAC == ENABLED)
   //Get default settings
   slaacGetDefaultSettings(&slaacSettings);
   //Set the network interface to be configured
   slaacSettings.interface = interface;

   //SLAAC initialization
   error = slaacInit(&slaacContext, &slaacSettings);
   //Failed to initialize SLAAC?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize SLAAC!\r\n");
   }

   //Start IPv6 address autoconfiguration process
   error = slaacStart(&slaacContext);
   //Failed to start SLAAC process?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start SLAAC!\r\n");
   }
#else
   //Set link-local address
   ipv6StringToAddr(APP_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);

   //Set IPv6 prefix
   ipv6StringToAddr(APP_IPV6_PREFIX, &ipv6Addr);
   ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IPV6_PREFIX_LENGTH);

   //Set global address
   ipv6StringToAddr(APP_IPV6_GLOBAL_ADDR, &ipv6Addr);
   ipv6SetGlobalAddr(interface, 0, &ipv6Addr);

   //Set default router
   ipv6StringToAddr(APP_IPV6_ROUTER, &ipv6Addr);
   ipv6SetDefaultRouter(interface, 0, &ipv6Addr);

   //Set primary and secondary DNS servers
   ipv6StringToAddr(APP_IPV6_PRIMARY_DNS, &ipv6Addr);
   ipv6SetDnsServer(interface, 0, &ipv6Addr);
   ipv6StringToAddr(APP_IPV6_SECONDARY_DNS, &ipv6Addr);
   ipv6SetDnsServer(interface, 1, &ipv6Addr);
#endif
#endif

   //Create CoAP test task
   task = osCreateTask("CoAP", coapTestTask, NULL, 750, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Create a task to blink the LED
   task = osCreateTask("Blink", blinkTask, NULL, 500, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Start the execution of tasks
   osStartKernel();

   //This function should never return
   return 0;
}
