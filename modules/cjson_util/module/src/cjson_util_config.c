/************************************************************
 * <bsn.cl fy=2014 v=epl>
 * 
 *           Copyright 2014 Big Switch Networks, Inc.          
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
 *
 *
 ***********************************************************/

#include <cjson_util/cjson_util_config.h>

/* <auto.start.cdefs(CJSON_UTIL_CONFIG_HEADER).source> */
#define __cjson_util_config_STRINGIFY_NAME(_x) #_x
#define __cjson_util_config_STRINGIFY_VALUE(_x) __cjson_util_config_STRINGIFY_NAME(_x)
cjson_util_config_settings_t cjson_util_config_settings[] =
{
#ifdef CJSON_UTIL_CONFIG_INCLUDE_LOGGING
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_INCLUDE_LOGGING), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_INCLUDE_LOGGING) },
#else
{ CJSON_UTIL_CONFIG_INCLUDE_LOGGING(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ CJSON_UTIL_CONFIG_LOG_OPTIONS_DEFAULT(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT) },
#else
{ CJSON_UTIL_CONFIG_LOG_BITS_DEFAULT(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ CJSON_UTIL_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef CJSON_UTIL_CONFIG_PORTING_STDLIB
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_PORTING_STDLIB), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_PORTING_STDLIB) },
#else
{ CJSON_UTIL_CONFIG_PORTING_STDLIB(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ CJSON_UTIL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef CJSON_UTIL_CONFIG_INCLUDE_UCLI
    { __cjson_util_config_STRINGIFY_NAME(CJSON_UTIL_CONFIG_INCLUDE_UCLI), __cjson_util_config_STRINGIFY_VALUE(CJSON_UTIL_CONFIG_INCLUDE_UCLI) },
#else
{ CJSON_UTIL_CONFIG_INCLUDE_UCLI(__cjson_util_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __cjson_util_config_STRINGIFY_VALUE
#undef __cjson_util_config_STRINGIFY_NAME

const char*
cjson_util_config_lookup(const char* setting)
{
    int i;
    for(i = 0; cjson_util_config_settings[i].name; i++) {
        if(!strcmp(cjson_util_config_settings[i].name, setting)) {
            return cjson_util_config_settings[i].value;
        }
    }
    return NULL;
}

int
cjson_util_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; cjson_util_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", cjson_util_config_settings[i].name, cjson_util_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(CJSON_UTIL_CONFIG_HEADER).source> */

