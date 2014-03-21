/**************************************************************************//**
 *
 * @file
 * @brief debug_counter Configuration Header
 *
 * @addtogroup debug_counter-config
 * @{
 *
 *****************************************************************************/
#ifndef __DEBUG_COUNTER_CONFIG_H__
#define __DEBUG_COUNTER_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef DEBUG_COUNTER_INCLUDE_CUSTOM_CONFIG
#include <debug_counter_custom_config.h>
#endif

/* <auto.start.cdefs(DEBUG_COUNTER_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING
#define DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT
#define DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT
#define DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * DEBUG_COUNTER_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef DEBUG_COUNTER_CONFIG_PORTING_STDLIB
#define DEBUG_COUNTER_CONFIG_PORTING_STDLIB 1
#endif

/**
 * DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS DEBUG_COUNTER_CONFIG_PORTING_STDLIB
#endif

/**
 * DEBUG_COUNTER_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef DEBUG_COUNTER_CONFIG_INCLUDE_UCLI
#define DEBUG_COUNTER_CONFIG_INCLUDE_UCLI 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct debug_counter_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} debug_counter_config_settings_t;

/** Configuration settings table. */
/** debug_counter_config_settings table. */
extern debug_counter_config_settings_t debug_counter_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* debug_counter_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int debug_counter_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(DEBUG_COUNTER_CONFIG_HEADER).header> */

#include "debug_counter_porting.h"

#endif /* __DEBUG_COUNTER_CONFIG_H__ */
/* @} */
