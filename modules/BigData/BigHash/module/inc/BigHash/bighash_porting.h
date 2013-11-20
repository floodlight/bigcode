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
 * @brief BigHash Porting Macros.
 *
 * @addtogroup bighash-porting
 * @{
 *
 ***************************************************************/
#ifndef __BIGHASH_PORTING_H__
#define __BIGHASH_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef BIGHASH_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define BIGHASH_MEMSET GLOBAL_MEMSET
    #elif BIGHASH_CONFIG_PORTING_STDLIB == 1
        #define BIGHASH_MEMSET memset
    #else
        #error The macro BIGHASH_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef BIGHASH_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define BIGHASH_MEMCPY GLOBAL_MEMCPY
    #elif BIGHASH_CONFIG_PORTING_STDLIB == 1
        #define BIGHASH_MEMCPY memcpy
    #else
        #error The macro BIGHASH_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef BIGHASH_MEMCMP
    #if defined(GLOBAL_MEMCMP)
        #define BIGHASH_MEMCMP GLOBAL_MEMCMP
    #elif BIGHASH_CONFIG_PORTING_STDLIB == 1
        #define BIGHASH_MEMCMP memcmp
    #else
        #error The macro BIGHASH_MEMCMP is required but cannot be defined.
    #endif
#endif

#ifndef BIGHASH_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define BIGHASH_STRNCPY GLOBAL_STRNCPY
    #elif BIGHASH_CONFIG_PORTING_STDLIB == 1
        #define BIGHASH_STRNCPY strncpy
    #else
        #error The macro BIGHASH_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef BIGHASH_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define BIGHASH_STRLEN GLOBAL_STRLEN
    #elif BIGHASH_CONFIG_PORTING_STDLIB == 1
        #define BIGHASH_STRLEN strlen
    #else
        #error The macro BIGHASH_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __BIGHASH_PORTING_H__ */
/* @} */
