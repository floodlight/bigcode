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
 * @brief FME Porting Definitions.
 *
 * @addtogroup fme-porting
 * @{
 *
 *
 ***************************************************************/
#ifndef __FME_PORTING_H__
#define __FME_PORTING_H__


#include <FME/fme_config.h>


/* <auto.start.portingmacro(ALL).define> */
#if FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef FME_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define FME_MEMSET GLOBAL_MEMSET
    #elif FME_CONFIG_PORTING_STDLIB == 1
        #define FME_MEMSET memset
    #else
        #error The macro FME_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef FME_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define FME_MEMCPY GLOBAL_MEMCPY
    #elif FME_CONFIG_PORTING_STDLIB == 1
        #define FME_MEMCPY memcpy
    #else
        #error The macro FME_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef FME_MEMMOVE
    #if defined(GLOBAL_MEMMOVE)
        #define FME_MEMMOVE GLOBAL_MEMMOVE
    #elif FME_CONFIG_PORTING_STDLIB == 1
        #define FME_MEMMOVE memmove
    #else
        #error The macro FME_MEMMOVE is required but cannot be defined.
    #endif
#endif

#ifndef FME_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define FME_STRNCPY GLOBAL_STRNCPY
    #elif FME_CONFIG_PORTING_STDLIB == 1
        #define FME_STRNCPY strncpy
    #else
        #error The macro FME_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef FME_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define FME_STRLEN GLOBAL_STRLEN
    #elif FME_CONFIG_PORTING_STDLIB == 1
        #define FME_STRLEN strlen
    #else
        #error The macro FME_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __FME_PORTING_H__ */
/*@}*/
