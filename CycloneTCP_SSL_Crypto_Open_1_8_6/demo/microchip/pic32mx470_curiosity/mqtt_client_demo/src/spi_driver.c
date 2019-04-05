/**
 * @file spi_driver.c
 * @brief SPI driver
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
#include <p32xxxx.h>
#include "core/net.h"
#include "spi_driver.h"
#include "debug.h"


/**
 * @brief SPI driver
 **/

const SpiDriver spiDriver =
{
   spiInit,
   spiSetMode,
   spiSetBitrate,
   spiAssertCs,
   spiDeassertCs,
   spiTransfer
};


/**
 * @brief SPI initialization
 * @return Error code
 **/

error_t spiInit(void)
{
   //Configure SS1 (RD4) as an output
   LATDSET = _LATD_LATD4_MASK;
   TRISDCLR = _TRISD_TRISD4_MASK;

   //Enable pull-up on SDI1 (AN26/RD3)
   CNPUDSET = _CNPUD_CNPUD3_MASK;
   //Disable analog pad
   ANSELDCLR = _ANSELD_ANSD3_MASK;

   //Remap SDO1 to RPD5
   RPD5R = 8;
   //Remap SDI1 to RPD3
   SDI1R = 0;

   //Select master mode operation
   SPI1CON = _SPI1CON_CKE_MASK | _SPI1CON_MSTEN_MASK;
   //Set SCK clock frequency
   SPI1BRG = ((50000000 / 2) / 5000000) - 1;
   //Enable SPI1 module
   SPI1CONSET = _SPI1CON_ON_MASK;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Set SPI mode
 * @param mode SPI mode (0, 1, 2 or 3)
 **/

error_t spiSetMode(uint_t mode)
{
   //Not implemented
   return ERROR_NOT_IMPLEMENTED;
}


/**
 * @brief Set SPI bitrate
 * @param bitrate Bitrate value
 **/

error_t spiSetBitrate(uint_t bitrate)
{
   //Not implemented
   return ERROR_NOT_IMPLEMENTED;
}


/**
 * @brief Assert CS
 **/

void spiAssertCs(void)
{
   //Assert CS pin
   LATDCLR = _LATD_LATD4_MASK;
   //CS setup time
   usleep(1);
}


/**
 * @brief Deassert CS
 **/

void spiDeassertCs(void)
{
   //CS hold time
   usleep(1);
   //Deassert CS pin
   LATDSET = _LATD_LATD4_MASK;
   //CS disable time
   usleep(1);
}


/**
 * @brief Transfer a single byte
 * @param[in] data The data to be written
 * @return The data received from the slave device
 **/

uint8_t spiTransfer(uint8_t data)
{
   //Ensure the TX buffer is empty
   while(!(SPI1STAT & _SPI1STAT_SPITBE_MASK));
   //Start to transfer data
   SPI1BUF = data;
   //Wait for the operation to complete
   while(!(SPI1STAT & _SPI1STAT_SPIRBF_MASK));

   //Return the received character
   return SPI1BUF;
}
