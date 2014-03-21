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

static bool
prefix_match(const char *haystack, const char *needle)
{
    return needle == NULL || strstr(haystack, needle) == haystack;
}

static ucli_status_t
debug_counter_ucli_ucli__config__(ucli_context_t* uc)
{
    UCLI_HANDLER_MACRO_MODULE_CONFIG(debug_counter)
}

static ucli_status_t
debug_counter_ucli_ucli__show__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "show", -1,
                      "$summary#show all debug counters.");

    const char *prefix = NULL;
    if (uc->pargs->count > 0) {
	UCLI_ARGPARSE_OR_RETURN(uc, "s", &prefix);
    }

    list_head_t *counters = debug_counter_list();
    list_links_t *cur;
    LIST_FOREACH(counters, cur) {
	debug_counter_t *counter = container_of(cur, links, debug_counter_t);
	if (prefix_match(counter->name, prefix)) {
	    ucli_printf(uc, "%s: %"PRIu64"\n", counter->name, counter->value);
	}
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
debug_counter_ucli_ucli__describe__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "describe", -1,
                      "$summary#show description of all debug counters.");
    list_head_t *counters = debug_counter_list();
    list_links_t *cur;
    bool first = true;

    const char *prefix = NULL;
    if (uc->pargs->count > 0) {
	UCLI_ARGPARSE_OR_RETURN(uc, "s", &prefix);
    }

    LIST_FOREACH(counters, cur) {
	debug_counter_t *counter = container_of(cur, links, debug_counter_t);
	if (prefix_match(counter->name, prefix)) {
	    if (first) {
		first = false;
	    } else {
		ucli_printf(uc, "\n");
	    }
	    ucli_printf(uc, "%s\n", counter->name);
	    ucli_printf(uc, "Value: %"PRIu64"\n", counter->value);
	    ucli_printf(uc, "Description: %s\n", counter->description);
	}
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
debug_counter_ucli_ucli__reset__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "reset", -1,
                      "$summary#reset all debug counters to zero.");
    list_head_t *counters = debug_counter_list();
    list_links_t *cur;

    const char *prefix = NULL;
    if (uc->pargs->count > 0) {
	UCLI_ARGPARSE_OR_RETURN(uc, "s", &prefix);
    }

    LIST_FOREACH(counters, cur) {
	debug_counter_t *counter = container_of(cur, links, debug_counter_t);
	if (prefix_match(counter->name, prefix)) {
	    debug_counter_reset(counter);
	}
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

