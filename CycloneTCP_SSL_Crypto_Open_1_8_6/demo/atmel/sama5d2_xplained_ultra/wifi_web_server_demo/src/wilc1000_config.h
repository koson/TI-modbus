/**
 * @file wilc1000_config.c
 * @brief WILC1000 configuration
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

#ifndef _WILC1000_CONFIG_H
#define _WILC1000_CONFIG_H

//Dependencies
#include <stdint.h>

//WILC1000 chip revision
#define CONF_WILC_USE_1000_REV_B

//SPI interface
#define CONF_WILC_USE_SPI     1

//Debug logs
#define CONF_WILC_DEBUG       1
//Debug output redirection
#define CONF_WILC_PRINTF      TRACE_PRINTF

//RST pin (PD9)
#define CONF_WILC_RST_PIN     {PIO_GROUP_D, PIO_PD9, PIO_OUTPUT, PIO_DEFAULT}

//CE pin (PD28)
#define CONF_WILC_CE_PIN      {PIO_GROUP_D, PIO_PD28, PIO_OUTPUT, PIO_DEFAULT}

//WAKE pin (PD10)
#define CONF_WILC_WAKE_PIN    {PIO_GROUP_D, PIO_PD10, PIO_OUTPUT, PIO_DEFAULT}

//CS pin (PD29)
#define CONF_WILC_CS_PIN      {PIO_GROUP_D, PIO_PD29, PIO_OUTPUT, PIO_DRVSTR_ME}

//IRQ pin (PA21)
#define CONF_WILC_IRQ_PIN     {PIO_GROUP_A, PIO_PA21, PIO_INPUT, PIO_PULLUP | PIO_IT_LOW_LEVEL}
#define CONF_WILC_IRQ_ID_PIO  ID_PIOA

//SCK pin (PD25)
#define CONF_WILC_SCK_PIN     {PIO_GROUP_D, PIO_PD25A_SPI1_SPCK, PIO_PERIPH_A, PIO_DRVSTR_ME}

//MOSI pin (PD26)
#define CONF_WILC_MOSI_PIN    {PIO_GROUP_D, PIO_PD26A_SPI1_MOSI, PIO_PERIPH_A, PIO_DRVSTR_ME}

//MISO pin (PD27)
#define CONF_WILC_MISO_PIN    {PIO_GROUP_D, PIO_PD27A_SPI1_MISO, PIO_PERIPH_A, PIO_DRVSTR_ME}

//SPI instance
#define CONF_WILC_SPI         SPI1
#define CONF_WILC_SPI_ID      ID_SPI1

//SPI clock
#define CONF_WILC_SPI_CLOCK   40000000

//Interrupt group priority
#define CONF_WILC_IRQ_PRIORITY 0

//Forward function declaration
extern void wilc1000EventHook(uint8_t msgType, void *msg);

//Callback function that processes Wi-Fi event notifications
#define CONF_WILC_EVENT_HOOK(msgType, msg) wilc1000EventHook(msgType, msg)

#endif
