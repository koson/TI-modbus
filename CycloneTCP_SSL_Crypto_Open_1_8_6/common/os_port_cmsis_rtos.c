/**
 * @file os_port_cmsis_rtos.c
 * @brief RTOS abstraction layer (CMSIS-RTOS)
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

//Switch to the appropriate trace level
#define TRACE_LEVEL TRACE_LEVEL_OFF

//Dependencies
#include <stdio.h>
#include <stdlib.h>
#include "os_port.h"
#include "os_port_cmsis_rtos.h"
#include "debug.h"


/**
 * @brief Kernel initialization
 **/

void osInitKernel(void)
{
//Check CMSIS-RTOS API version
#if (osCMSIS >= 0x10001)
   //Initialize the kernel
   osKernelInitialize();
#endif
}


/**
 * @brief Start kernel
 **/

void osStartKernel(void)
{
//Check CMSIS-RTOS API version
#if (osCMSIS >= 0x10001)
   //Start the kernel
   osKernelStart();
#else
   //Start the kernel
   osKernelStart(NULL, NULL);
#endif
}


/**
 * @brief Create a new task
 * @param[in] name A name identifying the task
 * @param[in] taskCode Pointer to the task entry function
 * @param[in] param A pointer to a variable to be passed to the task
 * @param[in] stackSize The initial size of the stack, in words
 * @param[in] priority The priority at which the task should run
 * @return If the function succeeds, the return value is a pointer to the
 *   new task. If the function fails, the return value is NULL
 **/

OsTask *osCreateTask(const char_t *name, OsTaskCode taskCode,
   void *param, size_t stackSize, int_t priority)
{
   osThreadId threadId;
   osThreadDef_t threadDef;

#if defined(osCMSIS_RTX)
   threadDef.pthread = (os_pthread) taskCode;
   threadDef.tpriority = (osPriority) priority;
   threadDef.instances = 1;
   threadDef.stacksize = stackSize * sizeof(uint_t);
#elif defined(osCMSIS_FreeRTOS)
   threadDef.pthread = (os_pthread) taskCode;
   threadDef.attr.name = name;
   threadDef.attr.attr_bits = 0;
   threadDef.attr.cb_mem = NULL;
   threadDef.attr.cb_size = 0;
   threadDef.attr.stack_mem = NULL;
   threadDef.attr.stack_size = stackSize * sizeof(uint_t);
   threadDef.attr.priority = (osPriority_t) priority;
   threadDef.attr.tz_module = 0;
   threadDef.attr.reserved = 0;
#else
   threadDef.name = (char_t *) name;
   threadDef.pthread = (os_pthread) taskCode;
   threadDef.tpriority = (osPriority) priority;
   threadDef.instances = 1;
   threadDef.stacksize = stackSize;
#endif

   //Create a new thread
   threadId = osThreadCreate(&threadDef, param);
   //Return a handle to the newly created thread
   return (OsTask *) threadId;
}


/**
 * @brief Delete a task
 * @param[in] task Pointer to the task to be deleted
 **/

void osDeleteTask(OsTask *task)
{
#if defined(osCMSIS_RTX)
   //Delete the specified thread
   osThreadTerminate((osThreadId) task);
#elif defined(osCMSIS_FreeRTOS)
   //Delete the specified thread
   if(task == NULL)
      osThreadExit();
   else
      osThreadTerminate((osThreadId_t) task);
#else
   //Delete the specified thread
   osThreadTerminate((osThreadId) task);
#endif
}


/**
 * @brief Delay routine
 * @param[in] delay Amount of time for which the calling task should block
 **/

void osDelayTask(systime_t delay)
{
   //Delay the thread for the specified duration
   osDelay(delay);
}


/**
 * @brief Yield control to the next task
 **/

void osSwitchTask(void)
{
   //Force a context switch
   osThreadYield();
}


/**
 * @brief Suspend scheduler activity
 **/

void osSuspendAllTasks(void)
{
#if defined(osCMSIS_RTX) || defined(osCMSIS_FreeRTOS)
   //Not implemented
#else
   //Make sure the operating system is running
   if(osKernelRunning())
   {
      //Suspend all threads
      osThreadSuspendAll();
   }
#endif
}


/**
 * @brief Resume scheduler activity
 **/

void osResumeAllTasks(void)
{
#if defined(osCMSIS_RTX) || defined(osCMSIS_FreeRTOS)
   //Not implemented
#else
   //Make sure the operating system is running
   if(osKernelRunning())
   {
      //Resume all threads
      osThreadResumeAll();
   }
#endif
}


/**
 * @brief Create an event object
 * @param[in] event Pointer to the event object
 * @return The function returns TRUE if the event object was successfully
 *   created. Otherwise, FALSE is returned
 **/

bool_t osCreateEvent(OsEvent *event)
{
   osSemaphoreDef_t semaphoreDef;

#if defined(osCMSIS_RTX)
   semaphoreDef.semaphore = event->cb;
#elif defined(osCMSIS_FreeRTOS)
   semaphoreDef.name = NULL;
   semaphoreDef.attr_bits = 0;
   semaphoreDef.cb_mem = NULL;
   semaphoreDef.cb_size = 0;
#else
   semaphoreDef.dummy = 0;
#endif

   //Create a binary semaphore object
   event->id = osSemaphoreCreate(&semaphoreDef, 1);

   //Check whether the returned semaphore ID is valid
   if(event->id != NULL)
   {
      //Force the specified event to the nonsignaled state
      osSemaphoreWait(event->id, 0);
      //Event successfully created
      return TRUE;
   }
   else
   {
      //Failed to create event object
      return FALSE;
   }
}


/**
 * @brief Delete an event object
 * @param[in] event Pointer to the event object
 **/

void osDeleteEvent(OsEvent *event)
{
   //Make sure the semaphore ID is valid
   if(event->id != NULL)
   {
      //Properly dispose the event object
      osSemaphoreDelete(event->id);
   }
}


/**
 * @brief Set the specified event object to the signaled state
 * @param[in] event Pointer to the event object
 **/

void osSetEvent(OsEvent *event)
{
   //Set the specified event to the signaled state
   osSemaphoreRelease(event->id);
}


/**
 * @brief Set the specified event object to the nonsignaled state
 * @param[in] event Pointer to the event object
 **/

void osResetEvent(OsEvent *event)
{
#if defined(osCMSIS_RTX)
   //Force the specified event to the nonsignaled state
   while(osSemaphoreWait(event->id, 0) > 0);
#else
   //Force the specified event to the nonsignaled state
   osSemaphoreWait(event->id, 0);
#endif
}


/**
 * @brief Wait until the specified event is in the signaled state
 * @param[in] event Pointer to the event object
 * @param[in] timeout Timeout interval
 * @return The function returns TRUE if the state of the specified object is
 *   signaled. FALSE is returned if the timeout interval elapsed
 **/

bool_t osWaitForEvent(OsEvent *event, systime_t timeout)
{
   int32_t ret;

   //Wait until the specified event is in the signaled
   //state or the timeout interval elapses
   if(timeout == INFINITE_DELAY)
   {
      //Infinite timeout period
      ret = osSemaphoreWait(event->id, osWaitForever);
   }
   else
   {
#if defined(osCMSIS_RTX)
      systime_t n;

      //Loop until the assigned time period has elapsed
      do
      {
         //Limit the timeout value
         n = MIN(timeout, 10000);
         //Wait for the specified time interval
         ret = osSemaphoreWait(event->id, n);
         //Decrement timeout value
         timeout -= n;

         //Check timeout value
      } while(ret == 0 && timeout > 0);
#else
      //Wait for the specified time interval
      ret = osSemaphoreWait(event->id, timeout);
#endif
   }

#if defined(osCMSIS_RTX)
   //Check return value
   if(ret > 0)
   {
      //Force the event back to the nonsignaled state
      while(osSemaphoreWait(event->id, 0) > 0);

      //The specified event is in the signaled state
      return TRUE;
   }
   else
   {
      //The timeout interval elapsed
      return FALSE;
   }
#elif defined(osCMSIS_FreeRTOS)
   //Check return value
   if(ret > 0)
      return TRUE;
   else
      return FALSE;
#else
   //Check return value
   if(ret == osOK)
      return TRUE;
   else
      return FALSE;
#endif
}


/**
 * @brief Set an event object to the signaled state from an interrupt service routine
 * @param[in] event Pointer to the event object
 * @return TRUE if setting the event to signaled state caused a task to unblock
 *   and the unblocked task has a priority higher than the currently running task
 **/

bool_t osSetEventFromIsr(OsEvent *event)
{
   //Set the specified event to the signaled state
   osSemaphoreRelease(event->id);

   //The return value is not relevant
   return FALSE;
}


/**
 * @brief Create a semaphore object
 * @param[in] semaphore Pointer to the semaphore object
 * @param[in] count The maximum count for the semaphore object. This value
 *   must be greater than zero
 * @return The function returns TRUE if the semaphore was successfully
 *   created. Otherwise, FALSE is returned
 **/

bool_t osCreateSemaphore(OsSemaphore *semaphore, uint_t count)
{
   osSemaphoreDef_t semaphoreDef;

#if defined(osCMSIS_RTX)
   semaphoreDef.semaphore = semaphore->cb;
#elif defined(osCMSIS_FreeRTOS)
   semaphoreDef.name = NULL;
   semaphoreDef.attr_bits = 0;
   semaphoreDef.cb_mem = NULL;
   semaphoreDef.cb_size = 0;
#else
   semaphoreDef.dummy = 0;
#endif

   //Create a semaphore object
   semaphore->id = osSemaphoreCreate(&semaphoreDef, count);

   //Check whether the returned semaphore ID is valid
   if(semaphore->id != NULL)
      return TRUE;
   else
      return FALSE;
}


/**
 * @brief Delete a semaphore object
 * @param[in] semaphore Pointer to the semaphore object
 **/

void osDeleteSemaphore(OsSemaphore *semaphore)
{
   //Make sure the semaphore ID is valid
   if(semaphore->id != NULL)
   {
      //Properly dispose the specified semaphore
      osSemaphoreDelete(semaphore->id);
   }
}


/**
 * @brief Wait for the specified semaphore to be available
 * @param[in] semaphore Pointer to the semaphore object
 * @param[in] timeout Timeout interval
 * @return The function returns TRUE if the semaphore is available. FALSE is
 *   returned if the timeout interval elapsed
 **/

bool_t osWaitForSemaphore(OsSemaphore *semaphore, systime_t timeout)
{
   int32_t ret;

   //Wait until the semaphore is available or the timeout interval elapses
   if(timeout == INFINITE_DELAY)
   {
      //Infinite timeout period
      ret = osSemaphoreWait(semaphore->id, osWaitForever);
   }
   else
   {
#if defined(osCMSIS_RTX)
      systime_t n;

      //Loop until the assigned time period has elapsed
      do
      {
         //Limit the timeout value
         n = MIN(timeout, 10000);
         //Wait for the specified time interval
         ret = osSemaphoreWait(semaphore->id, n);
         //Decrement timeout value
         timeout -= n;

         //Check timeout value
      } while(ret == 0 && timeout > 0);
#else
      //Wait for the specified time interval
      ret = osSemaphoreWait(semaphore->id, timeout);
#endif
   }

#if defined(osCMSIS_RTX) || defined(osCMSIS_FreeRTOS)
   //Check return value
   if(ret > 0)
      return TRUE;
   else
      return FALSE;
#else
   //Check return value
   if(ret == osOK)
      return TRUE;
   else
      return FALSE;
#endif
}


/**
 * @brief Release the specified semaphore object
 * @param[in] semaphore Pointer to the semaphore object
 **/

void osReleaseSemaphore(OsSemaphore *semaphore)
{
   //Release the semaphore
   osSemaphoreRelease(semaphore->id);
}


/**
 * @brief Create a mutex object
 * @param[in] mutex Pointer to the mutex object
 * @return The function returns TRUE if the mutex was successfully
 *   created. Otherwise, FALSE is returned
 **/

bool_t osCreateMutex(OsMutex *mutex)
{
   osMutexDef_t mutexDef;

#if defined(osCMSIS_RTX)
   mutexDef.mutex = mutex->cb;
#elif defined(osCMSIS_FreeRTOS)
   mutexDef.name = NULL;
   mutexDef.attr_bits = 0;
   mutexDef.cb_mem = NULL;
   mutexDef.cb_size = 0;
#else
   mutexDef.dummy = 0;
#endif

   //Create a mutex object
   mutex->id = osMutexCreate(&mutexDef);

   //Check whether the returned mutex ID is valid
   if(mutex->id != NULL)
      return TRUE;
   else
      return FALSE;
}


/**
 * @brief Delete a mutex object
 * @param[in] mutex Pointer to the mutex object
 **/

void osDeleteMutex(OsMutex *mutex)
{
   //Make sure the mutex ID is valid
   if(mutex->id != NULL)
   {
      //Properly dispose the specified mutex
      osMutexDelete(mutex->id);
   }
}


/**
 * @brief Acquire ownership of the specified mutex object
 * @param[in] mutex Pointer to the mutex object
 **/

void osAcquireMutex(OsMutex *mutex)
{
   //Obtain ownership of the mutex object
   osMutexWait(mutex->id, osWaitForever);
}


/**
 * @brief Release ownership of the specified mutex object
 * @param[in] mutex Pointer to the mutex object
 **/

void osReleaseMutex(OsMutex *mutex)
{
   //Release ownership of the mutex object
   osMutexRelease(mutex->id);
}


/**
 * @brief Retrieve system time
 * @return Number of milliseconds elapsed since the system was last started
 **/

systime_t osGetSystemTime(void)
{
   systime_t time;

#if defined(osCMSIS_RTX)
   //Forward function declaration
   extern uint32_t rt_time_get(void);

   //Get current tick count
   time = rt_time_get();
#elif defined(osCMSIS_FreeRTOS)
   time = osKernelGetTickCount();
#else
   //Get current tick count
   time = osKernelSysTick();
#endif

   //Convert system ticks to milliseconds
   return OS_SYSTICKS_TO_MS(time);
}


/**
 * @brief Allocate a memory block
 * @param[in] size Bytes to allocate
 * @return A pointer to the allocated memory block or NULL if
 *   there is insufficient memory available
 **/

void *osAllocMem(size_t size)
{
   void *p;

   //Enter critical section
   osSuspendAllTasks();
   //Allocate a memory block
   p = malloc(size);
   //Leave critical section
   osResumeAllTasks();

   //Debug message
   TRACE_DEBUG("Allocating %u bytes at 0x%08X\r\n", size, (uint_t) p);

   //Return a pointer to the newly allocated memory block
   return p;
}


/**
 * @brief Release a previously allocated memory block
 * @param[in] p Previously allocated memory block to be freed
 **/

void osFreeMem(void *p)
{
   //Make sure the pointer is valid
   if(p != NULL)
   {
      //Debug message
      TRACE_DEBUG("Freeing memory at 0x%08X\r\n", (uint_t) p);

      //Enter critical section
      osSuspendAllTasks();
      //Free memory block
      free(p);
      //Leave critical section
      osResumeAllTasks();
   }
}
