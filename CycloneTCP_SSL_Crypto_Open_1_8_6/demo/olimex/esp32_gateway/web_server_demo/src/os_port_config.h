/**
 * @file os_port_config.h
 * @brief RTOS port configuration file
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

#ifndef _OS_PORT_CONFIG_H
#define _OS_PORT_CONFIG_H

//Select underlying RTOS
#define USE_FREERTOS

//Trace output redirection
#define TRACE_PRINTF(...) uart_printf(__VA_ARGS__)
#define TRACE_ARRAY(p, a, n) debugDisplayArray(NULL, p, a, n)

int uart_printf(const char *format, ...);

#endif
