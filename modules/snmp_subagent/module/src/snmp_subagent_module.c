/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <snmp_subagent/snmp_subagent_config.h>

#include "snmp_subagent_log.h"

static int
datatypes_init__(void)
{
#define SNMP_SUBAGENT_ENUMERATION_ENTRY(_enum_name, _desc)     AIM_DATATYPE_MAP_REGISTER(_enum_name, _enum_name##_map, _desc,                               AIM_LOG_INTERNAL);
#include <snmp_subagent/snmp_subagent.x>
    return 0;
}

void __snmp_subagent_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
    datatypes_init__();
}

