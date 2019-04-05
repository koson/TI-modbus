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
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8742_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "mqtt_sn/mqtt_sn_client.h"
#include "rng/yarrow.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-04-29"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::429"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::429"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//MQTT-SN gateway name
//#define APP_GATEWAY_NAME "192.168.0.100"

//MQTT-SN gateway port
#define APP_GATEWAY_PORT 1883   //MQTT-SN over TCP
//#define APP_GATEWAY_PORT 8883 //MQTT-SN over DTLS

//MQTT-SN client identifier
#define APP_CLIENT_ID "client12345678"

//Predefined topics
MqttSnPredefinedTopic predefinedTopics[] =
{
   {"topic1", 1},
   {"topic2", 2},
   {"topic3", 3}
};

//Global variables
RNG_HandleTypeDef RNG_Handle;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
MqttSnClientContext mqttSnClientContext;
YarrowContext yarrowContext;
uint8_t seed[32];


/**
 * @brief System clock configuration
 **/

void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct;
   RCC_ClkInitTypeDef RCC_ClkInitStruct;

   //Enable Power Control clock
   __HAL_RCC_PWR_CLK_ENABLE();

   //The voltage scaling allows optimizing the power consumption when the device is
   //clocked below the maximum system frequency, to update the voltage scaling value
   //regarding system frequency refer to product datasheet.
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

   //Enable HSE Oscillator and activate PLL with HSE as source
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 8;
   RCC_OscInitStruct.PLL.PLLN = 360;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 7;
   HAL_RCC_OscConfig(&RCC_OscInitStruct);

   //Enable overdrive mode
   HAL_PWREx_EnableOverDrive();

   //Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
   //clocks dividers
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
   HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


/**
 * @brief DTLS initialization callback
 * @param[in] context Pointer to the MQTT-SN client context
 * @param[in] dtlsContext Pointer to the DTLS context
 * @return Error code
 **/

error_t mqttSnTestDtlsInitCallback(MqttSnClientContext *context,
   TlsContext *dtlsContext)
{
   error_t error;

   //Debug message
   TRACE_INFO("MQTT-SN: TLS initialization callback\r\n");

   //Set the PRNG algorithm to be used
   error = tlsSetPrng(dtlsContext, YARROW_PRNG_ALGO, &yarrowContext);
   //Any error to report?
   if(error)
      return error;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Publish callback function
 * @param[in] context Pointer to the MQTT-SN client context
 * @param[in] topic Topic name
 * @param[in] message Message payload
 * @param[in] length Length of the message payload
 * @param[in] qos QoS level used to publish the message
 * @param[in] retain This flag specifies if the message is to be retained
 **/

void mqttSnTestPublishCallback(MqttSnClientContext *context,
   const char_t *topic, const uint8_t *message, size_t length,
   MqttSnQosLevel qos, bool_t retain)
{
   //Debug message
   TRACE_INFO("PUBLISH packet received...\r\n");
   TRACE_INFO("  QoS: %u\r\n", qos);
   TRACE_INFO("  Retain: %u\r\n", retain);
   TRACE_INFO("  Topic: %s\r\n", topic);
   TRACE_INFO("  Message (%" PRIuSIZE " bytes):\r\n", length);
   TRACE_INFO_ARRAY("    ", message, length);

   //Check topic name
   if(!strcmp(topic, "board/leds/1"))
   {
      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
         BSP_LED_Toggle(LED2);
      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
         BSP_LED_On(LED2);
      else
         BSP_LED_Off(LED2);
   }
   else if(!strcmp(topic, "board/leds/2"))
   {
      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
         BSP_LED_Toggle(LED3);
      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
         BSP_LED_On(LED3);
      else
         BSP_LED_Off(LED3);
   }
}


/**
 * @brief Establish MQTT-SN connection
 * @param[in] context Pointer to the MQTT-SN client context
 * @return Error code
 **/

error_t mqttSnTestConnect(MqttSnClientContext *context)
{
   error_t error;
   IpAddr ipAddr;

#if (APP_GATEWAY_PORT == 1883)
   //MQTT-SN over UDP
   mqttSnClientSetTransportProtocol(context, MQTT_SN_TRANSPORT_PROTOCOL_UDP);

#elif (APP_GATEWAY_PORT == 8883)
   //MQTT-SN over DTLS
   mqttSnClientSetTransportProtocol(context, MQTT_SN_TRANSPORT_PROTOCOL_DTLS);
   //Register DTLS initialization callback
   mqttSnClientRegisterDtlsInitCallback(context, mqttSnTestDtlsInitCallback);
#endif

   //Register publish callback function
   mqttSnClientRegisterPublishCallback(context, mqttSnTestPublishCallback);

   //Set the list of predefined topics
   mqttSnClientSetPredefinedTopics(context, predefinedTopics,
      arraysize(predefinedTopics));

   //Set communication timeout (20s)
   mqttSnClientSetTimeout(context, 20000);
   //Set keep-alive value (60s)
   mqttSnClientSetKeepAlive(context, 60000);
   //Set client identifier
   mqttSnClientSetIdentifier(context, APP_CLIENT_ID);

   //Set Will message
   mqttSnClientSetWillMessage(context, "board/status",
      "offline", 7, MQTT_SN_QOS_LEVEL_0, FALSE);

   //Start of exception handling block
   do
   {
#ifdef APP_GATEWAY_NAME
      //Resolve MQTT-SN gateway name
      error = getHostByName(NULL, APP_GATEWAY_NAME, &ipAddr, 0);
      //Any error to report?
      if(error)
         break;

      //Specify the address of the gateway
      error = mqttSnClientSetGateway(context, &ipAddr, APP_GATEWAY_PORT);
      //Any error to report?
      if(error)
         break;
#else
      //Set the broadcast address to be used (gateway discovery procedure)
      ipStringToAddr("255.255.255.255", &ipAddr);

      //Debug message
      TRACE_INFO("\r\n\r\nSearching MQTT-SN gateway...\r\n");

      //Search for a gateway
      error = mqttSnClientSearchGateway(context, &ipAddr, APP_GATEWAY_PORT);
      //Any error to report?
      if(error)
         break;
#endif

      //Debug message
      TRACE_INFO("Connecting to MQTT-SN gateway...\r\n");

      //Establish connection with the MQTT-SN gateway
      error = mqttSnClientConnect(context, TRUE);
      //Any error to report?
      if(error)
         break;

      //Subscribe to the desired topics
      error = mqttSnClientSubscribe(context, "board/leds/+",
         MQTT_SN_QOS_LEVEL_1);
      //Any error to report?
      if(error)
         break;

      //Send PUBLISH packet
      error = mqttSnClientPublish(context, "board/status",
         "online", 6, MQTT_SN_QOS_LEVEL_1, TRUE, FALSE, NULL);
      //Any error to report?
      if(error)
         break;

      //End of exception handling block
   } while(0);

   //Check status code
   if(error)
   {
      //Close connection
      mqttSnClientDisconnect(context);
   }

   //Return status code
   return error;
}


/**
 * @brief MQTT-SN test task
 **/

void mqttSnTestTask(void *param)
{
   error_t error;
   bool_t connectionState;
   uint_t buttonState;
   uint_t prevButtonState;
   char_t buffer[16];
   Ipv4Addr ipv4Addr;

   //Initialize variables
   connectionState = FALSE;
   prevButtonState = 0;

   //Initialize MQTT-SN client context
   mqttSnClientInit(&mqttSnClientContext);

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
               //Connect to the MQTT-SN gateway
               error = mqttSnTestConnect(&mqttSnClientContext);

               //Successful connection?
               if(!error)
               {
                  //The MQTT-SN client is connected to the gateway
                  connectionState = TRUE;
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

         //Any change detected?
         if(buttonState != prevButtonState)
         {
            if(buttonState)
               strcpy(buffer, "pressed");
            else
               strcpy(buffer, "released");

            //Send PUBLISH packet
            error = mqttSnClientPublish(&mqttSnClientContext, "board/buttons/1",
               buffer, strlen(buffer), MQTT_SN_QOS_LEVEL_1, FALSE, FALSE, NULL);

            //Save current state
            prevButtonState = buttonState;
         }

         //Check status code
         if(!error)
         {
            //Process events
            error = mqttSnClientTask(&mqttSnClientContext, 100);
         }

         //Connection to MQTT-SN gateway lost?
         if(error)
         {
            //Close connection
            mqttSnClientDisconnect(&mqttSnClientContext);
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

   //HAL library initialization
   HAL_Init();
   //Configure the system clock
   SystemClock_Config();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("**************************************\r\n");
   TRACE_INFO("*** CycloneTCP MQTT-SN Client Demo ***\r\n");
   TRACE_INFO("**************************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: STM32F429\r\n");
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
   netSetHostname(interface, "MQTTSNClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &stm32f4x9EthDriver);
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

   //Create MQTT-SN test task
   task = osCreateTask("MQTT-SN", mqttSnTestTask, NULL, 750, OS_TASK_PRIORITY_NORMAL);
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
