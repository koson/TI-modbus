/**
 * @file ctr.h
 * @brief Counter(CTR) mode
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCrypto Open.
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

#ifndef _CTR_H
#define _CTR_H

//Dependencies
#include "core/crypto.h"

//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif

//CTR encryption and decryption routines
error_t ctrEncrypt(const CipherAlgo *cipher, void *context, uint_t m,
   uint8_t *t, const uint8_t *p, uint8_t *c, size_t length);

error_t ctrDecrypt(const CipherAlgo *cipher, void *context, uint_t m,
   uint8_t *t, const uint8_t *c, uint8_t *p, size_t length);

//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
