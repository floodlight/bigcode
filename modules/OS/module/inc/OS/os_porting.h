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

#ifndef __OS_PORTING_H__
#define __OS_PORTING_H__


#include <OS/os_config.h>


/* <auto.start.portingmacro(ALL).define> */
#if OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef OS_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define OS_MEMSET GLOBAL_MEMSET
    #elif OS_CONFIG_PORTING_STDLIB == 1
        #define OS_MEMSET memset
    #else
        #error The macro OS_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef OS_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define OS_STRNCPY GLOBAL_STRNCPY
    #elif OS_CONFIG_PORTING_STDLIB == 1
        #define OS_STRNCPY strncpy
    #else
        #error The macro OS_STRNCPY is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __OS_PORTING_H__ */
