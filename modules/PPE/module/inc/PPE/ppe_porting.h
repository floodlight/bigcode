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
 * @brief PPE Porting Definitions.
 *
 *
 * @addtogroup ppe-porting
 * @{
 *
 ****************************************************************/

#ifndef __PPE_PORTING_H__
#define __PPE_PORTING_H__


#include <PPE/ppe_config.h>


/* <auto.start.portingmacro(ALL).define> */
#if PPE_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef PPE_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define PPE_MEMSET GLOBAL_MEMSET
    #elif PPE_CONFIG_PORTING_STDLIB == 1
        #define PPE_MEMSET memset
    #else
        #error The macro PPE_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef PPE_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define PPE_MEMCPY GLOBAL_MEMCPY
    #elif PPE_CONFIG_PORTING_STDLIB == 1
        #define PPE_MEMCPY memcpy
    #else
        #error The macro PPE_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef PPE_MEMMOVE
    #if defined(GLOBAL_MEMMOVE)
        #define PPE_MEMMOVE GLOBAL_MEMMOVE
    #elif PPE_CONFIG_PORTING_STDLIB == 1
        #define PPE_MEMMOVE memmove
    #else
        #error The macro PPE_MEMMOVE is required but cannot be defined.
    #endif
#endif

#ifndef PPE_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define PPE_STRNCPY GLOBAL_STRNCPY
    #elif PPE_CONFIG_PORTING_STDLIB == 1
        #define PPE_STRNCPY strncpy
    #else
        #error The macro PPE_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef PPE_MEMCMP
    #if defined(GLOBAL_MEMCMP)
        #define PPE_MEMCMP GLOBAL_MEMCMP
    #elif PPE_CONFIG_PORTING_STDLIB == 1
        #define PPE_MEMCMP memcmp
    #else
        #error The macro PPE_MEMCMP is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __PPE_PORTING_H__ */
/*@}*/
