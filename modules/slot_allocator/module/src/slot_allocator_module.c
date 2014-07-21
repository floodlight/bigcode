/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <slot_allocator/slot_allocator_config.h>

#include "slot_allocator_log.h"

static int
datatypes_init__(void)
{
#define INDEX_ALLOCATOR_ENUMERATION_ENTRY(_enum_name, _desc)     AIM_DATATYPE_MAP_REGISTER(_enum_name, _enum_name##_map, _desc,                               AIM_LOG_INTERNAL);
#include <slot_allocator/slot_allocator.x>
    return 0;
}

void __slot_allocator_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
    datatypes_init__();
}

