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

#include <BigRing/bigring_config.h>

/* <auto.start.cdefs(BIGRING_CONFIG_HEADER).source> */
#define __bigring_config_STRINGIFY_NAME(_x) #_x
#define __bigring_config_STRINGIFY_VALUE(_x) __bigring_config_STRINGIFY_NAME(_x)
bigring_config_settings_t bigring_config_settings[] =
{
#ifdef BIGRING_CONFIG_INCLUDE_LOGGING
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_INCLUDE_LOGGING), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_INCLUDE_LOGGING) },
#else
{ BIGRING_CONFIG_INCLUDE_LOGGING(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_LOG_OPTIONS_DEFAULT
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_LOG_OPTIONS_DEFAULT), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ BIGRING_CONFIG_LOG_OPTIONS_DEFAULT(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_LOG_BITS_DEFAULT
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_LOG_BITS_DEFAULT), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_LOG_BITS_DEFAULT) },
#else
{ BIGRING_CONFIG_LOG_BITS_DEFAULT(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ BIGRING_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_PORTING_STDLIB
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_PORTING_STDLIB), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_PORTING_STDLIB) },
#else
{ BIGRING_CONFIG_PORTING_STDLIB(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ BIGRING_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_INCLUDE_UCLI
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_INCLUDE_UCLI), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_INCLUDE_UCLI) },
#else
{ BIGRING_CONFIG_INCLUDE_UCLI(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGRING_CONFIG_INCLUDE_LOCKING
    { __bigring_config_STRINGIFY_NAME(BIGRING_CONFIG_INCLUDE_LOCKING), __bigring_config_STRINGIFY_VALUE(BIGRING_CONFIG_INCLUDE_LOCKING) },
#else
{ BIGRING_CONFIG_INCLUDE_LOCKING(__bigring_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __bigring_config_STRINGIFY_VALUE
#undef __bigring_config_STRINGIFY_NAME

const char*
bigring_config_lookup(const char* setting)
{
    int i;
    for(i = 0; bigring_config_settings[i].name; i++) {
        if(strcmp(bigring_config_settings[i].name, setting)) {
            return bigring_config_settings[i].value;
        }
    }
    return NULL;
}

int
bigring_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; bigring_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", bigring_config_settings[i].name, bigring_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(BIGRING_CONFIG_HEADER).source> */

