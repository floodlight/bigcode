/**************************************************************************//**
 *
 * @file
 * @brief slot_allocator Configuration Header
 *
 * @addtogroup slot_allocator-config
 * @{
 *
 *****************************************************************************/
#ifndef __INDEX_ALLOCATOR_CONFIG_H__
#define __INDEX_ALLOCATOR_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef INDEX_ALLOCATOR_INCLUDE_CUSTOM_CONFIG
#include <slot_allocator_custom_config.h>
#endif

/* <auto.start.cdefs(INDEX_ALLOCATOR_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING
#define INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT
#define INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT
#define INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB
#define INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB 1
#endif

/**
 * INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB
#endif

/**
 * INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI
#define INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct slot_allocator_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} slot_allocator_config_settings_t;

/** Configuration settings table. */
/** slot_allocator_config_settings table. */
extern slot_allocator_config_settings_t slot_allocator_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* slot_allocator_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int slot_allocator_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(INDEX_ALLOCATOR_CONFIG_HEADER).header> */

#include "slot_allocator_porting.h"

#endif /* __INDEX_ALLOCATOR_CONFIG_H__ */
/* @} */
