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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <intrinsics.h>
#include <iorx63n.h>
#include "yrdkrx63n.h"
#include "mcu_info.h"
#include "hwsetup.h"
#include "lcd.h"
#include "r_rspi_rx600.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/rx63n_eth_driver.h"
#include "drivers/phy/dp83620_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "mqtt/mqtt_client.h"
#include "rng/yarrow.h"
#include "ext_int_driver.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "00-AB-CD-EF-00-63"

#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::63"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::63"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

//MQTT server name
#define APP_SERVER_NAME "iot.eclipse.org"

//MQTT server port
#define APP_SERVER_PORT 1883   //MQTT over TCP
//#define APP_SERVER_PORT 8883 //MQTT over SSL/TLS
//#define APP_SERVER_PORT 80   //MQTT over WebSocket
//#define APP_SERVER_PORT 443  //MQTT over secure WebSocket

//URI (for MQTT over WebSocket only)
#define APP_SERVER_URI "/ws"

//List of trusted CA certificates
char_t trustedCaList[] =
   "-----BEGIN CERTIFICATE-----"
   "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/"
   "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT"
   "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow"
   "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD"
   "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB"
   "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O"
   "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq"
   "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b"
   "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw"
   "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD"
   "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV"
   "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG"
   "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69"
   "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr"
   "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz"
   "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5"
   "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo"
   "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ"
   "-----END CERTIFICATE-----";

//Connection states
#define APP_STATE_NOT_CONNECTED 0
#define APP_STATE_CONNECTING    1
#define APP_STATE_CONNECTED     2

//Global variables
uint_t lcdLine = 0;
uint_t lcdColumn = 0;
uint_t connectionState = APP_STATE_NOT_CONNECTED;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
MqttClientContext mqttClientContext;
YarrowContext yarrowContext;
uint8_t seed[32];


/**
 * @brief Set cursor location
 * @param[in] line Line number
 * @param[in] column Column number
 **/

void lcdSetCursor(uint_t line, uint_t column)
{
   lcdLine = MIN(line, 8);
   lcdColumn = MIN(column, 12);
}


/**
 * @brief Write a character to the LCD display
 * @param[in] c Character to be written
 **/

void lcdPutChar(char_t c)
{
   uint8_t buffer[2];

   if(c == '\r')
   {
      lcdColumn = 0;
   }
   else if(c == '\n')
   {
      lcdColumn = 0;
      lcdLine++;
   }
   else if(lcdLine < 8 && lcdColumn < 12)
   {
      buffer[0] = c;
      buffer[1] = '\0';

      //Display current character
      lcd_display(lcdColumn * 8, lcdLine * 8, buffer);

      //Advance the cursor position
      if(++lcdColumn >= 12)
      {
         lcdColumn = 0;
         lcdLine++;
      }
   }
}


/**
 * @brief Random data generation callback function
 * @param[out] data Buffer where to store the random data
 * @param[in] lenght Number of bytes that are required
 * @return Error code
 **/

error_t webSocketRngCallback(uint8_t *data, size_t length)
{
   //Generate some random data
   return yarrowRead(&yarrowContext, data, length);
}


/**
 * @brief SSL/TLS initialization callback
 * @param[in] context Pointer to the MQTT client context
 * @param[in] tlsContext Pointer to the SSL/TLS context
 * @return Error code
 **/

error_t mqttTlsInitCallback(MqttClientContext *context,
   TlsContext *tlsContext)
{
   error_t error;

   //Debug message
   TRACE_INFO("MQTT: TLS initialization callback\r\n");

   //Set the PRNG algorithm to be used
   error = tlsSetPrng(tlsContext, YARROW_PRNG_ALGO, &yarrowContext);
   //Any error to report?
   if(error)
      return error;

   //Set the fully qualified domain name of the server
   error = tlsSetServerName(tlsContext, APP_SERVER_NAME);
   //Any error to report?
   if(error)
      return error;

   //Import the list of trusted CA certificates
   error = tlsSetTrustedCaList(tlsContext, trustedCaList, strlen(trustedCaList));
   //Any error to report?
   if(error)
      return error;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Publish callback function
 * @param[in] context Pointer to the MQTT client context
 * @param[in] topic Topic name
 * @param[in] message Message payload
 * @param[in] length Length of the message payload
 * @param[in] dup Duplicate delivery of the PUBLISH packet
 * @param[in] qos QoS level used to publish the message
 * @param[in] retain This flag specifies if the message is to be retained
 * @param[in] packetId Packet identifier
 **/

void mqttPublishCallback(MqttClientContext *context,
   const char_t *topic, const uint8_t *message, size_t length,
   bool_t dup, MqttQosLevel qos, bool_t retain, uint16_t packetId)
{
   //Debug message
   TRACE_INFO("PUBLISH packet received...\r\n");
   TRACE_INFO("  Dup: %u\r\n", dup);
   TRACE_INFO("  QoS: %u\r\n", qos);
   TRACE_INFO("  Retain: %u\r\n", retain);
   TRACE_INFO("  Packet Identifier: %u\r\n", packetId);
   TRACE_INFO("  Topic: %s\r\n", topic);
   TRACE_INFO("  Message (%" PRIuSIZE " bytes):\r\n", length);
   TRACE_INFO_ARRAY("    ", message, length);

   //Check topic name
   if(!strcmp(topic, "board/leds/1"))
   {
      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
         LED11 = !LED11;
      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
         LED11 = LED_ON;
      else
         LED11 = LED_OFF;
   }
   else if(!strcmp(topic, "board/leds/2"))
   {
      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
         LED10 = !LED10;
      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
         LED10 = LED_ON;
      else
         LED10 = LED_OFF;
   }
   else if(!strcmp(topic, "board/leds/3"))
   {
      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
         LED9 = !LED9;
      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
         LED9 = LED_ON;
      else
         LED9 = LED_OFF;
   }
}


/**
 * @brief Establish MQTT connection
 **/

error_t mqttConnect(void)
{
   error_t error;
   IpAddr ipAddr;
   MqttClientCallbacks mqttClientCallbacks;

   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");

   //Resolve MQTT server name
   error = getHostByName(NULL, APP_SERVER_NAME, &ipAddr, 0);
   //Any error to report?
   if(error)
      return error;

#if (APP_SERVER_PORT == 80 || APP_SERVER_PORT == 443)
   //Register RNG callback
   webSocketRegisterRandCallback(webSocketRngCallback);
#endif

   //Initialize MQTT client context
   mqttClientInit(&mqttClientContext);
   //Initialize MQTT client callbacks
   mqttClientInitCallbacks(&mqttClientCallbacks);

   //Attach application-specific callback functions
   mqttClientCallbacks.publishCallback = mqttPublishCallback;
#if (APP_SERVER_PORT == 8883 || APP_SERVER_PORT == 443)
   mqttClientCallbacks.tlsInitCallback = mqttTlsInitCallback;
#endif

   //Register MQTT client callbacks
   mqttClientRegisterCallbacks(&mqttClientContext, &mqttClientCallbacks);

   //Set the MQTT version to be used
   mqttClientSetProtocolLevel(&mqttClientContext,
      MQTT_PROTOCOL_LEVEL_3_1_1);

#if (APP_SERVER_PORT == 1883)
   //MQTT over TCP
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_TCP);
#elif (APP_SERVER_PORT == 8883)
   //MQTT over SSL/TLS
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_TLS);
#elif (APP_SERVER_PORT == 80)
   //MQTT over WebSocket
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_WS);
#elif (APP_SERVER_PORT == 443)
   //MQTT over secure WebSocket
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_WSS);
#endif

   //Set keep-alive value
   mqttClientSetKeepAlive(&mqttClientContext, 3600);

#if (APP_SERVER_PORT == 80 || APP_SERVER_PORT == 443)
   //Set the hostname of the resource being requested
   mqttClientSetHost(&mqttClientContext, APP_SERVER_NAME);
   //Set the name of the resource being requested
   mqttClientSetUri(&mqttClientContext, APP_SERVER_URI);
#endif

   //Set client identifier
   //mqttClientSetIdentifier(&mqttClientContext, "client12345678");

   //Set user name and password
   //mqttClientSetAuthInfo(&mqttClientContext, "username", "password");

   //Set Will message
   mqttClientSetWillMessage(&mqttClientContext, "board/status",
      "offline", 7, MQTT_QOS_LEVEL_0, FALSE);

   //Debug message
   TRACE_INFO("Connecting to MQTT server %s...\r\n", ipAddrToString(&ipAddr, NULL));

   //Start of exception handling block
   do
   {
      //Establish connection with the MQTT server
      error = mqttClientConnect(&mqttClientContext,
         &ipAddr, APP_SERVER_PORT, TRUE);
      //Any error to report?
      if(error)
         break;

      //Subscribe to the desired topics
      error = mqttClientSubscribe(&mqttClientContext,
         "board/leds/+", MQTT_QOS_LEVEL_1, NULL);
      //Any error to report?
      if(error)
         break;

      //Send PUBLISH packet
      error = mqttClientPublish(&mqttClientContext, "board/status",
         "online", 6, MQTT_QOS_LEVEL_1, TRUE, NULL);
      //Any error to report?
      if(error)
         break;

      //End of exception handling block
   } while(0);

   //Check status code
   if(error)
   {
      //Close connection
      mqttClientClose(&mqttClientContext);
   }

   //Return status code
   return error;
}


/**
 * @brief MQTT test task
 **/

void mqttTestTask(void *param)
{
   error_t error;
   uint_t buttonState;
   uint_t prevButton1State;
   uint_t prevButton2State;
   uint_t prevButton3State;
   char_t buffer[16];

   //Initialize variables
   prevButton1State = 0;
   prevButton2State = 0;
   prevButton3State = 0;

   //Endless loop
   while(1)
   {
      //Check connection state
      if(connectionState == APP_STATE_NOT_CONNECTED)
      {
         //Update connection state
         connectionState = APP_STATE_CONNECTING;

         //Try to connect to the MQTT server
         error = mqttConnect();

         //Failed to connect to the MQTT server?
         if(error)
         {
            //Update connection state
            connectionState = APP_STATE_NOT_CONNECTED;
            //Recovery delay
            osDelayTask(2000);
         }
         else
         {
            //Update connection state
            connectionState = APP_STATE_CONNECTED;
         }
      }
      else
      {
         //Process incoming events
         error = mqttClientProcessEvents(&mqttClientContext, 100);

         //Connection to MQTT server lost?
         if(error != NO_ERROR && error != ERROR_TIMEOUT)
         {
            //Close connection
            mqttClientClose(&mqttClientContext);
            //Update connection state
            connectionState = APP_STATE_NOT_CONNECTED;
            //Recovery delay
            osDelayTask(2000);
         }
         else
         {
            //Initialize status code
            error = NO_ERROR;

            //Get user button state
            buttonState = !SW1;

            //Any change detected?
            if(buttonState != prevButton1State)
            {
               if(buttonState)
                  strcpy(buffer, "pressed");
               else
                  strcpy(buffer, "released");

               //Send PUBLISH packet
               error = mqttClientPublish(&mqttClientContext, "board/buttons/1",
                  buffer, strlen(buffer), MQTT_QOS_LEVEL_1, TRUE, NULL);

               //Save current state
               prevButton1State = buttonState;
            }

            //Check status code
            if(!error)
            {
               //Get SW2 button state
               buttonState = !SW2;

               //Any change detected?
               if(buttonState != prevButton2State)
               {
                  if(buttonState)
                     strcpy(buffer, "pressed");
                  else
                     strcpy(buffer, "released");

                  //Send PUBLISH packet
                  error = mqttClientPublish(&mqttClientContext, "board/buttons/2",
                     buffer, strlen(buffer), MQTT_QOS_LEVEL_1, TRUE, NULL);

                  //Save current state
                  prevButton2State = buttonState;
               }
            }

            //Check status code
            if(!error)
            {
               //Get SW3 button state
               buttonState = !SW3;

               //Any change detected?
               if(buttonState != prevButton3State)
               {
                  if(buttonState)
                     strcpy(buffer, "pressed");
                  else
                     strcpy(buffer, "released");

                  //Send PUBLISH packet
                  error = mqttClientPublish(&mqttClientContext, "board/buttons/3",
                     buffer, strlen(buffer), MQTT_QOS_LEVEL_1, TRUE, NULL);

                  //Save current state
                  prevButton3State = buttonState;
               }
            }

            //Failed to publish data?
            if(error)
            {
               //Close connection
               mqttClientClose(&mqttClientContext);
               //Update connection state
               connectionState = APP_STATE_NOT_CONNECTED;
               //Recovery delay
               osDelayTask(2000);
            }
         }
      }
   }
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

   //Point to the network interface
   NetInterface *interface = &netInterface[0];

   //Initialize LCD display
   lcdSetCursor(3, 0);
   printf("IPv4 Addr\r\n");

   //Endless loop
   while(1)
   {
#if (IPV4_SUPPORT == ENABLED)
      //Display IPv4 host address
      lcdSetCursor(4, 0);
      ipv4GetHostAddr(interface, &ipv4Addr);
      printf("%-16s\r\n", ipv4AddrToString(ipv4Addr, buffer));
#endif

      //Move cursor
      lcdSetCursor(7, 0);

      //Display connection state
      if(connectionState == APP_STATE_CONNECTING)
         printf("Connecting  \r\n");
      else if(connectionState == APP_STATE_CONNECTED)
         printf("Connected   \r\n");
      else
         printf("Disconnected\r\n");

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
      LED4 = LED_ON;
      osDelayTask(100);
      LED4 = LED_OFF;
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

   //Initialize hardware
   hardware_setup();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("*** CycloneTCP MQTT Client Demo ***\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: RX63N\r\n");
   TRACE_INFO("\r\n");

   //Initialize SPI bus
   R_RSPI_Init(RSPI_CHANNEL_0);
   //Initialize LCD display
   lcd_initialize();
   //Clear display
   lcd_clear();

   //Welcome message
   lcdSetCursor(0, 0);
   printf("MQTT Client\r\nDemo\r\n");

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
   netSetHostname(interface, "MQTTClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &rx63nEthDriver);
   netSetPhyDriver(interface, &dp83620PhyDriver);
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
   task = osCreateTask("User Task", userTask, NULL, 500, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Create MQTT test task
   task = osCreateTask("MQTT", mqttTestTask, NULL, 750, OS_TASK_PRIORITY_NORMAL);
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

   //Enable interrupts
   __enable_interrupt();

   //Start the execution of tasks
   osStartKernel();

   //This function should never return
   return 0;
}


/**
 * @brief Set up the timer that will generate the tick interrupt
 **/

void vApplicationSetupTimerInterrupt(void)
{
   //Disable protection
   SYSTEM.PRCR.WORD = 0xA50B;
   //Cancel CMT0 module stop state
   MSTP(CMT0) = 0;
   //Enable protection
   SYSTEM.PRCR.WORD = 0xA500;

   //Select PCLK/8 clock
   CMT0.CMCR.BIT.CKS = 0;
   //Set the compare match value
   CMT0.CMCOR = ((PCLK_HZ / configTICK_RATE_HZ) - 1) / 8;

   //Interrupt on compare match
   CMT0.CMCR.BIT.CMIE = 1;

   //Set interrupt priority
   _IPR(_CMT0_CMI0) = configKERNEL_INTERRUPT_PRIORITY;
   //Enable compare match interrupt
   _IEN(_CMT0_CMI0) = 1;

   //Start timer
   CMT.CMSTR0.BIT.STR0 = 1;
}
