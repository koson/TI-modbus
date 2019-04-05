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
#include "samg55.h"
#include "debug.h"


/**
 * @brief Debug UART initialization
 * @param[in] baudrate UART baudrate
 **/

void debugInit(uint32_t baudrate)
{
   //Enable PIO peripheral clock
   PMC->PMC_PCER0 = (1 << ID_PIOA);
   //Enable FLEXCOM7 peripheral clock
   PMC->PMC_PCER0 = (1 << ID_FLEXCOM7);

   //Set FLEXCOM operation mode
   FLEXCOM7->FLEXCOM_MR = FLEXCOM_MR_OPMODE_USART;

   //Disable interrupts on TXD and RXD pins
   PIOA->PIO_IDR = PIO_PA28B_TXD7 | PIO_PA27B_RXD7;
   //No pull-up resistors
   PIOA->PIO_PUDR = PIO_PA28B_TXD7 | PIO_PA27B_RXD7;
   //Assign corresponding pins to Peripheral B function
   PIOA->PIO_ABCDSR[0] |= PIO_PA28B_TXD7 | PIO_PA27B_RXD7;
   PIOA->PIO_ABCDSR[1] &= ~(PIO_PA28B_TXD7 | PIO_PA27B_RXD7);
   //Disable the PIO from controlling the corresponding pins
   PIOA->PIO_PDR = PIO_PA28B_TXD7 | PIO_PA27B_RXD7;

   //Reset transmitter and receiver
   USART7->US_CR = US_CR_RSTTX | US_CR_RSTRX;
   //Disable interrupts
   USART7->US_IDR = 0xFFFFFFFF;

   //Configure baud rate
   USART7->US_BRGR = SystemCoreClock / (16 * baudrate);

   //Configure mode register
   USART7->US_MR = US_MR_CHMODE_NORMAL | US_MR_CHRL_8_BIT | US_MR_PAR_NO;

   //Disable DMA channel
   USART7->US_PTCR = US_PTCR_TXTDIS | US_PTCR_RXTDIS;

   //Enable transmitter and receiver
   USART7->US_CR = US_CR_TXEN | US_CR_RXEN;
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
      while(!(USART7->US_CSR & US_CSR_TXEMPTY));
      //Send character
      USART7->US_THR = c;
      //Wait for the transfer to complete
      while(!(USART7->US_CSR & US_CSR_TXEMPTY));

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
