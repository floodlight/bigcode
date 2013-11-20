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
 * @brief Indented Object Formatter Configuration Header
 *
 * @addtogroup iof-config
 * @{
 *
 ***************************************************************/

#ifndef __IOF_CONFIG_H__
#define __IOF_CONFIG_H__


#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef IOF_INCLUDE_CUSTOM_CONFIG
#include <iof_custom_config.h>
#endif




/* <auto.start.cdefs(IOF_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * IOF_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef IOF_CONFIG_INCLUDE_LOGGING
#define IOF_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * IOF_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef IOF_CONFIG_LOG_OPTIONS_DEFAULT
#define IOF_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * IOF_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log options. */


#ifndef IOF_CONFIG_LOG_BITS_DEFAULT
#define IOF_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log options. */


#ifndef IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * IOF_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef IOF_CONFIG_PORTING_STDLIB
#define IOF_CONFIG_PORTING_STDLIB 1
#endif

/**
 * IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS IOF_CONFIG_PORTING_STDLIB
#endif

/**
 * IOF_CONFIG_DEFAULT_INDENT_FACTOR
 *
 * How many indent characters to print per level. */


#ifndef IOF_CONFIG_DEFAULT_INDENT_FACTOR
#define IOF_CONFIG_DEFAULT_INDENT_FACTOR 4
#endif

/**
 * IOF_CONFIG_DEFAULT_INDENT_INITIAL
 *
 * Default value for the initial indentation level of an IOF object. */


#ifndef IOF_CONFIG_DEFAULT_INDENT_INITIAL
#define IOF_CONFIG_DEFAULT_INDENT_INITIAL 2
#endif

/**
 * IOF_CONFIG_DEFAULT_PUSH_STRING
 *
 * Default value for the push scope character. */


#ifndef IOF_CONFIG_DEFAULT_PUSH_STRING
#define IOF_CONFIG_DEFAULT_PUSH_STRING " = {"
#endif

/**
 * IOF_CONFIG_DEFAULT_POP_STRING
 *
 * Default value for the pop scope character. */


#ifndef IOF_CONFIG_DEFAULT_POP_STRING
#define IOF_CONFIG_DEFAULT_POP_STRING "}"
#endif

/**
 * IOF_CONFIG_DEFAULT_INDENT_STRING
 *
 * Default string used for indentation. */


#ifndef IOF_CONFIG_DEFAULT_INDENT_STRING
#define IOF_CONFIG_DEFAULT_INDENT_STRING " "
#endif

/**
 * IOF_CONFIG_DEFAULT_INDENT_TERMINATOR
 *
 * Default string used for the termination of the indentation. */


#ifndef IOF_CONFIG_DEFAULT_INDENT_TERMINATOR
#define IOF_CONFIG_DEFAULT_INDENT_TERMINATOR IOF_CONFIG_DEFAULT_INDENT_STRING
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct iof_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} iof_config_settings_t;

/** Configuration settings table. */
/** iof_config_settings table. */
extern iof_config_settings_t iof_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* iof_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int iof_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(IOF_CONFIG_HEADER).header> */



#include "iof_porting.h"
#endif /* __IOF_CONFIG_H__ */
/*@}*/
