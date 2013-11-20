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

#include <VPI/vpi_config.h>
#include <VPI/vpi.h>
#include "vpi_int.h"
#include <stdlib.h>
#include <string.h>


/* <auto.start.cdefs(VPI_CONFIG_HEADER).source> */
#define __vpi_config_STRINGIFY_NAME(_x) #_x
#define __vpi_config_STRINGIFY_VALUE(_x) __vpi_config_STRINGIFY_NAME(_x)
vpi_config_settings_t vpi_config_settings[] =
{
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_LOG_OPTIONS_DEFAULT
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_LOG_OPTIONS_DEFAULT), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_LOG_OPTIONS_DEFAULT) },
#else
{ VPI_CONFIG_LOG_OPTIONS_DEFAULT(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_LOG_BITS_DEFAULT
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_LOG_BITS_DEFAULT), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_LOG_BITS_DEFAULT) },
#else
{ VPI_CONFIG_LOG_BITS_DEFAULT(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT) },
#else
{ VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_UDP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_UDP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_UDP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_UDP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_TCP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_TCP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_TCP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_TCP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_VETH
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_VETH), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_VETH) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_VETH(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_PCAP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_PCAP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_PCAP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_PCAP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_BPP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_BPP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_BPP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_BPP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_DUMP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_DUMP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_DUMP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_DUMP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_QUEUE
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_QUEUE), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_QUEUE) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_QUEUE(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_INTERFACE_TAP
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_INTERFACE_TAP), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_INTERFACE_TAP) },
#else
{ VPI_CONFIG_INCLUDE_INTERFACE_TAP(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_BRIDGING
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_BRIDGING), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_BRIDGING) },
#else
{ VPI_CONFIG_INCLUDE_BRIDGING(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_PORTING_STDLIB
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_PORTING_STDLIB), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_PORTING_STDLIB) },
#else
{ VPI_CONFIG_PORTING_STDLIB(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS) },
#else
{ VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_ATEXIT
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_ATEXIT), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_ATEXIT) },
#else
{ VPI_CONFIG_INCLUDE_ATEXIT(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_MAX_PACKET
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_MAX_PACKET), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_MAX_PACKET) },
#else
{ VPI_CONFIG_MAX_PACKET(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_CREATE_SPEC_MAX
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_CREATE_SPEC_MAX), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_CREATE_SPEC_MAX) },
#else
{ VPI_CONFIG_CREATE_SPEC_MAX(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_LOGGING
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_LOGGING), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_LOGGING) },
#else
{ VPI_CONFIG_INCLUDE_LOGGING(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_LOG_OUTPUT_DEFAULT
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_LOG_OUTPUT_DEFAULT), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_LOG_OUTPUT_DEFAULT) },
#else
{ VPI_CONFIG_LOG_OUTPUT_DEFAULT(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_PYTHON_BINDINGS
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_PYTHON_BINDINGS), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_PYTHON_BINDINGS) },
#else
{ VPI_CONFIG_INCLUDE_PYTHON_BINDINGS(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
#ifdef VPI_CONFIG_INCLUDE_UCLI
    { __vpi_config_STRINGIFY_NAME(VPI_CONFIG_INCLUDE_UCLI), __vpi_config_STRINGIFY_VALUE(VPI_CONFIG_INCLUDE_UCLI) },
#else
{ VPI_CONFIG_INCLUDE_UCLI(__vpi_config_STRINGIFY_NAME), "__undefined__" },
#endif
    { NULL, NULL }
};
#undef __vpi_config_STRINGIFY_VALUE
#undef __vpi_config_STRINGIFY_NAME

const char*
vpi_config_lookup(const char* setting)
{
    int i;
    for(i = 0; vpi_config_settings[i].name; i++) {
        if(strcmp(vpi_config_settings[i].name, setting)) {
            return vpi_config_settings[i].value;
        }
    }
    return NULL;
}

int
vpi_config_show(struct aim_pvs_s* pvs)
{
    int i;
    for(i = 0; vpi_config_settings[i].name; i++) {
        aim_printf(pvs, "%s = %s\n", vpi_config_settings[i].name, vpi_config_settings[i].value);
    }
    return i;
}

/* <auto.end.cdefs(VPI_CONFIG_HEADER).source> */



