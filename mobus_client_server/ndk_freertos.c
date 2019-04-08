/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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
 *  ======== ndk_freertos.c ========
 */

#include <stdbool.h>
#include <stdlib.h>

#include <signal.h>
#include <time.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/os/oskern.h>

#include <FreeRTOS.h>
#include <task.h>

/* Compute number of microseconds per tick based on user's kernel settings */
#define NDK_FREERTOS_USEC_PER_TICK ((1000000) / (configTICK_RATE_HZ))

static bool hookOK = false;

/* Socket file descriptor table */
#define MAXSOCKETS 10
uint32_t ti_ndk_socket_max_fd = MAXSOCKETS;
void *ti_ndk_socket_fdtable[MAXSOCKETS];

/* NDK memory manager page size and number of pages [used by mmAlloc()] */
#define RAW_PAGE_SIZE 3072
#define RAW_PAGE_COUNT 6

const int ti_ndk_config_Global_rawPageSize  = RAW_PAGE_SIZE;
const int ti_ndk_config_Global_rawPageCount = RAW_PAGE_COUNT;

/* P.I.T. (page information table) */
#ifdef __ti__
#pragma DATA_SECTION(ti_ndk_config_Global_pit, ".bss:NDK_MMBUFFER");
#pragma DATA_SECTION(ti_ndk_config_Global_pitBuffer, ".bss:NDK_MMBUFFER");
PITENTRY ti_ndk_config_Global_pit[RAW_PAGE_COUNT];
unsigned char ti_ndk_config_Global_pitBuffer[RAW_PAGE_SIZE * RAW_PAGE_COUNT];
#elif defined (__IAR_SYSTEMS_ICC__)
PITENTRY ti_ndk_config_Global_pit[RAW_PAGE_COUNT];
unsigned char ti_ndk_config_Global_pitBuffer[RAW_PAGE_SIZE * RAW_PAGE_COUNT];
#else
PITENTRY ti_ndk_config_Global_pit[RAW_PAGE_COUNT]
        __attribute__ ((section(".bss:NDK_MMBUFFER")));
unsigned char ti_ndk_config_Global_pitBuffer[RAW_PAGE_SIZE * RAW_PAGE_COUNT]
        __attribute__ ((section(".bss:NDK_MMBUFFER")));
#endif

/* Memory bucket sizes */
#define SMALLEST 48
#define LARGEST (RAW_PAGE_SIZE)

const int ti_ndk_config_Global_smallest = SMALLEST;
const int ti_ndk_config_Global_largest  = LARGEST;

/* Memory Slot Tracking */
uint32_t ti_ndk_config_Global_Id2Size[] =
        {SMALLEST, 96, 128, 256, 512, 1536, LARGEST};

/*
 *  Local Packet Buffer Pool Definitions
 *
 *  The below variables/defines are used to override the defaults that are set
 *  in the Packet Buffer Manager (PBM) file src/stack/pbm/pbm_data.c
 */

/*
 *  Number of buffers in PBM packet buffer free pool
 *
 *  The number of buffers in the free pool can have a significant effect
 *  on performance, especially in UDP packet loss. Increasing this number
 *  will increase the size of the static packet pool use for both sending
 *  and receiving packets.
 */
#define PKT_NUM_FRAMEBUF 10

/* Size of Ethernet frame buffer */
#define PKT_SIZE_FRAMEBUF   1536

const int ti_ndk_config_Global_numFrameBuf = PKT_NUM_FRAMEBUF;
const int ti_ndk_config_Global_sizeFrameBuf = PKT_SIZE_FRAMEBUF;

/* Data space for packet buffers */
#ifdef __ti__
#pragma DATA_ALIGN(ti_ndk_config_Global_pBufMem, 128);
#pragma DATA_SECTION(ti_ndk_config_Global_pBufMem, ".bss:NDK_PACKETMEM");
unsigned char
        ti_ndk_config_Global_pBufMem[PKT_NUM_FRAMEBUF * PKT_SIZE_FRAMEBUF];
#elif defined (__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 128
unsigned char
        ti_ndk_config_Global_pBufMem[PKT_NUM_FRAMEBUF * PKT_SIZE_FRAMEBUF];
#else
unsigned char ti_ndk_config_Global_pBufMem[PKT_NUM_FRAMEBUF * PKT_SIZE_FRAMEBUF]
        __attribute__ ((aligned(128), section(".bss:NDK_PACKETMEM")));
#endif

#ifdef __ti__
#pragma DATA_ALIGN(ti_ndk_config_Global_pHdrMem, 128);
#pragma DATA_SECTION(ti_ndk_config_Global_pHdrMem, ".bss:NDK_PACKETMEM");
unsigned char ti_ndk_config_Global_pHdrMem[PKT_NUM_FRAMEBUF * sizeof(PBM_Pkt)];
#elif defined (__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 128
unsigned char ti_ndk_config_Global_pHdrMem[PKT_NUM_FRAMEBUF * sizeof(PBM_Pkt)];
#else
unsigned char ti_ndk_config_Global_pHdrMem[PKT_NUM_FRAMEBUF * sizeof(PBM_Pkt)]
        __attribute__ ((aligned(128), section(".bss:NDK_PACKETMEM")));
#endif

/*
 * ======== _TaskInit ========
 * Called from NETCTRL init
 */
void _TaskInit()
{
    hookOK = true;
}

/*
 * ======== _TaskShutdown ========
 * Called from NETCTRL shutdown
 */
void _TaskShutdown()
{
    hookOK = false;
}

/*
 * ======== TaskBlock ========
 * Block a task
 */
void TaskBlock(void *h)
{
    vTaskSuspend(NULL);
}

/*
 * ======== ti_ndk_os_threadStub ========
 * Wrapper function used by TaskCreate(). This function conforms to the
 * FreeRTOS Task function signature:
 *
 *     void vFunction(void *pvParameters)
 *
 * It is needed in order to pass the Task arguments that were passed in the
 * TaskCreate() call into the user's actual Task function. It also ensures the
 * Task exits properly, as required by FreeRTOS.
 */
static void ti_ndk_os_threadStub(void *threadArgs)
{
    ti_ndk_os_TaskArgs *args = (ti_ndk_os_TaskArgs *)threadArgs;

    /* open the file descriptor session for the user here */
    fdOpenSession(TaskSelf());

    /* call user thread function */
    args->arg0(args->arg1, args->arg2);

    /* close the file descriptor session for the user here */
    fdCloseSession(TaskSelf());

    /* User thread has exited, free the Task arguments */
    free(args);

    /* Exit task. FreeRTOS Tasks cannot simply fall out of their functions: */
    TaskExit();
}

/*
 * ======== TaskCreate ========
 * Create a task
 */
void *TaskCreate(void (*pFun)(), char *Name, int Priority, uint32_t StackSize,
        uintptr_t Arg1, uintptr_t Arg2, uintptr_t argReserved)
{
    BaseType_t status;
    TaskHandle_t task = NULL;
    ti_ndk_os_TaskArgs *taskArgs;

    taskArgs = malloc(sizeof(ti_ndk_os_TaskArgs));
    if (!taskArgs) {
        /* Error: couldn't alloc task function args */
        return (NULL);
    }
    taskArgs->arg0 = pFun;
    taskArgs->arg1 = Arg1;
    taskArgs->arg2 = Arg2;

    status = xTaskCreate((TaskFunction_t)ti_ndk_os_threadStub, Name,
            StackSize / sizeof(portSTACK_TYPE), (void *)taskArgs,
            (UBaseType_t)Priority, &task);

    if (status != pdPASS) {
        free(taskArgs);
        return (NULL);
    }

    return ((void *)task);
}

/*
 * ======== TaskDestroy ========
 * Destroy a task
 */
void TaskDestroy(void *h)
{
    if (h == TaskSelf()) {
        vTaskDelete(NULL);
    }
    else {
        vTaskDelete((TaskHandle_t)h);
    }
}

/*
 * ======== TaskSetEnv ========
 * Sets the task's Environment pointer
 */
void TaskSetEnv(void *h, int Slot, void *hEnv)
{
    if (Slot) {
        return;
    }

    if (!hookOK) {
        /*
         * DbgPrintf() will not output this message for FreeRTOS currently.
         * However, it will still shut down the stack if the stack has been
         * configured to do so for the DBG_ERROR level
         */
        DbgPrintf(DBG_ERROR,"TaskSetEnv: FATAL: NDK not initialized");
        return;
    }

    vTaskSetThreadLocalStoragePointer((TaskHandle_t)h, NDK_TLS_INDEX, hEnv);
}

/*
 * ======== TaskGetEnv ========
 * Gets the task's Environment pointer
 */
void *TaskGetEnv(void *h, int Slot)
{
    if (Slot) {
        return (0);
    }

    if (!hookOK) {
        /*
         * DbgPrintf() will not output this message for FreeRTOS currently.
         * However, it will still shut down the stack if the stack has been
         * configured to do so for the DBG_ERROR level
         */
        DbgPrintf(DBG_ERROR,"TaskGetEnv: FATAL: NDK not initialized");
        return (0);
    }

    return ((void *)pvTaskGetThreadLocalStoragePointer((TaskHandle_t)h,
            NDK_TLS_INDEX));
}

/*
 * ======== TaskExit ========
 * Exits and destroys a task
 */
void TaskExit()
{
    /*
     * FreeRTOS tasks must "self delete." They cannot fall out of the
     * Task function without this code, otherwise a run time error will occur.
     */
    vTaskDelete(NULL);
}

/*
 * ======== TaskGetPri ========
 * Get a task's priority
 */
int TaskGetPri(void *h)
{
    return ((int)uxTaskPriorityGet((TaskHandle_t)h));
}

/*
 * ======== TaskSetPri ========
 * Set a task's priority
 */
int TaskSetPri(void *h, int priority)
{
    int oldPri = TaskGetPri(h);
    vTaskPrioritySet((TaskHandle_t)h, (UBaseType_t)priority);
    return (oldPri);
}

/*
 * ======== TaskSelf ========
 * Return handle of task itself
 */
void *TaskSelf()
{
    return ((void *)xTaskGetCurrentTaskHandle());
}

/*
 * ======== TaskSleep ========
 * Put a task into sleep
 */
void TaskSleep(uint32_t delay)
{
    uint32_t secs;
    uint32_t remMs;
    uint32_t remUs;
    uint32_t ticks;
    uint32_t remTicks;

    secs = delay / 1000; /* Whole number of seconds in delay */
    remMs = delay - (secs * 1000); /* Remainder of milliseconds in delay */
    remUs = remMs * 1000;

    ticks = secs * configTICK_RATE_HZ;
    remTicks =
        (remUs + NDK_FREERTOS_USEC_PER_TICK - 1) / NDK_FREERTOS_USEC_PER_TICK;

    ticks += remTicks;

    vTaskDelay((TickType_t)ticks);
}

/*
 * ======== TaskYield ========
 * Yield task
 */
void TaskYield()
{
     taskYIELD();
}

/* Our NETCTRL callback functions */
static void networkOpen();
static void networkClose();
static void networkIPAddr(uint32_t IPAddr, uint32_t IfIdx, uint32_t fAdd);
static char *hostName = "tisoc";

extern void llTimerTick();

/*
 *  ======== networkOpen ========
 *  This function is called after the configuration has booted
 */
static void networkOpen()
{
    extern void netOpenHook();

    /* call user defined network open hook */
    netOpenHook();
}

/*
 *  ======== networkClose ========
 *  This function is called when the network is shutting down,
 *  or when it no longer has any IP addresses assigned to it.
 */
static void networkClose()
{
    /* call user defined network close hook */
}

/*
 *  ======== networkIPAddr ========
 *  This function is called whenever an IP address binding is
 *  added or removed from the system.
 */
static void networkIPAddr(uint32_t IPAddr, uint32_t IfIdx, uint32_t fAdd)
{
    extern void netIPAddrHook();

    /* call user defined network IP address hook */
    netIPAddrHook(IPAddr, IfIdx, fAdd);
}

/*
 *  ======== initTcp ========
 *  Configure the stack's TCP settings
 */
static void initTcp(void *hCfg)
{
    int transmitBufSize = 1024;
    int receiveBufSize = 1024;
    int receiveBufLimit = 2048;

    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPTXBUF, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&transmitBufSize, NULL);
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXBUF, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&receiveBufSize, NULL);
    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXLIMIT, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&receiveBufLimit, NULL);
}

/*
 *  ======== initIp ========
 *  Configure the stack's IP settings
 */
static void initIp(void *hCfg)
{
    CI_SERVICE_DHCPC dhcpc;
    unsigned char DHCP_OPTIONS[] = { DHCPOPT_SUBNET_MASK };

    /* Add global hostname to hCfg (to be claimed in all connected domains) */
    CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0,
            strlen(hostName), (unsigned char *)hostName, NULL);

    /* Use DHCP to obtain IP address on interface 1 */
    memset(&dhcpc, 0, sizeof(dhcpc));
    dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
    dhcpc.cisargs.IfIdx  = 1;
    dhcpc.param.pOptions = DHCP_OPTIONS;
    dhcpc.param.len = 1;
    CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0,
            sizeof(dhcpc), (unsigned char *)&dhcpc, NULL);
}

/*
 *  ======== initUdp ========
 *  Configure the stack's UDP settings
 */
void initUdp(void *hCfg)
{
    int receiveBufSize = 2048;

    CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKUDPRXLIMIT, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&receiveBufSize, NULL);
}

/*
 *  ======== ndkStackThread ========
 *  NDK stack's main thread function
 */
static void ndkStackThread(void *argUnused)
{
    void *hCfg;
    int rc;
    timer_t ndkHeartBeat;
    struct sigevent sev;
    struct itimerspec its;
    struct itimerspec oldIts;
    int ndkHeartBeatCount = 0;

    /* create the NDK timer tick */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_value.sival_ptr = &ndkHeartBeatCount;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_notify_function = &llTimerTick;

    rc = timer_create(CLOCK_MONOTONIC, &sev, &ndkHeartBeat);
    if (rc != 0) {
        while(1);
    }

    /* start the NDK 100ms timer */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 100000000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000;

    rc = timer_settime(ndkHeartBeat, 0, &its, NULL);
    if (rc != 0) {
        while(1);
    }

    rc = NC_SystemOpen(NC_PRIORITY_LOW, NC_OPMODE_INTERRUPT);
    if (rc) {
        while(1);
    }

    /* create and build the system configuration from scratch. */
    hCfg = CfgNew();
    if (!hCfg) {
        goto main_exit;
    }

    /* IP, TCP, and UDP config */
    initIp(hCfg);
    initTcp(hCfg);
    initUdp(hCfg);

    /* config low priority tasks stack size */
    rc = 2048;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKLOW, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&rc, NULL);

    /* config norm priority tasks stack size */
    rc = 2048;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKNORM, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&rc, NULL);

    /* config high priority tasks stack size */
    rc = 2048;
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKHIGH, CFG_ADDMODE_UNIQUE,
            sizeof(uint32_t), (unsigned char *)&rc, NULL);

    do
    {
        rc = NC_NetStart(hCfg, networkOpen, networkClose, networkIPAddr);
    } while(rc > 0);

    /* Shut down the stack */
    CfgFree(hCfg);

main_exit:
    NC_SystemClose();

    /* stop and delete the NDK heartbeat */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;

    rc = timer_settime(ndkHeartBeat, 0, &its, &oldIts);

    rc = timer_delete(ndkHeartBeat);
}

/*
 * ======== ti_ndk_config_Global_startupFxn ========
 * Called to start up the NDK. In BIOS, this can be called as a BIOS startup
 * function, or from main(). In FreeRTOS, this should be called from main().
 */
void ti_ndk_config_Global_startupFxn()
{
    BaseType_t status;
    TaskHandle_t ndkThread = NULL;

    status = xTaskCreate((TaskFunction_t)ndkStackThread, NULL,
            2048 / sizeof(portSTACK_TYPE), NULL, (UBaseType_t)5, &ndkThread);

    if (status != pdPASS) {
        /* Error: could not create NDK stack thread */
        while(1);
    }
}
