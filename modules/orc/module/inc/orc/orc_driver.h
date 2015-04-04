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

#ifndef ORC_DRIVER_H
#define ORC_DRIVER_H

/**
 * Driver API for asic orc code.  If you want to support a new ASIC
 * this is the API your driver needs to support.
 */


typedef int l3_intf_id_t;               /* unique ID for l3 interfaces */
typedef int l3_next_hop_id_t;           /* unique ID for next_hop entries */

struct orc_driver_s;
#include "orc/ports.h"
#include "orc/options.h"
#include "orc/utils.h"

#include <limits.h>     /* for INT_MAX */


typedef struct orc_driver_s {
    /***
     * Initialize the driver and pass any unparsed arguments from
     * the command line to the driver
     */

    int (*init_driver)(struct orc_options_s * options, int argc, char * argv[]);

    /***
     * Get a list of physical ports from the driver.
     *
     * @param[in] ports: a caller-allocated array of port_t pointers
     *      note that the caller allocates the array but not
     *      the entry.  This allows the driver to create
     *      driver-specific state per port as long as the state
     *      starts with the port_t struct.  See debug driver
     *      for example.
     * @param[in] *num max number of ports allowed
     *
     * @param[out] ports: driver fills in the ports[i]->index field
     *      but leaves the rest untouched
     * @param[out] *num: the number of ports exported by the driver
     *
     * On successful return, the driver has filed in one port_t
     * structure for each physical port on the ASIC.
     *
     * In each physical port, all fields are left untouched
     * (for the caller to initialized later) except:
     *  * port->index : initialized with the human facing front
     *          panel port number
     *
     *  this port->index is used to make the matching Linux
     *  virtual interface,
     *  e.g., port->index = 5 --> "asic05"
     *
     * @return 0 on success, -1 on error
     */

    int (*discover_ports)(port_t * ports[], int * num);


    /***
     * Send a single packet out a physical port.
     *
     * This is the slow-path Linux-to-ASIC TX function
     * @return 0 on succes, -1 on error
     */

    int (*tx_pkt)(port_t * port, u8 * pkt, unsigned int len);

    /***
     * Start the receive process/thread/whathave you
     * in the ASIC SDK to slow-path send from ASIC-to-Linux
     *
     * At this point, the orc-asicd will have filed in all
     * of the ports[i]->fd fields, so they point to the corresponding
     * file descriptors for each virtual interface.
     *
     * Actual packet receiption is handled asynchronously by the
     * driver.  For example, one possible implementation of this
     * is to start a thread to poll the ASIC SDK for packets
     * sent from ASIC to CPU and then the driver sends them out
     * the fd of the corresponding Linux interface.  So:
     *
     * 0) Spawn a thread to monitor ASIC SDK
     * 1) Packet arrives at front panel port p1
     * 2) Packet is processed through ASIC and sent to CPU
     * 3) ASIC SDK intercepts packet and sends it out the file descriptor
     *      corresponding to ports[1]->fd
     *
     * @return 0 on success, -1 on error
     */
    int (*start_rx)(port_t * ports[], int num_ports);

    /***
     * Disables ASIC-to_Linux packet receiption.
     *
     * Typically called before shutting down.
     */
    int (*stop_rx)(void);

    /***
     * Enable "Raw" Mode on a port where 100% of the
     * packets that arrive in the front panel are sent out
     * the virtual interface.  This is an inherently lossy
     * process because the front panel interface is likely significantly
     * faster than the CPU/kernel interface.  In implementation, this
     * is likely "send all packets to CPU".
     */
    int (*raw_port_enable)(port_t * port);

    /***
     * Disable "raw" mode; reset the packets sent to the interface
     * to only the ones listed as L3 interfaces, e.g., via the
     * add_l3_v4_interface() calls.
     */
    int (*raw_port_disable)(port_t * port);


    /***
     * Add/Del an L3 interface
     * Tell the ASIC SDK/driver that we have a new L3 interface.
     * The expectation is:
     *  1) The SDK will do what ever it has to do to make routing work for that
     *      IP, e.g., add the MAC address to some hardware tables
     *  2) Send any packets with ip.dst = $ipv4_addr to the Linux Kernel via
     *      the mechiansm in start_rx().  This ties the Linux Kernel's ARP
     *      lookup algorithms to the ASIC.
     *
     * For Add:
     *  @param[out] on success (return 0), *l3_intf_id will point to a unique
     *      is for this l3_interface
     *
     *  TODO: decide how to handle vlans; short term expectation is that each
     *          interface will have its own unique vlan
     *
     *  @return 0 on success, -1 on failure
     */
    int (*add_l3_v4_interface)(
            port_t *port,
            u8 hw_mac[6],
            int mtu,
            u32 ipv4_addr,
            l3_intf_id_t *l3_intf_id
        );
    int (*update_l3_v4_interface)(
            port_t *port,
            u8 hw_mac[6],
            int mtu,
            u32 ipv4_addr,
            l3_intf_id_t l3_intf_id
        );
    int (*del_l3_interface)(port_t * port, l3_intf_id_t l3_intf_id);

    /***
     * Add/Del a "Next Hop" Entry
     * In Routing terms, the 'Next Hop' is the new post-routing destination MAC
     * address of the packet,
     * e.g., if the next hop in the path is a gateway, the MAC address of the
     *  gateway, or if the next hop is the final destination, it is the MAC
     *  address of the final destination
     *
     *  The l3_intf_id param identifies which virtual (and thus physical) port
     *      the packet should egress to get to this next_hop.  This parameter
     *      should be the result of an id from add_l3_interface()
     *
     *  @out param: on success, *l3_next_hop_id should be a unique id for this
     *      next_hop entry
     *
     *  TODO: decide how to handle VLANs: likely we just inherit from the l3_intf
     *
     *  NOTE 1: we have intentionally decoupled next_hop information from routing
     *      because it is common for multiple routes to share the same next hop and
     *      it eases the bookkeeping of the driver to not have to do reference
     *      counting
     *
     *  NOTE 2: the next_hop id of INT_MAX is reserved to specify "the next hop
     *      is to the software/Linux Kernel" and thus should never be returned
     *      by the add_l3_next_hop() function.
     *
     *  NOTE 3: repeated calls too add_l3_v4_next_hop() are not guaranteed
     *      to return the same l3_next_hop_id, so the caller is responsible
     *      for understanding if a next_hop entry is already allocated or
     *      risk resource exhaustion
     */
    #define NEXT_HOP_KERNEL INT_MAX
    int (*add_l3_v4_next_hop)(
            port_t * port,                              /* a valid port_t */
            l3_intf_id_t l3_intf_id,                    /* egress interface */
            u8 next_hop_hw_mac[6],                      /* of the next_hop */
            l3_next_hop_id_t * l3_next_hop_id
        );
    int (*del_l3_next_hop)(l3_next_hop_id_t l3_next_hop_id);

    /***
     * Add/Del a route to the ASIC
     *
     * Standard CIDR prefix of ip/mask to send traffic to the destination
     *  as denoted by the next_hop_id, as returned by add_l3_v4_next_hop()
     *
     * If next_hop_id == NEXT_HOP_KERNEL, the expectation is that traffic
     * that matches this route should be forwarded to the kernel via the
     * start_rx() method, documented above.
     *
     * The expectation is that multiple ADD calls for the same CIDR prefix
     *  will overwrite/update the next_hop entry and not result in two routes.
     *
     * To delete a route, call del_route() with the same parmeters as
     *  add_route(); there is no 'id' for routes.
     */

    int (*add_l3_v4_route)(
            u32 ip_dst,                     /* e.g., 192.168.0.0 */
            u32 netmask,                    /* e.g., 255.255.0.0 */
            l3_next_hop_id_t l3_next_hop_id
        );

    int (*del_l3_v4_route)(
            u32 ip_dst,                     /* e.g., 192.168.0.0 */
            u32 netmask,                    /* e.g., 255.255.0.0 */
            l3_next_hop_id_t l3_next_hop_id
        );

    /****
     *  Dump driver specific debugging information to the log file.
     *  Generally called on SIGUSR1
     */

    void (*log_debug_info)();

} orc_driver_t;


#endif
