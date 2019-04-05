/**
 * @file uart_driver.c
 * @brief UART driver
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
#include "stm32l4xx.h"
#include "core/net.h"
#include "ppp/ppp_hdlc.h"
#include "uart_driver.h"
#include "debug.h"

//Enable hardware flow control
#define APP_UART_HW_FLOW_CTRL ENABLED

//Variable declaration
static UART_HandleTypeDef UART_Handle;


/**
 * @brief UART driver
 **/

const UartDriver uartDriver =
{
   uartInit,
   uartEnableIrq,
   uartDisableIrq,
   uartStartTx
};


/**
 * @brief UART configuration
 * @return Error code
 **/

error_t uartInit(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;

   //Debug message
   TRACE_INFO("Initializing UART...\r\n");

   //Enable PWR clock
   __HAL_RCC_PWR_CLK_ENABLE();
   //Enable VDDIO2 power supply for pins PG[15:2]
   HAL_PWREx_EnableVddIO2();

   //Enable GPIOB and GPIOG clocks
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOG_CLK_ENABLE();

   //Enable USART1 clock
   __HAL_RCC_USART1_CLK_ENABLE();

   //Configure USART1_TX (PB6)
   GPIO_InitStructure.Pin = GPIO_PIN_6;
   GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStructure.Pull = GPIO_NOPULL;
   GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
   GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

   //Configure USART1_RX (PG10)
   GPIO_InitStructure.Pin = GPIO_PIN_10;
   GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStructure.Pull = GPIO_PULLUP;
   GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
   GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

#if (APP_UART_HW_FLOW_CTRL == ENABLED)
   //Configure USART1_CTS (PG11) and USART1_RTS (PG12)
   GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12;
   GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStructure.Pull = GPIO_NOPULL;
   GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
   GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

   //Configure USART1
   UART_Handle.Instance = USART1;
   UART_Handle.Init.BaudRate = 115200;
   UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
   UART_Handle.Init.StopBits = UART_STOPBITS_1;
   UART_Handle.Init.Parity = UART_PARITY_NONE;
   UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
   UART_Handle.Init.Mode = UART_MODE_TX_RX;
   HAL_UART_Init(&UART_Handle);

#else
   //Configure USART1_CTS (PG11) as an input
   GPIO_InitStructure.Pin = GPIO_PIN_11;
   GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
   GPIO_InitStructure.Pull = GPIO_PULLUP;
   GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

   //Configure USART1_RTS (PG12) as an output
   GPIO_InitStructure.Pin = GPIO_PIN_12;
   GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStructure.Pull = GPIO_NOPULL;
   GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
   HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

   //Assert USART1_RTS
   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);

   //Configure USART1
   UART_Handle.Instance = USART1;
   UART_Handle.Init.BaudRate = 115200;
   UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
   UART_Handle.Init.StopBits = UART_STOPBITS_1;
   UART_Handle.Init.Parity = UART_PARITY_NONE;
   UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
   UART_Handle.Init.Mode = UART_MODE_TX_RX;
   HAL_UART_Init(&UART_Handle);
#endif

   //Configure USART1 interrupts
   __HAL_UART_ENABLE_IT(&UART_Handle, UART_IT_TXE);
   __HAL_UART_ENABLE_IT(&UART_Handle, UART_IT_RXNE);

   //Set priority grouping (4 bits for pre-emption priority, no bits
   //for subpriority)
   NVIC_SetPriorityGrouping(3);

   //Configure Ethernet interrupt priority
   NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(3, 12, 0));

   //Enable USART1
   __HAL_UART_ENABLE(&UART_Handle);

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Enable UART interrupts
 **/

void uartEnableIrq(void)
{
   //Enable USART1 interrupts
   NVIC_EnableIRQ(USART1_IRQn);
}


/**
 * @brief Disable UART interrupts
 **/

void uartDisableIrq(void)
{
   //Disable USART1 interrupt
   NVIC_DisableIRQ(USART1_IRQn);
}


/**
 * @brief Start transmission
 **/

void uartStartTx(void)
{
   //Enable TXE interrupt
   __HAL_UART_ENABLE_IT(&UART_Handle, UART_IT_TXE);
}


/**
 * @brief UART interrupt handler
 **/

void USART1_IRQHandler(void)
{
   int_t c;
   bool_t flag;
   NetInterface *interface;

   //Enter interrupt service routine
   osEnterIsr();

   //This flag will be set if a higher priority task must be woken
   flag = FALSE;

   //Point to the PPP network interface
   interface = &netInterface[0];

   //TXE interrupt?
   if(__HAL_UART_GET_FLAG(&UART_Handle, UART_FLAG_TXE) != RESET &&
      __HAL_UART_GET_IT_SOURCE(&UART_Handle, UART_IT_TXE) != RESET)
   {
      //Get next character
      flag |= pppHdlcDriverReadTxQueue(interface, &c);

      //Valid character read?
      if(c != EOF)
      {
         //Send data byte
         UART_Handle.Instance->TDR = c;
      }
      else
      {
         //Disable TXE interrupt
         __HAL_UART_DISABLE_IT(&UART_Handle, UART_IT_TXE);
      }
   }

   //RXNE interrupt?
   if(__HAL_UART_GET_FLAG(&UART_Handle, UART_FLAG_RXNE) != RESET &&
      __HAL_UART_GET_IT_SOURCE(&UART_Handle, UART_IT_RXNE) != RESET)
   {
      //Read data byte
      c = UART_Handle.Instance->RDR;
      //Process incoming character
      flag |= pppHdlcDriverWriteRxQueue(interface, c);
   }

   //ORE interrupt?
   if(__HAL_UART_GET_FLAG(&UART_Handle, UART_FLAG_ORE) != RESET &&
      __HAL_UART_GET_IT_SOURCE(&UART_Handle, UART_IT_RXNE) != RESET)
   {
      //Clear ORE interrupt flag
      __HAL_UART_CLEAR_OREFLAG(&UART_Handle);
   }

   //Leave interrupt service routine
   osExitIsr(flag);
}
