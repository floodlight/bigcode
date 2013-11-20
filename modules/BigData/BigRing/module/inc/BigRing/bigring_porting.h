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
 * @brief BigRing Porting Macros.
 *
 * @addtogroup bigring_porting
 * @{
 *
 ***************************************************************/
#ifndef __BIGRING_PORTING_H__
#define __BIGRING_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef BIGRING_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define BIGRING_MEMSET GLOBAL_MEMSET
    #elif BIGRING_CONFIG_PORTING_STDLIB == 1
        #define BIGRING_MEMSET memset
    #else
        #error The macro BIGRING_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef BIGRING_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define BIGRING_MEMCPY GLOBAL_MEMCPY
    #elif BIGRING_CONFIG_PORTING_STDLIB == 1
        #define BIGRING_MEMCPY memcpy
    #else
        #error The macro BIGRING_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef BIGRING_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define BIGRING_STRNCPY GLOBAL_STRNCPY
    #elif BIGRING_CONFIG_PORTING_STDLIB == 1
        #define BIGRING_STRNCPY strncpy
    #else
        #error The macro BIGRING_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef BIGRING_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define BIGRING_STRLEN GLOBAL_STRLEN
    #elif BIGRING_CONFIG_PORTING_STDLIB == 1
        #define BIGRING_STRLEN strlen
    #else
        #error The macro BIGRING_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __BIGRING_PORTING_H__ */
/* @} */
