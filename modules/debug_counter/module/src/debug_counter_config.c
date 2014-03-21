/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <debug_counter/debug_counter_config.h>

/* <auto.start.cdefs(DEBUG_COUNTER_CONFIG_HEADER).source> */
#define __debug_counter_config_STRINGIFY_NAME(_x) #_x
#define __debug_counter_config_STRINGIFY_VALUE(_x) __debug_counter_config_STRINGIFY_NAME(_x)
debug_counter_config_settings_t debug_counter_config_settings[] =
{
#ifdef DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING) },
#else
{ DEBUG_COUNTER_CONFIG_INCLUDE_LOGGING(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ DEBUG_COUNTER_CONFIG_LOG_OPTIONS_DEFAULT(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT) },
#else
{ DEBUG_COUNTER_CONFIG_LOG_BITS_DEFAULT(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ DEBUG_COUNTER_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef DEBUG_COUNTER_CONFIG_PORTING_STDLIB
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_PORTING_STDLIB), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_PORTING_STDLIB) },
#else
{ DEBUG_COUNTER_CONFIG_PORTING_STDLIB(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef DEBUG_COUNTER_CONFIG_INCLUDE_UCLI
    { __debug_counter_config_STRINGIFY_NAME(DEBUG_COUNTER_CONFIG_INCLUDE_UCLI), __debug_counter_config_STRINGIFY_VALUE(DEBUG_COUNTER_CONFIG_INCLUDE_UCLI) },
#else
{ DEBUG_COUNTER_CONFIG_INCLUDE_UCLI(__debug_counter_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __debug_counter_config_STRINGIFY_VALUE
#undef __debug_counter_config_STRINGIFY_NAME

const char*
debug_counter_config_lookup(const char* setting)
{
    int i;
    for(i = 0; debug_counter_config_settings[i].name; i++) {
        if(strcmp(debug_counter_config_settings[i].name, setting)) {
            return debug_counter_config_settings[i].value;
        }
    }
    return NULL;
}

int
debug_counter_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; debug_counter_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", debug_counter_config_settings[i].name, debug_counter_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(DEBUG_COUNTER_CONFIG_HEADER).source> */

