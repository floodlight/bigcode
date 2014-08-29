/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <slot_allocator/slot_allocator_config.h>

#if INDEX_ALLOCATOR_CONFIG_INCLUDE_UCLI == 1

#include <uCli/ucli.h>
#include <uCli/ucli_argparse.h>
#include <uCli/ucli_handler_macros.h>

static ucli_status_t
slot_allocator_ucli_ucli__config__(ucli_context_t* uc)
{
    UCLI_HANDLER_MACRO_MODULE_CONFIG(slot_allocator)
}

/* <auto.ucli.handlers.start> */
/* <auto.ucli.handlers.end> */

static ucli_module_t
slot_allocator_ucli_module__ =
    {
        "slot_allocator_ucli",
        NULL,
        slot_allocator_ucli_ucli_handlers__,
        NULL,
        NULL,
    };

ucli_node_t*
slot_allocator_ucli_node_create(void)
{
    ucli_node_t* n;
    ucli_module_init(&slot_allocator_ucli_module__);
    n = ucli_node_create("slot_allocator", NULL, &slot_allocator_ucli_module__);
    ucli_node_subnode_add(n, ucli_module_log_node_create("slot_allocator"));
    return n;
}

#else
void*
slot_allocator_ucli_node_create(void)
{
    return NULL;
}
#endif

