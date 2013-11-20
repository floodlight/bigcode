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

#include <BigHash/bighash_config.h>

/* <auto.start.cdefs(BIGHASH_CONFIG_HEADER).source> */
#define __bighash_config_STRINGIFY_NAME(_x) #_x
#define __bighash_config_STRINGIFY_VALUE(_x) __bighash_config_STRINGIFY_NAME(_x)
bighash_config_settings_t bighash_config_settings[] =
{
#ifdef BIGHASH_CONFIG_INCLUDE_LOGGING
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_INCLUDE_LOGGING), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_INCLUDE_LOGGING) },
#else
{ BIGHASH_CONFIG_INCLUDE_LOGGING(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_LOG_BITS_DEFAULT
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_LOG_BITS_DEFAULT), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_LOG_BITS_DEFAULT) },
#else
{ BIGHASH_CONFIG_LOG_BITS_DEFAULT(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_PORTING_STDLIB
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_PORTING_STDLIB), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_PORTING_STDLIB) },
#else
{ BIGHASH_CONFIG_PORTING_STDLIB(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ BIGHASH_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_INCLUDE_UCLI
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_INCLUDE_UCLI), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_INCLUDE_UCLI) },
#else
{ BIGHASH_CONFIG_INCLUDE_UCLI(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef BIGHASH_CONFIG_INCLUDE_LOCKING
    { __bighash_config_STRINGIFY_NAME(BIGHASH_CONFIG_INCLUDE_LOCKING), __bighash_config_STRINGIFY_VALUE(BIGHASH_CONFIG_INCLUDE_LOCKING) },
#else
{ BIGHASH_CONFIG_INCLUDE_LOCKING(__bighash_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __bighash_config_STRINGIFY_VALUE
#undef __bighash_config_STRINGIFY_NAME

const char*
bighash_config_lookup(const char* setting)
{
    int i;
    for(i = 0; bighash_config_settings[i].name; i++) {
        if(strcmp(bighash_config_settings[i].name, setting)) {
            return bighash_config_settings[i].value;
        }
    }
    return NULL;
}

int
bighash_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; bighash_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", bighash_config_settings[i].name, bighash_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(BIGHASH_CONFIG_HEADER).source> */

