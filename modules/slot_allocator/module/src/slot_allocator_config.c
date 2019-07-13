/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <slot_allocator/slot_allocator_config.h>

/* <auto.start.cdefs(INDEX_ALLOCATOR_CONFIG_HEADER).source> */
#define __slot_allocator_config_STRINGIFY_NAME(_x) #_x
#define __slot_allocator_config_STRINGIFY_VALUE(_x) __slot_allocator_config_STRINGIFY_NAME(_x)
slot_allocator_config_settings_t slot_allocator_config_settings[] =
{
#ifdef INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING) },
#else
{ INDEX_ALLOCATOR_CONFIG_INCLUDE_LOGGING(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ INDEX_ALLOCATOR_CONFIG_LOG_OPTIONS_DEFAULT(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT) },
#else
{ INDEX_ALLOCATOR_CONFIG_LOG_BITS_DEFAULT(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ INDEX_ALLOCATOR_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB) },
#else
{ INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI
    { __slot_allocator_config_STRINGIFY_NAME(INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI), __slot_allocator_config_STRINGIFY_VALUE(INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI) },
#else
{ INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI(__slot_allocator_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __slot_allocator_config_STRINGIFY_VALUE
#undef __slot_allocator_config_STRINGIFY_NAME

const char*
slot_allocator_config_lookup(const char* setting)
{
    int i;
    for(i = 0; slot_allocator_config_settings[i].name; i++) {
        if(!strcmp(slot_allocator_config_settings[i].name, setting)) {
            return slot_allocator_config_settings[i].value;
        }
    }
    return NULL;
}

int
slot_allocator_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; slot_allocator_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", slot_allocator_config_settings[i].name, slot_allocator_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(INDEX_ALLOCATOR_CONFIG_HEADER).source> */

