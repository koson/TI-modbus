/**
 * @file usbd_conf.h
 * @brief USB device low-level configuration
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Pro.
 *
 * This software is provided under a commercial license. You may
 * use this software under the conditions stated in the license
 * terms. This source code cannot be redistributed.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.6
 **/

#ifndef _USBD_CONF_H
#define _USBD_CONF_H

//Dependencies
#include "stm32f7xx_hal.h"

//Settings
#define USBD_MAX_NUM_INTERFACES     1
#define USBD_MAX_NUM_CONFIGURATION  1
#define USBD_MAX_STR_DESC_SIZ       256
#define USBD_SUPPORT_USER_STRING    0
#define USBD_SELF_POWERED           1
#define USBD_DEBUG_LEVEL            0

//Debug macros
#if (USBD_DEBUG_LEVEL > 0)
   #define USBD_UsrLog(...) printf(__VA_ARGS__); printf("\r\n");
#else
   #define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)
   #define USBD_ErrLog(...) printf("ERROR: "); printf(__VA_ARGS__); printf("\r\n");
#else
   #define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
   #define USBD_DbgLog(...) printf("DEBUG: "); printf(__VA_ARGS__); printf("\r\n");
#else
   #define USBD_DbgLog(...)
#endif

#endif
