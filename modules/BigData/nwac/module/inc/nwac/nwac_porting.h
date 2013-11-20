/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 ***************************************************************/
/************************************************************//**
 *
 * @file
 * @brief nwac Porting Macros.
 *
 * @addtogroup nwac-porting
 * @{
 *
 ***************************************************************/
#ifndef __NWAC_PORTING_H__
#define __NWAC_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef NWAC_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define NWAC_MEMSET GLOBAL_MEMSET
    #elif NWAC_CONFIG_PORTING_STDLIB == 1
        #define NWAC_MEMSET memset
    #else
        #error The macro NWAC_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef NWAC_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define NWAC_MEMCPY GLOBAL_MEMCPY
    #elif NWAC_CONFIG_PORTING_STDLIB == 1
        #define NWAC_MEMCPY memcpy
    #else
        #error The macro NWAC_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef NWAC_MEMCMP
    #if defined(GLOBAL_MEMCMP)
        #define NWAC_MEMCMP GLOBAL_MEMCMP
    #elif NWAC_CONFIG_PORTING_STDLIB == 1
        #define NWAC_MEMCMP memcmp
    #else
        #error The macro NWAC_MEMCMP is required but cannot be defined.
    #endif
#endif

#ifndef NWAC_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define NWAC_STRNCPY GLOBAL_STRNCPY
    #elif NWAC_CONFIG_PORTING_STDLIB == 1
        #define NWAC_STRNCPY strncpy
    #else
        #error The macro NWAC_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef NWAC_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define NWAC_STRLEN GLOBAL_STRLEN
    #elif NWAC_CONFIG_PORTING_STDLIB == 1
        #define NWAC_STRLEN strlen
    #else
        #error The macro NWAC_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __NWAC_PORTING_H__ */
/* @} */
