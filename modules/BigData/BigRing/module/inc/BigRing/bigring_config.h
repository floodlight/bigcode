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
 * @brief BigRing Configuration Header
 *
 * @addtogroup bigring-config
 * @{
 *
 ***************************************************************/
#ifndef __BIGRING_CONFIG_H__
#define __BIGRING_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef BIGRING_INCLUDE_CUSTOM_CONFIG
#include <bigring_custom_config.h>
#endif

/* <auto.start.cdefs(BIGRING_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * BIGRING_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef BIGRING_CONFIG_INCLUDE_LOGGING
#define BIGRING_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * BIGRING_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef BIGRING_CONFIG_LOG_OPTIONS_DEFAULT
#define BIGRING_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * BIGRING_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef BIGRING_CONFIG_LOG_BITS_DEFAULT
#define BIGRING_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * BIGRING_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef BIGRING_CONFIG_PORTING_STDLIB
#define BIGRING_CONFIG_PORTING_STDLIB 1
#endif

/**
 * BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS BIGRING_CONFIG_PORTING_STDLIB
#endif

/**
 * BIGRING_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef BIGRING_CONFIG_INCLUDE_UCLI
#define BIGRING_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * BIGRING_CONFIG_INCLUDE_LOCKING
 *
 * Include locking syncronization. */


#ifndef BIGRING_CONFIG_INCLUDE_LOCKING
#define BIGRING_CONFIG_INCLUDE_LOCKING 1
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct bigring_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} bigring_config_settings_t;

/** Configuration settings table. */
/** bigring_config_settings table. */
extern bigring_config_settings_t bigring_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* bigring_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int bigring_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(BIGRING_CONFIG_HEADER).header> */

#include "bigring_porting.h"

#endif /* __BIGRING_CONFIG_H__ */
/* @} */
