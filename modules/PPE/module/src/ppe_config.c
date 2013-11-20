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

#include <PPE/ppe_config.h>
#include <PPE/ppe.h>
#include "ppe_int.h"
#include <stdlib.h>
#include <string.h>



/* <auto.start.cdefs(PPE_CONFIG_HEADER).source> */
#define __ppe_config_STRINGIFY_NAME(_x) #_x
#define __ppe_config_STRINGIFY_VALUE(_x) __ppe_config_STRINGIFY_NAME(_x)
ppe_config_settings_t ppe_config_settings[] =
{
#ifdef PPE_CONFIG_INCLUDE_LOGGING
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_INCLUDE_LOGGING), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_INCLUDE_LOGGING) },
#else
{ PPE_CONFIG_INCLUDE_LOGGING(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_LOG_OPTIONS_DEFAULT
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_LOG_OPTIONS_DEFAULT), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ PPE_CONFIG_LOG_OPTIONS_DEFAULT(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_LOG_BITS_DEFAULT
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_LOG_BITS_DEFAULT), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_LOG_BITS_DEFAULT) },
#else
{ PPE_CONFIG_LOG_BITS_DEFAULT(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ PPE_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_PORTING_STDLIB
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_PORTING_STDLIB), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_PORTING_STDLIB) },
#else
{ PPE_CONFIG_PORTING_STDLIB(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ PPE_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS) },
#else
{ PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_INCLUDE_UCLI
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_INCLUDE_UCLI), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_INCLUDE_UCLI) },
#else
{ PPE_CONFIG_INCLUDE_UCLI(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PPE_CONFIG_INCLUDE_UTM
    { __ppe_config_STRINGIFY_NAME(PPE_CONFIG_INCLUDE_UTM), __ppe_config_STRINGIFY_VALUE(PPE_CONFIG_INCLUDE_UTM) },
#else
{ PPE_CONFIG_INCLUDE_UTM(__ppe_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __ppe_config_STRINGIFY_VALUE
#undef __ppe_config_STRINGIFY_NAME

const char*
ppe_config_lookup(const char* setting)
{
    int i;
    for(i = 0; ppe_config_settings[i].name; i++) {
        if(strcmp(ppe_config_settings[i].name, setting)) {
            return ppe_config_settings[i].value;
        }
    }
    return NULL;
}

int
ppe_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; ppe_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", ppe_config_settings[i].name, ppe_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(PPE_CONFIG_HEADER).source> */



