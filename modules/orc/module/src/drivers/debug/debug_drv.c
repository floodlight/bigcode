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

/***********
 * Example orc driver for debugging
 */

#include <stdio.h>
#include <stdlib.h>

#include <net/ethernet.h>

#include <netinet/in.h>

#include "orc/utils.h"
#include "orc/orc_logger.h"
#include "orc/orc_driver.h"

#define PREFIX "DEBUG_DRV: "

int fake_ports = 4;
int internal_offset = 100;

typedef struct debug_port {
    port_t port;
    int internal_index;
} debug_port_t;

/***************
 * init: just print args
 */

int
debug_init_driver(int argc, char * argv[])
{
    int i;
    for (i=0; i<argc; i++)
        printf(PREFIX " arg %d = %s\n", i, argv[i]);

    return 0;
}

/******************
 * discover_ports: generate some fake ports
 */

int
debug_discover_ports(port_t * ports[], int * num)
{
    int i;
    debug_port_t * dport;

    if (fake_ports > *num)
    {
        orc_err(" need to pre-allocate more ports: want %d got %d\n",
                        fake_ports, *num);
        return -1;
    }
    for (i=0; i<fake_ports; i++)
    {
        dport = malloc(sizeof(*dport));
        if (dport == NULL)
        {
            orc_err("Out of mem allocating ports\n");
            return -1;
        }
        dport->port.index = i+1;
        dport->internal_index = i + internal_offset;
        ports[i] = (port_t *) dport;
    }
    *num = fake_ports;
    return 0;
}

/*********
 * tx: just print to STDOUT
 */

int
debug_tx_pkt(port_t *port, u8 *pkt, unsigned int len)
{
    struct ether_header * eth = (struct ether_header *) pkt;
    printf(PREFIX " fake sending packet from port %s to ASIC: "
                    ETH_FORMAT " -> " ETH_FORMAT " :: (0x%.4x) %u bytes\n",
                    port->name,
                    ETH_ADDR_PRINT(eth->ether_shost),
                    ETH_ADDR_PRINT(eth->ether_dhost),
                    ntohs(eth->ether_type),
                    len);
    return 0;
}

int debug_start_rx(port_t * ports[], int num_ports)
{
    printf(PREFIX "Fake starting RX\n");

    return 0;
}

int debug_stop_rx()
{
    printf(PREFIX "Fake stoping RX\n");

    return 0;
}

int debug_add_l3_v4_interface(
        port_t *port,
        u8 hw_mac[6],
        int mtu,
        u32 ipv4_addr,
        l3_intf_id_t * l3_intf_id
    )
{
    debug_port_t * dport = (debug_port_t *) port;
    printf(PREFIX "Fake adding L3 Interface: mac=" ETH_FORMAT
            " ip=" IPV4_FORMAT
            " on port %s with mtu %d\n",
            ETH_ADDR_PRINT(hw_mac),
            IPV4_ADDR_PRINT(ipv4_addr),
            port->name,
            mtu);
    *l3_intf_id = dport -> internal_index;
    port -> l3_intf_id = *l3_intf_id;

    // a real driver would cache the l3_intf_id here for something
    // useful
    return 0;
}

int debug_del_l3_interface(port_t * port, l3_intf_id_t l3_intf_id)
{
    printf(PREFIX "Fake deleting L3 Interface by ID: %d\n", l3_intf_id);

    // a real driver would reclaim state here
    return 0;
}

int debug_add_l3_v4_next_hop(
        port_t * port,
        l3_intf_id_t l3_intf_id,
        u8 next_hop_hw_mac[6],
        l3_next_hop_id_t * l3_next_hop_id
        )
{
    static int NEXT_HOP_ID = 0;
    printf(PREFIX "Fake adding L3 NextHop: next_hop_mac=" ETH_FORMAT
            " on L3 interface %d\n",
            ETH_ADDR_PRINT(next_hop_hw_mac),
            l3_intf_id);
    *l3_next_hop_id = NEXT_HOP_ID++;

    // a real driver would cache the l3_next_hop_id here for something
    // useful
    return 0;
}

int debug_del_l3_next_hop(l3_next_hop_id_t l3_next_hop_id)
{
    printf(PREFIX "Fake deleting L3 Next Hop by ID: %d\n", l3_next_hop_id);

    // a real driver would reclaim state here
    return 0;
}

int debug_add_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id)
{
    printf(PREFIX "Fake adding L3 v4 route: " IPV4_FORMAT "/" IPV4_FORMAT
            " --> next_hop_id=%d\n",
            IPV4_ADDR_PRINT(ip_dst),
            IPV4_ADDR_PRINT(netmask),
            l3_next_hop_id);
    return 0;
}

int debug_del_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id)
{
    printf(PREFIX "Fake deleting L3 v4 route: " IPV4_FORMAT "/" IPV4_FORMAT
            " --> next_hop_id=%d\n",
            IPV4_ADDR_PRINT(ip_dst),
            IPV4_ADDR_PRINT(netmask),
            l3_next_hop_id);
    return 0;
}
/*****
 * Actual hooks into this driver; DRIVER_HOOKS
 * is the symbol that the main program looks for
 */


orc_driver_t DRIVER_HOOKS = {
    .init_driver = debug_init_driver,
    .discover_ports = debug_discover_ports,
    .tx_pkt = debug_tx_pkt,
    .start_rx = debug_start_rx,
    .stop_rx = debug_stop_rx,
    .add_l3_v4_interface = debug_add_l3_v4_interface,
    .del_l3_interface = debug_del_l3_interface,
    .add_l3_v4_next_hop = debug_add_l3_v4_next_hop,
    .del_l3_next_hop = debug_del_l3_next_hop,
    .add_l3_v4_route    = debug_add_l3_v4_route,
    .del_l3_v4_route    = debug_del_l3_v4_route,
};

