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
 * @brief FME Configuration Definitions.
 *
 *
 * @addtogroup fme-config
 * @{
 *
 ***************************************************************/
#ifndef __FME_CONFIG_H__
#define __FME_CONFIG_H__


#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef FME_INCLUDE_CUSTOM_CONFIG
#include <fme_custom_config.h>
#endif

/**
 * This module requires POSIX (for performance timing only)
 */
#define AIM_CONFIG_INCLUDE_POSIX 1


/* <auto.start.cdefs(FME_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * FME_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef FME_CONFIG_INCLUDE_LOGGING
#define FME_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * FME_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef FME_CONFIG_LOG_OPTIONS_DEFAULT
#define FME_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * FME_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log options. */


#ifndef FME_CONFIG_LOG_BITS_DEFAULT
#define FME_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log options. */


#ifndef FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * FME_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef FME_CONFIG_PORTING_STDLIB
#define FME_CONFIG_PORTING_STDLIB 1
#endif

/**
 * FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS FME_CONFIG_PORTING_STDLIB
#endif

/**
 * FME_CONFIG_INCLUDE_UCLI
 *
 * Include Generic UCLI support. */


#ifndef FME_CONFIG_INCLUDE_UCLI
#define FME_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * FME_CONFIG_INCLUDE_UTM
 *
 * Include the FME Unit Test Module. */


#ifndef FME_CONFIG_INCLUDE_UTM
#define FME_CONFIG_INCLUDE_UTM 0
#endif

/**
 * FME_CONFIG_KEY_SIZE_WORDS
 *
 * The maximum FME key size (in words). */


#ifndef FME_CONFIG_KEY_SIZE_WORDS
#define FME_CONFIG_KEY_SIZE_WORDS 20
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct fme_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} fme_config_settings_t;

/** Configuration settings table. */
/** fme_config_settings table. */
extern fme_config_settings_t fme_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* fme_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int fme_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(FME_CONFIG_HEADER).header> */



#include "fme_porting.h"
#endif /* __FME_CONFIG_H__ */
/*@}*/
