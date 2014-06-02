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

#ifndef NETLINK_MON
#define NETLINK_MON

#include "options.h"
#include "orc_driver.h"


/**
 * Create a SOCK_DGRAM socket to monitor netlink
 * information.  Socket is set in NON_BLOCK
 */
int netlink_mon_create(orc_options_t * options);

/**
 * Takes a NON_BLOCK socket from netlink_mon_create() and reads the next event
 * off of the stack and makes the corresponding call into the orc driver
 *
 * Typically called after epoll/poll/select
 *
 */

int netlink_mon_handler(orc_options_t * options, orc_driver_t * drv, int sock);

#endif
