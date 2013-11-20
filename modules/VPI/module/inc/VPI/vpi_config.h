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
/************************************************************//**
 *
 * @file
 * @brief VPI Configuration Definitions.
 *
 * @addtogroup vpi-config
 * @{
 ***************************************************************/

#ifndef __VPI_CONFIG_H__
#define __VPI_CONFIG_H__

#ifdef GLOBAL_INCLUDE_CUSTOM_CONFIG
#include <global_custom_config.h>
#endif

#ifdef VPI_INCLUDE_CUSTOM_CONFIG
#include <vpi_custom_config.h>
#endif

/**
 * This module requires POSIX
 */
#define AIM_CONFIG_INCLUDE_POSIX 1
/**
 * This module requires GNU_SOURCE (pcap dependency)
 */
#define AIM_CONFIG_INCLUDE_GNU_SOURCE 1


/* <auto.start.cdefs(VPI_CONFIG_HEADER).header> */
#include <AIM/aim.h>
/**
 * VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
 *
 * Set the default for including or excluding VPI interfaces.  */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#define VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT 1
#endif

/**
 * VPI_CONFIG_LOG_OPTIONS_DEFAULT
 *
 * Default enabled log options. */


#ifndef VPI_CONFIG_LOG_OPTIONS_DEFAULT
#define VPI_CONFIG_LOG_OPTIONS_DEFAULT AIM_LOG_OPTIONS_DEFAULT
#endif

/**
 * VPI_CONFIG_LOG_BITS_DEFAULT
 *
 * Default enabled log options. */


#ifndef VPI_CONFIG_LOG_BITS_DEFAULT
#define VPI_CONFIG_LOG_BITS_DEFAULT AIM_LOG_BITS_BASELINE
#endif

/**
 * VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
 *
 * Default enabled custom log options. */


#ifndef VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
#define VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT 0
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK
 *
 * Include the loopback interface. */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK
#define VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_UDP
 *
 * Include Transport over UDP sockets */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_UDP
#define VPI_CONFIG_INCLUDE_INTERFACE_UDP VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_TCP
 *
 * Include Transport over TCP sockets */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_TCP
#define VPI_CONFIG_INCLUDE_INTERFACE_TCP VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_VETH
 *
 * Include Transport over Eth/Veth Devices */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_VETH
#define VPI_CONFIG_INCLUDE_INTERFACE_VETH VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_PCAP
 *
 * Include Transport through PCAP */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_PCAP
#define VPI_CONFIG_INCLUDE_INTERFACE_PCAP VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP
 *
 * Include the PCAP dump interface. */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP
#define VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_BPP
 *
 * Include interface to BigPatchPanels */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_BPP
#define VPI_CONFIG_INCLUDE_INTERFACE_BPP VPI_CONFIG_INCLUDE_INTERFACE_UDP
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_DUMP
 *
 * Include the packet dump interface. */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_DUMP
#define VPI_CONFIG_INCLUDE_INTERFACE_DUMP VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_QUEUE
 *
 * Include the packet queue interface. */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_QUEUE
#define VPI_CONFIG_INCLUDE_INTERFACE_QUEUE VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_INTERFACE_TAP
 *
 * Include the TAP packet interface. */


#ifndef VPI_CONFIG_INCLUDE_INTERFACE_TAP
#define VPI_CONFIG_INCLUDE_INTERFACE_TAP VPI_CONFIG_INCLUDE_INTERFACE_DEFAULT
#endif

/**
 * VPI_CONFIG_INCLUDE_BRIDGING
 *
 * Include transparent bridging between VPI instances. Requires pthreads. */


#ifndef VPI_CONFIG_INCLUDE_BRIDGING
#define VPI_CONFIG_INCLUDE_BRIDGING 1
#endif

/**
 * VPI_CONFIG_PORTING_STDLIB
 *
 * Default all porting macros to use the C standard libraries. */


#ifndef VPI_CONFIG_PORTING_STDLIB
#define VPI_CONFIG_PORTING_STDLIB 1
#endif

/**
 * VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
 *
 * Include standard library headers for stdlib porting macros. */


#ifndef VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS
#define VPI_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS VPI_CONFIG_PORTING_STDLIB
#endif

/**
 * VPI_CONFIG_INCLUDE_ATEXIT
 *
 * Automatically destroy all VPIs at exit (requires atexit()) */


#ifndef VPI_CONFIG_INCLUDE_ATEXIT
#define VPI_CONFIG_INCLUDE_ATEXIT 1
#endif

/**
 * VPI_CONFIG_MAX_PACKET
 *
 * Maximum packet size. */


#ifndef VPI_CONFIG_MAX_PACKET
#define VPI_CONFIG_MAX_PACKET 10000
#endif

/**
 * VPI_CONFIG_CREATE_SPEC_MAX
 *
 * Maximum create specification length. */


#ifndef VPI_CONFIG_CREATE_SPEC_MAX
#define VPI_CONFIG_CREATE_SPEC_MAX 512
#endif

/**
 * VPI_CONFIG_INCLUDE_LOGGING
 *
 * Include or exclude logging. */


#ifndef VPI_CONFIG_INCLUDE_LOGGING
#define VPI_CONFIG_INCLUDE_LOGGING 1
#endif

/**
 * VPI_CONFIG_LOG_OUTPUT_DEFAULT
 *
 * Default output function for log messages. Can be changed and runtime. */


#ifndef VPI_CONFIG_LOG_OUTPUT_DEFAULT
#define VPI_CONFIG_LOG_OUTPUT_DEFAULT printf
#endif

/**
 * VPI_CONFIG_INCLUDE_PYTHON_BINDINGS
 *
 * Include Python bindings. */


#ifndef VPI_CONFIG_INCLUDE_PYTHON_BINDINGS
#define VPI_CONFIG_INCLUDE_PYTHON_BINDINGS 0
#endif

/**
 * VPI_CONFIG_INCLUDE_UCLI
 *
 * Include UCLI support. */


#ifndef VPI_CONFIG_INCLUDE_UCLI
#define VPI_CONFIG_INCLUDE_UCLI 0
#endif



/**
 * All compile time options can be queried or displayed
 */

/** Configuration settings structure. */
typedef struct vpi_config_settings_s {
    /** name */
    const char* name;
    /** value */
    const char* value;
} vpi_config_settings_t;

/** Configuration settings table. */
/** vpi_config_settings table. */
extern vpi_config_settings_t vpi_config_settings[];

/**
 * @brief Lookup a configuration setting.
 * @param setting The name of the configuration option to lookup.
 */
const char* vpi_config_lookup(const char* setting);

/**
 * @brief Show the compile-time configuration.
 * @param pvs The output stream.
 */
int vpi_config_show(struct aim_pvs_s* pvs);

/* <auto.end.cdefs(VPI_CONFIG_HEADER).header> */

#include "vpi_porting.h"

#endif /* __VPI_CONFIG_H__ */
/*@}*/
