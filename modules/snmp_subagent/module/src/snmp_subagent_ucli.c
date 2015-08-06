/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <snmp_subagent/snmp_subagent_config.h>

#if SNMP_SUBAGENT_CONFIG_INCLUDE_UCLI == 1

#include <uCli/ucli.h>
#include <uCli/ucli_argparse.h>
#include <uCli/ucli_handler_macros.h>

static ucli_status_t
snmp_subagent_ucli_ucli__config__(ucli_context_t* uc)
{
    UCLI_HANDLER_MACRO_MODULE_CONFIG(snmp_subagent)
}

/* <auto.ucli.handlers.start> */
/* <auto.ucli.handlers.end> */

static ucli_module_t
snmp_subagent_ucli_module__ =
    {
        "snmp_subagent_ucli",
        NULL,
        snmp_subagent_ucli_ucli_handlers__,
        NULL,
        NULL,
    };

ucli_node_t*
snmp_subagent_ucli_node_create(void)
{
    ucli_node_t* n;
    ucli_module_init(&snmp_subagent_ucli_module__);
    n = ucli_node_create("snmp_subagent", NULL, &snmp_subagent_ucli_module__);
    ucli_node_subnode_add(n, ucli_module_log_node_create("snmp_subagent"));
    return n;
}

#else
void*
snmp_subagent_ucli_node_create(void)
{
    return NULL;
}
#endif

