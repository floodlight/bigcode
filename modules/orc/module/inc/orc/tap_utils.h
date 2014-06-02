/************************************************************
 * <bsn.cl fy=2013 v=epl>
 * 
 *        Copyright 2013, 2014 Big Switch Networks, Inc.       
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
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/

#ifndef TAP_UTILS_H
#define TAP_UTILS_H

#include "ports.h"

int tap_interfaces_create(char * ifnam_prefix, port_t * ports[], int num_ports);


/***
 * Create a tap interface called "devname"
 * if return is 0, then *fd points to a valid file descriptor
 * else error
 */

int create_tap_iface(char * devname, int * fd);


/**
 * Set the socket to a non-blocking mode
 */
int set_socket_non_blocking(int fd);

/***
 * Given a Linux interface name, lookup and copy the return index into
 * *if_index
 *
 * @return 0 on success, -1 on failure
 */
int interface_name_to_index(char name[IFNAMSIZ], int * if_index);

/***
 * Given a Linux interface index, lookup and copy the first buflen bytes
 * of its name into buf.
 * @return A pointer to buf, for niceities
 */
char * interface_index_to_name(int index, char * buf, int buflen);


/***
 * Lookup a port_t structure from its underlying Linux interface index
 */
port_t * interface_index_to_port_t(int index);

/***
 * Lookup an interface's MAC address from its underlying Linux interface index
 * @return 0 on success, -1 on failure
 */
int interface_name_to_hw_mac(char name[IFNAMSIZ], u8 hw_mac[6]);

/***
 * Lookup an interface's MTU from it's underlying Linux interface index
 * @return 0 on success, -1 on failure
 */
int interface_name_to_mtu(char name[IFNAMSIZ], int * mtu);

#endif
