/************************************************************
 * <bsn.cl fy=2013 v=epl-1.0>
 *
 *        Copyright 2013, 2014 Big Switch Networks, Inc.
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
 * </bsn.cl>
 ************************************************************
 *
 *  ELS Config Information
 *
 ***********************************************************/
#include <ELS/els_config.h>
#include <ELS/els.h>
#include "els_int.h"
#include <stdlib.h>
#include <string.h>



/* <auto.start.cdefs(ELS_CONFIG_HEADER).source> */
#define __els_config_STRINGIFY_NAME(_x) #_x
#define __els_config_STRINGIFY_VALUE(_x) __els_config_STRINGIFY_NAME(_x)
els_config_settings_t els_config_settings[] =
{
#ifdef ELS_CONFIG_INCLUDE_LOGGING
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_INCLUDE_LOGGING), __els_config_STRINGIFY_VALUE(ELS_CONFIG_INCLUDE_LOGGING) },
#else
{ ELS_CONFIG_INCLUDE_LOGGING(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_LOG_OPTIONS_DEFAULT
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_LOG_OPTIONS_DEFAULT), __els_config_STRINGIFY_VALUE(ELS_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ ELS_CONFIG_LOG_OPTIONS_DEFAULT(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_LOG_BITS_DEFAULT
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_LOG_BITS_DEFAULT), __els_config_STRINGIFY_VALUE(ELS_CONFIG_LOG_BITS_DEFAULT) },
#else
{ ELS_CONFIG_LOG_BITS_DEFAULT(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __els_config_STRINGIFY_VALUE(ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ ELS_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_LOG_OUTPUT_DEFAULT
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_LOG_OUTPUT_DEFAULT), __els_config_STRINGIFY_VALUE(ELS_CONFIG_LOG_OUTPUT_DEFAULT) },
#else
{ ELS_CONFIG_LOG_OUTPUT_DEFAULT(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_PORTING_STDLIB
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_PORTING_STDLIB), __els_config_STRINGIFY_VALUE(ELS_CONFIG_PORTING_STDLIB) },
#else
{ ELS_CONFIG_PORTING_STDLIB(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __els_config_STRINGIFY_VALUE(ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ ELS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_INCLUDE_UCLI
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_INCLUDE_UCLI), __els_config_STRINGIFY_VALUE(ELS_CONFIG_INCLUDE_UCLI) },
#else
{ ELS_CONFIG_INCLUDE_UCLI(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_INCLUDE_EVENTFD
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_INCLUDE_EVENTFD), __els_config_STRINGIFY_VALUE(ELS_CONFIG_INCLUDE_EVENTFD) },
#else
{ ELS_CONFIG_INCLUDE_EVENTFD(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ELS_CONFIG_MAX_PROMPT
    { __els_config_STRINGIFY_NAME(ELS_CONFIG_MAX_PROMPT), __els_config_STRINGIFY_VALUE(ELS_CONFIG_MAX_PROMPT) },
#else
{ ELS_CONFIG_MAX_PROMPT(__els_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __els_config_STRINGIFY_VALUE
#undef __els_config_STRINGIFY_NAME

const char*
els_config_lookup(const char* setting)
{
    int i;
    for(i = 0; els_config_settings[i].name; i++) {
        if(strcmp(els_config_settings[i].name, setting)) {
            return els_config_settings[i].value;
        }
    }
    return NULL;
}

int
els_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; els_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", els_config_settings[i].name, els_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(ELS_CONFIG_HEADER).source> */



