/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <debug_counter/debug_counter_config.h>
#include <debug_counter/debug_counter.h>
#include <inttypes.h>

#if DEBUG_COUNTER_CONFIG_INCLUDE_UCLI == 1

#include <uCli/ucli.h>
#include <uCli/ucli_argparse.h>
#include <uCli/ucli_handler_macros.h>

static ucli_status_t
debug_counter_ucli_ucli__config__(ucli_context_t* uc)
{
    UCLI_HANDLER_MACRO_MODULE_CONFIG(debug_counter)
}

static ucli_status_t
debug_counter_ucli_ucli__show__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "show", 0,
                      "$summary#show all debug counters.");
    list_head_t *counters = debug_counter_list();
    list_links_t *cur;
    LIST_FOREACH(counters, cur) {
	debug_counter_t *counter = container_of(cur, links, debug_counter_t);
	ucli_printf(uc, "%s: %"PRIu64"\n", counter->name, counter->value);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
debug_counter_ucli_ucli__describe__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "describe", 0,
                      "$summary#show description of all debug counters.");
    list_head_t *counters = debug_counter_list();
    list_links_t *cur;
    LIST_FOREACH(counters, cur) {
	debug_counter_t *counter = container_of(cur, links, debug_counter_t);
	if (cur != counters->links.next) {
	    ucli_printf(uc, "\n", counter->name);
	}
	ucli_printf(uc, "%s\n", counter->name);
	ucli_printf(uc, "Value: %"PRIu64"\n", counter->value);
	ucli_printf(uc, "Description: %s\n", counter->description);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
debug_counter_ucli_ucli__reset__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "reset", 0,
                      "$summary#reset all debug counters to zero.");
    list_head_t *counters = debug_counter_list();
    list_links_t *cur;
    LIST_FOREACH(counters, cur) {
	debug_counter_t *counter = container_of(cur, links, debug_counter_t);
	debug_counter_reset(counter);
    }
    return UCLI_STATUS_OK;
}

/* <auto.ucli.handlers.start> */
static ucli_command_handler_f debug_counter_ucli_ucli_handlers__[] =
{
    debug_counter_ucli_ucli__config__,
    debug_counter_ucli_ucli__show__,
    debug_counter_ucli_ucli__describe__,
    debug_counter_ucli_ucli__reset__,
    NULL
};
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

