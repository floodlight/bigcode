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
 * @brief VPI Porting Definitions.
 *
 * @addtogroup vpi-porting
 * @{
 *
 ***************************************************************/
#ifndef __VPI_PORTING_H__
#define __VPI_PORTING_H__

#include <VPI/vpi_config.h>

#include <inttypes.h>

/* <auto.start.portingmacro(ALL).define> */
#if VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef VPI_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define VPI_MEMCPY GLOBAL_MEMCPY
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_MEMCPY memcpy
    #else
        #error The macro VPI_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef VPI_STRCMP
    #if defined(GLOBAL_STRCMP)
        #define VPI_STRCMP GLOBAL_STRCMP
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_STRCMP strcmp
    #else
        #error The macro VPI_STRCMP is required but cannot be defined.
    #endif
#endif

#ifndef VPI_STRCPY
    #if defined(GLOBAL_STRCPY)
        #define VPI_STRCPY GLOBAL_STRCPY
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_STRCPY strcpy
    #else
        #error The macro VPI_STRCPY is required but cannot be defined.
    #endif
#endif

#ifndef VPI_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define VPI_STRLEN GLOBAL_STRLEN
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_STRLEN strlen
    #else
        #error The macro VPI_STRLEN is required but cannot be defined.
    #endif
#endif

#ifndef VPI_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define VPI_MEMSET GLOBAL_MEMSET
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_MEMSET memset
    #else
        #error The macro VPI_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef VPI_STRCAT
    #if defined(GLOBAL_STRCAT)
        #define VPI_STRCAT GLOBAL_STRCAT
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_STRCAT strcat
    #else
        #error The macro VPI_STRCAT is required but cannot be defined.
    #endif
#endif

#ifndef VPI_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define VPI_STRNCPY GLOBAL_STRNCPY
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_STRNCPY strncpy
    #else
        #error The macro VPI_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef VPI_MEMMOVE
    #if defined(GLOBAL_MEMMOVE)
        #define VPI_MEMMOVE GLOBAL_MEMMOVE
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_MEMMOVE memmove
    #else
        #error The macro VPI_MEMMOVE is required but cannot be defined.
    #endif
#endif

#ifndef VPI_STRCHR
    #if defined(GLOBAL_STRCHR)
        #define VPI_STRCHR GLOBAL_STRCHR
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_STRCHR strchr
    #else
        #error The macro VPI_STRCHR is required but cannot be defined.
    #endif
#endif

#ifndef VPI_ATOI
    #if defined(GLOBAL_ATOI)
        #define VPI_ATOI GLOBAL_ATOI
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_ATOI atoi
    #else
        #error The macro VPI_ATOI is required but cannot be defined.
    #endif
#endif

#ifndef VPI_SSCANF
    #if defined(GLOBAL_SSCANF)
        #define VPI_SSCANF GLOBAL_SSCANF
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_SSCANF sscanf
    #else
        #error The macro VPI_SSCANF is required but cannot be defined.
    #endif
#endif

#ifndef VPI_SPRINTF
    #if defined(GLOBAL_SPRINTF)
        #define VPI_SPRINTF GLOBAL_SPRINTF
    #elif VPI_CONFIG_PORTING_STDLIB == 1
        #define VPI_SPRINTF sprintf
    #else
        #error The macro VPI_SPRINTF is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __VPI_PORTING_H__ */
/*@}*/
