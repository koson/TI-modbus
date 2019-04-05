/**
 * @file debug.c
 * @brief Debugging facilities
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
#include "fsl_device_registers.h"
#include "fsl_power.h"
#include "fsl_reset.h"
#include "fsl_clock.h"
#include "fsl_iocon.h"
#include "fsl_usart.h"
#include "debug.h"


/**
 * @brief Debug UART initialization
 * @param[in] baudrate UART baudrate
 **/

void debugInit(uint32_t baudrate)
{
   usart_config_t usartConfig;

   //Enable IOCON clock
   CLOCK_EnableClock(kCLOCK_Iocon);

   //Enable GPIO0 clocks
   CLOCK_EnableClock(kCLOCK_Gpio0);

   //Configure P0_29 pin as FC0_RXD_SDA_MOSI
   IOCON_PinMuxSet(IOCON, 0, 29, IOCON_FUNC1 | IOCON_MODE_PULLUP |
      IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);

   //Configure P0_30 pin as FC0_TXD_SCL_MISO
   IOCON_PinMuxSet(IOCON, 0, 30, IOCON_FUNC1 | IOCON_MODE_INACT |
      IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);

   //Set FLEXCOMM0 clock source
   CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0);

   //Reset FLEXCOMM0 peripheral
   RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

   //Get default USART configuration
   USART_GetDefaultConfig(&usartConfig);
   //Adjust baudrate
   usartConfig.baudRate_Bps = baudrate;

   //Configure USART module
   USART_Init(USART0, &usartConfig, CLOCK_GetFreq(kCLOCK_Flexcomm0));
}


/**
 * @brief Display the contents of an array
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @param[in] prepend String to prepend to the left of each line
 * @param[in] data Pointer to the data array
 * @param[in] length Number of bytes to display
 **/

void debugDisplayArray(FILE *stream,
   const char_t *prepend, const void *data, size_t length)
{
   uint_t i;

   for(i = 0; i < length; i++)
   {
      //Beginning of a new line?
      if((i % 16) == 0)
         fprintf(stream, "%s", prepend);
      //Display current data byte
      fprintf(stream, "%02" PRIX8 " ", *((uint8_t *) data + i));
      //End of current line?
      if((i % 16) == 15 || i == (length - 1))
         fprintf(stream, "\r\n");
   }
}


/**
 * @brief Write character to stream
 * @param[in] c The character to be written
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @return On success, the character written is returned. If a writing
 *   error occurs, EOF is returned
 **/

int_t fputc(int_t c, FILE *stream)
{
   //Standard output or error output?
   if(stream == stdout || stream == stderr)
   {
      //Character to be written
      uint8_t ch = c;

      //Transmit data
      USART_WriteBlocking(USART0, &ch, 1);

      //On success, the character written is returned
      return c;
   }
   //Unknown output?
   else
   {
      //If a writing error occurs, EOF is returned
      return EOF;
   }
}
