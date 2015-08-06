/**************************************************************************//**
 *
 * @file
 * @brief snmp_subagent Configuration Header
 *
 * @addtogroup snmp_subagent-config
 * @{
 *
 *****************************************************************************/
#ifndef __SNMP_SUBAGENT_CONFIG_H__
#define __SNMP_SUBAGENT_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif
#ifdef SNMP_SUBAGENT_INCLUDE_CUSTOM_CONFIG
#include <snmp_subagent_custom_config.h>
#endif

/* <auto.start.cdefs(SNMP_SUBAGENT_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING
#define SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT
#define SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log bits. */


#ifndef SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT
#define SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_DEFAULT
#endif

/**
 * SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log bits. */


#ifndef SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * SNMP_SUBAGENT_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef SNMP_SUBAGENT_CONFIG_PORTING_STDLIB
#define SNMP_SUBAGENT_CONFIG_PORTING_STDLIB 1
#endif

/**
 * SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS SNMP_SUBAGENT_CONFIG_PORTING_STDLIB
#endif

/**
 * SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI
 *
 * Include generic uCli support. */


#ifndef SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI
#define SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI 0
#endif

/**
 * SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT
 *
 * Include the example subagent client (equivalent of nstAgentSubagentObject) */


#ifndef SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT
#define SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT 0
#endif

/**
 * SNMP_SUBAGENT_CONFIG_AGENT_NAME
 *
 * Subagent name. */


#ifndef SNMP_SUBAGENT_CONFIG_AGENT_NAME
#define SNMP_SUBAGENT_CONFIG_AGENT_NAME "snmp-subagent"
#endif

/**
 * SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION
 *
 * Subagent description. */


#ifndef SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION
#define SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION "snmp-subagent"
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct snmp_subagent_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} snmp_subagent_config_settings_t;

/** Configuration settings table. */
/** snmp_subagent_config_settings table. */
extern snmp_subagent_config_settings_t snmp_subagent_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* snmp_subagent_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int snmp_subagent_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(SNMP_SUBAGENT_CONFIG_HEADER).header> */

#include "snmp_subagent_porting.h"

#endif /* __SNMP_SUBAGENT_CONFIG_H__ */
/* @} */
