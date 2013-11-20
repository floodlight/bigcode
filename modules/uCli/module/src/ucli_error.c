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
#include <stdarg.h>

int
ucli_e_arg(ucli_context_t* uc, const char* arg, const char* desc)
{
    ucli_error(uc, "argument '%s' is not a valid %s", arg, desc);
    return UCLI_STATUS_E_ARG;
}

int
ucli_verror(ucli_context_t* uc, const char* fmt, va_list vargs)
{
    if( (uc->flags & UCLI_CONTEXT_F_SILENT) == 0) {
        ucli_printf(uc, "error: ");
        ucli_vprintf(uc, fmt, vargs);
        ucli_printf(uc, "\n");
    }
    return UCLI_STATUS_E_ERROR;
}

int
ucli_error(ucli_context_t* uc, const char* fmt, ...)
{
    /* TODO - customize */
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = ucli_verror(uc, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
ucli_e_internal(ucli_context_t* uc, const char* fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);
    ucli_printf(uc, "internal error: ");
    ucli_vprintf(uc, fmt, vargs);
    ucli_printf(uc, "\n");
    return UCLI_STATUS_E_INTERNAL;
}




