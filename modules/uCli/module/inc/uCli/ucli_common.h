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

/****************************************************************
 *
 * @file
 * @brief uCli Common Definitions and datatypes.
 *
 * @addtogroup ucli-ucli
 * @{
 *
 ***************************************************************/

#ifndef __UCLI_COMMON_H__
#define __UCLI_COMMON_H__

#include <uCli/ucli_config.h>
#include <BigList/biglist.h>

/******************************************************************************
 *
 * Positional arguments are passed to the handler in this structure.
 *
 *
 *****************************************************************************/
typedef struct ucli_pargs_s {

    /** Raw arguments */
    const char** args;
    /** Raw argument count */
    int count;

    /** Internal use only  */
    const char* args__[UCLI_CONFIG_MAX_ARGS];

} ucli_pargs_t;

/******************************************************************************
 *
 * Command handlers can define and receive optional "keyword=value"
 * arguments as well as positional arguments.
 *
 * This defines a single keyword structure.
 *
 *****************************************************************************/
typedef struct ucli_kwarg_t {
    /** The keyword name */
    const char* name;
    /** The value of the keyword */
    const char* value;
} ucli_kwarg_t;

/******************************************************************************
 *
 * All keyword arguments are passed to the handler in this structure.
 *
 *
 *****************************************************************************/
typedef struct ucli_kwargs_s {
    /** Array of keywords */
    ucli_kwarg_t** kwargs;
    /** keyword structure count */
    int count;

    /** Internal use only */
    const ucli_kwarg_t* kwargs__[UCLI_CONFIG_MAX_ARGS];
} ucli_kwargs_t;


/******************************************************************************
 *
 * The following are the return values for commands and
 * command processing.
 *
 *
 *****************************************************************************/
/* <auto.start.enum(ucli_status).define> */
/** ucli_status */
typedef enum ucli_status_e {
    UCLI_STATUS_OK = 0,
    UCLI_STATUS_CONTINUE = 1,
    UCLI_STATUS_E_NOT_FOUND = -1,
    UCLI_STATUS_E_ERROR = -2,
    UCLI_STATUS_E_ARG = -3,
    UCLI_STATUS_E_INTERNAL = -4,
    UCLI_STATUS_E_EXIT = -5,
    UCLI_STATUS_E_IO = -6,
    UCLI_STATUS_E_PARAM = -7,
} ucli_status_t;
/* <auto.end.enum(ucli_status).define> */


struct ucli_context_s;
/******************************************************************************
 *
 * This is the signature for every command handler.
 *
 *
 *****************************************************************************/
typedef ucli_status_t (*ucli_command_handler_f)(struct ucli_context_s*);


/******************************************************************************
 *
 * This structure registers a handler for a given command.
 *
 *
 *****************************************************************************/
typedef struct ucli_command_s {

    /** The name of the command -- this is what gets typed by the user
     * to access the handler.
     */
    const char* command;

    /** The command handler implementation */
    ucli_command_handler_f handler;

    /**
     * If you require a specific number of arguments you can shortcut
     * checking for them by specifiying the count here.
     * You won't get called unless they exist.
     *
     * If you accept variable arguments, set this to -1 and
     * no checking will be performed.
     */
    int arg_count;

    /**
     * This structure contains command help information.
     */
    struct {
        /** Command summary. */
        const char* summary;

        /** Argument descriptions */
        const char* args;

        /** Full details. */
        const char* full;
    } help;

    /** Optional command group */
    char* group;

    /** Thread context for this command. */
    int threaded;

    /** Internal use only */
    char* initspec__;

    /** The number of times this command has been invoked and succeeded. */
    int success_count;
    /** The number of times this command has been invoked and failed. */
    int fail_count;

} ucli_command_t;

/******************************************************************************
 *
 * All command handlers receive their arguments
 * and execution context within this structure.
 *
 *****************************************************************************/
typedef struct ucli_context_s {

    /** External output vector for this context */
    aim_pvs_t* epvs;

    /** The output stream you should pass to PVS functions for this context. */
    aim_pvs_t pvs;

    /** Module's private data */
    void* cookie;

    /** Positional arguments */
    ucli_pargs_t* pargs;

    /** Keyword arguments */
    ucli_kwargs_t* kwargs;

    /** No output while executing */
#define UCLI_CONTEXT_F_SILENT               ( 1<<0 )
    /** Get the command handler's information */
#define UCLI_CONTEXT_F_GET_COMMAND_INFO     ( 1<<1 )
    /** flags */
    uint32_t flags;

    /** The entire UCLI instance in which we are executing */
    struct ucli_s* root;

    /** Our parent node */
    struct ucli_node_s* parent;

    /**
     * Command information structure.
     * The structure this points to should be initialized with
     * your command info when UCLI_CONTEXT_F_GET_COMMAND_INFO is
     * specified.
     */
    ucli_command_t* info;

} ucli_context_t;


struct ucli_node_s;

/**
 * Data structure for context command completion.
 */
typedef struct ucli_completion_s {
    /** The full next token */
    char* next_token;
    /** The string to append to the current line to complete, if applicable */
    char* append_string;

    /**
     * The possible next tokens, if a complete match cannot be made
     */
    struct {
        /** List of possible matches */
        biglist_t* list;
        /** count */
        int count;
    } /** Possible match tokens */ possibilities;

    /**
     * Internal
     * Indicate that the help message for a node should be displayed.
     */
    struct ucli_node_s* help_node;

} ucli_completion_t;


/**
 * @brief Convert an input string to character data.
 * @param string The input string.
 * @param size Receives the size of the returned data buffer.
 * @returns a new buffer containing the bytes specified in the string.
 * @note You must free the bytes when done.
 */
uint8_t* ucli_util_data_from_string(const char* string, int* size);

/**
 * @brief Convert a data buffer to a string.
 * @param uc The current context (for errors).
 * @param data The data buffer to convert.
 * @param size The size of the data buffer
 * @param columns The number of columns in which the data should be formatted.
 */
char* ucli_util_data_to_string(ucli_context_t* uc, uint8_t* data, int size,
                               int columns);

/******************************************************************************
 *
 * Use this macro to declare the properties of your command handler.
 * Every handler must include this macro in the beginning of the implementation.
 *
 *
 *****************************************************************************/
#ifndef UCLI_COMMAND_INIT
#define UCLI_COMMAND_INIT /* Nothing */
#endif


/** Command handler initialization and registration. */
#define UCLI_COMMAND_INFO(_uc, _name, _arg_count, _initspec)            \
    UCLI_COMMAND_INIT;                                                  \
    do {                                                                \
        if(_uc->flags & UCLI_CONTEXT_F_GET_COMMAND_INFO) {              \
            _uc->info->command = _name;                                 \
            _uc->info->arg_count = _arg_count;                          \
            _uc->info->initspec__ = _initspec;                          \
            return UCLI_STATUS_OK;                                      \
        }                                                               \
    } while(0);


#endif /* __UCLI_COMMANDS_H__ */
/*@}*/
