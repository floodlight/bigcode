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
 * @brief Error Reporting.
 *
 *
 * @addtogroup ucli-ucli
 * @{
 *
 *
 ***************************************************************/
#ifndef __UCLI_ERRORS_H__
#define __UCLI_ERRORS_H__

/**
 * @brief Report a generic error during command processing to the user.
 * @param uc The current context.
 * @param fmt The format specification.
 * @returns UCLI_STATUS_E_ERROR
 *
 * @details Prints the given error message to the current context
 * and returns UCLI_STATUS_E_ERROR.
 *
 */
int ucli_error(ucli_context_t* uc, const char* fmt, ...);

/**
 * @brief The varargs version of ucli_error()
 */
int ucli_verror(ucli_context_t* uc, const char* fmt, va_list vargs);


/**
 * @brief Report an argument format error to the user.
 * @param uc The current context.
 * @param arg The argument string.
 * @param desc The description of the given argument type.
 *
 * @note You will seldom need to call this function directly.
 * It is called for you during errors in ucli_argparse().
 */
int ucli_e_arg(ucli_context_t* uc, const char* arg, const char* desc);

/**
 * @brief Report an internal error.
 * @param uc The current context.
 * @param fmt The internal error format string.
 * @note This will notify the user than an internal error has occured in the
 * code. This should only be used for code, assertion, and unexpected
 * logic errors only. Proper execution which results in an error condition
 * should not be reported with this function.
 */
int ucli_e_internal(ucli_context_t* uc, const char* fmt, ...);


#endif /* __UCLI_ERRORS_H__ */
/*@}*/
