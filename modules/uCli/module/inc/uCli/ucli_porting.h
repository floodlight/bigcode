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
 *  /module/inc/uCli/ucli_porting.h
 *
 * @file
 * @brief uCli Porting Definitions.
 *
 * @addtogroup ucli-porting
 * @{
 *
 *
 ***************************************************************/
#ifndef __UCLI_PORTING_H__
#define __UCLI_PORTING_H__


#include <uCli/ucli_config.h>


/* <auto.start.portingmacro(ALL).define> */
#if UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef UCLI_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define UCLI_MEMSET GLOBAL_MEMSET
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_MEMSET memset
    #else
        #error The macro UCLI_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define UCLI_MEMCPY GLOBAL_MEMCPY
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_MEMCPY memcpy
    #else
        #error The macro UCLI_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define UCLI_STRNCPY GLOBAL_STRNCPY
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRNCPY strncpy
    #else
        #error The macro UCLI_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRCPY
    #if defined(GLOBAL_STRCPY)
        #define UCLI_STRCPY GLOBAL_STRCPY
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRCPY strcpy
    #else
        #error The macro UCLI_STRCPY is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRCMP
    #if defined(GLOBAL_STRCMP)
        #define UCLI_STRCMP GLOBAL_STRCMP
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRCMP strcmp
    #else
        #error The macro UCLI_STRCMP is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRNCMP
    #if defined(GLOBAL_STRNCMP)
        #define UCLI_STRNCMP GLOBAL_STRNCMP
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRNCMP strncmp
    #else
        #error The macro UCLI_STRNCMP is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRCHR
    #if defined(GLOBAL_STRCHR)
        #define UCLI_STRCHR GLOBAL_STRCHR
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRCHR strchr
    #else
        #error The macro UCLI_STRCHR is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define UCLI_STRLEN GLOBAL_STRLEN
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRLEN strlen
    #else
        #error The macro UCLI_STRLEN is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRCAT
    #if defined(GLOBAL_STRCAT)
        #define UCLI_STRCAT GLOBAL_STRCAT
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRCAT strcat
    #else
        #error The macro UCLI_STRCAT is required but cannot be defined.
    #endif
#endif

#ifndef UCLI_STRSTR
    #if defined(GLOBAL_STRSTR)
        #define UCLI_STRSTR GLOBAL_STRSTR
    #elif UCLI_CONFIG_PORTING_STDLIB == 1
        #define UCLI_STRSTR strstr
    #else
        #error The macro UCLI_STRSTR is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __UCLI_PORTING_H__ */
/*@}*/
