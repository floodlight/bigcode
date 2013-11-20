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

#include <nwac/nwac_config.h>

/* <auto.start.cdefs(NWAC_CONFIG_HEADER).source> */
#define __nwac_config_STRINGIFY_NAME(_x) #_x
#define __nwac_config_STRINGIFY_VALUE(_x) __nwac_config_STRINGIFY_NAME(_x)
nwac_config_settings_t nwac_config_settings[] =
{
#ifdef NWAC_CONFIG_INCLUDE_LOGGING
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_INCLUDE_LOGGING), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_INCLUDE_LOGGING) },
#else
{ NWAC_CONFIG_INCLUDE_LOGGING(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_LOG_OPTIONS_DEFAULT
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_LOG_OPTIONS_DEFAULT), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ NWAC_CONFIG_LOG_OPTIONS_DEFAULT(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_LOG_BITS_DEFAULT
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_LOG_BITS_DEFAULT), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_LOG_BITS_DEFAULT) },
#else
{ NWAC_CONFIG_LOG_BITS_DEFAULT(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_PORTING_STDLIB
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_PORTING_STDLIB), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_PORTING_STDLIB) },
#else
{ NWAC_CONFIG_PORTING_STDLIB(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ NWAC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_INCLUDE_UCLI
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_INCLUDE_UCLI), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_INCLUDE_UCLI) },
#else
{ NWAC_CONFIG_INCLUDE_UCLI(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef NWAC_CONFIG_INCLUDE_LOCKING
    { __nwac_config_STRINGIFY_NAME(NWAC_CONFIG_INCLUDE_LOCKING), __nwac_config_STRINGIFY_VALUE(NWAC_CONFIG_INCLUDE_LOCKING) },
#else
{ NWAC_CONFIG_INCLUDE_LOCKING(__nwac_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __nwac_config_STRINGIFY_VALUE
#undef __nwac_config_STRINGIFY_NAME

const char*
nwac_config_lookup(const char* setting)
{
    int i;
    for(i = 0; nwac_config_settings[i].name; i++) {
        if(strcmp(nwac_config_settings[i].name, setting)) {
            return nwac_config_settings[i].value;
        }
    }
    return NULL;
}

int
nwac_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; nwac_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", nwac_config_settings[i].name, nwac_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(NWAC_CONFIG_HEADER).source> */

