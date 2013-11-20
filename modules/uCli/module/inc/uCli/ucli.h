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
 * @brief uCli Interface.
 *
 * @addtogroup ucli-ucli
 * @{
 *
 ***************************************************************/
#ifndef __UCLI_H__
#define __UCLI_H__

#include <uCli/ucli_config.h>

#include <uCli/ucli_common.h>
#include <uCli/ucli_module.h>
#include <uCli/ucli_node.h>
#include <uCli/ucli_errors.h>
#include <uCli/ucli_argparse.h>

/**
 * @brief Initialize the uCli library.
 * @note This function must be called at least once before creating
 * any uCli objects.
 */
int ucli_init(void);

/**
 * @brief De-initialize the uCli library.
 * @note Deallocates and data allocated during ucli_init().
 */
void ucli_denit(void);


/**
 * uCli instances are referred to using this opaque handle.
 */
typedef struct ucli_s ucli_t;


/**
 * @brief Create a uCli instance.
 * @param name Optional name for this uCli instance.
 * @param module Optional module to add to the root node at create time.
 * @param root Optional root node to use.
 */
ucli_t* ucli_create(const char* name, ucli_module_t* module,
                    ucli_node_t* root);

/**
 * @brief Set or change the default PVS.
 * @param ucli The ucli object.
 * @param pvs The pvs.
 */
void ucli_pvs_set(ucli_t* ucli, aim_pvs_t* pvs);

/**
 * @brief Add a node to the root of the given uCli instance.
 * @param ucli The ucli object.
 * @param node The node to add.
 */
int ucli_node_add(ucli_t* ucli, ucli_node_t* node);

/**
 * @brief Add a module to the root node of the given uCli instance.
 * @param ucli The ucli object.
 * @param mod The module to add.
 */
int ucli_module_add(ucli_t* ucli, ucli_module_t* mod);

/**
 * @brief Destroy a uCli instance.
 * @param ucli The instance to destroy.
 *
 * @note This will destroy and deallocate all nodes and subnodes.
 */
int ucli_destroy(ucli_t* ucli);


/** Echo all commands during dispatch */
#define UCLI_F_DISPATCH_ECHO_ALL                 (1 << 0)
/** Echo only failed command during dispatch */
#define UCLI_F_DISPATCH_ECHO_FAIL                (1 << 1)
/** Continue dispatching even on failure */
#define UCLI_F_DISPATCH_CONTINUE_ON_FAIL         (1 << 2)
/** Print success or failure for each dispatch. */
#define UCLI_F_DISPATCH_BLOCK_STATUS             (1 << 3)
/** Continue dispatching the block even on failure. */
#define UCLI_F_DISPATCH_BLOCKS_CONTINUE_ON_FAIL  (1 << 4)

/**
 * @brief Get the dispatch flags.
 * @param ucli The ucli object.
 * @param flags_rv Receives the current flags.
 */
int ucli_flags_get(ucli_t* ucli, uint32_t* flags_rv);

/**
 * @brief Set the dispatch flags.
 * @param ucli The ucli object.
 * @param flags The new flags.
 */
int ucli_flags_set(ucli_t* ucli, uint32_t flags);

/**
 * @brief Modify the dispatch flags.
 * @param ucli The ucli object.
 * @param andflags The existing flags will first be ANDed with this value.
 * @param orflags  The resulting flags will then be ORed with this value.
 */
int ucli_flags_mod(ucli_t* ucli, uint32_t andflags, uint32_t orflags);


/**
 * @brief Dispatch a command string.
 * @param ucli The ucli object.
 * @param pvs The output stream to use during this dispatch.
 * @param str The command string.
 *
 * @note This string can be single command, or several commands
 * separated by semicolons.
 */
int ucli_dispatch_string(ucli_t* ucli, aim_pvs_t* pvs,
                         const char* str);

/**
 * @brief Dispatch an array of strings in sequence.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param str The string array pointer.
 *
 * @note The given string array must be terminated with a NULL entry.
 *
 */
int ucli_dispatch_strings(ucli_t* ucli, aim_pvs_t* pvs,
                          const char** str);


/**
 * @brief Dispatch the program ARGV[] array.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param argv The ARGV[] array.
 *
 */
int ucli_dispatch_argv(ucli_t* ucli, aim_pvs_t* pvs, char** argv);


/**
 * @brief Convert an ARGV array to an array of individual commands
 * with arguments.
 * @param argv The ARGV[] array.
 *
 * @details
 * When command are passed through the program command line they are
 * already broken into tokens, even if those tokens span multiple commands.
 *
 * Any command given on the command-line through the ARGV array must be
 * prefixed with '-'.
 *
 * This function reconstructes the command+arguments be reassembling the
 * ARGV[] array into individual strings on '-' boundaries.
 */
char** ucli_argv_to_strings(char** argv);

/**
 * @brief Free the array the was returned by ucli_argv_to_strings()
 * @param args The args array returned from ucli_argv_to_strings().
 */
void ucli_argv_to_strings_free(char** args);


/**
 * @brief Read the given filename as a list of commands and execute them.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param filename The name of the file containing commands to execute.
 */
int ucli_dispatch_filename(ucli_t* ucli, aim_pvs_t* pvs,
                           const char* filename);

/**
 * @brief Read commands from the given FILE* and execute them.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param fp The file pointer.
 */
int ucli_dispatch_filep(ucli_t* ucli, aim_pvs_t* pvs, FILE* fp);

/**
 * uCli Commands can be instantiated directly in the code for execution.
 * This structure represents a block of this compiled-in commands.
 */
typedef struct ucli_block_s {
    /** The name of this block of commands -- mostly for the creator's use */
    const char* name;
    /** The array of commands in this block. */
    const char* commands[UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK];
} ucli_block_t;

/**
 * @brief Dispatch a command block.
 * @param ucli The ucli object,
 * @param pvs The output stream.
 * @param block Pointer to the block to execute.
 */
int ucli_dispatch_block(ucli_t* ucli, aim_pvs_t* pvs, ucli_block_t* block);

/**
 * @brief Dispatch a table of command blocks.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param block Pointer to the block array.
 * @param name The name of the specific block within the table to
 * execute. If name is NULL then all blocks are executed.
 *
 */
int ucli_dispatch_blocks(ucli_t* ucli, aim_pvs_t* pvs,
                         ucli_block_t* block, char* name);



/**
 * @brief Dispatch a tokenized string.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param args The argument tokens.
 */
int ucli_dispatch_args(ucli_t* ucli, aim_pvs_t* pvs, char** args);

/**
 * @brief Print output from the current command in the current context.
 * @param uc The current context.
 * @param fmt The format specification.
 *
 * @note All output from all command handlers should go through this function.
 */
int ucli_printf(ucli_context_t* uc, const char* fmt, ...);

/**
 * @brief The vararg version of the ucli_printf() function.
 */
int ucli_vprintf(ucli_context_t* uc, const char* fmt, va_list vargs);

/**
 * @brief The PVS-compatible definition for output to ucli_printf().
 * @param pvs The current context's PVS
 * @param fmt The format string.
 * @param vargs The format argument list.
 *
 * @note This function is populated in context->pvs.
 * @note You don't need to call it directly.
 * @note Instead, pass the &context->pvs as the PVS instance
 * @note for functions which require it.
 */
int ucli_pvs_vprintf(aim_pvs_t* pvs, const char* fmt, va_list vargs);

/**
 * @brief Initialize a uCli context.
 * @param uc The context pointer.
 * @param ucli The parent uCli for this context.
 * @param pvs The underlying output PVS for this context.
 *
 * @note This function is available to support testing only.
 * You would never need to call it under normal circumstances.
 */
int ucli_context_init(ucli_context_t* uc, ucli_t* ucli, aim_pvs_t* pvs);

/**
 * @brief Dump a uCli.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 *
 * @note Dumps the tree of nodes and modules contained within the uCli.
 */
int ucli_dump(ucli_t* ucli, aim_pvs_t* pvs);

/**
 * @brief Resets the current node as the root.
 * @param ucli The ucli object.
 */
void ucli_reset(ucli_t* ucli);

/**
 * @brief Returns a copy of the ucli object.
 * @param ucli The ucli object to duplicate.
 * @returns The new ucli copy.
 */
ucli_t* ucli_copy(ucli_t* ucli);

/**
 * @brief Returns a command completion.
 * @param ucli The ucli object.
 * @param line The current input line.
 * @returns NULL if there is no completion.
 * @returns The completion structure if completion information is available.
 *
 * @note You must free the returned structure with ucli_completion_free()
 */
ucli_completion_t* ucli_complete_string(ucli_t* ucli, const char* line);

/**
 * @brief Returns a command completion.
 * @param ucli The ucli object.
 * @param args The tokens of the current input line.
 * @returns NULL if there is no completion.
 * @returns The completion structure if completion information is available.
 */
ucli_completion_t* ucli_complete_args(ucli_t* ucli, const char** args);

/**
 * @brief Free a completion structure.
 * @param ucomp The completion structure to free.
 *
 * @note Structures returned from ucli_complete_string() and
 * ucli_complete_args() must be freed with this function.
 */
void ucli_completion_free(ucli_completion_t* ucomp);

/**
 * @brief Perform a completion event.
 * @param ucli The ucli object.
 * @param line The current input line.
 *
 * @details This will perform full completion processing in the
 * context of the current ucli object. It will display help and
 * completion options to the user when appropriate.
 *
 * @returns The string that should be appended to the current
 * input line.
 * @returns NULL if nothing should be appended to the current input line.
 *
 */
char* ucli_complete_hook(ucli_t* ucli, const char* line);

/**
 * @brief Enable or disable a command in the current node.
 * @param ucli The ucli object.
 * @param command The command
 * @param enable The enable or disable state.
 */
int ucli_command_enable(ucli_t* ucli, const char* command, int enable);

/**
 * @brief Enable or disable a command group.
 * @param ucli The ucli object.
 * @param group The group
 * @param enable The enable or disable state.
 */
int ucli_group_enable(ucli_t* ucli, const char* group, int enable);


/**
 * @brief Display help for a command group.
 * @param uc The ucli object.
 * @param group The group.
 * @param pvs The output PVS.
 * @param pfx Prefix for command help.
 */
int ucli_group_help(ucli_t* uc, const char* group, aim_pvs_t* pvs,
                    const char* pfx);

/**
 * @brief Print the current prompt to the given pvs.
 * @param ucli The ucli object.
 * @param pvs The output stream.
 * @param prompt The base prompt.
 *
 * @note The full uCli prompt consists of the (base_prompt) + (the path
 * to the currently selected node).
 */
int ucli_prompt_print(ucli_t* ucli, aim_pvs_t* pvs, const char* prompt);

/**
 * @brief Get the current prompt.
 * @param ucli The ucli object.
 * @param dst The buffer that receives the prompt string.
 * @param dst_size The size of the destination buffer.
 * @param prompt The base prompt in use.
 *
 * @note The full uCli prompt consists of the (base_prompt) + (the path
 * to the currently selected node).
 */
char* ucli_prompt_get(ucli_t* ucli, char* dst, int dst_size,
                      const char* prompt);

#if UCLI_CONFIG_INCLUDE_FGETS_LOOP == 1 || UCLI_CONFIG_INCLUDE_ELS_LOOP == 1
/**
 * @brief Run an input loop on stdin to get and dispatch commands
 * the given ucli.
 * @param ucli The ucli object.
 * @param prompt The base prompt to use.
 */
int ucli_run(ucli_t* ucli, const char* prompt);

#endif


#if UCLI_CONFIG_INCLUDE_MODULE_NODES == 1

/**
 * @brief Generate a node containing all builtin module nodes.
 */
ucli_node_t*
ucli_module_nodes_create(const char* node_name);

#endif /* UCLI_CONFIG_INCLUDE_MODULE_NODES */



#endif /* __UCLI_H__ */
/*@}*/
