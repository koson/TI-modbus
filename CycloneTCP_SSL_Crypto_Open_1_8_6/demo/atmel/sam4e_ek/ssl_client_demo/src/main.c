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
#include "sam4e.h"
#include "sam4e_ek.h"
#include "ili93xx.h"
#include "os_port.h"
#include "resource_manager.h"
#include "core/net.h"
#include "drivers/mac/sam4e_eth_driver.h"
#include "drivers/phy/ksz8051_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "tls.h"
#include "tls_cipher_suites.h"
#include "rng/yarrow.h"
#include "ext_int_driver.h"
#include "error.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-04-16"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::416"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::416"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//Server hostname and port
#define APP_SERVER_NAME "www.oryx-embedded.com"
#define APP_SERVER_PORT 443
#define APP_REQUEST_URI "/test.php"

//Compilation options
#define APP_SET_CIPHER_SUITES DISABLED
#define APP_SET_SERVER_NAME DISABLED
#define APP_SET_TRUSTED_CA_LIST DISABLED
#define APP_SET_CLIENT_CERT DISABLED

//Trusted CA bundle
#define APP_CA_CERT_BUNDLE "certs/ca_cert_bundle.pem"

//Client's certificate and private key
#define APP_CLIENT_CERT "certs/client_rsa_cert.pem"
#define APP_CLIENT_PRIVATE_KEY "certs/client_rsa_key.pem"

//List of preferred ciphersuites
static const uint16_t cipherSuites[] =
{
   TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
   TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
   TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
   TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
   TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
   TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
   TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
   TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
   TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
   TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
   TLS_RSA_WITH_AES_128_GCM_SHA256,
   TLS_RSA_WITH_AES_256_GCM_SHA384,
   TLS_RSA_WITH_AES_128_CBC_SHA,
   TLS_RSA_WITH_AES_256_CBC_SHA,
   TLS_RSA_WITH_3DES_EDE_CBC_SHA
};

//Credentials
char_t *clientCert = NULL;
size_t clientCertLength = 0;
char_t *clientPrivateKey = NULL;
size_t clientPrivateKeyLength = 0;
char_t *trustedCaList = NULL;
size_t trustedCaListLength = 0;

//Global variables
uint_t lcdLine = 0;
uint_t lcdColumn = 0;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
YarrowContext yarrowContext;
uint8_t seed[32];


/**
 * @brief Set cursor location
 * @param[in] line Line number
 * @param[in] column Column number
 **/

void lcdSetCursor(uint_t line, uint_t column)
{
   lcdLine = MIN(line, 20);
   lcdColumn = MIN(column, 20);
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
   else if(lcdLine < 20 && lcdColumn < 20)
   {
      //Display current character
      ili93xx_draw_char(lcdColumn * 12 + 2, lcdLine * 16 + 2, c);

      //Advance the cursor position
      if(++lcdColumn >= 20)
      {
         lcdColumn = 0;
         lcdLine++;
      }
   }
}


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Enable PIO peripheral clocks
   PMC->PMC_PCER0 = (1 << ID_PIOA) | (1 << ID_PIOD);

   //Configure LED1
   PIO_LED1->PIO_PER = LED1;
   PIO_LED1->PIO_OER = LED1;
   PIO_LED1->PIO_SODR = LED1;

   //Configure LED2
   PIO_LED2->PIO_PER = LED2;
   PIO_LED2->PIO_OER = LED2;
   PIO_LED2->PIO_SODR = LED2;

   //Configure LED3
   PIO_LED3->PIO_PER = LED3;
   PIO_LED3->PIO_OER = LED3;
   PIO_LED3->PIO_SODR = LED3;

   //Configure LED4
   PIO_LED4->PIO_PER = LED4;
   PIO_LED4->PIO_OER = LED4;
   PIO_LED4->PIO_SODR = LED4;

   //Configure up button
   PIO_BT_UP->PIO_PER = BT_UP;
   PIO_BT_UP->PIO_ODR = BT_UP;

   //Configure down button
   PIO_BT_DOWN->PIO_PER = BT_DOWN;
   PIO_BT_DOWN->PIO_ODR = BT_DOWN;
}


/**
 * @brief LCD display initialization
 **/

void lcdInit(void)
{
   uint_t i;
   struct ili93xx_opt_t config;

   //Enable PIO peripheral clocks
   PMC->PMC_PCER0 = (1 << ID_PIOC) | (1 << ID_PIOD);
   //Enable SMC peripheral clock
   PMC->PMC_PCER0 = (1 << ID_SMC);

   //Configure LCD data bus
   PIO_LCD_DATA->PIO_PDR = LCD_DATA;
   PIO_LCD_DATA->PIO_IDR = LCD_DATA;
   PIO_LCD_DATA->PIO_ABCDSR[0] &= ~LCD_DATA;
   PIO_LCD_DATA->PIO_ABCDSR[1] &= ~LCD_DATA;

   //Configure LCD control pins
   PIO_LCD_RS->PIO_PDR = LCD_RS;
   PIO_LCD_RS->PIO_IDR =  LCD_RS;
   PIO_LCD_RS->PIO_ABCDSR[0] &= ~LCD_RS;
   PIO_LCD_RS->PIO_ABCDSR[1] &= ~LCD_RS;

   PIO_LCD_RD->PIO_PDR = LCD_RD;
   PIO_LCD_RD->PIO_IDR =  LCD_RD;
   PIO_LCD_RD->PIO_ABCDSR[0] &= ~LCD_RD;
   PIO_LCD_RD->PIO_ABCDSR[1] &= ~LCD_RD;

   PIO_LCD_WR->PIO_PDR = LCD_WR;
   PIO_LCD_WR->PIO_IDR =  LCD_WR;
   PIO_LCD_WR->PIO_ABCDSR[0] &= ~LCD_WR;
   PIO_LCD_WR->PIO_ABCDSR[1] &= ~LCD_WR;

   PIO_LCD_NCS1->PIO_PDR = LCD_NCS1;
   PIO_LCD_NCS1->PIO_IDR =  LCD_NCS1;
   PIO_LCD_NCS1->PIO_ABCDSR[0] &= ~LCD_NCS1;
   PIO_LCD_NCS1->PIO_ABCDSR[1] &= ~LCD_NCS1;

   //Configure LCD backlight pin
   PIO_LCD_BL->PIO_IDR = LCD_BL;
   PIO_LCD_BL->PIO_OER = LCD_BL;
   PIO_LCD_BL->PIO_PER = LCD_BL;
   PIO_LCD_BL->PIO_CODR = LCD_BL;

   //Configure SMC interface
   SMC->SMC_CS_NUMBER[1].SMC_SETUP = SMC_SETUP_NWE_SETUP(2) |
      SMC_SETUP_NCS_WR_SETUP(2) |
      SMC_SETUP_NRD_SETUP(2) |
      SMC_SETUP_NCS_RD_SETUP(2);

   SMC->SMC_CS_NUMBER[1].SMC_PULSE = SMC_PULSE_NWE_PULSE(4) |
      SMC_PULSE_NCS_WR_PULSE(4) |
      SMC_PULSE_NRD_PULSE(10) |
      SMC_PULSE_NCS_RD_PULSE(10);

   SMC->SMC_CS_NUMBER[1].SMC_CYCLE = SMC_CYCLE_NWE_CYCLE(10) |
      SMC_CYCLE_NRD_CYCLE(22);

   SMC->SMC_CS_NUMBER[1].SMC_MODE = SMC_MODE_READ_MODE |
      SMC_MODE_WRITE_MODE;

   //Turn on LCD backlight
   for(i = 0; i < 25; i++)
   {
      PIO_LCD_BL->PIO_CODR = LCD_BL;
      usleep(100);
      PIO_LCD_BL->PIO_SODR = LCD_BL;
      usleep(100);
   }

   //Set display parameters
   config.ul_width = ILI93XX_LCD_WIDTH;
   config.ul_height = ILI93XX_LCD_HEIGHT;
   config.foreground_color = COLOR_WHITE;
   config.background_color = COLOR_BLUE;

   //Initialize LCD
   ili93xx_init(&config);
   ili93xx_set_foreground_color(COLOR_BLUE);
   ili93xx_draw_filled_rectangle(0, 0, ILI93XX_LCD_WIDTH, ILI93XX_LCD_HEIGHT);
   ili93xx_set_foreground_color(COLOR_WHITE);
   ili93xx_display_on();
}


/**
 * @brief SSL client test routine
 * @return Error code
 **/

error_t sslClientTest(void)
{
   error_t error;
   size_t length;
   IpAddr ipAddr;
   static char_t buffer[256];

   //Underlying socket
   Socket *socket = NULL;
   //SSL/TLS context
   TlsContext *tlsContext = NULL;

   //Debug message
   TRACE_INFO("Resolving server name...\r\n");

   //Resolve SSL server name
   error = getHostByName(NULL, APP_SERVER_NAME, &ipAddr, 0);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Create a new socket to handle the request
   socket = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP);
   //Any error to report?
   if(!socket)
   {
      //Debug message
      TRACE_INFO("Failed to open socket!\r\n");
      //Exit immediately
      return ERROR_OPEN_FAILED;
   }

   //Start of exception handling block
   do
   {
      //Debug message
      TRACE_INFO("Connecting to SSL server %s\r\n", ipAddrToString(&ipAddr, NULL));

      //Connect to the SSL server
      error = socketConnect(socket, &ipAddr, APP_SERVER_PORT);
      //Any error to report?
      if(error)
         break;

      //Initialize SSL/TLS context
      tlsContext = tlsInit();
      //Initialization failed?
      if(!tlsContext)
      {
         //Report an error
         error = ERROR_OUT_OF_MEMORY;
         //Exit immediately
         break;
      }

      //Bind TLS to the relevant socket
      error = tlsSetSocket(tlsContext, socket);
      //Any error to report?
      if(error)
         break;

      //Select client operation mode
      error = tlsSetConnectionEnd(tlsContext, TLS_CONNECTION_END_CLIENT);
      //Any error to report?
      if(error)
         break;

      //Set the PRNG algorithm to be used
      error = tlsSetPrng(tlsContext, YARROW_PRNG_ALGO, &yarrowContext);
      //Any error to report?
      if(error)
         break;

#if (APP_SET_CIPHER_SUITES == ENABLED)
      //Preferred cipher suite list
      error = tlsSetCipherSuites(tlsContext, cipherSuites, arraysize(cipherSuites));
      //Any error to report?
      if(error)
         break;
#endif

#if (APP_SET_SERVER_NAME == ENABLED)
      //Set the fully qualified domain name of the server
      error = tlsSetServerName(tlsContext, APP_SERVER_NAME);
      //Any error to report?
      if(error)
         break;
#endif

#if (APP_SET_TRUSTED_CA_LIST == ENABLED)
      //Import the list of trusted CA certificates
      error = tlsSetTrustedCaList(tlsContext, trustedCaList, trustedCaListLength);
      //Any error to report?
      if(error)
         break;
#endif

#if (APP_SET_CLIENT_CERT == ENABLED)
      //Import the client's certificate
      error = tlsAddCertificate(tlsContext, clientCert,
         clientCertLength, clientPrivateKey, clientPrivateKeyLength);
      //Any error to report?
      if(error)
         break;
#endif

      //Establish a secure session
      error = tlsConnect(tlsContext);
      //TLS handshake failure?
      if(error)
         break;

      //Format HTTP request
      sprintf(buffer, "GET %s HTTP/1.0\r\nHost: %s:%u\r\n\r\n",
         APP_REQUEST_URI, APP_SERVER_NAME, APP_SERVER_PORT);

      //Debug message
      TRACE_INFO("\r\n");
      TRACE_INFO("HTTP request:\r\n%s", buffer);

      //Send the request
      error = tlsWrite(tlsContext, buffer, strlen(buffer), NULL, 0);
      //Any error to report?
      if(error)
         break;

      //Debug message
      TRACE_INFO("HTTP response:\r\n");

      //Read the whole response
      while(1)
      {
         //Read data
         error = tlsRead(tlsContext, buffer, sizeof(buffer) - 1, &length, 0);
         //End of stream?
         if(error)
            break;

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Debug message
         TRACE_INFO("%s", buffer);
      }

      //Terminate SSL/TLS session
      error = tlsShutdown(tlsContext);
      //Any error to report?
      if(error)
         break;

      //End of exception handling block
   } while(0);

   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to communicate with SSL server!\r\n");
   }

   //Release SSL/TLS context
   tlsFree(tlsContext);
   //Close socket
   socketClose(socket);

   //Debug message
   TRACE_INFO("Connection closed...\r\n");

   //Return status code
   return error;
}


/**
 * @brief User task
 **/

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
   lcdSetCursor(9, 0);
   printf("IPv6 Global Addr\r\n");
   lcdSetCursor(13, 0);
   printf("Press user button\r\nto run test\r\n");

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
      lcdSetCursor(10, 0);
      ipv6GetGlobalAddr(interface, 0, &ipv6Addr);
      printf("%-40s\r\n", ipv6AddrToString(&ipv6Addr, buffer));
#endif

      //Up button pressed?
      if(!(PIO_BT_UP->PIO_PDSR & BT_UP))
      {
         //SSL client test routine
         sslClientTest();

         //Wait for the up button to be released
         while(!(PIO_BT_UP->PIO_PDSR & BT_UP));
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
      PIO_LED3->PIO_CODR = LED3;
      osDelayTask(100);
      PIO_LED3->PIO_SODR = LED3;
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

   //Update system core clock
   SystemCoreClockUpdate();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("******************************\r\n");
   TRACE_INFO("*** CycloneSSL Client Demo ***\r\n");
   TRACE_INFO("******************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: SAM4E\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //Initialize LCD display
   lcdInit();

   //Welcome message
   lcdSetCursor(0, 0);
   printf("SSL Client Demo\r\n");

   //Generate a random seed

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

   //Debug message
   TRACE_INFO("Loading credentials...\r\n");

   //Start of exception handling block
   do
   {
      //Load trusted CA certificates
      error = resGetData(APP_CA_CERT_BUNDLE, (uint8_t **) &trustedCaList, &trustedCaListLength);
      //Any error to report?
      if(error)
         break;

      //Load client's certificate
      error = resGetData(APP_CLIENT_CERT, (uint8_t **) &clientCert, &clientCertLength);
      //Any error to report?
      if(error)
         break;

      //Load client's private key
      error = resGetData(APP_CLIENT_PRIVATE_KEY, (uint8_t **) &clientPrivateKey, &clientPrivateKeyLength);
      //Any error to report?
      if(error)
         break;

      //End of exception handling block
   } while(0);

   //Check error code
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to load credentials!\r\n");
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
   netSetHostname(interface, "SSLClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &sam4eEthDriver);
   netSetPhyDriver(interface, &ksz8051PhyDriver);
   //Set external interrupt line driver
   netSetExtIntDriver(interface, &extIntDriver);
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
   task = osCreateTask("User Task", userTask, NULL, 600, OS_TASK_PRIORITY_NORMAL);
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
