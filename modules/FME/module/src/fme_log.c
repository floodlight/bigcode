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

#include <FME/fme_config.h>
#include <FME/fme.h>
#include "fme_int.h"

#include "fme_log.h"

/* <auto.start.enum(fme_log_flag).source> */
aim_map_si_t fme_log_flag_map[] =
{
    { "match", FME_LOG_FLAG_MATCH },
    { "timeout", FME_LOG_FLAG_TIMEOUT },
    { NULL, 0 }
};

aim_map_si_t fme_log_flag_desc_map[] =
{
    { "None", FME_LOG_FLAG_MATCH },
    { "None", FME_LOG_FLAG_TIMEOUT },
    { NULL, 0 }
};

const char*
fme_log_flag_name(fme_log_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, fme_log_flag_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'fme_log_flag'";
    }
}

int
fme_log_flag_value(const char* str, fme_log_flag_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, fme_log_flag_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
fme_log_flag_desc(fme_log_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, fme_log_flag_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'fme_log_flag'";
    }
}

int
fme_log_flag_valid(fme_log_flag_t e)
{
    return aim_map_si_i(NULL, e, fme_log_flag_map, 0) ? 1 : 0;
}

/* <auto.end.enum(fme_log_flag).source> */

/*
 * FME log struct
 */
AIM_LOG_STRUCT_DEFINE(
                      FME_CONFIG_LOG_OPTIONS_DEFAULT,
                      FME_CONFIG_LOG_BITS_DEFAULT,
                      fme_log_flag_map, /* Custom log map */
                      FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT
                      );
