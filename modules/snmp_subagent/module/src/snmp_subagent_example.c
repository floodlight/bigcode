#include <snmp_subagent/snmp_subagent_config.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "snmp_subagent_log.h"

int
snmp_subagent_example_client(int enable, void* cookie)
{
    static int value = 42;  /* default value */
    oid  example_oid[] =
        { 1, 3, 6, 1, 4, 1, 37538, 37538, 37538, 37538, 37538, 0 };

    netsnmp_register_int_instance("snmp_subagent_example_client_integer",
                                  example_oid,
                                  OID_LENGTH(example_oid),
                                  &value, NULL);
    return 0;
}
