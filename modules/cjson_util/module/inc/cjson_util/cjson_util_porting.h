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

#ifndef CJSON_UTIL_SSCANF
    #if defined(GLOBAL_SSCANF)
        #define CJSON_UTIL_SSCANF GLOBAL_SSCANF
    #elif CJSON_UTIL_CONFIG_PORTING_STDLIB == 1
        #define CJSON_UTIL_SSCANF sscanf
    #else
        #error The macro CJSON_UTIL_SSCANF is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __CJSON_UTIL_PORTING_H__ */
/* @} */
