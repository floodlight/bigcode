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
 * @brief uCli Argument Parsing.
 *
 * @addtogroup ucli-argparse
 * @{
 *
 *
 ***************************************************************/

#ifndef __UCLI_ARGPARSE_H__
#define __UCLI_ARGPARSE_H__

#include <AIM/aim_datatypes.h>

/**
 * @brief Parse all arguments in the current context according to the
 * given argument specification.
 *
 * @param uc The current context.
 * @param fmt The argument type definitions.
 *
 * @returns OK on sucessful parse
 * @returns E_ARG on failure.
 *
 * @note Parse and datatype errors will be reported automatically
 * on failure.
 *
 */
int ucli_argparse(ucli_context_t* uc, const char* fmt, ...);

/**
 * @brief The vararg version of ucli_argparse()
 */
int ucli_vargparse(ucli_context_t* uc, const char* fmt, aim_va_list_t* vargs);



/**
 * @brief Attempt to parse all arguments in the current context according to
 * the given argument specification.
 * @param uc The current context.
 * @param fmt Th eargument type definitions.
 *
 * @returns OK on successful parse.
 * @returns E_ARG on failure.
 *
 * @note No error is reported to the user. This function is designed
 * to allow the parsing of multiple types of arguments in the same command
 * by calling it for every combination of supported arguments.
 */
int ucli_argparse_try(ucli_context_t* uc, const char* fmt, ...);

/**
 * @brief The vararg version of ucli_argparse_try().
 */
int ucli_vargparse_try(ucli_context_t* uc, const char* fmt, aim_va_list_t* vargs);


/**************************************************************************//**
 *
 * Use this macro for most argument parsing operations.
 *
 * It will attempt to parse your command's arguments as per
 * your format specification, or report errors and return from
 * your command handler.
 *
 * You can assume that all of your handler code that executes after
 * this macro has received the appropriatly typed and formatted
 * argument variables.
 *
 *****************************************************************************/

#define UCLI_ARGPARSE_OR_RETURN(_context, _fmt, ...)                    \
    do {                                                                \
        if(ucli_argparse(_context, _fmt, __VA_ARGS__) < 0) {            \
            return UCLI_STATUS_E_ARG;                                   \
        }                                                               \
    } while(0)

#endif /* __UCLI_ARGPARSE_H__ */
/*@}*/
