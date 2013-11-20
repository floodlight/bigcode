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
#include <uCli/ucli.h>
#include <uCli/ucli_config.h>
#include <uCli/ucli_argparse.h>
#include <BigList/biglist.h>
#include <inttypes.h>
#include <AIM/aim_datatypes.h>

int
ucli_vargparse_try(ucli_context_t* uc, const char* fmt, aim_va_list_t* vargs)
{
    int rv;
    uint32_t flags = uc->flags;
    uc->flags |= UCLI_CONTEXT_F_SILENT;
    rv = ucli_vargparse(uc, fmt, vargs);
    uc->flags = flags;
    return rv;
}

int
ucli_argparse_try(ucli_context_t* uc, const char* fmt, ...)
{
    int rv;
    aim_va_list_t vargs;
    va_start(vargs.val, fmt);
    rv = ucli_vargparse_try(uc, fmt, &vargs);
    va_end(vargs.val);
    return rv;
}

int
ucli_argparse(ucli_context_t* uc, const char* fmt, ...)
{
    int rv;
    aim_va_list_t vargs;
    va_start(vargs.val, fmt);
    rv = ucli_vargparse(uc, fmt, &vargs);
    va_end(vargs.val);
    return rv;
}

static int
ucli_vargparse_type__(ucli_context_t* uc,
                      char c, char* type, const char* arg,
                      aim_va_list_t* vargs)
{
    int rv;
    aim_datatype_t* dt;

    if(arg == NULL) {
        return UCLI_STATUS_E_ARG;
    }

    if(type && type[0] == '%') {
        /* Immediate string specified. The argument must be that string. */
        if(type[1] == 0) {
            /* No string */
            return UCLI_STATUS_E_INTERNAL;
        }
        if(!UCLI_STRCMP(type+1, arg)) {
            return UCLI_STATUS_OK;
        }
        ucli_error(uc, "expected '%s', not '%s'", type+1, arg);
        return UCLI_STATUS_E_ARG;
    }

    dt = aim_datatype_find(c, type);
    if(dt == NULL) {
        /* Unrecognized type */
        ucli_error(uc, "<bug: no handler for type '%c:%s'>", (c) ? c : '.', (type) ? type : "[NULL]");
        AIM_LOG_ERROR("bug: no handler for type '%c:%s'", (c) ? c : '.', (type) ? type : "[NULL]");
        return UCLI_STATUS_E_INTERNAL;
    }

    {
        aim_datatype_context_t dtc;
        dtc.dt = dt;
        dtc.epvs = &uc->pvs;
        rv = dt->from_str(&dtc, arg, vargs);
    }


    if( (rv < 0) && dt->desc) {
        ucli_e_arg(uc, arg, dt->desc);
    }
    return rv;
}


int
ucli_vargparse(ucli_context_t* uc, const char* fmt, aim_va_list_t* vargs)
{
    int rv;
    int i;
    const char* type;

    for(i = 0, type = fmt; *type; type++, i++) {
        char typeName[32];

        UCLI_MEMSET(typeName, 0, sizeof(typeName));

        if(*type == '{') {
            /* Read type name */
            const char* src;
            char* dst;
            UCLI_MEMSET(typeName, 0, sizeof(typeName));
            for(src = ++type, dst=typeName; *src && *src != '}';
                src++, dst++) {
                *dst = *src;
            }
            if(*src != '}') {
                /* Error in format string */
                UCLI_MERROR("bad internal format string.");
                return -1;
            }
            else {
                /* next character is the next format specifier */
                type = src;
            }

            rv = ucli_vargparse_type__(uc, 0, typeName, uc->pargs->args[i],
                                       vargs);
            if(rv < 0) {
                return rv;
            }
        }
        else {
            rv = ucli_vargparse_type__(uc, *type, NULL, uc->pargs->args[i],
                                       vargs);
            if(rv < 0) {
                return rv;
            }
        }
    }

    return UCLI_STATUS_OK;
}
