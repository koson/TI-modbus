/*
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_freertos.c ========
 */

/* Standard header files */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "msp432e401y.h"
#include "system_msp432e401y.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/msp432e4_eth_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "ftp/ftp_client.h"
#include "modbus/modbus_client.h"
#include "modbus/modbus_server.h"
#include "error.h"
#include "debug.h"

//Application configuration
#define APP_MAC_ADDR "70-FF-76-1C-61-51" //Ethernet address

/*
 * device 1: 61-85
 * device 2: 63-19
 * device 3: 62-EC
 * device 4: 62-2D
 * device 5: 61-AD
 * device 6: 62-3B
 * device 7: 62-A4
 * device 8: 62-ED
 * device 9: 5F-6E
 * device 10: 5D-D8
 * device 11: 63-27
 * device 12: 5F-8E
 * device 13: 5E-FE
 * device 14: 62-B6
 * device 15: 5F-68
 * device 16: 62-39
 * device 17: 5F-FE
 * device 18: 5F-20
 * device 19: 63-1F
 * device 20: 5F-67
 *
 */

//#define APP_USE_DHCP ENABLED //Coment out if want to enable DHCP
#define APP_IPV4_HOST_ADDR "192.168.2.120"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.2.20"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

//#define APP_USE_SLAAC ENABLED //Coment out if want to enable Slacc for IPv6
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::432"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::432"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

#define APP_SERVER_NAME "192.168.2.230" //ip address of the server host (Typhoon HIL device)
#define APP_SERVER_PORT 502

//Global variables
DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
ModbusClientContext modbusClientContext; //modbus client object
ModbusServerSettings modbusServerSettings;
ModbusServerContext modbusServerContext; //modbus server object
int device_id = 100;

uint16_t serverReg[2]; //server registers
bool_t serverCoil[2]; //server coils

//Forward declaration of functions
void modbusServerLockCallback(void);
void modbusServerUnlockCallback(void);

error_t modbusServerReadCoilCallback(uint16_t address, bool_t *state);

error_t modbusServerWriteCoilCallback(uint16_t address, bool_t state,
                                      bool_t commit);

error_t modbusServerReadRegCallback(uint16_t address, uint16_t *value);

error_t modbusServerWriteRegCallback(uint16_t address, uint16_t value,
                                     bool_t commit);


/**
 * @brief I/O initialization for LED blinking and switches
 **/

void ioInit(void)
{
    //Enable GPIO clocks
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //Configure LED1 (PN1) and LED2 (PN0)
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);

    //Configure USR_SW1 (PJ0) and USR_SW2 (PJ1)
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //Enable weak pull-up on USR_SW1 and USR_SW2
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

// read/write function
error_t modbusClient(void)
{
    error_t error;
    ModbusExceptionCode exceptionCode;
    uint16_t regValue[5]; //read registers
    uint16_t regValueW[5]; //write registers

    do
    {

        //Read holding registers
        /*error = modbusClientReadHoldingRegs(&modbusClientContext, 2000, 1, regValue);

        //Check status code
        if(error == NO_ERROR)
        {
            //Debug message
            TRACE_INFO("Register 2000: 0x%04" PRIX16 "\r\n", regValue[0]);
            //TRACE_INFO("Register 30001: 0x%04" PRIX16 "\r\n", regValue[1]);
        }
        else if(error == ERROR_EXCEPTION_RECEIVED)
        {
            //Retrieve exception code
            modbusClientGetExceptionCode(&modbusClientContext, &exceptionCode);
            //Debug message
            TRACE_WARNING("Exception %u received!\r\n", exceptionCode);
        }
        else
        {
            //A protocol error has occurred
            break;
        }*/

        //Write holding registers
        regValueW[0] = serverReg[0];
        regValueW[1] = serverReg[1];

        error = modbusClientWriteMultipleRegs(&modbusClientContext, (device_id - 1)*10, 2, regValueW);

        //Check status code
        if(error == NO_ERROR)
        {
            //Debug message
            TRACE_INFO("Device %i \n", device_id);
            TRACE_INFO("Register 0 write: 0x%04" PRIX16 "\r\n", regValueW[0]);
            TRACE_INFO("Register 1 write: 0x%04" PRIX16 "\r\n", regValueW[1]);
        }
        else if(error == ERROR_EXCEPTION_RECEIVED)
        {
            //Retrieve exception code
            modbusClientGetExceptionCode(&modbusClientContext, &exceptionCode);
            //Debug message
            TRACE_WARNING("Exception %u received!\r\n", exceptionCode);
        }
        else
        {
            //A protocol error has occurred
            break;
        }

        osDelayTask(1000); //comment when not debugging with terminal

        //End of exception handling block
    } while(0);

    //Return status code
    return error;
}

void userTask(void *param)
{
    error_t error;
    IpAddr ipAddr;

    do
    {
        //Debug message
        TRACE_INFO("\r\n\r\nResolving server name...\r\n");

        //Resolve Modbus/TCP server name
        error = getHostByName(NULL, APP_SERVER_NAME, &ipAddr, 0);
        //Any error to report?
        if(error)
        {
            //Debug message
            TRACE_ERROR("Failed to resolve server name!\r\n");
            //break;
        }

        //Set timeout value for blocking operations
        error = modbusClientSetTimeout(&modbusClientContext, 15000);
        //Any error to report?
        //if(error)
            //break;

        //Debug message
        TRACE_INFO("Connecting to Modbus/TCP server %s\r\n",
                   ipAddrToString(&ipAddr, NULL));


        //Establish connection with the Modbus/TCP server
        error = modbusClientConnect(&modbusClientContext, &ipAddr,
                                    APP_SERVER_PORT);
        //Any error to report?
        if(error)
        {
            //Debug message
            TRACE_ERROR("Failed to connect to Modbus/TCP server!\r\n");
            //break;
        }
        osDelayTask(5000);
        //End of exception handling block
    } while(error);

    //Endless loop
    while(1)
    {
        modbusClient();
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
        //blink LED D1
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        osDelayTask(100);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
        osDelayTask(900);
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    error_t error;
    NetInterface *interface; //Ethernet object
    OsTask *task;
    MacAddr macAddr;
    IpAddr ipAddr;

#if (APP_USE_DHCP == DISABLED)
    Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
    Ipv6Addr ipv6Addr;
#endif

#ifdef __TI_ARM__
    //System initialization
    SystemInit();
#endif

    //Initialize Free RTOS kernel
    osInitKernel();

    //Configure debug UART, 115200 baud rate
    debugInit(115200);

    //Start-up message
    TRACE_INFO("\r\n");
    TRACE_INFO("**********************************\r\n");
    TRACE_INFO("*** Modbus client-server **********\r\n");
    TRACE_INFO("**********************************\r\n");
    TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
    TRACE_INFO("Target: MSP432E401Y\r\n");
    TRACE_INFO("\r\n");

    //Configure I/Os
    ioInit();

    //TCP/IP stack initialization
    error = netInit();
    //Any error to report?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
    }

    //Configure the first Ethernet interface, one interface per NIC
    interface = &netInterface[0];
    //Set interface name
    netSetInterfaceName(interface, "eth0");
    //Set host name
    netSetHostname(interface, "MBClientServerDemo");
    //Select the relevant network adapter
    netSetDriver(interface, &msp432e4EthDriver);
    //Set host (TI device) MAC address
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
#if (APP_USE_DHCP == ENABLED) //if using DHCP
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

#if (IPV6_SUPPORT == ENABLED) //if using Slaac
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

    //Get default settings
    modbusServerGetDefaultSettings(&modbusServerSettings);
    //Bind Modbus/TCP server to the desired interface
    modbusServerSettings.interface = &netInterface[0];
    //Listen to port 502
    modbusServerSettings.port = MODBUS_TCP_PORT;
    //Callback functions
    modbusServerSettings.lockCallback = modbusServerLockCallback;
    modbusServerSettings.unlockCallback = modbusServerUnlockCallback;
    modbusServerSettings.readCoilCallback = modbusServerReadCoilCallback;
    modbusServerSettings.writeCoilCallback = modbusServerWriteCoilCallback;
    modbusServerSettings.readRegCallback = modbusServerReadRegCallback;
    modbusServerSettings.writeRegValueCallback = modbusServerWriteRegCallback;

    //Modbus/TCP server initialization
    error = modbusServerInit(&modbusServerContext, &modbusServerSettings);
    //Failed to initialize Modbus/TCP server?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize Modbus/TCP server!\r\n");
    }

    //Start Modbus/TCP server
    error = modbusServerStart(&modbusServerContext);
    //Failed to start Modbus/TCP server?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to start Modbus/TCP server!\r\n");
    }

    modbusClientInit(&modbusClientContext);


    //Create user task, this handles the Modbus client
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

    //Close Modbus/TCP connection
    modbusClientDisconnect(&modbusClientContext);

    //Release Modbus/TCP client context
    modbusClientDeinit(&modbusClientContext);

    return (0);
}



//*****************************************************************************
//
//! \brief Application defined malloc failed hook, used to debug errors
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    while(1)
    {
    }
}

/**
 * @brief Lock Modbus table callback function
 **/

void modbusServerLockCallback(void)
{
}


/**
 * @brief Unlock Modbus table callback function
 **/

void modbusServerUnlockCallback(void)
{
}


/**
 * @brief Get coil state callback function
 * @param[in] address Coil address
 * @param[out] state Current coil state
 * @return Error code
 **/

error_t modbusServerReadCoilCallback(uint16_t address, bool_t *state)
{
    error_t error;

    //Check register address
    if(address == 0)
    {
        //Retrieve the state of the LED
        *state = serverCoil[0];
        //Successful processing
        error = NO_ERROR;
    }
    else if(address == 1)
    {
        //Retrieve the state of the LED
        *state = serverCoil[1];
        //Successful processing
        error = NO_ERROR;
    }
    else
    {
        //The register address is not acceptable
        error = ERROR_INVALID_ADDRESS;
    }

    //Return status code
    return error;
}


/**
 * @brief Set coil state callback function
 * @param[in] address Address of the coil
 * @param[in] state Desired coil state
 * @param[in] commit This flag indicates the current phase (validation phase
 *   or write phase if the validation was successful)
 * @return Error code
 **/

error_t modbusServerWriteCoilCallback(uint16_t address, bool_t state,
                                      bool_t commit)
{
    error_t error;

    //Check register address
    if(address == 0)
    {
        //Write phase?
        if(commit)
        {
            //Update the state of the LED
            //if(state)
            //BSP_LED_On(LED2);
            //else
            //BSP_LED_Off(LED2);

            //Save the state of the LED
            serverCoil[0] = state;
        }

        //Successful processing
        error = NO_ERROR;
    }
    else if(address == 1)
    {
        //Write phase?
        if(commit)
        {
            //Update the state of the LED
            //if(state)
            //BSP_LED_On(LED3);
            //else
            //BSP_LED_Off(LED3);

            //Save the state of the LED
            serverCoil[1] = state;
        }

        //Successful processing
        error = NO_ERROR;
    }
    else
    {
        //The register address is not acceptable
        error = ERROR_INVALID_ADDRESS;
    }

    //Return status code
    return error;
}


/**
 * @brief Get register value callback function
 * @param[in] address Register address
 * @param[out] state Current register value
 * @return Error code
 **/

error_t modbusServerReadRegCallback(uint16_t address, uint16_t *value)
{
    error_t error;

    //Check register address
    if(address == 0)
    {
        //Retrieve the state of the LED
        *value = serverReg[0];
        //Successful processing
        error = NO_ERROR;
    }
    else if(address == 1)
    {
        //Retrieve the state of the LED
        *value = serverReg[1];
        //Successful processing
        error = NO_ERROR;
    }
    else
    {
        //The register address is not acceptable
        error = ERROR_INVALID_ADDRESS;
    }

    //Return status code
    return error;
}


/**
 * @brief Set register value callback function
 * @param[in] address Register address
 * @param[in] state Desired register value
 * @param[in] commit This flag indicates the current phase (validation phase
 *   or write phase if the validation was successful)
 * @return Error code
 **/

error_t modbusServerWriteRegCallback(uint16_t address, uint16_t value,
                                     bool_t commit)
{
    error_t error;

    //Check register address
    if(address == 0)
    {
        //Write phase?
        if(commit)
        {
            //Update the state of the LED
            //if(value & 1)
            //BSP_LED_On(LED2);
            //else
            //BSP_LED_Off(LED2);

            //Save the state of the LED
            //ledState[0] = value;

            //write register
            serverReg[0] = value;
        }

        //Successful processing
        error = NO_ERROR;
    }
    else if(address == 1)
    {
        //Write phase?
        if(commit)
        {
            //Update the state of the LED
            //if(value & 1)
            //BSP_LED_On(LED3);
            //else
            //BSP_LED_Off(LED3);

            //Save the state of the LED
            //ledState[1] = value;
            //write register
            serverReg[1] = value;
        }

        //Successful processing
        error = NO_ERROR;
    }
    else
    {
        //The register address is not acceptable
        error = ERROR_INVALID_ADDRESS;
    }

    //Return status code
    return error;
}
