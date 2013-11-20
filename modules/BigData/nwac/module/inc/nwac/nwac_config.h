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
 * @brief nwac Configuration Header
 *
 * @addtogroup nwac-config
 * @{
 *
 ***************************************************************/
#ifndef __NWAC_CONFIG_H__
#define __NWAC_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef NWAC_INCLUDE_CUSTOM_CONFIG
#include <nwac_custom_config.h>
#endif

/* <auto.start.cdefs(NWAC_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * NWAC_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef NWAC_CONFIG_INCLUDE_LOGGING
#define NWAC_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * NWAC_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef NWAC_CONFIG_LOG_OPTIONS_DEFAULT
#define NWAC_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * NWAC_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef NWAC_CONFIG_LOG_BITS_DEFAULT
#define NWAC_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * NWAC_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef NWAC_CONFIG_PORTING_STDLIB
#define NWAC_CONFIG_PORTING_STDLIB 1
#endif

/**
 * NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS NWAC_CONFIG_PORTING_STDLIB
#endif

/**
 * NWAC_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef NWAC_CONFIG_INCLUDE_UCLI
#define NWAC_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * NWAC_CONFIG_INCLUDE_LOCKING
 *
 * Include locking support. */


#ifndef NWAC_CONFIG_INCLUDE_LOCKING
#define NWAC_CONFIG_INCLUDE_LOCKING 1
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct nwac_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} nwac_config_settings_t;

/** Configuration settings table. */
/** nwac_config_settings table. */
extern nwac_config_settings_t nwac_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* nwac_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int nwac_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(NWAC_CONFIG_HEADER).header> */

#include "nwac_porting.h"

#endif /* __NWAC_CONFIG_H__ */
/* @} */
