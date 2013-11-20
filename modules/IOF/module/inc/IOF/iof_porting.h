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
 *
 *  IOF Porting Header
 *
 * @file
 * @brief Indented Object Formatter Porting Definitions
 *
 * @addtogroup iof-porting
 * @{
 *
 ***************************************************************/

#ifndef __IOF_PORTING_H__
#define __IOF_PORTING_H__


#include <IOF/iof_config.h>


/* <auto.start.portingmacro(ALL).define> */
#if IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef IOF_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define IOF_MEMSET GLOBAL_MEMSET
    #elif IOF_CONFIG_PORTING_STDLIB == 1
        #define IOF_MEMSET memset
    #else
        #error The macro IOF_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef IOF_STRCMP
    #if defined(GLOBAL_STRCMP)
        #define IOF_STRCMP GLOBAL_STRCMP
    #elif IOF_CONFIG_PORTING_STDLIB == 1
        #define IOF_STRCMP strcmp
    #else
        #error The macro IOF_STRCMP is required but cannot be defined.
    #endif
#endif

#ifndef IOF_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define IOF_MEMCPY GLOBAL_MEMCPY
    #elif IOF_CONFIG_PORTING_STDLIB == 1
        #define IOF_MEMCPY memcpy
    #else
        #error The macro IOF_MEMCPY is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __IOF_PORTING_H__ */
/*@}*/
