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
 * @brief cjson_util Configuration Header
 *
 * @addtogroup cjson_util-config
 * @{
 *
 ***********************************************************/

#ifndef __CJSON_UTIL_CONFIG_H__
#define __CJSON_UTIL_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef CJSON_UTIL_INCLUDE_CUSTOM_CONFIG
#include <cjson_util_custom_config.h>
#endif

/* <auto.start.cdefs(CJSON_UTIL_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * CJSON_UTIL_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef CJSON_UTIL_CONFIG_INCLUDE_LOGGING
#define CJSON_UTIL_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT
#define CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT
#define CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * CJSON_UTIL_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef CJSON_UTIL_CONFIG_PORTING_STDLIB
#define CJSON_UTIL_CONFIG_PORTING_STDLIB 1
#endif

/**
 * CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS CJSON_UTIL_CONFIG_PORTING_STDLIB
#endif

/**
 * CJSON_UTIL_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef CJSON_UTIL_CONFIG_INCLUDE_UCLI
#define CJSON_UTIL_CONFIG_INCLUDE_UCLI 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct cjson_util_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} cjson_util_config_settings_t;

/** Configuration settings table. */
/** cjson_util_config_settings table. */
extern cjson_util_config_settings_t cjson_util_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* cjson_util_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int cjson_util_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(CJSON_UTIL_CONFIG_HEADER).header> */

#include "cjson_util_porting.h"

#endif /* __CJSON_UTIL_CONFIG_H__ */
/* @} */
