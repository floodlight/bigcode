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

#ifndef __OS_CONFIG_H__
#define __OS_CONFIG_H__


#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef OS_INCLUDE_CUSTOM_CONFIG
#include <os_custom_config.h>
#endif




/* <auto.start.cdefs(OS_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * OS_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef OS_CONFIG_INCLUDE_LOGGING
#define OS_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * OS_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef OS_CONFIG_LOG_OPTIONS_DEFAULT
#define OS_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * OS_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log options. */


#ifndef OS_CONFIG_LOG_BITS_DEFAULT
#define OS_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log options. */


#ifndef OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * OS_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef OS_CONFIG_PORTING_STDLIB
#define OS_CONFIG_PORTING_STDLIB 1
#endif

/**
 * OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS OS_CONFIG_PORTING_STDLIB
#endif

/**
 * OS_CONFIG_INCLUDE_UCLI
 *
 * Include generic UCLI support. */


#ifndef OS_CONFIG_INCLUDE_UCLI
#define OS_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * OS_CONFIG_INCLUDE_POSIX
 *
 * Use POSIX version. */


#ifndef OS_CONFIG_INCLUDE_POSIX
#define OS_CONFIG_INCLUDE_POSIX 0
#endif

/**
 * OS_CONFIG_INCLUDE_WINDOWS
 *
 * Use Windows version. */


#ifndef OS_CONFIG_INCLUDE_WINDOWS
#define OS_CONFIG_INCLUDE_WINDOWS 0
#endif

/**
 * OS_CONFIG_INCLUDE_OSX
 *
 * Use Mac OSX version. */


#ifndef OS_CONFIG_INCLUDE_OSX
#define OS_CONFIG_INCLUDE_OSX 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct os_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} os_config_settings_t;

/** Configuration settings table. */
/** os_config_settings table. */
extern os_config_settings_t os_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* os_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int os_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(OS_CONFIG_HEADER).header> */



#include "os_porting.h"
#endif /* __OS_CONFIG_H__ */
