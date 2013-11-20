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

/**
 * Sometimes we need to make a call for every submodule in our node.
 */
#define FOREACH_MODULE(_node, _exprs)                                   \
    do {                                                                \
        biglist_t* _ble;                                                \
        ucli_module_t* _module;                                         \
        BIGLIST_FOREACH_DATA(_ble, _node->modules, ucli_module_t*, _module) { \
            _exprs;                                                     \
        }                                                               \
    } while(0)

/**
 * Sometimes we need to make a simple call for every subnode in our node.
 */
#define FOREACH_SUBNODE(_node, _exprs)                                  \
    do {                                                                \
        biglist_t* _ble;                                                \
        ucli_node_t* _subnode;                                          \
        BIGLIST_FOREACH_DATA(_ble, _node->subnodes, ucli_node_t*, _subnode) { \
            _exprs;                                                     \
        }                                                               \
    } while(0)

/**
 * Sometimes we need to do both.
 */
#define FOREACH_MODULE_AND_SUBNODE(_node, _modexprs, _nodeexprs)        \
    do {                                                                \
        FOREACH_MODULE(_node, _modexprs);                               \
        FOREACH_SUBNODE(_node, _nodeexprs);                             \
    } while(0)                                                          \

ucli_node_t*
ucli_node_create(const char* name, ucli_node_t* parent,
                 ucli_module_t* mod)
{
    ucli_node_t* rv;

    rv = aim_zmalloc(sizeof(*rv));

    if(rv == NULL) {
        UCLI_MERROR("Failed to allocate ucli_node_t structure.");
        return NULL;
    }

    rv->name = name;

    if(mod) {
        ucli_node_module_add(rv, mod);
    }

    rv->refcount = 1;

    if(parent) {
        ucli_node_subnode_add(parent, rv);
    }

    return rv;
}


void
ucli_node_help(ucli_node_t* node, aim_pvs_t* pvs)
{
    FOREACH_MODULE_AND_SUBNODE(node,
                               ucli_module_help(_module, pvs),
                               aim_printf(pvs, "  %-16s%s\n", _subnode->name, _subnode->help ? _subnode->help : "")
                               );
}


ucli_status_t
ucli_node_dispatch(ucli_node_t* node,
                   ucli_context_t* uc,
                   ucli_node_t** selectNode)
{
    ucli_status_t rv;
    int found = 0;
    biglist_t* ble;
    ucli_module_t* m;
    ucli_node_t* n;

    /*
     * Special arguments.
     */
    if(!UCLI_STRCMP(uc->pargs->args[0], "?") ||
       !UCLI_STRCMP(uc->pargs->args[0], "help")) {
        ucli_node_help(node, &uc->pvs);
        return UCLI_STATUS_OK;
    }

    if(!UCLI_STRCMP(uc->pargs->args[0], "..")) {
        if(node->parent != NULL) {
            *selectNode = node->parent;
        }
        else {
            /* Silent */
        }
        return UCLI_STATUS_OK;
    }


    /*
     * Dispatch to all of our submodules
     */
    BIGLIST_FOREACH_DATA(ble, node->modules, ucli_module_t*, m) {
        rv = ucli_module_dispatch(m, uc);

        if(rv == UCLI_STATUS_E_NOT_FOUND) {
            continue;
        }
        else if(rv == UCLI_STATUS_CONTINUE) {
            found = 1;
            continue;
        }
        else {
            /* Command handled (succesfully or not) */
            return rv;
        }
    }
    if(found) {
        return UCLI_STATUS_OK;
    }

    /*
     * Look in all of our subnodes
     */
    BIGLIST_FOREACH_DATA(ble, node->subnodes, ucli_node_t*, n) {
        rv = ucli_node_dispatch(n, uc, selectNode);
        if(rv != UCLI_STATUS_E_NOT_FOUND) {
            return rv;
        }
    }

    /*
     * Check to see if the first argument is one of our node
     * subnode names
     */
    BIGLIST_FOREACH_DATA(ble, node->subnodes, ucli_node_t*, n) {
        if(!UCLI_STRCMP(uc->pargs->args[0],
                        n->name)) {
            if(uc->pargs->count == 1) {
                /* Select our subnode and return */
                *selectNode = n;
                return UCLI_STATUS_OK;
            }
            else {
                /* Send the args to our subnode */
                uc->pargs->args++;
                uc->pargs->count--;
                return ucli_node_dispatch(n, uc, selectNode);
            }
        }
    }


    /*
     * Nothin'
     */
    return UCLI_STATUS_E_NOT_FOUND;
}

int
ucli_node_module_add(ucli_node_t* node, ucli_module_t* mod)
{
    if(node == NULL || mod == NULL) {
        return -1;
    }
    node->modules = biglist_append(node->modules, mod);
    return 0;
}

int
ucli_node_subnode_add(ucli_node_t* node, ucli_node_t* subnode)
{
    if(node == NULL || subnode == NULL) {
        return -1;
    }
    node->subnodes = biglist_append(node->subnodes, subnode);
    subnode->parent = node;
    return 0;
}

int
ucli_node_module_remove(ucli_node_t* node, ucli_module_t* mod)
{
    if(node == NULL || mod == NULL) {
        return -1;
    }
    node->modules = biglist_remove(node->modules, mod);
    return 0;
}

int
ucli_node_subnode_remove(ucli_node_t* node, ucli_node_t* subnode)
{
    if(node == NULL || subnode == NULL) {
        return -1;
    }
    node->subnodes = biglist_remove(node->subnodes, subnode);
    return 0;
}

int
ucli_node_dump(ucli_node_t* node, aim_pvs_t* pvs)
{
    iof_t iof;
    biglist_t* ble;

    iof_init(&iof, pvs);

    iof_push(&iof, "node@%p \"%s\"", node, node->name);
    iof_push(&iof, "modules");
    BIGLIST_FOREACH(ble, node->modules) {
        ucli_module_dump((ucli_module_t*)(ble->data), &iof.inherit);
    }
    iof_pop(&iof);
    iof_push(&iof, "subnodes");
    BIGLIST_FOREACH(ble, node->subnodes) {
        ucli_node_dump((ucli_node_t*)(ble->data), &iof.inherit);
    }
    iof_pop(&iof);
    iof_pop(&iof);
    return 0;
}

int
ucli_node_destroy(ucli_node_t* node)
{
    biglist_t* ble;

    if(--node->refcount > 0) {
        return 0;
    }

    BIGLIST_FOREACH(ble, node->modules) {
        ucli_module_destroy((ucli_module_t*)(ble->data));
    }
    biglist_free(node->modules);
    BIGLIST_FOREACH(ble, node->subnodes) {
        ucli_node_destroy((ucli_node_t*)(ble->data));
    }
    biglist_free(node->subnodes);

    aim_free(node);
   return 0;
}

int
ucli_node_command_enable(ucli_node_t* node, const char* command, int enable)
{
    biglist_t* ble;
    ucli_module_t* mod;

    if(node == NULL || command == NULL) {
        return UCLI_STATUS_E_PARAM;
    }

    BIGLIST_FOREACH_DATA(ble, node->modules, ucli_module_t*, mod) {
        if(ucli_module_command_enable(mod, command, enable) == UCLI_STATUS_OK) {
            return UCLI_STATUS_OK;
        }
    }

    /* Do not dispatch to subnodes */
    return UCLI_STATUS_E_NOT_FOUND;
}



int
ucli_node_group_enable(ucli_node_t* node, const char* group,
                              int enable)
{
    FOREACH_MODULE_AND_SUBNODE(node,
                               ucli_module_group_enable(_module, group, enable),
                               ucli_node_group_enable(_subnode, group, enable)
                               );

    return UCLI_STATUS_OK;
}

int
ucli_node_group_help(ucli_node_t* node, const char* group, aim_pvs_t* pvs,
                     const char* pfx)
{
    FOREACH_MODULE_AND_SUBNODE(node,
                               ucli_module_group_help(_module, group, pvs, pfx),
                               ucli_node_group_help(_subnode, group, pvs, pfx)
                               );
    return UCLI_STATUS_OK;
}


ucli_completion_t*
ucli_node_complete(ucli_node_t* node, const char** tokens)
{
    biglist_t* ble;
    ucli_completion_t* c = NULL;
    ucli_module_t* mod;
    ucli_node_t* subnode;
    biglist_t* possibilities = NULL;
    int count;

    if(node == NULL || tokens == NULL || tokens[0] == NULL) {

        /*
         * We are at our node, but with no tokens yet.
         * Instead of providing just the options,
         * display the help summary.
         */
        c = aim_zmalloc(sizeof(*c));
        c->help_node = node;
        return c;
    }

    /*
     * Can we complete with one of our modules?
     */
    BIGLIST_FOREACH_DATA(ble, node->modules, ucli_module_t*, mod) {
        c = ucli_module_complete(mod, tokens);
        if(c) {
            return c;
        }
    }

    /*
     * If there is only one token, then it must be a partial name
     * of one of our subnodes.
     *
     * If there is more than one token, the current token must already
     * be a name for one of our subnodes.
     *
     */
    BIGLIST_FOREACH_DATA(ble, node->subnodes, ucli_node_t*, subnode) {
        if(!UCLI_STRCMP(tokens[0], subnode->name)) {
            /* first token is an exact match */
            break;
        }
    }
    if(subnode) {
            /*
             * The first token is an exact match for a subnode already.
             * The remaining tokens should be sent to that subnode.
             */
        return ucli_node_complete(subnode, tokens+1);
    }

    count = 0;
    BIGLIST_FOREACH_DATA(ble, node->subnodes, ucli_node_t*, subnode) {
        if(!UCLI_STRNCMP(subnode->name, tokens[0], UCLI_STRLEN(tokens[0]))) {
            possibilities = biglist_append(possibilities,
                                           aim_strdup(subnode->name));
            count++;
        }
    }

    if(count == 0) {
        return NULL;
    }
    else {
        ucli_completion_t* c = aim_zmalloc(sizeof(*c));
        c->possibilities.list = possibilities;
        c->possibilities.count = count;
        if(count == 1) {
            c->next_token = aim_strdup((char*)(c->possibilities.list->data));
            c->append_string = aim_strdup(c->next_token + UCLI_STRLEN(tokens[0]));
        }
        return c;
    }
}

