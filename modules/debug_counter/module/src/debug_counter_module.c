/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <debug_counter/debug_counter_config.h>

#include "debug_counter_int.h"
#include "debug_counter_log.h"

static int
datatypes_init__(void)
{
#define DEBUG_COUNTER_ENUMERATION_ENTRY(_enum_name, _desc)     AIM_DATATYPE_MAP_REGISTER(_enum_name, _enum_name##_map, _desc,                               AIM_LOG_INTERNAL);
#include <debug_counter/debug_counter.x>
    return 0;
}

void __debug_counter_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
    datatypes_init__();
    debug_counter_module_init();
}

