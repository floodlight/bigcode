/************************************************************
 * <bsn.cl fy=2014 v=epl>
 * 
 *           Copyright 2014 Big Switch Networks, Inc.          
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
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ********************************************************//**
 *
 * @file
 * @brief orc Porting Macros.
 *
 * @addtogroup orc-porting
 * @{
 *
 ***********************************************************/
#ifndef __ORC_PORTING_H__
#define __ORC_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if ORC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef ORC_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define ORC_MEMSET GLOBAL_MEMSET
    #elif ORC_CONFIG_PORTING_STDLIB == 1
        #define ORC_MEMSET memset
    #else
        #error The macro ORC_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef ORC_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define ORC_MEMCPY GLOBAL_MEMCPY
    #elif ORC_CONFIG_PORTING_STDLIB == 1
        #define ORC_MEMCPY memcpy
    #else
        #error The macro ORC_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef ORC_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define ORC_STRNCPY GLOBAL_STRNCPY
    #elif ORC_CONFIG_PORTING_STDLIB == 1
        #define ORC_STRNCPY strncpy
    #else
        #error The macro ORC_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef ORC_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define ORC_SNPRINTF GLOBAL_SNPRINTF
    #elif ORC_CONFIG_PORTING_STDLIB == 1
        #define ORC_SNPRINTF snprintf
    #else
        #error The macro ORC_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef ORC_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define ORC_STRLEN GLOBAL_STRLEN
    #elif ORC_CONFIG_PORTING_STDLIB == 1
        #define ORC_STRLEN strlen
    #else
        #error The macro ORC_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __ORC_PORTING_H__ */
/* @} */
