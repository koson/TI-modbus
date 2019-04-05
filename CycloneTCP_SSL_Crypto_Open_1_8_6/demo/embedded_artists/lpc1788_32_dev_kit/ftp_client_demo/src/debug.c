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
#include "lpc177x_8x.h"
#include "debug.h"


/**
 * @brief Debug UART initialization
 * @param[in] baudrate UART baudrate
 **/

void debugInit(uint32_t baudrate)
{
   uint32_t pclk;

   //Power up GPIO
   LPC_SC->PCONP |= PCONP_PCGPIO;
   //Power up UART2
   LPC_SC->PCONP |= PCONP_PCUART2;

   //Configure P0.10 (U2_TX) and P0.11 (U2_RX)
   LPC_IOCON->P0_10 = IOCON_MODE_PULL_UP | IOCON_FUNC_1;
   LPC_IOCON->P0_11 = IOCON_MODE_PULL_UP | IOCON_FUNC_1;

   //Check the UART0 peripheral clock
   switch(LPC_SC->PCLKSEL & PCLKSEL_PCLKDIV)
   {
   case PCLKSEL_PCLKDIV_1:
      pclk = SystemCoreClock;
      break;
   case PCLKSEL_PCLKDIV_2:
      pclk = SystemCoreClock / 2;
      break;
   case PCLKSEL_PCLKDIV_3:
      pclk = SystemCoreClock / 3;
      break;
   default:
      pclk = SystemCoreClock / 4;
      break;
   }

   //Configure UART0 (8 bits, no parity, 1 stop bit)
   LPC_UART2->LCR = LCR_DLAB | LCR_WORD_LENGTH_SELECT_8;
   //Set baudrate
   LPC_UART2->DLM = MSB(pclk / 16 / baudrate);
   LPC_UART2->DLL = LSB(pclk / 16 / baudrate);
   //Clear DLAB
   LPC_UART2->LCR &= ~LCR_DLAB;

   //Enable and reset FIFOs
   LPC_UART2->FCR = FCR_TX_FIFO_RESET | FCR_RX_FIFO_RESET | FCR_FIFO_ENABLE;
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
      //Wait for the transmitter to be ready
      while(!(LPC_UART2->LSR & LSR_THRE));
      //Send character
      LPC_UART2->THR = c;
      //Wait for the transfer to complete
      while(!(LPC_UART2->LSR & LSR_TEMT));

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
