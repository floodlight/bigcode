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
#include <stdlib.h>
#include <string.h>



/* <auto.start.cdefs(FME_CONFIG_HEADER).source> */
#define __fme_config_STRINGIFY_NAME(_x) #_x
#define __fme_config_STRINGIFY_VALUE(_x) __fme_config_STRINGIFY_NAME(_x)
fme_config_settings_t fme_config_settings[] =
{
#ifdef FME_CONFIG_INCLUDE_LOGGING
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_INCLUDE_LOGGING), __fme_config_STRINGIFY_VALUE(FME_CONFIG_INCLUDE_LOGGING) },
#else
{ FME_CONFIG_INCLUDE_LOGGING(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_LOG_OPTIONS_DEFAULT
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_LOG_OPTIONS_DEFAULT), __fme_config_STRINGIFY_VALUE(FME_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ FME_CONFIG_LOG_OPTIONS_DEFAULT(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_LOG_BITS_DEFAULT
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_LOG_BITS_DEFAULT), __fme_config_STRINGIFY_VALUE(FME_CONFIG_LOG_BITS_DEFAULT) },
#else
{ FME_CONFIG_LOG_BITS_DEFAULT(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __fme_config_STRINGIFY_VALUE(FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ FME_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_PORTING_STDLIB
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_PORTING_STDLIB), __fme_config_STRINGIFY_VALUE(FME_CONFIG_PORTING_STDLIB) },
#else
{ FME_CONFIG_PORTING_STDLIB(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __fme_config_STRINGIFY_VALUE(FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ FME_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_INCLUDE_UCLI
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_INCLUDE_UCLI), __fme_config_STRINGIFY_VALUE(FME_CONFIG_INCLUDE_UCLI) },
#else
{ FME_CONFIG_INCLUDE_UCLI(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_INCLUDE_UTM
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_INCLUDE_UTM), __fme_config_STRINGIFY_VALUE(FME_CONFIG_INCLUDE_UTM) },
#else
{ FME_CONFIG_INCLUDE_UTM(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef FME_CONFIG_KEY_SIZE_WORDS
    { __fme_config_STRINGIFY_NAME(FME_CONFIG_KEY_SIZE_WORDS), __fme_config_STRINGIFY_VALUE(FME_CONFIG_KEY_SIZE_WORDS) },
#else
{ FME_CONFIG_KEY_SIZE_WORDS(__fme_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __fme_config_STRINGIFY_VALUE
#undef __fme_config_STRINGIFY_NAME

const char*
fme_config_lookup(const char* setting)
{
    int i;
    for(i = 0; fme_config_settings[i].name; i++) {
        if(strcmp(fme_config_settings[i].name, setting)) {
            return fme_config_settings[i].value;
        }
    }
    return NULL;
}

int
fme_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; fme_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", fme_config_settings[i].name, fme_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(FME_CONFIG_HEADER).source> */



