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

#include "ucli_int.h"

#include <uCli/ucli.h>
#include "ucli_log.h"

#include <stdarg.h>
#include <stdio.h>

#if UCLI_CONFIG_INCLUDE_PTHREADS == 1

#include <pthread.h>

/**
 * Context for handlers executing in their own threads.
 */
typedef struct ucli_thread_context_s {
    ucli_command_t* cp;
    ucli_context_t* uc;
} ucli_thread_context_t;

/**
 * Threaded handler execution.
 */
static void*
handler_thread__(void* p)
{
    ucli_thread_context_t* utc = (ucli_thread_context_t*)p;
    utc->cp->handler(utc->uc);
    aim_free(utc->uc);
    aim_free(utc);
    return NULL;
}

/**
 * Execute a command handler.
 * Most are executed in the current thread.
 * Some may request to be executed in a separate thread as a background hander.
 */
static ucli_status_t
handler_execute__(ucli_command_t* cp, ucli_context_t* uc)
{
    if(cp->threaded == 0) {
        /** foreground handler */
        return cp->handler(uc);
    }
    else {
        /**
         * background handler
         * Todo: child thread management
         */
        pthread_t pth;
        ucli_thread_context_t* ctx = aim_zmalloc(sizeof(*ctx));
        ctx->cp = cp;
        ctx->uc = aim_zmalloc(sizeof(*(ctx->uc)));
        ctx->uc->epvs = uc->epvs;
        ctx->uc->pvs = uc->pvs;
        ctx->uc->cookie = uc->cookie;
        pthread_create(&pth, NULL, handler_thread__, ctx);
        return UCLI_STATUS_OK;
    }
}

#else

/**
 * Execute a command handler (no thread support.)
 */
static ucli_status_t
handler_execute__(ucli_command_t* cp, ucli_context_t* uc)
{
    return cp->handler(uc);
}

#endif /* UCLI_CONFIG_INCLUDE_PTHREADS */



/**************************************************************************//**
 *
 * Dispatch arguments to the given module.
 *
 *
 *****************************************************************************/

ucli_status_t
ucli_module_dispatch(ucli_module_t* mod,
                     ucli_context_t* uc)
{
    ucli_status_t rv;
    ucli_command_t* cp;
    biglist_t* ble;

    BIGLIST_FOREACH_DATA(ble, mod->command_list, ucli_command_t*, cp) {
        if(!UCLI_STRCMP(cp->command, uc->pargs->args[0])) {

            if(cp->handler) {

                if(cp->arg_count >= 0 &&
                   uc->pargs->count != (cp->arg_count + 1)) {
                    cp->fail_count++;
                    return ucli_error(uc, "%d arguments are required for command '%s' (%d given)",
                                      cp->arg_count, cp->command, uc->pargs->count-1);

                }

                uc->pargs->args++;
                uc->pargs->count--;

                uc->cookie = mod->cookie;

                rv = handler_execute__(cp, uc);

                if(rv < 0) {
                    cp->fail_count++;
                }
                else {
                    cp->success_count++;
                }

                uc->pargs->count++;
                uc->pargs->args--;

                return rv;

            }
            else {
                return UCLI_STATUS_E_INTERNAL;
            }
        }
    }
    return UCLI_STATUS_E_NOT_FOUND;
}

/**
 * Attempt to complete the input tokens.
 */
ucli_completion_t*
ucli_module_complete(ucli_module_t* mod, const char** tokens)
{
    ucli_command_t* cp;
    ucli_completion_t* ucomp;
    biglist_t* ble;
    biglist_t* possibilities = NULL;
    int count = 0;

    BIGLIST_FOREACH_DATA(ble, mod->command_list, ucli_command_t*, cp) {
        if(!UCLI_STRCMP(cp->command, tokens[0])) {
            /* Exact match already */
            break;
        }
    }

    if(cp) {
        /*
         * First token is an exact match for this command.
         * At this point we just provide the argument help string, if available.
         */
        char* help;
        possibilities = NULL;
        count = UCLI_STRLEN(cp->command)+UCLI_STRLEN(cp->help.args)+2;

        help = aim_zmalloc(count);
        aim_snprintf(help, count, "%s %s", cp->command, cp->help.args);
        possibilities = biglist_append(possibilities, help);
        ucomp = aim_zmalloc(sizeof(*ucomp));
        ucomp->possibilities.list = possibilities;
        ucomp->possibilities.count = 1;
        return ucomp;
    }

    /*
     * Determine the list of possible matches.
     */

    possibilities = NULL;
    count = 0;

    BIGLIST_FOREACH_DATA(ble, mod->command_list, ucli_command_t*, cp) {
        if(!UCLI_STRNCMP(cp->command, tokens[0],
                         UCLI_STRLEN(tokens[0]))) {
            possibilities = biglist_append(possibilities,
                                           aim_strdup(cp->command));
            count++;
        }
    }

    if(count == 0) {
        /* No matches at all */
        return NULL;
    }
    else {
        ucomp = aim_zmalloc(sizeof(*ucomp));
        ucomp->possibilities.list = possibilities;
        ucomp->possibilities.count = count;
        if(count == 1) {
            /* Provide full completion information */
            ucomp->next_token = aim_strdup((char*)(ucomp->possibilities.list->data));
            ucomp->append_string = aim_strdup((ucomp->next_token+
                                               UCLI_STRLEN(tokens[0])));
        }
        return ucomp;
    }
}

int
ucli_module_help(ucli_module_t* mod, aim_pvs_t* pvs)
{
    ucli_command_t* cp;
    biglist_t* ble;

    if(mod == NULL) {
        return -1;
    }

    BIGLIST_FOREACH_DATA(ble, mod->command_list, ucli_command_t*, cp) {
        aim_printf(pvs, "  %-16s%s\n",
                   cp->command, cp->help.summary ? cp->help.summary : "");
    }

    return 0;
}

static void
ucli_module_command_list_dump__(biglist_t* list, aim_pvs_t* ap)
{
    iof_t iof;
    biglist_t* ble;
    ucli_command_t* cp;

    iof_init(&iof, ap);
    BIGLIST_FOREACH_DATA(ble, list, ucli_command_t*, cp) {
        iof_iprintfn(&iof, "%s", cp->command);
        if(cp->group) {
            iof_uprintf(&iof, " [ %s ]", cp->group);
        }
        iof_uprintf(&iof, "\n");
    }
}

int
ucli_module_dump(ucli_module_t* mod, aim_pvs_t* ap)
{
    iof_t iof;

    iof_init(&iof, ap);
    iof_push(&iof, "module@%p: \"%s\"", mod, mod->name);
    iof_push(&iof, "commands");
    ucli_module_command_list_dump__(mod->command_list, &iof.inherit);
    iof_pop(&iof);
    iof_push(&iof, "disabled");
    ucli_module_command_list_dump__(mod->disabled_list, &iof.inherit);
    iof_pop(&iof);
    return 0;
}

void
initcmd__(ucli_command_t* cmd)
{
    if(cmd->initspec__) {
        if(UCLI_STRCHR(cmd->initspec__, '$') == NULL) {
            /* Assume command summary. */
            cmd->help.summary = aim_strdup(cmd->initspec__);
        }
        else {

            /*
             * Split by keyword data
             */
            ucli_tokens_t* specs = ucli_tokens(cmd->initspec__, "$");
            const char** s;

            /*
             * Split by keyword and value
             */
            for(s = specs->tokens; *s; s++) {
                ucli_tokens_t* option = ucli_tokens(*s, "#");
                if(option->count != 2) {
                    UCLI_MWARN("bad init spec for command '%s'", cmd->command);
                }
                else if(!UCLI_STRCMP(option->tokens[0], "summary")) {
                    cmd->help.summary = aim_strdup(option->tokens[1]);
                }
                else if(!UCLI_STRCMP(option->tokens[0], "args")) {
                    cmd->help.args = aim_strdup(option->tokens[1]);
                }
                else if(!UCLI_STRCMP(option->tokens[0], "full")) {
                    cmd->help.full = aim_strdup(option->tokens[1]);
                }
                else if(!UCLI_STRCMP(option->tokens[0], "group")) {
                    cmd->group = aim_strdup(option->tokens[1]);
                }
                else if(!UCLI_STRCMP(option->tokens[0], "thread")) {
                    cmd->threaded = atoi(option->tokens[1]);
                }
                else {
                    UCLI_MWARN("unknown keyword in help spec: '%s'",
                              option->tokens[0]);
                }
                ucli_tokens_free(option);
            }
            ucli_tokens_free(specs);
        }

        /*
         * Provide defaults where appropriate.
         */
        if(cmd->help.full == NULL) {
            cmd->help.full = aim_strdup(cmd->help.summary);
        }
        if(cmd->help.args == NULL) {
            if(cmd->arg_count != 0) {
                cmd->help.args = aim_strdup("    [argument help is not available.]");
            }
            else {
                cmd->help.args = aim_strdup("");
            }
        }
    }
}


int
ucli_module_init(ucli_module_t* mod)
{
    if(mod) {
        int i;
        ucli_context_t uc;

        if(mod->init) {
            mod->init(mod);
        }

        for(i = 0; mod->handlers[i]; i++) {
            ucli_command_t* cp = aim_zmalloc(sizeof(*cp));
            UCLI_MEMSET(&uc, 0, sizeof(uc));
            uc.flags = UCLI_CONTEXT_F_GET_COMMAND_INFO;
            uc.info = cp;
            mod->handlers[i](&uc);
            cp->handler = mod->handlers[i];
            cp->threaded = 0;
            initcmd__(cp);
            mod->command_list = biglist_append(mod->command_list, cp);
        }
    }
    return 0;
}


int
ucli_module_destroy(ucli_module_t* mod)
{
    if(mod) {
        /**
         * Add the disabled list back to the command list
         * so they can be destroyed properly.
         */
        mod->command_list = biglist_append_list(mod->command_list,
                                                mod->disabled_list);

        /**
         * Destroy all commands
         */
        if(mod->command_list) {
            ucli_command_t* cp;
            biglist_t* ble;
            BIGLIST_FOREACH_DATA(ble, mod->command_list, ucli_command_t*, cp) {
                if(cp->help.summary) {
                    aim_free((void*)cp->help.summary);
                }
                if(cp->help.args) {
                    aim_free((void*)cp->help.args);
                }
                if(cp->help.full) {
                    aim_free((void*)cp->help.full);
                }
                if(cp->group) {
                    aim_free((void*)cp->group);
                }
                aim_free(cp);
            }
            biglist_free(mod->command_list);
            mod->command_list = NULL;
        }

        if(mod->destroy) {
            mod->destroy(mod);
        }
    }
    return 0;
}

static int
ucli_module_command_group_enable__(ucli_module_t* mod, const char* command,
                                   const char* group, int enable)
{
    biglist_t** src_list;
    biglist_t** dst_list;

    biglist_t* ble;
    ucli_command_t* cp;

    if(mod == NULL || (command == NULL && group == NULL)) {
        return UCLI_STATUS_E_PARAM;
    }

    src_list = (enable) ? &mod->disabled_list : &mod->command_list;
    dst_list = (enable) ? &mod->command_list : &mod->disabled_list;

    BIGLIST_FOREACH_DATA(ble, (*src_list), ucli_command_t*, cp) {
        if( (command == NULL || !UCLI_STRCMP(cp->command, command)) &&
            (group == NULL || (cp->group && !UCLI_STRCMP(cp->group, group)))) {

            *src_list = biglist_remove_link_free(*src_list, ble);
            *dst_list = biglist_prepend(*dst_list, cp);

            return UCLI_STATUS_OK;
        }
    }

    return UCLI_STATUS_E_NOT_FOUND;
}

int
ucli_module_command_enable(ucli_module_t* mod, const char* command, int enable)
{
    int rv;
    do {
        rv = ucli_module_command_group_enable__(mod, command, NULL, enable);
    } while(rv == UCLI_STATUS_OK);
    return rv;
}

int
ucli_module_group_enable(ucli_module_t* mod, const char* group,
                            int enable)
{
    int rv;
    do {
        rv = ucli_module_command_group_enable__(mod, NULL, group, enable);
    } while(rv == UCLI_STATUS_OK);
    return rv;
}

int
ucli_module_group_help(ucli_module_t* mod, const char* group,
                       aim_pvs_t* pvs, const char* pfx)
{
    ucli_command_t* cp;
    biglist_t* ble;

    BIGLIST_FOREACH_DATA(ble, mod->command_list, ucli_command_t*, cp) {
        if(group == NULL || (cp->group && !UCLI_STRCMP(cp->group, group))) {
            char argstr[256];
            aim_snprintf(argstr, sizeof(argstr), "%s%s %s",
                          AIM_STRING_EMPTY_IF_NULL(pfx),
                          cp->command,
                          AIM_STRING_EMPTY_IF_NULL(cp->help.args));
            aim_printf(pvs, "%-32s%s\n",
                       argstr, AIM_STRING_EMPTY_IF_NULL(cp->help.summary));
        }
    }
    return UCLI_STATUS_OK;
}


