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

#include <IOF/iof.h>
#include "iof_log.h"

static int
iof_inherit_vprintf__(aim_pvs_t* pvs, const char* fmt, va_list vargs)
{
    AIM_REFERENCE(vargs);
    /** Sentinal only, should never be called. */
    AIM_LOG_INTERNAL("iof_inherit_vprintf() called: pvs=%p fmt='%s'",
                     pvs, fmt);
    AIM_LOG_INTERNAL("The caller was expected to create an IOF object for output.");
    return -1;
}

int
iof_init(iof_t* iof, aim_pvs_t* pvs)
{
    if(iof == NULL || pvs == NULL) {
        return -1;
    }

    /*
     * We support inheriting the caller's IOF instance
     * if thats what was passed in.
     */
    if(pvs->vprintf == iof_inherit_vprintf__) {
        /** We assume the cookie is already a proper IOF structure and
         * inherit the current setttings.
         */
        IOF_MEMCPY(iof, (iof_t*)pvs->object.cookie, sizeof(*iof));
    }
    else {
        IOF_MEMSET(iof, 0, sizeof(*iof));

        iof->level = IOF_CONFIG_DEFAULT_INDENT_INITIAL;
        iof->indent_factor = IOF_CONFIG_DEFAULT_INDENT_FACTOR;
        iof->push_string = IOF_CONFIG_DEFAULT_PUSH_STRING;
        iof->pop_string = IOF_CONFIG_DEFAULT_POP_STRING;
        iof->indent_string = IOF_CONFIG_DEFAULT_INDENT_STRING;
        iof->indent_terminator = IOF_CONFIG_DEFAULT_INDENT_TERMINATOR;

        iof->pvs = pvs;
        iof->inherit.vprintf = iof_inherit_vprintf__;
    }

    /* The inherited IOF structure is always the current one */
    iof->inherit.object.cookie = iof;
    return 0;
}

int
iof_push(iof_t* iof, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = iof_vpush(iof, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
iof_vpush(iof_t* iof, const char* fmt, va_list vargs)
{
    iof_indent(iof);
    iof_uvprintf(iof, fmt, vargs);
    iof_uprintf(iof, "%s\n", iof->push_string);
    iof->level++;
    return 0;
}

int
iof_pop(iof_t* iof)
{
    --iof->level;
    iof_indent(iof);
    iof_uprintf(iof, "%s\n", iof->pop_string);
    return 0;
}

int
iof_indent(iof_t* iof)
{
    int i;
    for(i = 0; i < iof->level*iof->indent_factor - 1; i++) {
        iof_uprintf(iof, "%s", iof->indent_string);
    }
    iof_uprintf(iof, "%s", iof->indent_terminator);
    return 0;
}

int
iof_iprintf(iof_t* iof, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = iof_ivprintf(iof, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
iof_iprintfn(iof_t* iof, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = iof_ivprintfn(iof, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
iof_ivprintf(iof_t* iof, const char* fmt, va_list vargs)
{
    int rv;
    rv = iof_ivprintfn(iof, fmt, vargs);
    rv += iof_uprintf(iof, "\n");
    return rv;
}

int
iof_ivprintfn(iof_t* iof, const char* fmt, va_list vargs)
{
    iof_indent(iof);
    iof_uvprintf(iof, fmt, vargs);
    return 0;
}

int
iof_uprintf(iof_t* iof, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = iof_uvprintf(iof, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
iof_uvprintf(iof_t* iof, const char* fmt, va_list vargs)
{
    return aim_vprintf(iof->pvs, fmt, vargs);
}


