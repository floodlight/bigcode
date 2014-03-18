/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <debug_counter/debug_counter_config.h>

#if DEBUG_COUNTER_CONFIG_INCLUDE_UCLI == 1

#include <uCli/ucli.h>
#include <uCli/ucli_argparse.h>
#include <uCli/ucli_handler_macros.h>

static ucli_status_t
debug_counter_ucli_ucli__config__(ucli_context_t* uc)
{
    UCLI_HANDLER_MACRO_MODULE_CONFIG(debug_counter)
}

/* <auto.ucli.handlers.start> */
/* <auto.ucli.handlers.end> */

static ucli_module_t
debug_counter_ucli_module__ =
    {
        "debug_counter_ucli",
        NULL,
        debug_counter_ucli_ucli_handlers__,
        NULL,
        NULL,
    };

ucli_node_t*
debug_counter_ucli_node_create(void)
{
    ucli_node_t* n;
    ucli_module_init(&debug_counter_ucli_module__);
    n = ucli_node_create("debug_counter", NULL, &debug_counter_ucli_module__);
    ucli_node_subnode_add(n, ucli_module_log_node_create("debug_counter"));
    return n;
}

#else
void*
debug_counter_ucli_node_create(void)
{
    return NULL;
}
#endif

