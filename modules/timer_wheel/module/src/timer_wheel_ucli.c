/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <timer_wheel/timer_wheel_config.h>

#if TIMER_WHEEL_CONFIG_INCLUDE_UCLI == 1

#include <uCli/ucli.h>
#include <uCli/ucli_argparse.h>
#include <uCli/ucli_handler_macros.h>

static ucli_status_t
timer_wheel_ucli_ucli__config__(ucli_context_t* uc)
{
    UCLI_HANDLER_MACRO_MODULE_CONFIG(timer_wheel)
}

/* <auto.ucli.handlers.start> */
static ucli_command_handler_f timer_wheel_ucli_ucli_handlers__[] =
{
    timer_wheel_ucli_ucli__config__,
    NULL
};
/* <auto.ucli.handlers.end> */

static ucli_module_t
timer_wheel_ucli_module__ =
    {
        "timer_wheel_ucli",
        NULL,
        timer_wheel_ucli_ucli_handlers__,
        NULL,
        NULL,
    };

ucli_node_t*
timer_wheel_ucli_node_create(void)
{
    ucli_node_t* n;
    ucli_module_init(&timer_wheel_ucli_module__);
    n = ucli_node_create("timer_wheel", NULL, &timer_wheel_ucli_module__);
    ucli_node_subnode_add(n, ucli_module_log_node_create("timer_wheel"));
    return n;
}

#else
void*
timer_wheel_ucli_node_create(void)
{
    return NULL;
}
#endif

