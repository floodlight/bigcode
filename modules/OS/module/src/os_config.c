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

#include <OS/os_config.h>
#include <OS/os.h>
#include "os_int.h"
#include <stdlib.h>
#include <string.h>



/* <auto.start.cdefs(OS_CONFIG_HEADER).source> */
#define __os_config_STRINGIFY_NAME(_x) #_x
#define __os_config_STRINGIFY_VALUE(_x) __os_config_STRINGIFY_NAME(_x)
os_config_settings_t os_config_settings[] =
{
#ifdef OS_CONFIG_INCLUDE_LOGGING
    { __os_config_STRINGIFY_NAME(OS_CONFIG_INCLUDE_LOGGING), __os_config_STRINGIFY_VALUE(OS_CONFIG_INCLUDE_LOGGING) },
#else
{ OS_CONFIG_INCLUDE_LOGGING(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_LOG_OPTIONS_DEFAULT
    { __os_config_STRINGIFY_NAME(OS_CONFIG_LOG_OPTIONS_DEFAULT), __os_config_STRINGIFY_VALUE(OS_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ OS_CONFIG_LOG_OPTIONS_DEFAULT(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_LOG_BITS_DEFAULT
    { __os_config_STRINGIFY_NAME(OS_CONFIG_LOG_BITS_DEFAULT), __os_config_STRINGIFY_VALUE(OS_CONFIG_LOG_BITS_DEFAULT) },
#else
{ OS_CONFIG_LOG_BITS_DEFAULT(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __os_config_STRINGIFY_NAME(OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __os_config_STRINGIFY_VALUE(OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_PORTING_STDLIB
    { __os_config_STRINGIFY_NAME(OS_CONFIG_PORTING_STDLIB), __os_config_STRINGIFY_VALUE(OS_CONFIG_PORTING_STDLIB) },
#else
{ OS_CONFIG_PORTING_STDLIB(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __os_config_STRINGIFY_NAME(OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __os_config_STRINGIFY_VALUE(OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ OS_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_INCLUDE_UCLI
    { __os_config_STRINGIFY_NAME(OS_CONFIG_INCLUDE_UCLI), __os_config_STRINGIFY_VALUE(OS_CONFIG_INCLUDE_UCLI) },
#else
{ OS_CONFIG_INCLUDE_UCLI(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_INCLUDE_POSIX
    { __os_config_STRINGIFY_NAME(OS_CONFIG_INCLUDE_POSIX), __os_config_STRINGIFY_VALUE(OS_CONFIG_INCLUDE_POSIX) },
#else
{ OS_CONFIG_INCLUDE_POSIX(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_INCLUDE_WINDOWS
    { __os_config_STRINGIFY_NAME(OS_CONFIG_INCLUDE_WINDOWS), __os_config_STRINGIFY_VALUE(OS_CONFIG_INCLUDE_WINDOWS) },
#else
{ OS_CONFIG_INCLUDE_WINDOWS(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef OS_CONFIG_INCLUDE_OSX
    { __os_config_STRINGIFY_NAME(OS_CONFIG_INCLUDE_OSX), __os_config_STRINGIFY_VALUE(OS_CONFIG_INCLUDE_OSX) },
#else
{ OS_CONFIG_INCLUDE_OSX(__os_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __os_config_STRINGIFY_VALUE
#undef __os_config_STRINGIFY_NAME

const char*
os_config_lookup(const char* setting)
{
    int i;
    for(i = 0; os_config_settings[i].name; i++) {
        if(strcmp(os_config_settings[i].name, setting)) {
            return os_config_settings[i].value;
        }
    }
    return NULL;
}

int
os_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; os_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", os_config_settings[i].name, os_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(OS_CONFIG_HEADER).source> */


