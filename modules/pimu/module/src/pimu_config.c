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

#include <pimu/pimu_config.h>

/* <auto.start.cdefs(PIMU_CONFIG_HEADER).source> */
#define __pimu_config_STRINGIFY_NAME(_x) #_x
#define __pimu_config_STRINGIFY_VALUE(_x) __pimu_config_STRINGIFY_NAME(_x)
pimu_config_settings_t pimu_config_settings[] =
{
#ifdef PIMU_CONFIG_INCLUDE_LOGGING
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_INCLUDE_LOGGING), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_INCLUDE_LOGGING) },
#else
{ PIMU_CONFIG_INCLUDE_LOGGING(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_LOG_OPTIONS_DEFAULT
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_LOG_OPTIONS_DEFAULT), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ PIMU_CONFIG_LOG_OPTIONS_DEFAULT(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_LOG_BITS_DEFAULT
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_LOG_BITS_DEFAULT), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_LOG_BITS_DEFAULT) },
#else
{ PIMU_CONFIG_LOG_BITS_DEFAULT(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_PORTING_STDLIB
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_PORTING_STDLIB), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_PORTING_STDLIB) },
#else
{ PIMU_CONFIG_PORTING_STDLIB(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ PIMU_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_INCLUDE_UCLI
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_INCLUDE_UCLI), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_INCLUDE_UCLI) },
#else
{ PIMU_CONFIG_INCLUDE_UCLI(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_PACKET_KEY_SIZE
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_PACKET_KEY_SIZE), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_PACKET_KEY_SIZE) },
#else
{ PIMU_CONFIG_PACKET_KEY_SIZE(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_GROUP_COUNT
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_GROUP_COUNT), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_GROUP_COUNT) },
#else
{ PIMU_CONFIG_GROUP_COUNT(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_MAX_ETHER_TYPES
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_MAX_ETHER_TYPES), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_MAX_ETHER_TYPES) },
#else
{ PIMU_CONFIG_MAX_ETHER_TYPES(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef PIMU_CONFIG_MAX_ETHER_DAS
    { __pimu_config_STRINGIFY_NAME(PIMU_CONFIG_MAX_ETHER_DAS), __pimu_config_STRINGIFY_VALUE(PIMU_CONFIG_MAX_ETHER_DAS) },
#else
{ PIMU_CONFIG_MAX_ETHER_DAS(__pimu_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __pimu_config_STRINGIFY_VALUE
#undef __pimu_config_STRINGIFY_NAME

const char*
pimu_config_lookup(const char* setting)
{
    int i;
    for(i = 0; pimu_config_settings[i].name; i++) {
        if(strcmp(pimu_config_settings[i].name, setting)) {
            return pimu_config_settings[i].value;
        }
    }
    return NULL;
}

int
pimu_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; pimu_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", pimu_config_settings[i].name, pimu_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(PIMU_CONFIG_HEADER).source> */

