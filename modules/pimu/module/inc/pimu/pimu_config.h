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
 * @brief pimu Configuration Header
 *
 * @addtogroup pimu-config
 * @{
 *
 ***************************************************************/
#ifndef __PIMU_CONFIG_H__
#define __PIMU_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef PIMU_INCLUDE_CUSTOM_CONFIG
#include <pimu_custom_config.h>
#endif

/* <auto.start.cdefs(PIMU_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * PIMU_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef PIMU_CONFIG_INCLUDE_LOGGING
#define PIMU_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * PIMU_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef PIMU_CONFIG_LOG_OPTIONS_DEFAULT
#define PIMU_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * PIMU_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef PIMU_CONFIG_LOG_BITS_DEFAULT
#define PIMU_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * PIMU_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef PIMU_CONFIG_PORTING_STDLIB
#define PIMU_CONFIG_PORTING_STDLIB 1
#endif

/**
 * PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS PIMU_CONFIG_PORTING_STDLIB
#endif

/**
 * PIMU_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef PIMU_CONFIG_INCLUDE_UCLI
#define PIMU_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * PIMU_CONFIG_PACKET_KEY_SIZE
 *
 * Size of each packet key. */


#ifndef PIMU_CONFIG_PACKET_KEY_SIZE
#define PIMU_CONFIG_PACKET_KEY_SIZE 64
#endif

/**
 * PIMU_CONFIG_GROUP_COUNT
 *
 * Maximum number of supported groups. */


#ifndef PIMU_CONFIG_GROUP_COUNT
#define PIMU_CONFIG_GROUP_COUNT 64
#endif

/**
 * PIMU_CONFIG_MAX_ETHER_TYPES
 *
 * Maximum number of prioritized ethernet types. */


#ifndef PIMU_CONFIG_MAX_ETHER_TYPES
#define PIMU_CONFIG_MAX_ETHER_TYPES 8
#endif

/**
 * PIMU_CONFIG_MAX_ETHER_DAS
 *
 * Maximum number of prioritized ethernet destination addresses. */


#ifndef PIMU_CONFIG_MAX_ETHER_DAS
#define PIMU_CONFIG_MAX_ETHER_DAS 8
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct pimu_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} pimu_config_settings_t;

/** Configuration settings table. */
/** pimu_config_settings table. */
extern pimu_config_settings_t pimu_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* pimu_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int pimu_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(PIMU_CONFIG_HEADER).header> */

#include "pimu_porting.h"

#endif /* __PIMU_CONFIG_H__ */
/* @} */
