/************************************************************
 * <bsn.cl fy=2013 v=epl-1.0>
 *
 *        Copyright 2013, 2014 Big Switch Networks, Inc.
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
 *  /module/inc/ELS/els_porting.h
 *
 * @file
 * @brief ELS Porting Definitions.
 *
 * @addtogroup els-porting
 * @{
 *
 ***********************************************************/
#ifndef __ELS_PORTING_H__
#define __ELS_PORTING_H__


#include <ELS/els_config.h>


/* <auto.start.portingmacro(ALL).define> */
#if ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef ELS_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define ELS_MEMSET GLOBAL_MEMSET
    #elif ELS_CONFIG_PORTING_STDLIB == 1
        #define ELS_MEMSET memset
    #else
        #error The macro ELS_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef ELS_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define ELS_STRNCPY GLOBAL_STRNCPY
    #elif ELS_CONFIG_PORTING_STDLIB == 1
        #define ELS_STRNCPY strncpy
    #else
        #error The macro ELS_STRNCPY is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */

#endif /* __ELS_PORTING_H__ */
/*@}*/
