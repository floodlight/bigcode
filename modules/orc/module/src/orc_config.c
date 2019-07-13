/************************************************************
 * <bsn.cl fy=2013 v=epl>
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
 *
 *
 ***********************************************************/

#include <orc/orc_config.h>

/* <auto.start.cdefs(ORC_CONFIG_HEADER).source> */
#define __orc_config_STRINGIFY_NAME(_x) #_x
#define __orc_config_STRINGIFY_VALUE(_x) __orc_config_STRINGIFY_NAME(_x)
orc_config_settings_t orc_config_settings[] =
{
#ifdef ORC_CONFIG_INCLUDE_LOGGING
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_INCLUDE_LOGGING), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_INCLUDE_LOGGING) },
#else
{ ORC_CONFIG_INCLUDE_LOGGING(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_LOG_OPTIONS_DEFAULT
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_LOG_OPTIONS_DEFAULT), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ ORC_CONFIG_LOG_OPTIONS_DEFAULT(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_LOG_BITS_DEFAULT
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_LOG_BITS_DEFAULT), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_LOG_BITS_DEFAULT) },
#else
{ ORC_CONFIG_LOG_BITS_DEFAULT(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ ORC_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_PORTING_STDLIB
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_PORTING_STDLIB), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_PORTING_STDLIB) },
#else
{ ORC_CONFIG_PORTING_STDLIB(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ ORC_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_INCLUDE_UCLI
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_INCLUDE_UCLI), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_INCLUDE_UCLI) },
#else
{ ORC_CONFIG_INCLUDE_UCLI(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef ORC_CONFIG_INCLUDE_AIM_MAIN
    { __orc_config_STRINGIFY_NAME(ORC_CONFIG_INCLUDE_AIM_MAIN), __orc_config_STRINGIFY_VALUE(ORC_CONFIG_INCLUDE_AIM_MAIN) },
#else
{ ORC_CONFIG_INCLUDE_AIM_MAIN(__orc_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __orc_config_STRINGIFY_VALUE
#undef __orc_config_STRINGIFY_NAME

const char*
orc_config_lookup(const char* setting)
{
    int i;
    for(i = 0; orc_config_settings[i].name; i++) {
        if(!strcmp(orc_config_settings[i].name, setting)) {
            return orc_config_settings[i].value;
        }
    }
    return NULL;
}

int
orc_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; orc_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", orc_config_settings[i].name, orc_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(ORC_CONFIG_HEADER).source> */

