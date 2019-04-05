/**
 * @file winc1500_config.c
 * @brief WINC1500 configuration
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

#ifndef _WINC1500_CONFIG_H
#define _WINC1500_CONFIG_H

//Dependencies
#include <stdint.h>

//SPI interface
#define CONF_WINC_USE_SPI          1

//Debug logs
#define CONF_WINC_DEBUG            1
//Debug output redirection
#define CONF_WINC_PRINTF           TRACE_PRINTF

//RST pin (RE3)
#define CONF_WINC_RST_INIT()       TRISECLR = _TRISE_TRISE3_MASK
#define CONF_WINC_RST_CLR()        LATECLR = _LATE_LATE3_MASK
#define CONF_WINC_RST_SET()        LATESET = _LATE_LATE3_MASK

//CE pin (RB14/AN14)
#define CONF_WINC_CE_INIT()        ANSELBCLR = _ANSELB_ANSB14_MASK, TRISBCLR = _TRISB_TRISB14_MASK
#define CONF_WINC_CE_CLR()         LATBCLR = _LATB_LATB14_MASK
#define CONF_WINC_CE_SET()         LATBSET = _LATB_LATB14_MASK

//WAKE pin (unused)
#define CONF_WINC_WAKE_INIT()
#define CONF_WINC_WAKE_CLR()
#define CONF_WINC_WAKE_SET()

//CS pin (RD4)
#define CONF_WINC_CS_INIT()        TRISDCLR = _TRISD_TRISD4_MASK
#define CONF_WINC_CS_CLR()         LATDCLR = _LATD_LATD4_MASK
#define CONF_WINC_CS_SET()         LATDSET = _LATD_LATD4_MASK

//IRQ pin (RD1/AN24)
#define CONF_WINC_IRQ_INIT()       ANSELDCLR = _ANSELD_ANSD1_MASK, CNPUDSET = _CNPUD_CNPUD1_MASK, INT1R = 0

//SCK pin (RD2/AN25/SCK1)
#define CONF_WINC_SCK_INIT()       ANSELDCLR = _ANSELD_ANSD2_MASK, RPD2R = 0

//MOSI pin (RD5/MOSI1)
#define CONF_WINC_MOSI_INIT()      RPD5R = 8

//MISO pin (RD3/AN26/MISO1)
#define CONF_WINC_MISO_INIT()      ANSELDCLR = _ANSELD_ANSD3_MASK, SDI1R = 0

//SPI instance (SPI1)
#define CONF_WINC_SPI_INSTANCE     1
//SPI clock prescaler
#define CONF_WINC_SPI_BRG          ((50000000 / 2) / 12500000) - 1

//External interrupt instance (INT1)
#define CONF_WINC_INT_INSTANCE     1

//IRQ handler
#define CONF_WINC_IRQ_HANDLER      ext1IrqHandler
//Interrupt priority
#define CONF_WINC_IRQ_PRIORITY     2
//Interrupt subpriority
#define CONF_WINC_IRQ_SUB_PRIORITY 0

//Forward function declaration
extern void winc1500EventHook(uint8_t msgType, void *msg);

//Callback function that processes Wi-Fi event notifications
#define CONF_WINC_EVENT_HOOK(msgType, msg) winc1500EventHook(msgType, msg)

#endif
