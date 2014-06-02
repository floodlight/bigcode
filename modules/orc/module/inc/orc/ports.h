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

#ifndef PORTS_H
#define PORTS_H

#include <net/if.h>

struct port_s;
typedef struct port_s port_t;

#include "orc_driver.h"

typedef struct port_s {
    unsigned short index;           // driver assigned index to the port: range 0-65535
    int if_index;                   // not driver assigned
    char name[IFNAMSIZ];            // not driver assigned
    int fd;                         // not driver assigned
    l3_intf_id_t l3_intf_id;        // FIXME a single physical port could have multiple l3_intefaces
} port_t;


#endif
