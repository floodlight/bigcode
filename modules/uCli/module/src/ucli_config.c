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

#include <uCli/ucli_config.h>
#include <uCli/ucli.h>
#include "ucli_int.h"
#include <stdlib.h>
#include <string.h>



/* <auto.start.cdefs(UCLI_CONFIG_HEADER).source> */
#define __ucli_config_STRINGIFY_NAME(_x) #_x
#define __ucli_config_STRINGIFY_VALUE(_x) __ucli_config_STRINGIFY_NAME(_x)
ucli_config_settings_t ucli_config_settings[] =
{
#ifdef UCLI_CONFIG_INCLUDE_LOGGING
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_INCLUDE_LOGGING), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_INCLUDE_LOGGING) },
#else
{ UCLI_CONFIG_INCLUDE_LOGGING(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_LOG_OPTIONS_DEFAULT
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_LOG_OPTIONS_DEFAULT), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ UCLI_CONFIG_LOG_OPTIONS_DEFAULT(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_LOG_BITS_DEFAULT
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_LOG_BITS_DEFAULT), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_LOG_BITS_DEFAULT) },
#else
{ UCLI_CONFIG_LOG_BITS_DEFAULT(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ UCLI_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_PORTING_STDLIB
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_PORTING_STDLIB), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_PORTING_STDLIB) },
#else
{ UCLI_CONFIG_PORTING_STDLIB(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ UCLI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_MAX_ARGS
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_MAX_ARGS), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_MAX_ARGS) },
#else
{ UCLI_CONFIG_MAX_ARGS(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_MAX_PROMPT
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_MAX_PROMPT), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_MAX_PROMPT) },
#else
{ UCLI_CONFIG_MAX_PROMPT(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_INCLUDE_FGETS_LOOP
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_INCLUDE_FGETS_LOOP), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_INCLUDE_FGETS_LOOP) },
#else
{ UCLI_CONFIG_INCLUDE_FGETS_LOOP(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_INCLUDE_ELS_LOOP
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_INCLUDE_ELS_LOOP), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_INCLUDE_ELS_LOOP) },
#else
{ UCLI_CONFIG_INCLUDE_ELS_LOOP(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_MAX_INPUT_LINE
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_MAX_INPUT_LINE), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_MAX_INPUT_LINE) },
#else
{ UCLI_CONFIG_MAX_INPUT_LINE(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_INCLUDE_TTY
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_INCLUDE_TTY), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_INCLUDE_TTY) },
#else
{ UCLI_CONFIG_INCLUDE_TTY(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK) },
#else
{ UCLI_CONFIG_MAX_COMMANDS_PER_BLOCK(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_INCLUDE_MODULE_NODES
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_INCLUDE_MODULE_NODES), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_INCLUDE_MODULE_NODES) },
#else
{ UCLI_CONFIG_INCLUDE_MODULE_NODES(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef UCLI_CONFIG_INCLUDE_PTHREADS
    { __ucli_config_STRINGIFY_NAME(UCLI_CONFIG_INCLUDE_PTHREADS), __ucli_config_STRINGIFY_VALUE(UCLI_CONFIG_INCLUDE_PTHREADS) },
#else
{ UCLI_CONFIG_INCLUDE_PTHREADS(__ucli_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __ucli_config_STRINGIFY_VALUE
#undef __ucli_config_STRINGIFY_NAME

const char*
ucli_config_lookup(const char* setting)
{
    int i;
    for(i = 0; ucli_config_settings[i].name; i++) {
        if(strcmp(ucli_config_settings[i].name, setting)) {
            return ucli_config_settings[i].value;
        }
    }
    return NULL;
}

int
ucli_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; ucli_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", ucli_config_settings[i].name, ucli_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(UCLI_CONFIG_HEADER).source> */



