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
//#include <stdbool.h> //original
//#include <mqueue.h>  //original
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
#include "error.h"
#include "debug.h"


/* RTOS header files */
//#include <FreeRTOS.h> //original
//#include <task.h> //original

/* Driver header files */
//#include <ti/drivers/GPIO.h> //original

/* Example/Board Header files */
//#include "Board.h" //original
//#include "board_funcs.h" //original
//#include "command_task.h" //original

//Application configuration
#define APP_MAC_ADDR "70-FF-76-LC-5D-D8"

//#define APP_USE_DHCP ENABLED
#define APP_IPV4_HOST_ADDR "192.168.1.51"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.1.20"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

//#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::429"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::429"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

#define APP_SERVER_NAME "192.168.1.50"
#define APP_SERVER_PORT 502

//Global variables
DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
ModbusClientContext modbusClientContext;

//extern void ti_ndk_config_Global_startupFxn(); //original
//extern void MSP_EXP432E401Y_initEMAC(void); //original

//mqd_t               mqCmdQueueBlock; //original
//mqd_t               mqCmdQueueNoBlock; //original
//mqd_t               mqCloudQueueBlock; //original
//mqd_t               mqCloudQueueNoBlock; //original

/**
 * @brief I/O initialization
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

error_t modbusClientTest(void)
{
    error_t error;
    ModbusExceptionCode exceptionCode;
    IpAddr ipAddr;
    uint16_t regValue[5];
    uint16_t regValueW[5]={4,3,0,0,0};

    modbusClientInit(&modbusClientContext);
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
            break;
        }

        //Set timeout value for blocking operations
        error = modbusClientSetTimeout(&modbusClientContext, 15000);
        //Any error to report?
        if(error)
            break;

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

        //Read holding registers
        error = modbusClientReadHoldingRegs(&modbusClientContext, 2000, 1, regValue);

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
        }

        //Write holding registers
        error = modbusClientWriteMultipleRegs(&modbusClientContext, 0, 2, regValueW);

        //Check status code
        if(error == NO_ERROR)
        {
            //Debug message
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


        //Close Modbus/TCP connection
        modbusClientDisconnect(&modbusClientContext);

        //End of exception handling block
    } while(0);

    //Release Modbus/TCP client context
    modbusClientDeinit(&modbusClientContext);

    //Return status code
    return error;
}

void userTask(void *param)
{
    //Endless loop
    while(1)
    {
        //USR_SW1 button pressed?
        if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0))
        {
            //Modbus client test routine
            modbusClientTest();

            //Wait for the USR_SW1 button to be released
            while(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0));
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
    NetInterface *interface;
    OsTask *task;
    MacAddr macAddr;

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

    //Initialize kernel
    osInitKernel();

    //Configure debug UART
    debugInit(115200);

    //Start-up message
    TRACE_INFO("\r\n");
    TRACE_INFO("**********************************\r\n");
    TRACE_INFO("*** Modbus client test ***********\r\n");
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

    //Configure the first Ethernet interface
    interface = &netInterface[0];
    //Set interface name
    netSetInterfaceName(interface, "eth0");
    //Set host name
    netSetHostname(interface, "MBClientDemo");
    //Select the relevant network adapter
    netSetDriver(interface, &msp432e4EthDriver);
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

    //struct mq_attr      attr; //original
    //int                 retc; //original

    /* Call driver init functions */
    //Board_initGeneral(); //original

    /* Initialize GPIO */
    //GPIO_init(); //original

    /* Configure UART0 */
    //ConfigureUART(); //original

    /* Configure ADC0 */
    //ConfigureADC0(); //original

    /* Configure the buttons */
    //ConfigureButtons(); //original

    /* Initialize EEPROM to store the CIK */
    //NVSInit(); //original

    //ti_ndk_config_Global_startupFxn(); // orignal

    //retc = CommandTaskInit();
    //if (retc != 0) {
    //    /* pthread_attr_setstacksize() failed */
    //    while (1);
    //} //ORIGINAL

    /* Create RTOS Queue */
    //attr.mq_maxmsg = 3; //original
    //attr.mq_msgsize = 132; //original
    //attr.mq_flags = 0; //original

    /* Create a message queue that will receive data in the cloud task in a
     * non-blocking fashion */
    //mqCloudQueueBlock = mq_open("CloudQueue", O_RDWR | O_CREAT, 0664, &attr);
    //if (mqCloudQueueBlock == ((mqd_t)(-1))) {
    //    /* mq_open() failed */
    //    while (1);
    //} //ORIGINAL

    /* Create a message queue that will send data to cloud task in a
     * non-blocking fashion */
    //mqCloudQueueNoBlock = mq_open("CloudQueue", O_RDWR | O_CREAT | O_NONBLOCK,
    //                          0664, &attr);
    //if (mqCloudQueueNoBlock == ((mqd_t)(-1))) {
    //    /* mq_open() failed */
    //    while (1);
    //} //ORIGINAL

    /* Create a message queue that will receive data in the command task */
    //mqCmdQueueBlock = mq_open("CmdQueue", O_RDWR | O_CREAT, 0664, &attr);
    //if (mqCmdQueueBlock == ((mqd_t)(-1))) {
    //    /* mq_open() failed */
    //    while (1);
    //} //ORIGINAL

    /* Create a message queue that will send data to command task in a
     * non-blocking fashion */
    //mqCmdQueueNoBlock = mq_open("CmdQueue", O_RDWR | O_CREAT | O_NONBLOCK,
    //                            0664, &attr);
    //if (mqCmdQueueNoBlock == ((mqd_t)(-1))) {
    //    /* mq_open() failed */
    //    while (1);
    //} //ORIGINAL

    /* Start the FreeRTOS scheduler */
    //vTaskStartScheduler();
    //osStartKernel();

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

    return (0);
}



//*****************************************************************************
//
//! \brief Application defined malloc failed hook
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
