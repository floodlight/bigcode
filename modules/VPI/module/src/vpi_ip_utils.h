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

#ifndef __VPI_IP_UTILS_H__
#define __VPI_IP_UTILS_H__

#include "vpi_int.h"

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1 || VPI_CONFIG_INCLUDE_INTERFACE_TCP == 1

#include "vpi_parser.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/**************************************************************************//**
 *
 * Determine the first local IP4 address.
 * Used for reverse-path create_specs on IP-transports.
 *
 *****************************************************************************/
int vpi_get_ip_local_endpoint(char* address_buffer);

int vpi_sockaddr_endpoint(struct sockaddr_in* dst, vpi_ip_endpoint_t* src);

char* vpi_sockaddr_format(struct sockaddr_in* sa, char* dst);

int
vpi_get_ip_src_for_dst_addr(vpi_ip_endpoint_t* remote_endpoint, char* src_address);

#endif
#endif /* __VPI_IP_UTILS_H__ */
