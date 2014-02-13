/**************************************************************************//**
 *
 * @file
 * @brief timer_wheel Configuration Header
 *
 * @addtogroup timer_wheel-config
 * @{
 *
 *****************************************************************************/
#ifndef __TIMER_WHEEL_CONFIG_H__
#define __TIMER_WHEEL_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef TIMER_WHEEL_INCLUDE_CUSTOM_CONFIG
#include <timer_wheel_custom_config.h>
#endif

/* <auto.start.cdefs(TIMER_WHEEL_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * TIMER_WHEEL_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef TIMER_WHEEL_CONFIG_INCLUDE_LOGGING
#define TIMER_WHEEL_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT
#define TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT
#define TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * TIMER_WHEEL_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef TIMER_WHEEL_CONFIG_PORTING_STDLIB
#define TIMER_WHEEL_CONFIG_PORTING_STDLIB 1
#endif

/**
 * TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS TIMER_WHEEL_CONFIG_PORTING_STDLIB
#endif

/**
 * TIMER_WHEEL_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef TIMER_WHEEL_CONFIG_INCLUDE_UCLI
#define TIMER_WHEEL_CONFIG_INCLUDE_UCLI 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct timer_wheel_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} timer_wheel_config_settings_t;

/** Configuration settings table. */
/** timer_wheel_config_settings table. */
extern timer_wheel_config_settings_t timer_wheel_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* timer_wheel_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int timer_wheel_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(TIMER_WHEEL_CONFIG_HEADER).header> */

#include "timer_wheel_porting.h"

#endif /* __TIMER_WHEEL_CONFIG_H__ */
/* @} */
