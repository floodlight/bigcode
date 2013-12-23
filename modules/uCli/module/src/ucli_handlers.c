/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 ***************************************************************/

#include <uCli/ucli.h>
#include <uCli/ucli_handler_macros.h>
#include <AIM/aim_pvs.h>

#if AIM_CONFIG_INCLUDE_PVS_SYSLOG == 1
#include <AIM/aim_pvs_syslog.h>
#endif


/**
 * Generic module log node.
 */
static ucli_status_t
ucli_ucli_mlog__opt__(ucli_context_t* uc)
{
    const char* module_name;
    aim_log_t* lobj;
    char* arg;
    int enable = 1;

    module_name = (const char*)(uc->cookie);
    UCLI_COMMAND_INFO(uc, "opt", 1,
                      "$summary#Modify log options."
                      "$args#[+|-]<option>");
    UCLI_ARGPARSE_OR_RETURN(uc, "s", &arg);
    lobj = aim_log_find(module_name);

    if(lobj == NULL) {
        return ucli_error(uc, "Module %s does not have a log registered.",
                          module_name);
    }

    if(arg[0] == '+') {
        enable = 1;
        arg++;
    }
    else if(arg[0] == '-' || arg[0] == '~') {
        enable = 0;
        arg++;
    }
    if(aim_log_option_name_set(lobj, arg, enable) < 0) {
        return ucli_error(uc, "%s is not a valid log option.", arg);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ucli_ucli_mlog__mod__(ucli_context_t* uc)
{
    const char* module_name;
    aim_log_t* lobj;
    char* arg;
    int enable = 1;

    UCLI_COMMAND_INFO(uc,
                      "mod", 1,
                      "$summary#Modify module log settings."
                      "$args#[+|-]<flag>");
    UCLI_ARGPARSE_OR_RETURN(uc, "s", &arg);

    module_name = (const char*)uc->cookie;
    lobj = aim_log_find(module_name);
    if(lobj == NULL) {
        return ucli_error(uc, "Module %s does not have a log registered.",
                          module_name);
    }

    if(arg[0] == '+') {
        arg++;
        enable = 1;
    }
    else if(arg[0] == '-' || arg[0] == '~') {
        arg++;
        enable = 0;
    }
    if(aim_log_flag_set(lobj, arg, enable) == 0) {
        return ucli_error(uc, "'%s' is not a valid log type for module '%s'",
                          arg, lobj->name);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ucli_ucli_mlog__show__(ucli_context_t* uc)
{
    const char* module_name;
    aim_log_t* lobj;
    UCLI_COMMAND_INFO(uc,
                      "show", 0,
                      "$summary#Show module log settings.");

    module_name = (const char*)uc->cookie;
    lobj = aim_log_find(module_name);
    if(lobj == NULL) {
        return ucli_error(uc, "Module %s does not have a log registered.",
                          module_name);
    }
    aim_log_show(lobj, &uc->pvs);
    return UCLI_STATUS_OK;

}

static ucli_status_t
ucli_ucli_mlog__set__(ucli_context_t* uc)
{
    const char* module_name;
    aim_log_t* lobj;
    char* f = NULL;
    aim_pvs_t* pvs = NULL;

    UCLI_COMMAND_INFO(uc,
                      "set", 1,
                      "$summary#Redirect log output to the given file.");

    UCLI_ARGPARSE_OR_RETURN(uc, "s", &f);

    module_name = (const char*)uc->cookie;
    lobj = aim_log_find(module_name);

    if(lobj == NULL) {
        return ucli_error(uc, "module %s does not have a log registered.",
                          module_name);
    }

    if(!UCLI_STRCMP(f, "syslog")) {
#if AIM_CONFIG_INCLUDE_PVS_SYSLOG == 1
        pvs = aim_pvs_syslog_get();
#else
        return ucli_error(uc, "syslog is not supported in this build.");
#endif
    }
    else {
        pvs = aim_pvs_fopen(f, "a");
    }
    pvs = aim_log_pvs_set(lobj, pvs);
    /* Note -- don't destroy, it might still be in use.
     * These should be reference counted. */
    return UCLI_STATUS_OK;
}


static ucli_status_t
ucli_ucli_modules__manifest__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "manifest", 0,
                      "$summary#Show all modules included in this build.");
#define DEPENDMODULE_BUILD(_mod) \
    ucli_printf(uc, "%s\n", #_mod);
#include <dependmodules.x>
    return UCLI_STATUS_OK;
}

static ucli_status_t
ucli_ucli_alog__show__(ucli_context_t* uc)
{
    aim_log_t* lobj;
    UCLI_COMMAND_INFO(uc,
                      "show", 0,
                      "$summary#Show all module log settings.");

    for(lobj = aim_log_list(); lobj; lobj = lobj->next) {
        aim_log_show(lobj, &uc->pvs);
        ucli_printf(uc, "\n");
    }
    return UCLI_STATUS_OK;

}

static ucli_status_t
ucli_ucli_alog__opt__(ucli_context_t* uc)
{
    char* arg;
    int enable = 1;

    UCLI_COMMAND_INFO(uc, "opt", 1,
                      "$summary#Modify all log options."
                      "$args#[+|-]<option>");
    UCLI_ARGPARSE_OR_RETURN(uc, "s", &arg);

    if(arg[0] == '+') {
        enable = 1;
        arg++;
    }
    else if(arg[0] == '-' || arg[0] == '~') {
        enable = 0;
        arg++;
    }
    aim_log_option_name_set_all(arg, enable);
    return UCLI_STATUS_OK;
}


static ucli_status_t
ucli_ucli_alog__mod__(ucli_context_t* uc)
{
    char* arg;
    int enable = 1;

    UCLI_COMMAND_INFO(uc,
                      "mod", 1,
                      "$summary#Modify all module log settings."
                      "$args#[+|-]<flag>");
    UCLI_ARGPARSE_OR_RETURN(uc, "s", &arg);

    if(arg[0] == '+') {
        arg++;
        enable = 1;
    }
    else if(arg[0] == '-' || arg[0] == '~') {
        arg++;
        enable = 0;
    }
    aim_log_flag_set_all(arg, enable);
    return UCLI_STATUS_OK;
}

static ucli_status_t
ucli_ucli_alog__set__(ucli_context_t* uc)
{
    aim_pvs_t* pvs;
    char* f = NULL;

    UCLI_COMMAND_INFO(uc,
                      "set", 1,
                      "$summary#Redirect all log output to the given file.");

    UCLI_ARGPARSE_OR_RETURN(uc, "s", &f);

    if(!UCLI_STRCMP(f, "syslog")) {
#if AIM_CONFIG_INCLUDE_PVS_SYSLOG == 1
        pvs = aim_pvs_syslog_get();
#else
        return ucli_error(uc, "syslog is not supported in this build.");
#endif
    }
    else {
        pvs = aim_pvs_fopen(f, "a");
    }
    /* Possible leak of previous values */
    aim_log_pvs_set_all(pvs);
    return UCLI_STATUS_OK;
}


/* <auto.ucli.handlers.start> */
/******************************************************************************
 *
 * These handler table(s) were autogenerated from the symbols in this
 * source file.
 *
 *****************************************************************************/
static ucli_command_handler_f ucli_ucli_mlog_handlers__[] =
{
    ucli_ucli_mlog__opt__,
    ucli_ucli_mlog__mod__,
    ucli_ucli_mlog__show__,
    ucli_ucli_mlog__set__,
    NULL
};
static ucli_command_handler_f ucli_ucli_modules_handlers__[] =
{
    ucli_ucli_modules__manifest__,
    NULL
};
static ucli_command_handler_f ucli_ucli_alog_handlers__[] =
{
    ucli_ucli_alog__show__,
    ucli_ucli_alog__opt__,
    ucli_ucli_alog__mod__,
    ucli_ucli_alog__set__,
    NULL
};
/******************************************************************************/
/* <auto.ucli.handlers.end> */

ucli_module_t ucli_modules_module =
    {
        "modules",
        NULL,
        ucli_ucli_modules_handlers__,
        NULL,
        NULL
    };

ucli_module_t ucli_alog_module =
    {
        "all",
        NULL,
        ucli_ucli_alog_handlers__,
        NULL,
        NULL
    };

static void
ucli_ucli_mlog_module_destroy__(struct ucli_module_s* mod)
{
    if(mod) {
        aim_free(mod->cookie);
        aim_free(mod);
    }
}

ucli_node_t*
ucli_module_log_node_create(const char* module_name)
{
    ucli_module_t* mod = aim_zmalloc(sizeof(*mod));
    mod->name = module_name;
    mod->cookie = aim_strdup(module_name);
    mod->handlers = ucli_ucli_mlog_handlers__;
    mod->destroy = ucli_ucli_mlog_module_destroy__;
    ucli_module_init(mod);
    return ucli_node_create("log", NULL, mod);
}

ucli_node_t*
ucli_module_nodes_create(const char* node_name)
{
    ucli_node_t* n;
    ucli_module_init(&ucli_modules_module);
    n = ucli_node_create(node_name, NULL, &ucli_modules_module);
#define DEPENDMODULE_UCLI(_mod)                                         \
    {                                                                   \
        extern ucli_node_t* _mod##_ucli_node_create(void);              \
        ucli_node_t* module_node = _mod##_ucli_node_create();           \
        if(module_node) {                                               \
            ucli_node_subnode_add(n, module_node);                      \
        }                                                               \
    }
#include <dependmodules.x>

    {
        /* The 'all' module node contains commands that can affect all nodes. */
        ucli_node_t* allnode;
        ucli_node_t* all_log_node;
        ucli_module_init(&ucli_alog_module);
        all_log_node = ucli_node_create("log", NULL, &ucli_alog_module);
        allnode = ucli_node_create("all", NULL, NULL);
        ucli_node_subnode_add(allnode, all_log_node);
        ucli_node_subnode_add(n, allnode);
    }

    return n;
}
