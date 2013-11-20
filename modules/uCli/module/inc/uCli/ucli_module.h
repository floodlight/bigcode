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
 * @brief uCli Modules
 *
 * @details
 *
 * The uCli Module is the fundamental command representation structure.
 *
 * The Module is designed to represent a set of related commands
 * that all work on the same underlying datastructures.
 *
 * All handlers in a module are passed the same custom data pointer.
 *
 *
 * @addtogroup ucli-module
 * @{
 *
 *
 ***************************************************************/
#ifndef __UCLI_MODULE_H__
#define __UCLI_MODULE_H__

#include <uCli/ucli_config.h>
#include <uCli/ucli_common.h>
#include <BigList/biglist.h>

/**************************************************************************//**
 *
 * This defines the properties of a single module.
 *
 *****************************************************************************/
typedef struct ucli_module_s {

    /** The name of this module */
    const char* name;

    /** Custom data pointer. All handlers receive this pointer. */
    void* cookie;

    /** The table of all commands in this module. */
    ucli_command_handler_f* handlers;

    /**
    * Optional initialization function for this module. Will be called
    * before any of the module's commands.
    *
    * This can be used to initialize the custom data, or perform
    * module-specific functional initialization.
    */
    int  (*init)(struct ucli_module_s* mod);

    /**
     * Optional destructor function for this module. Will be called
     * Will be called when the parent structures are destroyed.
     */
    void (*destroy)(struct ucli_module_s* mod);

    /**
     * Internal Use Only.
     *
     * This list is populated at init time from the data returned
     * by the command handlers to produce the infrastructure information
     * necessary to dispatch input to the module's command handlers.
     *
     * List of ucli_command_t structures.
     */
    biglist_t* command_list;

    /**
     * These commands are currently disabled or inactive.
     */
    biglist_t* disabled_list;

    /**
     * Internal Use Only.
     *
     * The reference count for this module.
     */
    int ref_count;

} ucli_module_t;

/**
 * @brief Dispatch arguments to the given module.
 * @param mod The module
 * @param uc The current context.
 *
 * @returns UCLI_STATUS_E_NOT_FOUND if the module does not recognize the command.
 * @returns UCLI_STATUS_OK if the module successfully executed the command.
 * @returns UCLI_STATUS_E_XXX if an error occured during processing.
 */
ucli_status_t ucli_module_dispatch(ucli_module_t* mod,
                                   ucli_context_t* uc);

/**
 * @brief Display the help information for the commands in this module.
 * @param mod The module
 * @param pvs The output stream.
 */
int ucli_module_help(ucli_module_t* mod, aim_pvs_t* pvs);

/**
 * @brief Dump the contents of the given module.
 * @param mod The module.
 * @param pvs The output stream.
 */
int ucli_module_dump(ucli_module_t* mod, aim_pvs_t* pvs);

/**
 * @brief Initialize a module structure.
 * @param mod The module.
 *
 * @details
 * This function calls the module's optional init() routine and
 * initializes the internal command tables.
 */
int ucli_module_init(ucli_module_t* mod);

/**
 * @brief Destroy a module structure.
 * @param mod The module.
 */
int ucli_module_destroy(ucli_module_t* mod);

/**
 * @brief Attempt command completion
 * @param mod The module.
 * @param tokens The current input tokens.
 */
ucli_completion_t* ucli_module_complete(ucli_module_t* mod,
                                        const char** tokens);

/**
 * @brief Enable or disable a command.
 * @param mod The module.
 * @param command The command name.
 * @param enable Enable state for the command.
 */
int ucli_module_command_enable(ucli_module_t* mod, const char* command,
                               int enable);

/**
 * @brief Enable or disable all commands in a group.
 * @param mod The module.
 * @param group The name of the group.
 * @param enable Enable or disable state for the group.
 */
int ucli_module_group_enable(ucli_module_t* mod, const char* group,
                             int enable);

/**
 * @brief Display group help.
 * @param mod The module.
 * @param group The group.
 * @param pvs The output PVS.
 * @param pfx Prefix for command help.
 */
int ucli_module_group_help(ucli_module_t* mod, const char* group,
                           aim_pvs_t* pvs, const char* pfx);

#endif /* __UCLI_MODULE_H__ */
/*@}*/
