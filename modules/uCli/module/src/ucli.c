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
#include <uCli/ucli_config.h>
#include "ucli_int.h"
#include "ucli_log.h"
#include <IOF/iof.h>
#include <uCli/ucli_argparse.h>

#if UCLI_CONFIG_INCLUDE_TTY == 1
#include <unistd.h>
#endif


void
__ucli_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
}


struct ucli_s {

    char log_string[32];

    /* Root Node */
    ucli_node_t* root;

    /* Current Node */
    ucli_node_t* current;

    /* Common Node */
    ucli_node_t* common;

    /* flags */
    uint32_t flags;

    /* default output pvs */
    aim_pvs_t* pvs;
};

static int ucli_dispatch_args__(ucli_t* ucli, aim_pvs_t* pvs,
                                ucli_pargs_t* pargs, ucli_kwargs_t* kwargs);

static void
ucli_parray_free__(void** parray, int freep)
{
    if(parray) {
        void** p;
        for(p = parray; *p; p++) {
            aim_free(*p);
        }
        if(freep) {
            aim_free(parray);
        }
    }
}

void
ucli_pvs_set(ucli_t* ucli, aim_pvs_t* pvs)
{
    ucli->pvs = pvs;
}

ucli_t*
ucli_create(const char* name, ucli_module_t* module, ucli_node_t* root)
{
    ucli_t* rv;

    rv = aim_zmalloc(sizeof(*rv));

    if(rv == NULL) {
        UCLI_MERROR("Failed to allocate ucli structure.");
        return NULL;
    }

    aim_strlcpy(rv->log_string, (name) ? name : "", sizeof(rv->log_string));


    rv->root = (root) ? root : ucli_node_create("__root__", NULL, NULL);

    if(rv->root == NULL) {
        ucli_destroy(rv);
        return rv;
    }

    rv->current = rv->root;

    if(module) {
        if(ucli_module_add(rv, module) < 0) {
            UCLI_MERROR("Adding module to new ucli failed.");
            ucli_destroy(rv);
            return NULL;
        }
    }

#if UCLI_CONFIG_INCLUDE_MODULE_NODES == 1
    ucli_node_add(rv, ucli_module_nodes_create("modules"));
#endif
    return rv;
}

int
ucli_destroy(ucli_t* ucli)
{
    if(ucli) {
        ucli_node_destroy(ucli->root);
        aim_free(ucli);
    }
    return 0;
}

int
ucli_flags_get(ucli_t* ucli, uint32_t* flags_rv)
{
    if(ucli && flags_rv) {
        *flags_rv = ucli->flags;
        return 0;
    }
    return -1;
}

int
ucli_flags_set(ucli_t* ucli, uint32_t flags)
{
    if(ucli) {
        ucli->flags = flags;
        return 0;
    }
    return -1;
}

int
ucli_flags_modify(ucli_t* ucli, uint32_t andflags, uint32_t orflags)
{
    if(ucli) {
        ucli->flags &= andflags;
        ucli->flags |= orflags;
        return 0;
    }
    return -1;
}


int
ucli_module_add(ucli_t* ucli, ucli_module_t* mod)
{
    if(ucli == NULL || mod == NULL) {
        return -1;
    }

    return ucli_node_module_add(ucli->root, mod);
}

int
ucli_node_add(ucli_t* ucli, ucli_node_t* node)
{
    if(ucli == NULL || node == NULL) {
        return -1;
    }
    return ucli_node_subnode_add(ucli->root, node);
}


/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/
static char* status__(int _rv)
{
#if UCLI_CONFIG_INCLUDE_TTY == 1
    if(isatty(2)) {
        return (_rv < 0) ? "\x1B[31mFAILED\x1B[39m." : "\x1B[32mOK\x1B[39m.";
    }
#endif
    return (_rv < 0) ? "FAILED." : "OK.";
}


#define UCLI_DISPATCH_CONTINUE(_ucli, _rv)                              \
    (_rv >= 0 || (_ucli->flags & UCLI_F_DISPATCH_CONTINUE_ON_FAIL))
#define UCLI_DISPATCH_BLOCKS_CONTINUE(_ucli, _rv) \
    (_rv >= 0 || (_ucli->flags & UCLI_F_DISPATCH_BLOCKS_CONTINUE_ON_FAIL))



/**
 * These are the possible return values for the tokenize function.
 */
typedef enum ucli_token_status_e {
    ucliTokenStatusNone = 1,
    ucliTokenStatusToken = 2,
    ucliTokenStatusError = -1
} ucli_token_status_t;


static ucli_token_status_t
ucli_next_token__(char** string, char delimiter, char** rv,
                  int keep_quotes)
{
    char *tok, *src, *dst;
    int escaped = 0;
    int quote = 0;

    /*
     * Skip leading delimiters.
     */
    for(src = *string; *src; src++) {
        if(*src != delimiter) {
            break;
        }
    }
    if(*src == 0) {
        /* All delimiters, no token */
        *rv = NULL;
        return ucliTokenStatusNone;
    }

    /* Token starts here */
    tok = aim_strdup(src);

    for(dst = tok; *src; src++) {
        if(escaped) {
            /* This character has been escaped, just absorb it as-is */
            *dst++ = *src;
            escaped=0;
        }
        else if(quote) {
            /* Currently in a quoted sequence */
            if(quote == *src) {
                /* End of current quoted sequence */
                quote = 0;
                if(keep_quotes) {
                    *dst++ = *src;
                }
            }
            else {
                /* Still in quoted sequence */
                *dst++ = *src;
            }
        }
        else if(*src == delimiter) {
            /*
             * End of token.
             * - Terminate and return current token.
             * - Return start point for next token to caller.
             */
            *string = src;
            *dst = 0;
            *rv = tok;
            return ucliTokenStatusToken;
        }
        else if(*src == '\\') {
            /* Start of escape sequence */
            escaped=1;
        }
        else if(*src == '"' || *src == '\'') {
            /* Start of quoted sequence */
            quote = *src;
            if(keep_quotes) {
                *dst++ = *src;
            }
        }
        else {
            /* Normal character */
            *dst++ = *src;
        }
    }

    if(escaped || quote) {
        /*
         * We ran out of characters before completed an escaped character or
         * quoted sequence. This is an error.
         *
         * We still return the current token to indicate where in the current
         * the error began.
         */
        *rv = tok;
        return ucliTokenStatusError;
    }
    else {
        /*
         * We reached the end of the input. Return the last token.
         */
        *string = src;
        *dst = 0;
        *rv = tok;
        return ucliTokenStatusToken;
    }
}

static int
ucli_string_tokenize__(char* string, char** argv, int max, char delimiter,
                       aim_pvs_t* epvs, char** error, int keep_quotes)
{
    int rv;
    int argc = 0;
    char* tok;
    char** saveptr = &string;

    while(argc < max) {
        rv = ucli_next_token__(saveptr, delimiter, &tok, keep_quotes);

        if(rv == ucliTokenStatusToken) {
            argv[argc++] = tok;
        }
        else if(rv == ucliTokenStatusNone) {
            break;
        }
        else if(rv == ucliTokenStatusError) {
            /* Return the error token string if requested */
            if(error) {
                *error = tok;
            }
            if(epvs) {
                aim_printf(epvs, "invalid input starting at '%s'\n", tok);
            }
            /* Free all previous tokens */
            while(argc >= 0) {
                aim_free(argv[argc--]);
            }
            return -1;
        }
    }
    return argc;
}

static int
ucli_dispatch_command__(ucli_t* ucli, aim_pvs_t* pvs, char* str)
{
    int i;
    int rv;
    ucli_pargs_t pargs;
    ucli_kwargs_t kwargs;

    char* tokens[UCLI_CONFIG_MAX_ARGS] = { 0 };
    int token_count;
    char* localstr = aim_strdup(str);

    UCLI_MEMSET(&pargs, 0, sizeof(pargs));
    UCLI_MEMSET(&kwargs, 0, sizeof(kwargs));
    pargs.args = pargs.args__;


    while(UCLI_STRCHR(localstr, '\n')) {
        *UCLI_STRCHR(localstr, '\n') = ' ';
    }

    /*
     * First pass token parse.
     */
    token_count = ucli_string_tokenize__(localstr, tokens, UCLI_CONFIG_MAX_ARGS-1, ' ',
                                         pvs, NULL, 0);
    if(token_count == -1) {
        /* Token error already reported. */
        ucli_parray_free__((void**)tokens, 0);
        return -1;
    }

    /*
     * Split positional args from keyword args here.
     */

    for(i = 0; i < token_count; i++) {
        pargs.args[pargs.count++] = tokens[i];
    }

    rv = ucli_dispatch_args__(ucli, pvs, &pargs, &kwargs);

    if((ucli->flags & UCLI_F_DISPATCH_ECHO_ALL) ||
       ( (ucli->flags & UCLI_F_DISPATCH_ECHO_FAIL) && rv < 0)) {
        aim_printf(pvs, "[ %s : %s ]\n", str, status__(rv));
    }

    ucli_parray_free__((void**)tokens, 0);
    aim_free(localstr);
    return rv;

}

int
ucli_dispatch_string(ucli_t* ucli, aim_pvs_t* pvs, const char* str)
{
    int rv = 0;
    int i;
    int count;
    char* commands[UCLI_CONFIG_MAX_ARGS] = { 0 };
    char* string = aim_strdup(str);
    char* p;

    /*
     * Individual commands can be separated with a semicolon or a '--'.
     *
     * The semicolon syntax is designed for the interactive command line.
     * The '--' syntax is designed for inputs from the command line.
     *
     *
     * Start by replacing all instances of "--" with a "; " before parsing.
     */
    while( (p = UCLI_STRSTR(string, "--")) ) {
        p[0] = ';';
        p[1] = ' ';
    }

    /*
     * Tokenize on semicolons
     */
    count = ucli_string_tokenize__((char*)string, commands,
                                   UCLI_CONFIG_MAX_ARGS-1,
                                   ';', pvs, NULL, 1);

    if(count == -1) {
        /* Error already reported in tokenize__() */
        rv = -1;
    }
    else {
        /* Dispatch the separated command strings */
        for(i = 0; i < count && UCLI_DISPATCH_CONTINUE(ucli, rv); i++) {
            rv = ucli_dispatch_command__(ucli, pvs, commands[i]);
        }
    }

    ucli_parray_free__((void**)commands, 0);
    aim_free(string);
    return rv;
}

int
ucli_dispatch_strings(ucli_t* ucli, aim_pvs_t* pvs,
                      const char** strings)
{
    const char** s = strings;
    while(s && *s) {
        ucli_dispatch_string(ucli, pvs, *s);
        s++;
    }
    return 0;
}


/**
 * @brief Reconstruct the ARGV commandline vector into a ucli-compatible command string.
 * @param argv The argument vector.
 */
char*
ucli_argv_to_string(char** argv)
{
    int i;
    int count;
    int charcount = 0;
    char* joined;

    /* Determine the number of input strings and maximum string length. */
    for(count = 0; argv[count]; count++) {
        charcount += UCLI_STRLEN(argv[count]) + 5;
    }

    /* Rejoin all strings with proper quotes where necessary */
    joined = aim_zmalloc(charcount+1);
    for(i = 0; i < count; i++) {
        if(argv[i][0] == '-' && argv[i][1] == '-') {
            /* --command */
            /* Replace with the '--' with ';' */
            UCLI_STRCAT(joined, ";");
            UCLI_STRCAT(joined, argv[i]+2);
        }
        else if(argv[i][0] == '-') {
            /* -command */
            /* Replace with the '-' with ';' */
            UCLI_STRCAT(joined, ";");
            UCLI_STRCAT(joined, argv[i]+1);
        }
        else {
            /*
             * Bare string. This might need to be quoted.
             */
            char* need_quotes = UCLI_STRCHR(argv[i], ' ');
            if(need_quotes) {
                UCLI_STRCAT(joined, "'");
            }
            UCLI_STRCAT(joined, argv[i]);
            if(need_quotes) {
                UCLI_STRCAT(joined, "'");
            }
        }
        UCLI_STRCAT(joined, " ");
    }
    return joined;
}

char**
ucli_argv_to_strings(char** argv)
{
    char* str;
    char** rv;

    str = ucli_argv_to_string(argv);
    rv = aim_zmalloc(sizeof(*rv)*UCLI_CONFIG_MAX_ARGS);
    ucli_string_tokenize__(str, rv, UCLI_CONFIG_MAX_ARGS-1, ';',
                           NULL, NULL, 1);
    aim_free(str);

    return rv;
}

void
ucli_argv_to_strings_free(char** args)
{
    int i;
    if(args) {
        for(i = 0; args[i]; i++) {
            aim_free(args[i]);
        }
        aim_free(args);
    }
}

int
ucli_dispatch_argv(ucli_t* ucli, aim_pvs_t* pvs, char** argv)
{
    int rv;
    char** strings = ucli_argv_to_strings(argv);
    if(strings == NULL) {
        return UCLI_STATUS_E_ERROR;
    }
    rv = ucli_dispatch_strings(ucli, pvs, (const char**)strings);
    ucli_argv_to_strings_free(strings);
    return rv;
}


int
ucli_dispatch_block(ucli_t* ucli, aim_pvs_t* pvs, ucli_block_t* block)
{
    int rv = 0;
    const char** cmd;
    for(cmd = block->commands; *cmd && UCLI_DISPATCH_CONTINUE(ucli, rv); cmd++) {
        rv = ucli_dispatch_string(ucli, pvs, *cmd);
    }
    if(ucli->flags & UCLI_F_DISPATCH_BLOCK_STATUS) {
        aim_printf(pvs, "%-24s: %s\n", block->name, status__(rv));
    }
    return rv;
}

int
ucli_dispatch_blocks(ucli_t* ucli, aim_pvs_t* pvs, ucli_block_t* blocks, char* name)
{
    int rv = 0;
    ucli_block_t* bp;
    int failcount = 0;

    for(bp = blocks; bp->name && UCLI_DISPATCH_BLOCKS_CONTINUE(ucli, rv);
        bp++) {
        if(name == NULL || !UCLI_STRCMP(bp->name, name)) {
            rv = ucli_dispatch_block(ucli, pvs, bp);
        }
        if(rv < 0) {
            failcount++;
        }
    }
    return failcount;
}

int
ucli_dispatch_filename(ucli_t* ucli, aim_pvs_t* pvs, const char* filename)
{
    int rv;
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
        aim_printf(pvs, "error: could not open file '%s'\n", filename);
        return UCLI_STATUS_E_IO;
    }
    rv = ucli_dispatch_filep(ucli, pvs, fp);
    fclose(fp);
    return rv;
}

int
ucli_dispatch_filep(ucli_t* ucli, aim_pvs_t* pvs, FILE* fp)
{
    int rv = 0;
    char* line;

    line = aim_zmalloc(UCLI_CONFIG_MAX_INPUT_LINE);

    if(line == NULL) {
        return ucli_e_internal(NULL, "line buffer allocation failed.");
    }

    while( (line = fgets(line, UCLI_CONFIG_MAX_INPUT_LINE, fp)) && (rv >= 0)) {
        line[strlen(line)-1]=0;
        if(line[0] && line[0] != '#') {
            aim_printf(pvs, "[ '%s' ]\n", line);
            rv = ucli_dispatch_string(ucli, pvs, line);
        }
    }
    return rv;
}

/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/
int
ucli_dispatch_args(ucli_t* ucli, aim_pvs_t* pvs, char** args)
{
    ucli_pargs_t pargs;
    ucli_kwargs_t kwargs;

    UCLI_MEMSET(&pargs, 0, sizeof(pargs));
    UCLI_MEMSET(&kwargs, 0, sizeof(kwargs));
    pargs.args = pargs.args__;

    for(pargs.count = 0;
        args[pargs.count] && pargs.count < UCLI_CONFIG_MAX_ARGS;
        pargs.count++) {
        pargs.args[pargs.count] = args[pargs.count];
    }
    return ucli_dispatch_args__(ucli, pvs, &pargs, &kwargs);
}

int
ucli_context_init(ucli_context_t* uc, ucli_t* ucli, aim_pvs_t* pvs)
{
    UCLI_MEMSET(uc, 0, sizeof(*uc));
    uc->root = ucli;
    uc->epvs = pvs;
    uc->pvs.vprintf = ucli_pvs_vprintf;
    uc->pvs.object.cookie = uc;
    uc->pvs.enabled = 1;
    return 0;
}

static int
ucli_dispatch_args__(ucli_t* ucli, aim_pvs_t* pvs,
                     ucli_pargs_t* pargs, ucli_kwargs_t* kwargs)
{
    int rv;
    ucli_context_t uc;

    if(ucli == NULL || pargs == NULL || pargs->count == 0) {
        return -1;
    }

    if(ucli->current == NULL) {
        UCLI_MINTERNAL("current node is NULL");
        return -1;
    }

    /** @fixme common command processing node */
    if(pargs) {
        if(pargs->count == 1 && !UCLI_STRCMP(pargs->args[0],
                                             "__dump")) {
            ucli_dump(ucli, pvs);
            return 0;
        }

        if(pargs->count == 3 && !UCLI_STRCMP(pargs->args[0],
                                             "__enable")) {
            ucli_node_command_enable(ucli->current, pargs->args[1],
                                     pargs->args[2][0] == '1');
            return 0;
        }
        if(pargs->count == 3 && !UCLI_STRCMP(pargs->args[0],
                                             "__genable")) {
            ucli_node_group_enable(ucli->current, pargs->args[1],
                                   pargs->args[2][0] == '1');
            return 0;
        }
        if(pargs->count >= 1 && !UCLI_STRCMP(pargs->args[0],
                                             "__pargs")) {
            int i;
            for(i = 0; i < pargs->count; i++) {
                printf("%d: '%s'\n", i, pargs->args[i]);
            }
            return 0;
        }
        if(pargs->count == 2 && !UCLI_STRCMP(pargs->args[0],
                                             "source")) {
            int rv = 0;
            ucli_node_t* current = ucli->current;
            ucli->current = ucli->root;
            rv = ucli_dispatch_filename(ucli, pvs, pargs->args[1]);
            ucli->current = current;
            return rv;
        }
    }

    ucli_context_init(&uc, ucli, pvs);
    uc.pargs = pargs;
    uc.kwargs = kwargs;

    rv = ucli_node_dispatch(ucli->current, &uc, &ucli->current);
    if(rv == UCLI_STATUS_E_NOT_FOUND) {
        ucli_error(&uc, "unknown command '%s'", pargs->args[0]);
    }
    return rv;
}

int
ucli_dump(ucli_t* ucli, aim_pvs_t* pvs)
{
    iof_t iof;
    iof_init(&iof, pvs);
    iof_push(&iof, "ucli@%p: \"%s\"", ucli, ucli->log_string);
    iof_iprintf(&iof, "current: %p (%s)", ucli->current, ucli->current->name);
    iof_push(&iof, "root");
    ucli_node_dump(ucli->root, &iof.inherit);
    iof_pop(&iof);
    return 0;
}


int
ucli_init(void)
{
    return 0;
}

void
ucli_denit(void)
{
}

void
ucli_reset(ucli_t* ucli)
{
    ucli->current = ucli->root;
}

ucli_t*
ucli_copy(ucli_t* ucli)
{
    ucli_t* n = aim_zmalloc(sizeof(*n));
    /* Intentional shallow copy */
    UCLI_MEMCPY(n, ucli, sizeof(*n));
    n->root->refcount++;

    return n;
}

int
ucli_prompt_print(ucli_t* ucli, aim_pvs_t* pvs, const char* prompt)
{
    char p[256];
    ucli_prompt_get(ucli, p, sizeof(p), prompt);
    aim_printf(pvs, "%s", p);
    return 0;
}

char*
ucli_prompt_get(ucli_t* ucli, char* dst, int dst_size, const char* prompt)
{
    char* p = dst;
    int rv;

    dst[0] = 0;

    if(prompt) {
        rv = aim_snprintf(p, dst_size, "%s", prompt);
        p += rv;
        dst_size-=rv;
    }
    if(ucli->current != ucli->root) {

        biglist_t* path = NULL;
        ucli_node_t* n;
        biglist_t* ble;

        for(n = ucli->current; n != ucli->root; n = n->parent) {
            path = biglist_prepend(path, n);
        }
        for(ble = path; ble; ble=ble->next) {
            rv = aim_snprintf(p, dst_size, ".%s", ((ucli_node_t*)(ble->data))->name);
            p += rv;
            dst_size -= rv;
        }
        biglist_free(path);
    }
    rv = aim_snprintf(p, dst_size, "> ");
    return dst;
}



ucli_completion_t*
ucli_complete_string(ucli_t* ucli, const char* line)
{
    ucli_tokens_t* tokens = ucli_tokens(line, " ");
    ucli_completion_t* c = ucli_complete_args(ucli, tokens->tokens);
    ucli_tokens_free(tokens);
    return c;
}

ucli_completion_t*
ucli_complete_args(ucli_t* ucli, const char** args)
{
    return ucli_node_complete(ucli->current, args);
}

void
ucli_completion_free(ucli_completion_t* c)
{
    if(c) {
        if(c->next_token) {
            aim_free(c->next_token);
        }
        if(c->append_string) {
            aim_free(c->append_string);
        }
        biglist_free_all(c->possibilities.list, ucli_biglist_free_element);
        aim_free(c);
    }
}


char*
ucli_complete_hook(ucli_t* ucli, const char* line)
{
    ucli_completion_t* c = ucli_complete_string(ucli, line);
    char* s;

    if(c) {
        if(c->append_string) {
            if(c->possibilities.count == 1) {
                /* Exact match -- append a courtesy space */
                s = aim_zmalloc(UCLI_STRLEN(c->append_string)+2);
                s = UCLI_STRCPY(s, c->append_string);
                UCLI_STRCAT(s, " ");
            }
            else {
                /* multiple options even though we can do a partial completion */
                s = c->append_string;
                c->append_string = NULL;
            }
            ucli_completion_free(c);
            return s;
        }
        else {
            biglist_t* ble;

            if(c->possibilities.list) {
                aim_printf(ucli->pvs, "\n");
                BIGLIST_FOREACH_DATA(ble, c->possibilities.list, char*, s) {
                    aim_printf(ucli->pvs, "    %s\n", s);
                }
                ucli_completion_free(c);
            }

            if(c->help_node) {
                aim_printf(ucli->pvs, "\n");
                ucli_node_help(c->help_node, ucli->pvs);
            }
            return NULL;
        }
    }
    return NULL;
}

int
ucli_command_enable(ucli_t* ucli, const char* command, int enable)
{
    if(ucli == NULL || command == NULL) {
        return UCLI_STATUS_E_PARAM;
    }
    return ucli_node_command_enable(ucli->current, command, enable);
}

int
ucli_group_enable(ucli_t* ucli, const char* group, int enable)
{
    return ucli_node_group_enable(ucli->root, group, enable);
}

int
ucli_group_help(ucli_t* uc, const char* group, aim_pvs_t* pvs, const char* pfx)
{
    return ucli_node_group_help(uc->root, group, pvs, pfx);
}


#if UCLI_CONFIG_INCLUDE_ELS_LOOP == 1

#include <ELS/els.h>
static const char* els_base_prompt__ = NULL;
static int
ucli_els_prompt__(char* p, int size, void* cookie)
{
    ucli_t* ucli = (ucli_t*)cookie;
    ucli_prompt_get(ucli, p, size, els_base_prompt__);
    return 0;
}
int
ucli_run(ucli_t* ucli, const char* prompt)
{
    els_t* els = els_create("ucli", ucli_els_prompt__, ucli);
    els_complete_set(els, (els_complete_f)ucli_complete_hook, ucli);
    els_start(els);
    els_base_prompt__ = prompt;
    ucli_pvs_set(ucli, &aim_pvs_stdout);
    for(;;) {
        char* line;
        els_prompt(els);
        els_waitline(els);
        line = els_getline(els);
        if(line) {
            if(line[0] == '^' && line[1] == 'D') {
                /* Ctrl-D eof */
                break;
            }
            else {
                ucli_dispatch_string(ucli, &aim_pvs_stdout, line);
            }
        }
    }
    els_destroy(els);
    return 0;
}
#endif /* UCLI_CONFIG_INCLUDE_ELS_LOOP */

#if UCLI_CONFIG_INCLUDE_FGETS_LOOP == 1
int
ucli_run(ucli_t* ucli, const char* prompt)
{
    char* cmdline;
    int status = UCLI_STATUS_OK;

    cmdline = aim_zmalloc(UCLI_CONFIG_MAX_INPUT_LINE);
    ucli_pvs_set(ucli, &aim_pvs_stdout);
    do {
        ucli_prompt_print(ucli, &aim_pvs_stdout, prompt);
        if(fgets(cmdline, UCLI_CONFIG_MAX_INPUT_LINE, stdin)) {
            if(UCLI_STRCHR(cmdline, '\n')) {
                *UCLI_STRCHR(cmdline, '\n') = 0;
            }
            status = ucli_dispatch_string(ucli, &aim_pvs_stdout, cmdline);
        }

    } while(status != UCLI_STATUS_E_EXIT);

    aim_free(cmdline);
    return 0;
}
#endif /* UCLI_CONFIG_INCLUDE_FGETS_LOOP */
