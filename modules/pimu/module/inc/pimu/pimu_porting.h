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
 * @brief pimu Porting Macros.
 *
 * @addtogroup pimu-porting
 * @{
 *
 ***************************************************************/
#ifndef __PIMU_PORTING_H__
#define __PIMU_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef PIMU_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define PIMU_MEMSET GLOBAL_MEMSET
    #elif PIMU_CONFIG_PORTING_STDLIB == 1
        #define PIMU_MEMSET memset
    #else
        #error The macro PIMU_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef PIMU_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define PIMU_MEMCPY GLOBAL_MEMCPY
    #elif PIMU_CONFIG_PORTING_STDLIB == 1
        #define PIMU_MEMCPY memcpy
    #else
        #error The macro PIMU_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef PIMU_MEMCMP
    #if defined(GLOBAL_MEMCMP)
        #define PIMU_MEMCMP GLOBAL_MEMCMP
    #elif PIMU_CONFIG_PORTING_STDLIB == 1
        #define PIMU_MEMCMP memcmp
    #else
        #error The macro PIMU_MEMCMP is required but cannot be defined.
    #endif
#endif

#ifndef PIMU_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define PIMU_STRNCPY GLOBAL_STRNCPY
    #elif PIMU_CONFIG_PORTING_STDLIB == 1
        #define PIMU_STRNCPY strncpy
    #else
        #error The macro PIMU_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef PIMU_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define PIMU_STRLEN GLOBAL_STRLEN
    #elif PIMU_CONFIG_PORTING_STDLIB == 1
        #define PIMU_STRLEN strlen
    #else
        #error The macro PIMU_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __PIMU_PORTING_H__ */
/* @} */
