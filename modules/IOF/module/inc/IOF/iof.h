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
 * @brief Indented Object Formatter Public Interface
 *
 * @addtogroup iof-iof
 * @{
 *
 ***************************************************************/

#ifndef __IOF_H__
#define __IOF_H__

#include <IOF/iof_config.h>
#include <AIM/aim.h>
#include <stdarg.h>



/**
 * IOF Object.
 */
typedef struct iof_s {

    /**
     * The output stream to use.
     */
    aim_pvs_t* pvs;

    /**
     * The current indentation level.
     */
    int level;

    /** Indentation factor - default is IOF_CONFIG_DEFAULT_INDENT_FACTOR */
    int indent_factor;

    /** Scope push string - default is IOF_CONFIG_DEFAULT_PUSH_STRING */
    const char* push_string;

    /** Scope pop string - default is IOF_CONFIG_DEFAULT_POP_STRING */
    const char* pop_string;

    /** indent string - default is IOF_CONFIG_DEFAULT_INDENT_STRING */
    const char* indent_string;

    /** indent term - default is IOF_CONFIG_DEFAULT_INDENT_TERMINATOR */
    const char* indent_terminator;

    /**
     * Use this PVS structure to pass this IOF object
     * to another function which expects a PVS but uses IOF output.
     * This will allow the child function's output to be nested
     * properly.
     */
    aim_pvs_t inherit;
} iof_t;

/**
 * @brief Initialize an IOF object.
 * @param iof Pointer to the IOF structure to initialize.
 * @param pvs The AIM output vector.
 *
 * @returns 0 on success, negative on error.
 */
int iof_init(iof_t* iof, aim_pvs_t* pvs);

/**
 * @brief Push a message and a new indentation level.
 * @param iof The IOF object.
 * @param fmt The message to output.
 *
 */
int iof_push(iof_t* iof, const char* fmt, ...);

/**
 * @brief Push a message and a new indentation level.
 * @param iof The IOF object.
 * @param fmt The format string for the message.
 * @param vargs The arguments for the format string.
 */
int iof_vpush(iof_t* iof, const char* fmt, va_list vargs);

/**
 * @brief Pop to previous indentation level.
 * @param iof The IOF object.
 */
int iof_pop(iof_t* iof);

/**
 * @brief Print a message at the current indentation level.
 * @param iof The IOF object.
 * @param fmt The message format string.
 * @note A newline is always appended.
 */
int iof_iprintf(iof_t* iof, const char* fmt, ...);

/**
 * @brief Print a message at the current indentation level.
 * @param iof The IOF object.
 * @param fmt The message format string.
 * @note No newline is printed at the end when calling this function.
 * This is equivalent to { iof_indent(); iof_uprintf() }
 */
int iof_iprintfn(iof_t* iof, const char* fmt, ...);

/**
 * @brief Print a message at the current indentation level.
 * @param iof The IOF object.
 * @param fmt The message format string.
 * @param vargs The arguments for the format string.
 */
int iof_ivprintf(iof_t* iof, const char* fmt, va_list vargs);

/**
 * @brief Print a message at the current indentation level.
 * @param iof The IOF object.
 * @param fmt The message format string.
 * @param vargs The arguments for the format string.
 * @note No newline is printed at the end when calling this function.
 * This is equivalent to { iof_indent(); iof_uvprintf() }
 */
int iof_ivprintfn(iof_t* iof, const char* fmt, va_list vargs);

/**
 * @brief Print a message (no indentation).
 * @param iof The IOF object.
 * @param fmt The message format string.
 *
 * @note The message is printed without indentation or newline.
 * Combine with iof_iprintf() to produce a single line
 * of output from multiple output calls.
 */
int iof_uprintf(iof_t* iof, const char* fmt, ...);

/**
 * @brief Print a message (no indentation).
 * @param iof The IOF object.
 * @param fmt The message format string.
 * @param vargs The arguments for the format string.
 *
 * @note The message is printed without indentation or newline.
 * Combine with iof_ivprintf() to produce a single line
 * of output from multiple output calls.
 */
int iof_uvprintf(iof_t* iof, const char* fmt, va_list vargs);

/**
 * @brief Indent the current line.
 * @brief iof The IOF object.
 *
 * @note There are few circumstances in which you would need to
 * call this function directly.
 */
int iof_indent(iof_t* iof);


#endif /* __IOF_H__ */
/*@}*/
