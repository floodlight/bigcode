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

/************************************************************//**
 *
 * @file
 * @brief uCli Nodes
 *
 * @details
 *
 * The uCli Node structure is the fundamental representation of the
 * CLI tree structure.
 *
 * Nodes are comprised of:
 *      * A set of uCli modules.
 *      * A set of uCli sub-nodes.
 *
 * A node is referred to by name, and represents the collection
 * of all inferior commands contained within.
 *
 *
 * @addtogroup ucli-node
 * @{
 *
 ***************************************************************/

#ifndef __UCLI_NODE_H__
#define __UCLI_NODE_H__

#include <BigList/biglist.h>

/**
 * The ucli node structure.
 */
typedef struct ucli_node_s {

    /** Reference count. */
    int refcount;

    /** This node's parent node. */
    struct ucli_node_s* parent;

    /**
     * The name of this node. Commands and command modes will refer
     * directly to this name.
     */
    const char* name;

    /** Help information for this node */
    const char* help;

    /** The list of all uCli Modules in this node. */
    biglist_t* modules;

    /** The list of all uCli subnodes in this node. */
    biglist_t* subnodes;

} ucli_node_t;

/**
 * @brief Create a node.
 * @param name The name of the new node.
 * @param parent The parent of this node (optional).
 * @param mod A module to add to this node (optional).
 */
ucli_node_t* ucli_node_create(const char* name, ucli_node_t* parent,
                              ucli_module_t* mod);

/**
 * @brief Display the node's help information.
 * @param node The node
 * @param pvs The output stream.
 */
void ucli_node_help(ucli_node_t* node, aim_pvs_t* pvs);

/**
 * @brief Destroy a node.
 * @param node The node to destroy.
 *
 * @details This will recursively destroy all subnodes and submodules
 * contained within this node, and then deallocate the node itself.
 */
int ucli_node_destroy(ucli_node_t* node);

/**
 * @brief Dispatch arguments to the given node.
 * @param node The node.
 * @param uc The current context.
 * @param select_node The new node that should be active after the
 * execution of this command.
 */
ucli_status_t ucli_node_dispatch(ucli_node_t* node,
                                 ucli_context_t* uc,
                                 ucli_node_t** select_node);

/**
 * @brief Add a module to the given node.
 * @param node The node.
 * @param mod The module to add.
 */
int ucli_node_module_add(ucli_node_t* node, ucli_module_t* mod);

/**
 * @brief Remove a module from the given node.
 * @param node The node from which to remove the module.
 * @param mod The module to remove.
 */
int ucli_node_module_remove(ucli_node_t* node, ucli_module_t* mod);

/**
 * @brief Add a node as a subnode to the given node.
 * @param node The node.
 * @param subnode The node which will be added as a subnode.
 */
int ucli_node_subnode_add(ucli_node_t* node, ucli_node_t* subnode);

/**
 * @brief Remove a node as a subnode from the given node.
 * @param node The node from which to remove the subnode.
 * @param subnode The subnode to remove.
 */
int ucli_node_subnode_remove(ucli_node_t* node, ucli_node_t* subnode);

/**
 * @brief Attempt command completion.
 * @param node The node.
 * @param tokens tokens The current input tokens.
 */
ucli_completion_t* ucli_node_complete(ucli_node_t* node, const char** tokens);

/**
 * @brief Enable or disable a command in this node.
 * @param node The node.
 * @param command The command.
 * @param enable The enable or disable state.
 */
int ucli_node_command_enable(ucli_node_t* node, const char* command,
                              int enable);

/**
 * @brief Enable or disable a group in this node.
 * @param node The node.
 * @param group The group.
 * @param enable The enable or disable state.
 */
int ucli_node_group_enable(ucli_node_t* node, const char* group,
                              int enable);

/**
 * @brief Dump the node and module structure starting at the given node.
 * @param node The root node to dump.
 * @param pvs The output stream.
 */
int ucli_node_dump(ucli_node_t* node, aim_pvs_t* pvs);

/**
 * @brief Dump group command help.
 * @param node The node.
 * @param group The group.
 * @param pvs The output PVS.
 * @param pfx Prefix for command help.
 */
int ucli_node_group_help(ucli_node_t* node, const char* group, aim_pvs_t* pvs,
                         const char* pfx);


#endif /* __UCLI_NODE_H__ */
/*@}*/

