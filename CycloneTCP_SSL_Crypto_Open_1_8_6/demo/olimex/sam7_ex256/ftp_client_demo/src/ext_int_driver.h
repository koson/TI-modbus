/**
 * @file ext_int_driver.h
 * @brief External interrupt line driver
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

#ifndef _EXT_INT_DRIVER_H
#define _EXT_INT_DRIVER_H

//External interrupt line driver
extern const ExtIntDriver extIntDriver;

//External interrupt related functions
error_t extIntInit(void);
void extIntEnableIrq(void);
void extIntDisableIrq(void);
void extIntHandler(void);

//Wrapper for the interrupt service routine
void piobIrqWrapper(void);

#endif
