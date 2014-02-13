/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <timer_wheel/timer_wheel_config.h>

#include "timer_wheel_log.h"

static int
datatypes_init__(void)
{
#define TIMER_WHEEL_ENUMERATION_ENTRY(_enum_name, _desc)     AIM_DATATYPE_MAP_REGISTER(_enum_name, _enum_name##_map, _desc,                               AIM_LOG_INTERNAL);
#include <timer_wheel/timer_wheel.x>
    return 0;
}

void __timer_wheel_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
    datatypes_init__();
}

