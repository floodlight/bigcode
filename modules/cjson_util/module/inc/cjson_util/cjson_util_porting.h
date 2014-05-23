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
 ********************************************************//**
 *
 * @file
 * @brief cjson_util Porting Macros.
 *
 * @addtogroup cjson_util-porting
 * @{
 *
 ***********************************************************/
#ifndef __CJSON_UTIL_PORTING_H__
#define __CJSON_UTIL_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef CJSON_UTIL_MALLOC
    #if defined(GLOBAL_MALLOC)
        #define CJSON_UTIL_MALLOC GLOBAL_MALLOC
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_MALLOC malloc
    #else
        #error The macro CJSON_UTIL_MALLOC is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_FREE
    #if defined(GLOBAL_FREE)
        #define CJSON_UTIL_FREE GLOBAL_FREE
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_FREE free
    #else
        #error The macro CJSON_UTIL_FREE is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define CJSON_UTIL_MEMSET GLOBAL_MEMSET
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_MEMSET memset
    #else
        #error The macro CJSON_UTIL_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define CJSON_UTIL_MEMCPY GLOBAL_MEMCPY
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_MEMCPY memcpy
    #else
        #error The macro CJSON_UTIL_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define CJSON_UTIL_STRNCPY GLOBAL_STRNCPY
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_STRNCPY strncpy
    #else
        #error The macro CJSON_UTIL_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_VSNPRINTF
    #if defined(GLOBAL_VSNPRINTF)
        #define CJSON_UTIL_VSNPRINTF GLOBAL_VSNPRINTF
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_VSNPRINTF vsnprintf
    #else
        #error The macro CJSON_UTIL_VSNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define CJSON_UTIL_SNPRINTF GLOBAL_SNPRINTF
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_SNPRINTF snprintf
    #else
        #error The macro CJSON_UTIL_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef CJSON_UTIL_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define CJSON_UTIL_STRLEN GLOBAL_STRLEN
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_STRLEN strlen
    #else
        #error The macro CJSON_UTIL_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __CJSON_UTIL_PORTING_H__ */
/* @} */
