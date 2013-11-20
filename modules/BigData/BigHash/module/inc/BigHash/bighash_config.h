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
 * @brief BigHash Configuration Header
 *
 * @addtogroup bighash-config
 * @{
 *
 ***************************************************************/
#ifndef __BIGHASH_CONFIG_H__
#define __BIGHASH_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef BIGHASH_INCLUDE_CUSTOM_CONFIG
#include <bighash_custom_config.h>
#endif

/* <auto.start.cdefs(BIGHASH_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * BIGHASH_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef BIGHASH_CONFIG_INCLUDE_LOGGING
#define BIGHASH_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT
#define BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * BIGHASH_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef BIGHASH_CONFIG_LOG_BITS_DEFAULT
#define BIGHASH_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * BIGHASH_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef BIGHASH_CONFIG_PORTING_STDLIB
#define BIGHASH_CONFIG_PORTING_STDLIB 1
#endif

/**
 * BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS BIGHASH_CONFIG_PORTING_STDLIB
#endif

/**
 * BIGHASH_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef BIGHASH_CONFIG_INCLUDE_UCLI
#define BIGHASH_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * BIGHASH_CONFIG_INCLUDE_LOCKING
 *
 * Include table syncronization and locking. */


#ifndef BIGHASH_CONFIG_INCLUDE_LOCKING
#define BIGHASH_CONFIG_INCLUDE_LOCKING 1
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct bighash_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} bighash_config_settings_t;

/** Configuration settings table. */
/** bighash_config_settings table. */
extern bighash_config_settings_t bighash_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* bighash_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int bighash_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(BIGHASH_CONFIG_HEADER).header> */

#include "bighash_porting.h"

#endif /* __BIGHASH_CONFIG_H__ */
/* @} */
