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

#include <IOF/iof_config.h>
#include <IOF/iof.h>

/* <auto.start.cdefs(IOF_CONFIG_HEADER).source> */
#define __iof_config_STRINGIFY_NAME(_x) #_x
#define __iof_config_STRINGIFY_VALUE(_x) __iof_config_STRINGIFY_NAME(_x)
iof_config_settings_t iof_config_settings[] =
{
#ifdef IOF_CONFIG_INCLUDE_LOGGING
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_INCLUDE_LOGGING), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_INCLUDE_LOGGING) },
#else
{ IOF_CONFIG_INCLUDE_LOGGING(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_LOG_OPTIONS_DEFAULT
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_LOG_OPTIONS_DEFAULT), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ IOF_CONFIG_LOG_OPTIONS_DEFAULT(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_LOG_BITS_DEFAULT
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_LOG_BITS_DEFAULT), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_LOG_BITS_DEFAULT) },
#else
{ IOF_CONFIG_LOG_BITS_DEFAULT(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ IOF_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_PORTING_STDLIB
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_PORTING_STDLIB), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_PORTING_STDLIB) },
#else
{ IOF_CONFIG_PORTING_STDLIB(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ IOF_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_DEFAULT_INDENT_FACTOR
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_DEFAULT_INDENT_FACTOR), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_DEFAULT_INDENT_FACTOR) },
#else
{ IOF_CONFIG_DEFAULT_INDENT_FACTOR(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_DEFAULT_INDENT_INITIAL
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_DEFAULT_INDENT_INITIAL), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_DEFAULT_INDENT_INITIAL) },
#else
{ IOF_CONFIG_DEFAULT_INDENT_INITIAL(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_DEFAULT_PUSH_STRING
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_DEFAULT_PUSH_STRING), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_DEFAULT_PUSH_STRING) },
#else
{ IOF_CONFIG_DEFAULT_PUSH_STRING(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_DEFAULT_POP_STRING
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_DEFAULT_POP_STRING), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_DEFAULT_POP_STRING) },
#else
{ IOF_CONFIG_DEFAULT_POP_STRING(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_DEFAULT_INDENT_STRING
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_DEFAULT_INDENT_STRING), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_DEFAULT_INDENT_STRING) },
#else
{ IOF_CONFIG_DEFAULT_INDENT_STRING(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef IOF_CONFIG_DEFAULT_INDENT_TERMINATOR
    { __iof_config_STRINGIFY_NAME(IOF_CONFIG_DEFAULT_INDENT_TERMINATOR), __iof_config_STRINGIFY_VALUE(IOF_CONFIG_DEFAULT_INDENT_TERMINATOR) },
#else
{ IOF_CONFIG_DEFAULT_INDENT_TERMINATOR(__iof_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __iof_config_STRINGIFY_VALUE
#undef __iof_config_STRINGIFY_NAME

const char*
iof_config_lookup(const char* setting)
{
    int i;
    for(i = 0; iof_config_settings[i].name; i++) {
        if(strcmp(iof_config_settings[i].name, setting)) {
            return iof_config_settings[i].value;
        }
    }
    return NULL;
}

int
iof_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; iof_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", iof_config_settings[i].name, iof_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(IOF_CONFIG_HEADER).source> */



