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
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/wifi/esp32_wifi_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "mqtt/mqtt_client.h"
#include "rng/yarrow.h"
#include "debug.h"

//First Wi-Fi interface (STA mode) configuration
#define APP_IF0_MAC_ADDR "00-00-00-00-00-00"

#define APP_IF0_USE_DHCP_CLIENT ENABLED
#define APP_IF0_IPV4_HOST_ADDR "192.168.0.20"
#define APP_IF0_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IF0_IPV4_DEFAULT_GATEWAY "192.168.0.254"
#define APP_IF0_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IF0_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_IF0_USE_SLAAC ENABLED
#define APP_IF0_IPV6_LINK_LOCAL_ADDR "fe80::32:1"

//Second Wi-Fi interface (AP mode) configuration
#define APP_IF1_MAC_ADDR "00-00-00-00-00-00"
#define APP_IF1_USE_DHCP_SERVER ENABLED
#define APP_IF1_IPV4_HOST_ADDR "192.168.8.1"
#define APP_IF1_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IF1_IPV4_DEFAULT_GATEWAY "0.0.0.0"
#define APP_IF1_IPV4_PRIMARY_DNS "0.0.0.0"
#define APP_IF1_IPV4_SECONDARY_DNS "0.0.0.0"
#define APP_IF1_IPV4_ADDR_RANGE_MIN "192.168.8.10"
#define APP_IF1_IPV4_ADDR_RANGE_MAX "192.168.8.99"

#define APP_IF1_USE_ROUTER_ADV ENABLED
#define APP_IF1_IPV6_LINK_LOCAL_ADDR "fe80::32:2"
#define APP_IF1_IPV6_PREFIX "fd00:1:2:3::"
#define APP_IF1_IPV6_PREFIX_LENGTH 64
#define APP_IF1_IPV6_GLOBAL_ADDR "fd00:1:2:3::32:2"

//Wi-Fi parameters (STA mode)
#define APP_WIFI_STA_SSID "TEST_ESP32_STA"
#define APP_WIFI_STA_PASSWORD "12345678"

//Wi-Fi parameters (AP mode)
#define APP_WIFI_AP_SSID "TEST_ESP32_AP"
#define APP_WIFI_AP_PASSWORD "1122334455"

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
uint_t connectionState = APP_STATE_NOT_CONNECTED;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
DhcpServerSettings dhcpServerSettings;
DhcpServerContext dhcpServerContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
NdpRouterAdvSettings ndpRouterAdvSettings;
NdpRouterAdvPrefixInfo ndpRouterAdvPrefixInfo[1];
NdpRouterAdvContext ndpRouterAdvContext;
MqttClientContext mqttClientContext;
YarrowContext yarrowContext;
uint8_t seed[32];

//Forward declaration of functions
error_t wifiStaInterfaceInit(void);
error_t wifiApInterfaceInit(void);

esp_err_t wifiConnect(void);
esp_err_t wifiEnableAp(void);
esp_err_t wifiEventHandler(void *ctx, system_event_t *event);


/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Configure LED (GPIO2)
   gpio_pad_select_gpio(2);
   gpio_set_direction(2, GPIO_MODE_OUTPUT);
   gpio_set_level(2, 0);

   //Configure user button (GPIO0)
   gpio_pad_select_gpio(0);
   gpio_set_direction(0, GPIO_MODE_INPUT);
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
      if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
         gpio_set_level(2, 1);
      else
         gpio_set_level(2, 0);
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
   uint_t prevButtonState;
   char_t buffer[16];

   //Initialize variables
   prevButtonState = 0;

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
            buttonState = !gpio_get_level(0);

            //Any change detected?
            if(buttonState != prevButtonState)
            {
               if(buttonState)
                  strcpy(buffer, "pressed");
               else
                  strcpy(buffer, "released");

               //Send PUBLISH packet
               error = mqttClientPublish(&mqttClientContext, "board/buttons/1",
                  buffer, strlen(buffer), MQTT_QOS_LEVEL_1, TRUE, NULL);

               //Save current state
               prevButtonState = buttonState;
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
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   //Endless loop
   while(1)
   {
      gpio_set_level(2, 1);
      osDelayTask(100);
      gpio_set_level(2, 0);
      osDelayTask(900);
   }
}


/**
 * @brief Main entry point
 **/

void app_main(void)
{
   error_t error;
   OsTask *task;

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("*** CycloneTCP MQTT Client Demo ***\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: ESP32\r\n");
   TRACE_INFO("\r\n");

   //Configure I/Os
   ioInit();

   //Initialize NVS memory
   nvs_flash_init();
   //Initialize ESP32 event loop
   esp_event_loop_init(wifiEventHandler, NULL);

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

   //Configure the first network interface (Wi-Fi STA mode)
   wifiStaInterfaceInit();

   //Configure the second network interface (Wi-Fi AP mode)
   wifiApInterfaceInit();

   //Create MQTT test task
   task = osCreateTask("MQTT", mqttTestTask, NULL, 3000, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Create a task to blink the LED
   task = osCreateTask("Blink", blinkTask, NULL, 1000, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Connect to Wi-Fi network (STA mode)
   wifiConnect();
}


/**
 * @brief Wi-Fi STA interface initialization
 **/

error_t wifiStaInterfaceInit(void)
{
   error_t error;
   NetInterface *interface;
   MacAddr macAddr;
#if (APP_IF0_USE_DHCP_CLIENT == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_IF0_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;
#endif

   //Configure the first network interface (Wi-Fi STA mode)
   interface = &netInterface[0];

   //Set interface name
   netSetInterfaceName(interface, "wlan0");
   //Set host name
   netSetHostname(interface, "FTPClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &esp32WifiStaDriver);
   //Set host MAC address
   macStringToAddr(APP_IF0_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
      //Exit immediately
      return error;
   }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_IF0_USE_DHCP_CLIENT == ENABLED)
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
      //Exit immediately
      return error;
   }

   //Start DHCP client
   error = dhcpClientStart(&dhcpClientContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP client!\r\n");
      //Exit immediately
      return error;
   }
#else
   //Set IPv4 host address
   ipv4StringToAddr(APP_IF0_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IF0_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IF0_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IF0_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IF0_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_IF0_USE_SLAAC == ENABLED)
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
      //Exit immediately
      return error;
   }

   //Start IPv6 address autoconfiguration process
   error = slaacStart(&slaacContext);
   //Failed to start SLAAC process?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start SLAAC!\r\n");
      //Exit immediately
      return error;
   }
#else
   //Set link-local address
   ipv6StringToAddr(APP_IF0_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);
#endif
#endif

   //Successful initialization
   return NO_ERROR;
}


/**
 * @brief Wi-Fi AP interface initialization
 **/

error_t wifiApInterfaceInit(void)
{
   error_t error;
   NetInterface *interface;
   MacAddr macAddr;
   Ipv4Addr ipv4Addr;
   Ipv6Addr ipv6Addr;

   //Configure the second network interface (Wi-Fi AP mode)
   interface = &netInterface[1];

   //Set interface name
   netSetInterfaceName(interface, "wlan1");
   //Set host name
   netSetHostname(interface, "FTPClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &esp32WifiApDriver);
   //Set host MAC address
   macStringToAddr(APP_IF1_MAC_ADDR, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
      //Exit immediately
      return error;
   }

#if (IPV4_SUPPORT == ENABLED)
   //Set IPv4 host address
   ipv4StringToAddr(APP_IF1_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IF1_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IF1_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IF1_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IF1_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);

#if (APP_IF1_USE_DHCP_SERVER == ENABLED)
   //Get default settings
   dhcpServerGetDefaultSettings(&dhcpServerSettings);
   //Set the network interface to be configured by DHCP
   dhcpServerSettings.interface = interface;
   //Lease time, in seconds, assigned to the DHCP clients
   dhcpServerSettings.leaseTime = 3600;

   //Lowest and highest IP addresses in the pool that are available
   //for dynamic address assignment
   ipv4StringToAddr(APP_IF1_IPV4_ADDR_RANGE_MIN, &dhcpServerSettings.ipAddrRangeMin);
   ipv4StringToAddr(APP_IF1_IPV4_ADDR_RANGE_MAX, &dhcpServerSettings.ipAddrRangeMax);

   //Subnet mask
   ipv4StringToAddr(APP_IF1_IPV4_SUBNET_MASK, &dhcpServerSettings.subnetMask);
   //Default gateway
   ipv4StringToAddr(APP_IF1_IPV4_DEFAULT_GATEWAY, &dhcpServerSettings.defaultGateway);
   //DNS servers
   ipv4StringToAddr(APP_IF1_IPV4_PRIMARY_DNS, &dhcpServerSettings.dnsServer[0]);
   ipv4StringToAddr(APP_IF1_IPV4_SECONDARY_DNS, &dhcpServerSettings.dnsServer[1]);

   //DHCP server initialization
   error = dhcpServerInit(&dhcpServerContext, &dhcpServerSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP server!\r\n");
      //Exit immediately
      return error;
   }

   //Start DHCP server
   error = dhcpServerStart(&dhcpServerContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP server!\r\n");
      //Exit immediately
      return error;
   }
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
   //Set link-local address
   ipv6StringToAddr(APP_IF1_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);

   //Set IPv6 prefix
   ipv6StringToAddr(APP_IF1_IPV6_PREFIX, &ipv6Addr);
   ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IF1_IPV6_PREFIX_LENGTH);

   //Set global address
   ipv6StringToAddr(APP_IF1_IPV6_GLOBAL_ADDR, &ipv6Addr);
   ipv6SetGlobalAddr(interface, 0, &ipv6Addr);

#if (APP_IF1_USE_ROUTER_ADV == ENABLED)
   //List of IPv6 prefixes to be advertised
   ipv6StringToAddr(APP_IF1_IPV6_PREFIX, &ndpRouterAdvPrefixInfo[0].prefix);
   ndpRouterAdvPrefixInfo[0].length = APP_IF1_IPV6_PREFIX_LENGTH;
   ndpRouterAdvPrefixInfo[0].onLinkFlag = TRUE;
   ndpRouterAdvPrefixInfo[0].autonomousFlag = TRUE;
   ndpRouterAdvPrefixInfo[0].validLifetime = 3600;
   ndpRouterAdvPrefixInfo[0].preferredLifetime = 1800;

   //Get default settings
   ndpRouterAdvGetDefaultSettings(&ndpRouterAdvSettings);
   //Set the underlying network interface
   ndpRouterAdvSettings.interface = interface;
   //Maximum time interval between unsolicited Router Advertisements
   ndpRouterAdvSettings.maxRtrAdvInterval = 60000;
   //Minimum time interval between unsolicited Router Advertisements
   ndpRouterAdvSettings.minRtrAdvInterval = 20000;
   //Router lifetime
   ndpRouterAdvSettings.defaultLifetime = 0;
   //List of IPv6 prefixes
   ndpRouterAdvSettings.prefixList = ndpRouterAdvPrefixInfo;
   ndpRouterAdvSettings.prefixListLength = arraysize(ndpRouterAdvPrefixInfo);

   //RA service initialization
   error = ndpRouterAdvInit(&ndpRouterAdvContext, &ndpRouterAdvSettings);
   //Failed to initialize DHCPv6 client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize RA service!\r\n");
      //Exit immediately
      return error;
   }

   //Start RA service
   error = ndpRouterAdvStart(&ndpRouterAdvContext);
   //Failed to start RA service?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start RA service!\r\n");
      //Exit immediately
      return error;
   }
#endif
#endif

   //Successful initialization
   return NO_ERROR;
}


/**
 * @brief Connect to Wi-Fi network (STA mode)
 * @return Error code
 **/

esp_err_t wifiConnect(void)
{
   esp_err_t ret;
   wifi_config_t config;

   //Debug message
   TRACE_INFO("ESP32: Connecting to Wi-Fi network (%s)...\r\n", APP_WIFI_STA_SSID);

   //Initialize configuration structure
   memset(&config, 0, sizeof(wifi_config_t));

   //Set Wi-Fi parameters
   strcpy((char_t *) config.sta.ssid, APP_WIFI_STA_SSID);
   strcpy((char_t *) config.sta.password, APP_WIFI_STA_PASSWORD);

   //Set Wi-Fi operating mode
   ret = esp_wifi_set_mode(WIFI_MODE_STA);

   //Check status code
   if(ret == ESP_OK)
   {
      //Configure STA interface
      ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
   }

   //Check status code
   if(ret == ESP_OK)
   {
      //Start STA interface
      ret = esp_wifi_start();
   }

   //Return status code
   return ret;
}


/**
 * @brief Create Wi-Fi network (AP mode)
 * @return Error code
 **/

esp_err_t wifiEnableAp(void)
{
   esp_err_t ret;
   wifi_config_t config;

   //Debug message
   TRACE_INFO("ESP32: Creating Wi-Fi network (%s)...\r\n", APP_WIFI_AP_SSID);

   //Initialize configuration structure
   memset(&config, 0, sizeof(wifi_config_t));

   //Set Wi-Fi parameters
   strcpy((char_t *) config.ap.ssid, APP_WIFI_AP_SSID);
   strcpy((char_t *) config.ap.password, APP_WIFI_AP_PASSWORD);
   config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
   config.ap.max_connection = 4;

   //Set Wi-Fi operating mode
   ret = esp_wifi_set_mode(WIFI_MODE_AP);

   //Check status code
   if(ret == ESP_OK)
   {
      //Configure AP interface
      ret = esp_wifi_set_config(ESP_IF_WIFI_AP, &config);
   }

   //Check status code
   if(ret == ESP_OK)
   {
      //Start AP interface
      ret = esp_wifi_start();
   }

   //Return status code
   return ret;
}


/**
 * @brief Wi-Fi event handler
 * @param[in] ctx Reserved parameter
 * @param[in] event Event information
 * @return Error code
 **/

esp_err_t wifiEventHandler(void *ctx, system_event_t *event)
{
   MacAddr macAddr;

   //Debug message
   TRACE_INFO("ESP32: Wi-Fi event handler (event = %u)\r\n", event->event_id);

   //Check event type
   switch (event->event_id)
   {
   case SYSTEM_EVENT_STA_START:
      //Debug message
      TRACE_INFO("ESP32: Station started\r\n");
      //Connect station to the AP
      esp_wifi_connect();
      break;

   case SYSTEM_EVENT_STA_CONNECTED:
      //Debug message
      TRACE_INFO("ESP32: Station connected\r\n");
      break;

   case SYSTEM_EVENT_STA_DISCONNECTED:
      //Debug message
      TRACE_INFO("ESP32: Station disconnected\r\n");
      //Reconnect station to the AP
      esp_wifi_connect();
      break;

   case SYSTEM_EVENT_AP_STACONNECTED:
      //Retrieve the MAC address of the station
      macCopyAddr(&macAddr, event->event_info.sta_connected.mac);
      //Debug message
      TRACE_INFO("ESP32: Station %s joining AP\r\n", macAddrToString(&macAddr, NULL));
      break;

   case SYSTEM_EVENT_AP_STADISCONNECTED:
      //Retrieve the MAC address of the station
      macCopyAddr(&macAddr, event->event_info.sta_connected.mac);
      //Debug message
      TRACE_INFO("ESP32: Station %s leaving AP\r\n", macAddrToString(&macAddr, NULL));
      break;

   default:
      break;
   }

   //Successful processing
   return ESP_OK;
}
