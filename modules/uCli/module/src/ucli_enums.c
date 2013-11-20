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

#include <uCli/ucli_config.h>
#include <uCli/ucli.h>
#include "ucli_int.h"

/* <auto.start.enum(ALL).source> */
aim_map_si_t ucli_status_map[] =
{
    { "ok", UCLI_STATUS_OK },
    { "continue", UCLI_STATUS_CONTINUE },
    { "e_not_found", UCLI_STATUS_E_NOT_FOUND },
    { "e_error", UCLI_STATUS_E_ERROR },
    { "e_arg", UCLI_STATUS_E_ARG },
    { "e_internal", UCLI_STATUS_E_INTERNAL },
    { "e_exit", UCLI_STATUS_E_EXIT },
    { "e_io", UCLI_STATUS_E_IO },
    { "e_param", UCLI_STATUS_E_PARAM },
    { NULL, 0 }
};

aim_map_si_t ucli_status_desc_map[] =
{
    { "None", UCLI_STATUS_OK },
    { "None", UCLI_STATUS_CONTINUE },
    { "None", UCLI_STATUS_E_NOT_FOUND },
    { "None", UCLI_STATUS_E_ERROR },
    { "None", UCLI_STATUS_E_ARG },
    { "None", UCLI_STATUS_E_INTERNAL },
    { "None", UCLI_STATUS_E_EXIT },
    { "None", UCLI_STATUS_E_IO },
    { "None", UCLI_STATUS_E_PARAM },
    { NULL, 0 }
};

const char*
ucli_status_name(ucli_status_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ucli_status_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ucli_status'";
    }
}

int
ucli_status_value(const char* str, ucli_status_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ucli_status_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ucli_status_desc(ucli_status_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ucli_status_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ucli_status'";
    }
}

int
ucli_status_valid(ucli_status_t e)
{
    return aim_map_si_i(NULL, e, ucli_status_map, 0) ? 1 : 0;
}

/* <auto.end.enum(ALL).source> */
