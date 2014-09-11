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
 ************************************************************
 ********************************************************//**
 *
 *  /module/inc/ELS/els_config.h
 *
 * @file
 * @brief ELS Configuration Definitions
 *
 *
 * @addtogroup els-config
 * @{
 *
 ***********************************************************/
#ifndef __ELS_CONFIG_H__
#define __ELS_CONFIG_H__


#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef ELS_INCLUDE_CUSTOM_CONFIG
#include <els_custom_config.h>
#endif




/* <auto.start.cdefs(ELS_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * ELS_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef ELS_CONFIG_INCLUDE_LOGGING
#define ELS_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * ELS_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef ELS_CONFIG_LOG_OPTIONS_DEFAULT
#define ELS_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * ELS_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log options. */


#ifndef ELS_CONFIG_LOG_BITS_DEFAULT
#define ELS_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log options. */


#ifndef ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * ELS_CONFIG_LOG_OUTPUT_DEFAULT
 *
 * Default output function for log messages. Can be changed at runtime. */


#ifndef ELS_CONFIG_LOG_OUTPUT_DEFAULT
#define ELS_CONFIG_LOG_OUTPUT_DEFAULT ELS_PRINTF
#endif

/**
 * ELS_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef ELS_CONFIG_PORTING_STDLIB
#define ELS_CONFIG_PORTING_STDLIB 1
#endif

/**
 * ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS ELS_CONFIG_PORTING_STDLIB
#endif

/**
 * ELS_CONFIG_INCLUDE_UCLI
 *
 * Include generic UCLI support. */


#ifndef ELS_CONFIG_INCLUDE_UCLI
#define ELS_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * ELS_CONFIG_INCLUDE_EVENTFD
 *
 * Include eventfd synchronization. */


#ifndef ELS_CONFIG_INCLUDE_EVENTFD
#define ELS_CONFIG_INCLUDE_EVENTFD 1
#endif

/**
 * ELS_CONFIG_MAX_PROMPT
 *
 * The maximum prompt length in characters. */


#ifndef ELS_CONFIG_MAX_PROMPT
#define ELS_CONFIG_MAX_PROMPT 512
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct els_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} els_config_settings_t;

/** Configuration settings table. */
/** els_config_settings table. */
extern els_config_settings_t els_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* els_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int els_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(ELS_CONFIG_HEADER).header> */



#include "els_porting.h"
#endif /* __ELS_CONFIG_H__ */
/*@}*/
