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
 *  /module/inc/uCli/ucli_config.h
 *
 * @file
 * @brief uCli Configuration Definitions.
 *
 * @addtogroup ucli-config
 * @{
 *
 ***************************************************************/
#ifndef __UCLI_CONFIG_H__
#define __UCLI_CONFIG_H__


#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef UCLI_INCLUDE_CUSTOM_CONFIG
#include <ucli_custom_config.h>
#endif




/* <auto.start.cdefs(UCLI_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * UCLI_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef UCLI_CONFIG_INCLUDE_LOGGING
#define UCLI_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * UCLI_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef UCLI_CONFIG_LOG_OPTIONS_DEFAULT
#define UCLI_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * UCLI_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log options. */


#ifndef UCLI_CONFIG_LOG_BITS_DEFAULT
#define UCLI_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log options. */


#ifndef UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * UCLI_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef UCLI_CONFIG_PORTING_STDLIB
#define UCLI_CONFIG_PORTING_STDLIB 1
#endif

/**
 * UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS UCLI_CONFIG_PORTING_STDLIB
#endif

/**
 * UCLI_CONFIG_MAX_ARGS
 *
 * Maximum number of arguments for any given command. */


#ifndef UCLI_CONFIG_MAX_ARGS
#define UCLI_CONFIG_MAX_ARGS 32
#endif

/**
 * UCLI_CONFIG_MAX_PROMPT
 *
 * Maximum prompt length. */


#ifndef UCLI_CONFIG_MAX_PROMPT
#define UCLI_CONFIG_MAX_PROMPT 32
#endif

/**
 * UCLI_CONFIG_INCLUDE_FGETS_LOOP
 *
 * Include input processing loop using fgets() */


#ifndef UCLI_CONFIG_INCLUDE_FGETS_LOOP
#define UCLI_CONFIG_INCLUDE_FGETS_LOOP 0
#endif

/**
 * UCLI_CONFIG_INCLUDE_ELS_LOOP
 *
 * Include input processing loop using ELS. */


#ifndef UCLI_CONFIG_INCLUDE_ELS_LOOP
#define UCLI_CONFIG_INCLUDE_ELS_LOOP 0
#endif

/**
 * UCLI_CONFIG_MAX_INPUT_LINE
 *
 * Maximum input line for file processing. */


#ifndef UCLI_CONFIG_MAX_INPUT_LINE
#define UCLI_CONFIG_MAX_INPUT_LINE 2048
#endif

/**
 * UCLI_CONFIG_INCLUDE_TTY
 *
 * Attempt pretty formatting for posix tty environments. Otherwise plain output for dumb terminals only.  */


#ifndef UCLI_CONFIG_INCLUDE_TTY
#define UCLI_CONFIG_INCLUDE_TTY 1
#endif

/**
 * UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK
 *
 * The maximum number of commands allowed in a command-block structure. */


#ifndef UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK
#define UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK 64
#endif

/**
 * UCLI_CONFIG_INCLUDE_MODULE_NODES
 *
 * Include all generic module nodes. */


#ifndef UCLI_CONFIG_INCLUDE_MODULE_NODES
#define UCLI_CONFIG_INCLUDE_MODULE_NODES 1
#endif

/**
 * UCLI_CONFIG_INCLUDE_PTHREADS
 *
 * Include support for command background threads. Requires POSIX. */


#ifndef UCLI_CONFIG_INCLUDE_PTHREADS
#define UCLI_CONFIG_INCLUDE_PTHREADS 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct ucli_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} ucli_config_settings_t;

/** Configuration settings table. */
/** ucli_config_settings table. */
extern ucli_config_settings_t ucli_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* ucli_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int ucli_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(UCLI_CONFIG_HEADER).header> */



#include "ucli_porting.h"
#endif /* __UCLI_CONFIG_H__ */
/*@}*/
