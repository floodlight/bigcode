/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <timer_wheel/timer_wheel_config.h>

/* <auto.start.cdefs(TIMER_WHEEL_CONFIG_HEADER).source> */
#define __timer_wheel_config_STRINGIFY_NAME(_x) #_x
#define __timer_wheel_config_STRINGIFY_VALUE(_x) __timer_wheel_config_STRINGIFY_NAME(_x)
timer_wheel_config_settings_t timer_wheel_config_settings[] =
{
#ifdef TIMER_WHEEL_CONFIG_INCLUDE_LOGGING
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_INCLUDE_LOGGING), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_INCLUDE_LOGGING) },
#else
{ TIMER_WHEEL_CONFIG_INCLUDE_LOGGING(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ TIMER_WHEEL_CONFIG_LOG_OPTIONS_DEFAULT(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT) },
#else
{ TIMER_WHEEL_CONFIG_LOG_BITS_DEFAULT(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ TIMER_WHEEL_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef TIMER_WHEEL_CONFIG_PORTING_STDLIB
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_PORTING_STDLIB), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_PORTING_STDLIB) },
#else
{ TIMER_WHEEL_CONFIG_PORTING_STDLIB(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef TIMER_WHEEL_CONFIG_INCLUDE_UCLI
    { __timer_wheel_config_STRINGIFY_NAME(TIMER_WHEEL_CONFIG_INCLUDE_UCLI), __timer_wheel_config_STRINGIFY_VALUE(TIMER_WHEEL_CONFIG_INCLUDE_UCLI) },
#else
{ TIMER_WHEEL_CONFIG_INCLUDE_UCLI(__timer_wheel_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __timer_wheel_config_STRINGIFY_VALUE
#undef __timer_wheel_config_STRINGIFY_NAME

const char*
timer_wheel_config_lookup(const char* setting)
{
    int i;
    for(i = 0; timer_wheel_config_settings[i].name; i++) {
        if(!strcmp(timer_wheel_config_settings[i].name, setting)) {
            return timer_wheel_config_settings[i].value;
        }
    }
    return NULL;
}

int
timer_wheel_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; timer_wheel_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", timer_wheel_config_settings[i].name, timer_wheel_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(TIMER_WHEEL_CONFIG_HEADER).source> */

