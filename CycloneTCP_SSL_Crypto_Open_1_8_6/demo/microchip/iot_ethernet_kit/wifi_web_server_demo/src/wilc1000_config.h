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

//RST pin (RD4)
#define CONF_WILC_RST_INIT()       TRISDCLR = _TRISD_TRISD4_MASK
#define CONF_WILC_RST_CLR()        LATDCLR = _LATD_LATD4_MASK
#define CONF_WILC_RST_SET()        LATDSET = _LATD_LATD4_MASK

//CE pin (RF3)
#define CONF_WILC_CE_INIT()        TRISFCLR = _TRISF_TRISF3_MASK
#define CONF_WILC_CE_CLR()         LATFCLR = _LATF_LATF3_MASK
#define CONF_WILC_CE_SET()         LATFSET = _LATF_LATF3_MASK

//WAKE pin (unused)
#define CONF_WILC_WAKE_INIT()
#define CONF_WILC_WAKE_CLR()
#define CONF_WILC_WAKE_SET()

//CS pin (RG9/AN11)
#define CONF_WILC_CS_INIT()        ANSELGCLR = _ANSELG_ANSG9_MASK, TRISGCLR = _TRISG_TRISG9_MASK
#define CONF_WILC_CS_CLR()         LATGCLR = _LATG_LATG9_MASK
#define CONF_WILC_CS_SET()         LATGSET = _LATG_LATG9_MASK

//IRQ pin (RD11/INT4)
#define CONF_WILC_IRQ_INIT()       INT4R = 3

//SCK pin (RG6/AN14/SCK2)
#define CONF_WILC_SCK_INIT()       ANSELGCLR = _ANSELG_ANSG6_MASK, RPG6R = 0

//MOSI pin (RG8/AN12/MOSI2)
#define CONF_WILC_MOSI_INIT()      ANSELGCLR = _ANSELG_ANSG8_MASK, RPG8R = 6

//MISO pin (RG7/AN13/MISO2)
#define CONF_WILC_MISO_INIT()      ANSELGCLR = _ANSELG_ANSG7_MASK, SDI2R = 1

//SPI instance (SPI2)
#define CONF_WILC_SPI_INSTANCE     2
//SPI clock prescaler
#define CONF_WILC_SPI_BRG          ((40000000 / 2) / 20000000) - 1

//External interrupt instance (INT4)
#define CONF_WILC_INT_INSTANCE     4

//IRQ handler
#define CONF_WILC_IRQ_HANDLER      ext4IrqHandler
//Interrupt priority
#define CONF_WILC_IRQ_PRIORITY     2
//Interrupt subpriority
#define CONF_WILC_IRQ_SUB_PRIORITY 0

//Forward function declaration
extern void wilc1000EventHook(uint8_t msgType, void *msg);

//Callback function that processes Wi-Fi event notifications
#define CONF_WILC_EVENT_HOOK(msgType, msg) wilc1000EventHook(msgType, msg)

#endif
