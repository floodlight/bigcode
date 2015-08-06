/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <snmp_subagent/snmp_subagent_config.h>

/* <auto.start.cdefs(SNMP_SUBAGENT_CONFIG_HEADER).source> */
#define __snmp_subagent_config_STRINGIFY_NAME(_x) #_x
#define __snmp_subagent_config_STRINGIFY_VALUE(_x) __snmp_subagent_config_STRINGIFY_NAME(_x)
snmp_subagent_config_settings_t snmp_subagent_config_settings[] =
{
#ifdef SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING) },
#else
{ SNMP_SUBAGENT_CONFIG_INCLUDE_LOGGING(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ SNMP_SUBAGENT_CONFIG_LOG_OPTIONS_DEFAULT(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT) },
#else
{ SNMP_SUBAGENT_CONFIG_LOG_BITS_DEFAULT(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ SNMP_SUBAGENT_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_PORTING_STDLIB
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_PORTING_STDLIB), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_PORTING_STDLIB) },
#else
{ SNMP_SUBAGENT_CONFIG_PORTING_STDLIB(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI) },
#else
{ SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT) },
#else
{ SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_AGENT_NAME
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_AGENT_NAME), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_AGENT_NAME) },
#else
{ SNMP_SUBAGENT_CONFIG_AGENT_NAME(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION
    { __snmp_subagent_config_STRINGIFY_NAME(SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION), __snmp_subagent_config_STRINGIFY_VALUE(SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION) },
#else
{ SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION(__snmp_subagent_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __snmp_subagent_config_STRINGIFY_VALUE
#undef __snmp_subagent_config_STRINGIFY_NAME

const char*
snmp_subagent_config_lookup(const char* setting)
{
    int i;
    for(i = 0; snmp_subagent_config_settings[i].name; i++) {
        if(strcmp(snmp_subagent_config_settings[i].name, setting)) {
            return snmp_subagent_config_settings[i].value;
        }
    }
    return NULL;
}

int
snmp_subagent_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; snmp_subagent_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", snmp_subagent_config_settings[i].name, snmp_subagent_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(SNMP_SUBAGENT_CONFIG_HEADER).source> */

