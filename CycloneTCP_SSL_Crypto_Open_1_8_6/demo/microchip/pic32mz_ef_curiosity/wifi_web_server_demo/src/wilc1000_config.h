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
#define CONF_WILC_USE_SPI          1

//Debug logs
#define CONF_WILC_DEBUG            1
//Debug output redirection
#define CONF_WILC_PRINTF           TRACE_PRINTF

//RST pin (RA9/AN27)
#define CONF_WILC_RST_INIT()       ANSELACLR = _ANSELA_ANSA9_MASK, TRISACLR = _TRISA_TRISA9_MASK
#define CONF_WILC_RST_CLR()        LATACLR = _LATA_LATA9_MASK
#define CONF_WILC_RST_SET()        LATASET = _LATA_LATA9_MASK

//CE pin (RE8/AN25)
#define CONF_WILC_CE_INIT()        ANSELECLR = _ANSELE_ANSE8_MASK, TRISECLR = _TRISE_TRISE8_MASK
#define CONF_WILC_CE_CLR()         LATECLR = _LATE_LATE8_MASK
#define CONF_WILC_CE_SET()         LATESET = _LATE_LATE8_MASK

//WAKE pin (unused)
#define CONF_WILC_WAKE_INIT()
#define CONF_WILC_WAKE_CLR()
#define CONF_WILC_WAKE_SET()

//CS pin (RD4)
#define CONF_WILC_CS_INIT()        TRISDCLR = _TRISD_TRISD4_MASK
#define CONF_WILC_CS_CLR()         LATDCLR = _LATD_LATD4_MASK
#define CONF_WILC_CS_SET()         LATDSET = _LATD_LATD4_MASK

//IRQ pin (RF13/AN30/INT1)
#define CONF_WILC_IRQ_INIT()       ANSELFCLR = _ANSELF_ANSF13_MASK, INT1R = 9

//SCK pin (RD1/SCK1)
#define CONF_WILC_SCK_INIT()       RPD1R = 0

//MOSI pin (RD3/MOSI1)
#define CONF_WILC_MOSI_INIT()      RPD3R = 5

//MISO pin (RD14/AN32/MISO1)
#define CONF_WILC_MISO_INIT()      ANSELDCLR = _ANSELD_ANSD14_MASK, SDI1R = 11

//SPI instance (SPI1)
#define CONF_WILC_SPI_INSTANCE     1
//SPI clock prescaler
#define CONF_WILC_SPI_BRG          ((40000000 / 2) / 20000000) - 1

//External interrupt instance (INT1)
#define CONF_WILC_INT_INSTANCE     1

//IRQ handler
#define CONF_WILC_IRQ_HANDLER      ext1IrqHandler
//Interrupt priority
#define CONF_WILC_IRQ_PRIORITY     2
//Interrupt subpriority
#define CONF_WILC_IRQ_SUB_PRIORITY 0

//Forward function declaration
extern void wilc1000EventHook(uint8_t msgType, void *msg);

//Callback function that processes Wi-Fi event notifications
#define CONF_WILC_EVENT_HOOK(msgType, msg) wilc1000EventHook(msgType, msg)

#endif
