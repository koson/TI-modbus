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
#include "stm32h743i_eval.h"
#include "stm32h743i_eval_io.h"
#include "stm32h743i_eval_lcd.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/stm32h7xx_eth_driver.h"
#include "drivers/phy/lan8742_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "ftp/ftp_client.h"
#include "error.h"
#include "debug.h"

//LCD frame buffers
#define LCD_FRAME_BUFFER_LAYER0 0xD0400000
#define LCD_FRAME_BUFFER_LAYER1 0xD0480000

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

//Global variables
uint_t lcdLine = 0;
uint_t lcdColumn = 0;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;


/**
 * @brief Set cursor location
 * @param[in] line Line number
 * @param[in] column Column number
 **/

void lcdSetCursor(uint_t line, uint_t column)
{
   lcdLine = MIN(line, 20);
   lcdColumn = MIN(column, 40);
}


/**
 * @brief Write a character to the LCD display
 * @param[in] c Character to be written
 **/

void lcdPutChar(char_t c)
{
   if(c == '\r')
   {
      lcdColumn = 0;
   }
   else if(c == '\n')
   {
      lcdColumn = 0;
      lcdLine++;
   }
   else if(lcdLine < 20 && lcdColumn < 40)
   {
      //Display current character
      BSP_LCD_DisplayChar(lcdColumn * 16, lcdLine * 24, c);

      //Advance the cursor position
      if(++lcdColumn >= 40)
      {
         lcdColumn = 0;
         lcdLine++;
      }
   }
}


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
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 5;
   RCC_OscInitStruct.PLL.PLLN = 160;
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

   //SDRAM
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER4;
   MPU_InitStruct.BaseAddress = 0xD0000000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
   MPU_InitStruct.SubRegionDisable = 0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //LCD frame buffer
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER5;
   MPU_InitStruct.BaseAddress = 0xD0400000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_4MB;
   MPU_InitStruct.SubRegionDisable = 0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //Enable MPU
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


/**
 * @brief FTP client test routine
 * @return Error code
 **/

error_t ftpClientTest(void)
{
   error_t error;
   size_t length;
   IpAddr ipAddr;
   FtpClientContext ftpContext;
   static char_t buffer[256];

   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");
   //Resolve FTP server name
   error = getHostByName(NULL, "ftp.gnu.org", &ipAddr, 0);

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Debug message
   TRACE_INFO("Connecting to FTP server %s\r\n", ipAddrToString(&ipAddr, NULL));
   //Connect to the FTP server
   error = ftpConnect(&ftpContext, NULL, &ipAddr, 21, FTP_NO_SECURITY | FTP_PASSIVE_MODE);

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to connect to FTP server!\r\n");
      //Exit immediately
      return error;
   }

   //Debug message
   TRACE_INFO("Successful connection\r\n");

   //Start of exception handling block
   do
   {
      //Login to the FTP server using the provided username and password
      error = ftpLogin(&ftpContext, "anonymous", "password", "");
      //Any error to report?
      if(error)
         break;

      //Open the specified file for reading
      error = ftpOpenFile(&ftpContext, "welcome.msg", FTP_FOR_READING | FTP_BINARY_TYPE);
      //Any error to report?
      if(error)
         break;

      //Dump the contents of the file
      while(1)
      {
         //Read data
         error = ftpReadFile(&ftpContext, buffer, sizeof(buffer) - 1, &length, 0);
         //End of file?
         if(error)
            break;

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Dump current data
         TRACE_INFO("%s", buffer);
      }

      //End the string with a line feed
      TRACE_INFO("\r\n");
      //Close the file
      error = ftpCloseFile(&ftpContext);

      //End of exception handling block
   } while(0);

   //Close the connection
   ftpClose(&ftpContext);
   //Debug message
   TRACE_INFO("Connection closed...\r\n");

   //Return status code
   return error;
}


/**
 * @brief User task
 **/

void userTask(void *param)
{
   char_t buffer[40];
#if (IPV4_SUPPORT == ENABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (IPV6_SUPPORT == ENABLED)
   Ipv6Addr ipv6Addr;
#endif

   //Point to the network interface
   NetInterface *interface = &netInterface[0];

   //Initialize LCD display
   lcdSetCursor(2, 0);
   printf("IPv4 Addr\r\n");
   lcdSetCursor(5, 0);
   printf("IPv6 Link-Local Addr\r\n");
   lcdSetCursor(8, 0);
   printf("IPv6 Global Addr\r\n");

   //Endless loop
   while(1)
   {
#if (IPV4_SUPPORT == ENABLED)
      //Display IPv4 host address
      lcdSetCursor(3, 0);
      ipv4GetHostAddr(interface, &ipv4Addr);
      printf("%-16s\r\n", ipv4AddrToString(ipv4Addr, buffer));
#endif

#if (IPV6_SUPPORT == ENABLED)
      //Display IPv6 link-local address
      lcdSetCursor(6, 0);
      ipv6GetLinkLocalAddr(interface, &ipv6Addr);
      printf("%-40s\r\n", ipv6AddrToString(&ipv6Addr, buffer));

      //Display IPv6 global address
      lcdSetCursor(9, 0);
      ipv6GetGlobalAddr(interface, 0, &ipv6Addr);
      printf("%-40s\r\n", ipv6AddrToString(&ipv6Addr, buffer));
#endif

      //User button pressed?
      if(!BSP_PB_GetState(BUTTON_KEY))
      {
         //FTP client test routine
         ftpClientTest();

         //Wait for the user button to be released
         while(!BSP_PB_GetState(BUTTON_KEY));
      }

      //Loop delay
      osDelayTask(100);
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
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("*** CycloneTCP FTP Client Demo ***\r\n");
   TRACE_INFO("**********************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: STM32H743\r\n");
   TRACE_INFO("\r\n");

   //Initialize I/Os
   BSP_IO_Init();

   //LED configuration
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED3);

   //Clear LEDs
   BSP_LED_Off(LED1);
   BSP_LED_Off(LED3);

   //Initialize user button
   BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

   //Initialize LCD display
   BSP_LCD_Init();
   BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
   BSP_LCD_SelectLayer(0);
   BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
   BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
   BSP_LCD_SetFont(&Font24);
   BSP_LCD_DisplayOn();

   //Clear LCD display
   BSP_LCD_Clear(LCD_COLOR_BLUE);

   //Welcome message
   lcdSetCursor(0, 0);
   printf("FTP Client Demo\r\n");

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
   netSetHostname(interface, "FTPClientDemo");
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

   //Create user task
   task = osCreateTask("User Task", userTask, NULL, 500, OS_TASK_PRIORITY_NORMAL);
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
