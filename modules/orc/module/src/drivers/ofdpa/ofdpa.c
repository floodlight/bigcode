/************************************************************
 * <bsn.cl fy=2013 v=epl>
 *       Copyright 2013, 2014 Big Switch Networks, Inc.
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
 * orc driver for ofpda. Translates orc calls into those
 * required to implement in OpenFlow via ofdpa.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> /* we'll use a POSIX thread for packet RX */
#include <semaphore.h>
#include <string.h>

#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>

#include "orc/utils.h"
#include "orc/orc_logger.h"
#include "orc/orc_driver.h"
#include "orc/tap_utils.h"
#include "ofdpa_api.h"

/*
 * This is the name of this driver
 * that we'll register with ofdpa
 * during initialization.
 */
#define CLIENT_NAME "orc client"

/*
 * VLAN-related info for packet RX
 */
#define ETH_HEADER_VLAN_TAG_BYTES 4
#define ETH_HEADER_BEFORE_VLAN_TAG_BYTES 8
#define JUMBO_FRAME_BYTES 9300

/*
 * Default values for fields we might not
 * care about in a VLAN flow.
 */
#define ACTION 1

/*
 * Switch port w/added RX flag.
 */
typedef struct ofdpa_port {
    port_t port;
    int rx; /* rx <= 0, don't respond to RX events; else, push packet to orc interface */
} ofdpa_port_t;

/*
 * L2 next hop w/ofdpa info.
 */
typedef struct ofdpa_next_hop {
    l3_next_hop_id_t id;
    ofdpa_port_t * port;
    u8 mac[6];
} ofdpa_next_hop_t;

/*
 * L3 IPv4 interface w/ofdpa info.
 */
typedef struct ofdpa_interface {
    l3_intf_id_t id;
    ofdpa_port_t * port;
    u8 mac[6];
    u32 ipv4;
    ofdpa_next_hop_t * next_hop;
} ofdpa_interface_t;

/*
 * Containers for managing state.
 */
#define MAX_INTERFACES 128
#define MAX_NEXT_HOPS 128
#define MAX_PORTS 128

ofdpa_interface_t * ofdpa_interfaces[MAX_INTERFACES];
ofdpa_next_hop_t * ofdpa_next_hops[MAX_NEXT_HOPS];
ofdpa_port_t * ofdpa_ports[MAX_PORTS];

/*
 * Default/bogus info required to
 * follow ofdpa pipeline.
 */
u8 default_next_hop_mac[6] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x00 };
ofdpa_next_hop_t default_next_hop;
port_t default_next_hop_port;
ofdpa_port_t default_next_hop_ofdpa_port;
uint32_t default_next_hop_group_id;
uint32_t default_interface_group_id;

/*
 * RX thread
 */
pthread_t rx_thread;
sem_t rx_lock;

/*
 * Unique ID generator locks
 */
sem_t next_hop_id_lock;
sem_t l3_intf_id_lock;

/*********
 * Driver function prototypes
 */
int ofdpa_init_driver(struct orc_options_s *, int, char **);
int ofdpa_discover_ports(port_t **, int *);
int ofdpa_tx_pkt(port_t *, u8 *, unsigned int);
int ofdpa_start_rx(port_t **, int);
int ofdpa_stop_rx(void);
int ofdpa_add_l3_v4_interface(port_t *, u8 *, int, u32, l3_intf_id_t *);
int ofdpa_update_l3_v4_interface(port_t *, u8 *, int, u32, l3_intf_id_t);
int ofdpa_del_l3_v4_interface(port_t *, l3_intf_id_t);
int ofdpa_add_l3_v4_next_hop(port_t *, l3_intf_id_t, u8 *, l3_next_hop_id_t *);
int ofdpa_del_l3_v4_next_hop(l3_next_hop_id_t);
int ofdpa_add_l3_v4_route(u32, u32, l3_next_hop_id_t);
int ofdpa_del_l3_v4_route(u32, u32, l3_next_hop_id_t);
void ofdpa_log_debug_info(void);

/*********
 * Helper function prototypes
 */
uint32_t add_group_l2_interface(uint32_t, uint32_t);
uint32_t add_group_l3_unicast(uint32_t, uint32_t, uint32_t, u8 *, u8 *);
int get_num_ofdpa_ports(void);
void * port_rx_monitor(void *);
void free_ports(void);
void add_interface(ofdpa_interface_t *);
ofdpa_interface_t * get_interface(l3_intf_id_t);
void del_interface(l3_intf_id_t);
void del_next_hop_from_interfaces(ofdpa_next_hop_t *);
void free_interfaces(void);
void add_next_hop(ofdpa_next_hop_t *);
ofdpa_next_hop_t * get_next_hop(l3_next_hop_id_t);
void del_next_hop(l3_next_hop_id_t);
void free_next_hops(void);
l3_intf_id_t generate_l3_intf_id(void);
l3_next_hop_id_t generate_l3_next_hop_id(void);
uint16_t compute_vlan_id(uint32_t);
int ofdpa_add_or_update_l3_v4_interface(port_t *, u8 *, int, u32, l3_intf_id_t *, bool);
int add_default_next_hop(void);
int clear_ofdpa_tables(void);
int clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_t);
void mac_to_str(char *, u8 *);
void ipv4_to_str(char *, uint32_t);

/***************
 * ofdpa_init_driver: start up ofdpa
 */
int ofdpa_init_driver(orc_options_t *options, int argc, char * argv[])
{
    /* Our mutexes */
    int c;
    if ((c = sem_init(&rx_lock, 1 /* shared b/t threads and processes */, 1 /* count from 1 (i.e. only one at a time) */)) < 0)
    {
        orc_fatal("semaphore init for rx_lock failed: %d\r\r\n", c);
        return -1;
    }
    if ((c = sem_init(&l3_intf_id_lock, 1, 1)) < 0)
    {
        orc_fatal("semaphore init for l3_intf_id_lock failed: %d\r\r\n", c);
        return -1;
    }
    if ((c = sem_init(&next_hop_id_lock, 1, 1)) < 0)
    {
        orc_fatal("semaphore init for next_hop_id_lock failed: %d\r\n", c);
        return -1;
    }
    
    ofdpaDebugLevels_t level = OFDPA_DEBUG_VERY_VERBOSE;
    OFDPA_CONTROL_t ctrl = OFDPA_ENABLE;
    orc_warn("Setting OF-DPA debug level to %d\r\n", level);
    ofdpaDebugLvl(level);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_API, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_MAPPING, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_RPC, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_OFDB, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_DATAPATH, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_G8131, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_MPLS_TP, ctrl);
    ofdpaDebugComponentSet(OFDPA_COMPONENT_Y1731, ctrl);
    
    /* First initialize */
    OFDPA_ERROR_t rc = ofdpaClientInitialize(CLIENT_NAME);
    if (rc == OFDPA_E_NONE) {
        orc_warn("ofdpa initialized\r\n");
    } else {
        orc_fatal("ofdpa initialization returned error %d\r\n", rc);
        return -1;
    }
    
    /* Then, bind packet socket for receiving packets. */
    rc = ofdpaClientPktSockBind();
    if (rc == OFDPA_E_NONE) {
        orc_warn("ofdpa RX socket bound\r\n");
    } else {
        orc_fatal("ofdpa RX socket bind returned error %d\r\n", rc);
        return -1;
    }
    rc = ofdpaClientEventSockBind();
    if (rc == OFDPA_E_NONE) {
        orc_warn("ofdpa event socket bound\r\n");
    } else {
        orc_fatal("ofdpa event socket bind returned error %d\r\n", rc);
        return -1;
    }
    
    /* Finally start from a clean state. */
    orc_warn("Clearing all flow and group tables\r\n");
    if (!clear_ofdpa_tables())
    {
        return -1;
    }
    
    return add_default_next_hop();
}

/******
 * add_group_l2_interface: Add a new L2 interface group.
 * @return 0 on error; otherwise the group ID
 */
uint32_t add_group_l2_interface(uint32_t vlan_id, uint32_t port_id)
{
    uint32_t group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L2_INTERFACE;
    ofdpaGroupEntry_t group;
    ofdpaGroupBucketEntry_t group_bucket;
    memset(&group, 0, sizeof(ofdpaGroupEntry_t));
    memset(&group_bucket, 0, sizeof(ofdpaGroupBucketEntry_t));
    
    uint32_t group_id = 0;
    
    OFDPA_ERROR_t rc = ofdpaGroupTypeSet(&group_id, group_type);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group type. rc=%d\r\n", rc);
        return 0;
    }
    
    rc = ofdpaGroupVlanSet(&group_id, vlan_id);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group VLAN. rc=%d\r\n", rc);
        return 0;
    }
    
    rc = ofdpaGroupPortIdSet(&group_id, port_id);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group port. rc=%d\r\n", rc);
        return 0;
    }
    
    rc = ofdpaGroupEntryInit(group_type, &group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L2 interface group. rc=%d\r\n", rc);
        return 0;
    }
    
    group.groupId = group_id;
    rc = ofdpaGroupAdd(&group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to push L2 interface group. rc=%d\r\n", rc);
        return 0;
    }
    orc_warn("L2 interface group %#010x added for VLAN %d out port %d\r\n", group_id, vlan_id, port_id);
    
    rc = ofdpaGroupBucketEntryInit(group_type, &group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L2 interface bucket. rc=%d\r\n", rc);
        return 0;
    }
    
    group_bucket.groupId = group_id;
    group_bucket.bucketIndex = 0;
    group_bucket.bucketData.l2Interface.popVlanTag = ACTION; /* yes, remove the VLAN tag */
    group_bucket.bucketData.l2Interface.outputPort = port_id;
    
    rc = ofdpaGroupBucketEntryAdd(&group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to add L2 interface bucket. rc=%d\r\n", rc);
        return 0;
    }
    orc_warn("L2 interface group %#010x bucket added for VLAN %d out port %d\r\n", group_id, vlan_id, port_id);

    return group_id;
}

/******
 * add_group_l3_unicast: Add a new L3 unicast group.
 * @return 0 on error; otherwise the group ID
 */
uint32_t add_group_l3_unicast(uint32_t index, uint32_t reference_group_id, uint32_t vlan_id, u8 src_mac[6], u8 dst_mac[6])
{
    uint32_t group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST;
    ofdpaGroupEntry_t group;
    ofdpaGroupBucketEntry_t group_bucket;
    memset(&group, 0, sizeof(ofdpaGroupEntry_t));
    memset(&group_bucket, 0, sizeof(ofdpaGroupBucketEntry_t));
    
    uint32_t group_id = 0;
    
    OFDPA_ERROR_t rc = ofdpaGroupTypeSet(&group_id, group_type);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L3 unicast group type. rc=%d\r\n", rc);
        return 0;
    }
    
    rc = ofdpaGroupIndexSet(&group_id, index);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L3 unicast group index/ID. rc=%d\r\n", rc);
        return 0;
    }
    
    rc = ofdpaGroupEntryInit(group_type, &group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L3 unicast group. rc=%d\r\n", rc);
        return 0;
    }
    
    group.groupId = group_id;
    rc = ofdpaGroupAdd(&group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to push L3 unicast group. rc=%d\r\n", rc);
        return 0;
    }
    orc_warn("L3 unicast group entry %#010x added.\r\n", group_id);
    
    memset(&group_bucket, 0, sizeof(ofdpaGroupBucketEntry_t));
    rc = ofdpaGroupBucketEntryInit(group_type, &group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L3 unicast bucket. rc=%d\r\n", rc);
        return 0;
    }
    
    group_bucket.groupId = group_id;
    group_bucket.referenceGroupId = reference_group_id;
    group_bucket.bucketIndex = 0;
    group_bucket.bucketData.l3Unicast.vlanId = vlan_id;
    memcpy(&(group_bucket.bucketData.l3Unicast.srcMac), src_mac, OFDPA_MAC_ADDR_LEN);
    memcpy(&(group_bucket.bucketData.l3Unicast.dstMac), dst_mac, OFDPA_MAC_ADDR_LEN);
    
    rc = ofdpaGroupBucketEntryAdd(&group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to add L3 unicast bucket. rc=%d\r\n", rc);
        return 0;
    }
    orc_warn("L3 unicast group %#010x bucket added for VLAN %d; Referencing L2 interface group %#010x\r\n", group_id, vlan_id, group_bucket.referenceGroupId);
    
    return group_id;
}

int add_default_next_hop()
{
    /* Init global structs */
    default_next_hop_port.index = 1; /* Assume port #1 */
    default_next_hop.id = generate_l3_next_hop_id(); /* ID is guaranteed to be unique */
    memcpy(&(default_next_hop.mac), &default_next_hop_mac, OFDPA_MAC_ADDR_LEN);
    default_next_hop.port = (ofdpa_port_t *) &default_next_hop_ofdpa_port;
    
    add_next_hop(&default_next_hop);
    
    /*
     * First do the L2 Interface group, since we need to reference it
     * in the L3 Unicast group as a bucket.
     */

    uint32_t group_id = add_group_l2_interface(default_next_hop_port.index, /* Use port (1) for unused VLAN */
                                                  default_next_hop_port.index);
    if (group_id == -1) {
        return -1;
    }
    
    default_interface_group_id = group_id;
    
    /*
     * Now, we can do the L3 Unicast group.
     */
    group_id = add_group_l3_unicast(default_next_hop.id,
                                                group_id,
                                                default_next_hop_port.index, /* Use port (1) for unused VLAN */
                                                default_next_hop_mac,
                                                default_next_hop_mac);
    if (group_id == 0) {
        return -1;
    }
    
    default_next_hop_group_id = group_id;

    return 0;
}

/******************
 * discover_ports: Ask ofdpa for the ports it has available.
 * We assume contiguous port numbering. The port_t array is
 * initialized with port_t structures. Only the index is set.
 * The caller must initialize the interfaces upon return.
 */
int ofdpa_discover_ports(port_t * ports[], int * num)
{
    int i, ofdpaPorts;
    ofdpa_port_t * ofport;
    
    ofdpaPorts = get_num_ofdpa_ports();
    orc_warn("OF-DPA has %d ports\r\n", ofdpaPorts);
    
    sem_wait(&rx_lock);
    
    /* Clear all driver-local ports */
    free_ports();
    
    /* Now reallocate */
    for (i = 0; i < ofdpaPorts; i++)
    {
        ofport = malloc(sizeof(*ofport));
        if (ofport == NULL)
        {
            orc_err("Malloc failed allocating ports\r\n");
            return -1;
        }
        ofport->rx = 0; /* we don't want to receive packets from this port at present; set in start_rx below */
        ofport->port.index = i+1; /* index starts at 1 */
        ports[i] = (port_t *) ofport;
        ofdpa_ports[i] = ofport;
        orc_debug("Created port %d for port %s\r\n", ofport->port.index, ofport->port.name);
    }
    *num = ofdpaPorts;
    
    sem_post(&rx_lock);
    
    return 0;
}

/*********
 * ofdpa_tx_packet: Send a packet-out message. The in-port will be set as the
 * special OpenFlow port CONTROLLER
 */
int ofdpa_tx_pkt(port_t *port, u8 *pkt, unsigned int len)
{
    struct ofdpa_port * ofdpa = (struct ofdpa_port *) port;
    
    ofdpa_buffdesc buf;
    buf.size = len;
    buf.pstart = (char *) pkt;

    OFDPA_ERROR_t rc = ofdpaPktSend(&buf, 0, port->index, OFDPA_PORT_CONTROLLER);
    if (rc == OFDPA_E_NONE) {
        orc_debug("TX packet out port %s, ID %d returned success code %d\r\n", ofdpa->port.name, ofdpa->port.index, rc);
        return 0;
    } else {
        orc_err("TX packet out port %s, ID %d, returned error %d\r\n", ofdpa->port.name, ofdpa->port.index, rc);
        return -1;
    }
}

/******
 * ofpda_start_rx: spawn our RX thread that will wait on packet RX events
 * as triggered by ofdpa
 */
int ofdpa_start_rx(port_t * ports[], int num_ports)
{
    sem_wait(&rx_lock);
    orc_warn("Setting RX on %d ports\r\n", num_ports);
    
    int i;
    for (i = 0; i < num_ports; i++)
    {
        orc_warn("Starting RX on port %d\r\n", ports[i]->index);
        ((ofdpa_port_t *) ports[i])->rx = 1; /* set flag to RX packets on this port */
    }
    
    if (rx_thread == 0)
    {
        int rc = pthread_create(&rx_thread, NULL /* default attributes */, port_rx_monitor, NULL /* no args to function ptr <-- */);
        if (rc != 0)
        {
            orc_fatal("RX thread creation failed with error code %d\r\n", rc);
            return -1;
        }
        orc_warn("Started RX thread\r\n");
    }
    sem_post(&rx_lock);
    
    return 0;
}

/******
 * ofdpa_stop_rx: clean up resources used to RX packets.
 * We also free all resources allocated for the driver and reset the flow tables.
 */
int ofdpa_stop_rx()
{
    orc_info("Received signal to stop receiving packets. Cleaning up OF-DPA driver resources...\r\n");
    
    /*
     * Free the socket for RX. This function
     * should only be called (typically?)
     * before shutdown, meaning there doesn't
     * need to be a way to restart the RX
     * socket.
     */
    int fd = ofdpaClientPktSockFdGet();
    if (fd > 0)
    {
        orc_warn("Closing RX socket file descriptor: %d\r\n", fd);
        close(fd);
    } else { /* Just log error so that we can continue killing stuff */
        orc_err("ofdpa did not return a valid file descriptor when attempting to close the event socket: %d\r\n", fd);
    }
    fd = ofdpaClientEventSockFdGet();
    if (fd > 0)
    {
        orc_warn("Closing event socket file descriptor: %d\r\n", fd);
        close(fd);
    } else { /* Just log error so that we can continue killing stuff */
        orc_err("ofdpa did not return a valid file descriptor when attempting to close the event socket: %d\r\n", fd);
    }
    
    /* Kill thread */
    pthread_cancel(rx_thread);
    
    /* Now free all the memory we ate */
    free_ports();
    free_interfaces();
    free_next_hops();
    
    /* And don't forget to destroy our locks/semaphores */
    sem_destroy(&rx_lock);
    sem_destroy(&l3_intf_id_lock);
    sem_destroy(&next_hop_id_lock);
    
    /* Purge existing routes to stop packet flow. */
    orc_warn("Clearing all flow and group tables\r\n");
    if (!clear_ofdpa_tables())
    {
        return -1;
    } else {
        return 0;
    }
}

/******
 * ofdpa_add_l3_v4_interface: associate an IPv4 address with a port. Anything destined for it will be sent to the Linux iface FD.
 * (1) add an entry to the VLAN table
 * (2) add an entry to the termination MAC table -- match on ethertype and dst MAC provided as argument, goto routing; default bridging
 * (3) add an entry to the routing table -- match on ethertype, dst MAC/IP, goto policy ACL, since we can't output to controller here
 * (4a) add an entry to the policy ACL table -- match same as routing table flow, output to CONTROLLER
 * (4b) add an entry to the bridging table -- match on dst MAC (must be non-IP, otherwise would have been picked up by routing tbl), output to CONTROLLER
 * (l3_intf_id_t is an int)
 */
int ofdpa_add_l3_v4_interface(port_t * port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t * l3_intf_id) {
    return ofdpa_add_or_update_l3_v4_interface(port, hw_mac, mtu, ipv4_addr, l3_intf_id, false);
}

/******
 * ofdpa_add_or_update_l3_v4_interface: add new or update existing L3 interface.
 * If an update, assumes flows have already been deleted.
 * This is technically a helper function, but we'll include it here, since it
 * contains the meat of adding and updating and many ofdpa function calls.
 */
int ofdpa_add_or_update_l3_v4_interface(port_t * port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t * l3_intf_id, bool is_update) {
    OFDPA_ERROR_t rc;
    ofdpaFlowEntry_t flow;
    
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize VLAN flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaVlanFlowEntry_t));

        /* Matches */
        flow.flowData.vlanFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.vlanFlowEntry.match_criteria.vlanId = compute_vlan_id(port->index) | 0x1000;
        flow.flowData.vlanFlowEntry.match_criteria.vlanIdMask = 0x1fff;
        
        /* Goto Table Instruction */
        flow.flowData.vlanFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_TERMINATION_MAC;
        
        /* Apply Actions Instruction */
        flow.flowData.vlanFlowEntry.setVlanIdAction = ACTION;
        flow.flowData.vlanFlowEntry.newVlanId = compute_vlan_id(port->index);
        
        orc_debug("Pushing VLAN flow for port %d with VLAN %d\r\n", flow.flowData.vlanFlowEntry.match_criteria.inPort, flow.flowData.vlanFlowEntry.newVlanId);
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("VLAN flow already exists for port %d with VLAN %d. Continuing\r\rn", flow.flowData.vlanFlowEntry.match_criteria.inPort, flow.flowData.vlanFlowEntry.newVlanId);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push VLAN flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed VLAN flow for port %d with VLAN %d\r\n", flow.flowData.vlanFlowEntry.match_criteria.inPort, flow.flowData.vlanFlowEntry.newVlanId);
        }
    }
    
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize untagged VLAN flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaVlanFlowEntry_t));
        
        /* Matches -- pick up any untagged packets to assign tag */
        flow.flowData.vlanFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.vlanFlowEntry.match_criteria.vlanId = 0x0000;
        flow.flowData.vlanFlowEntry.match_criteria.vlanIdMask = 0x0fff;
        
        /* Goto Table Instruction */
        flow.flowData.vlanFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_TERMINATION_MAC;
        
        /* Apply Actions Instruction */
        flow.flowData.vlanFlowEntry.setVlanIdAction = ACTION;
        flow.flowData.vlanFlowEntry.newVlanId = compute_vlan_id(port->index);
        
        orc_debug("Pushing untagged VLAN flow for port %d with VLAN %d\r\n", flow.flowData.vlanFlowEntry.match_criteria.inPort, flow.flowData.vlanFlowEntry.newVlanId);
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("Untagged VLAN flow already exists for port %d with VLAN %d. Continuing\r\rn", flow.flowData.vlanFlowEntry.match_criteria.inPort, flow.flowData.vlanFlowEntry.newVlanId);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push untagged VLAN flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed untagged VLAN flow for port %d with VLAN %d\r\n", flow.flowData.vlanFlowEntry.match_criteria.inPort, flow.flowData.vlanFlowEntry.newVlanId);
        }
    }
    
    /* Now do the termination MAC flow */
    char mac[OFDPA_MAC_ADDR_LEN * 2 * 2];
    mac_to_str(mac, hw_mac);
    
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_TERMINATION_MAC, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize termination MAC flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaTerminationMacFlowEntry_t));
        
        /* Matches */
        flow.flowData.terminationMacFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.terminationMacFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_EXACT_MASK;
        flow.flowData.terminationMacFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        memcpy(&(flow.flowData.terminationMacFlowEntry.match_criteria.destMac), hw_mac, OFDPA_MAC_ADDR_LEN);
        memset(&(flow.flowData.terminationMacFlowEntry.match_criteria.destMacMask), 0xff, OFDPA_MAC_ADDR_LEN);
        
        /* Goto Table Instruction */
        flow.flowData.terminationMacFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING;
        
        orc_debug("Termination MAC flow about to be added for port %d with MAC %s\r\n", flow.flowData.terminationMacFlowEntry.match_criteria.inPort, mac);
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("Termination MAC flow already exists for port %d with MAC %s. Continuing\r\n", flow.flowData.terminationMacFlowEntry.match_criteria.inPort, mac);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push termination MAC flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed Termination MAC flow for port %d with MAC %s\r\n", flow.flowData.terminationMacFlowEntry.match_criteria.inPort, mac);
        }
    }
    
    /* Now do the unicast routing flow */
    char str_ip[16];
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize unicast routing flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaUnicastRoutingFlowEntry_t));
        
        /* Matches */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4 = ipv4_addr;
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4Mask = OFDPA_IPV4_ADDR_EXACT_MASK; /* exact IPv4 match */
        
        /* We will specify the default group; required */
        flow.flowData.unicastRoutingFlowEntry.groupID = default_next_hop_group_id;
        
        /* Goto Table Instruction */
        flow.flowData.unicastRoutingFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_ACL_POLICY;
        
        ipv4_to_str(str_ip, flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4);
        orc_debug("Attempting to push exact unicast routing flow for IP %s\r\n", str_ip);
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("Exact unicast routing flow already exists for IP %s. Continuing\r\n", str_ip);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push exact unicast routing flow for IP %s. rc=%d\r\n", str_ip, rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed exact unicast routing flow for IP %s\r\n", str_ip);
        }
        
    }
    
    /* Now do the policy ACL flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize policy ACL flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
        
        /* Matches */
        flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
        flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
        memcpy(&(flow.flowData.policyAclFlowEntry.match_criteria.destMac), hw_mac, OFDPA_MAC_ADDR_LEN);
        memset(&(flow.flowData.policyAclFlowEntry.match_criteria.destMacMask), 0xff, OFDPA_MAC_ADDR_LEN);
        flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
        flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ipv4_addr;
        flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = OFDPA_IPV4_ADDR_EXACT_MASK; /* exact IPv4 match */
        
        /* Apply Actions Instruction */
        
        /*
         * OF-DPA IMPLEMENTATION NOTE:
         * Including this output (apply action) will cause the
         * goto-group (write action) set in the unicast routing
         * flow to be ignored. This is not correct according to
         * the OpenFlow spec, but it allows us to send to the CPU
         * w/o also sending to a next hop group.
         */
        flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt up to ORC (will pass into RX thread) */
        
        ipv4_to_str(str_ip, flow.flowData.policyAclFlowEntry.match_criteria.destIp4);
        
        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("Policy ACL flow already exists for MAC %s, IP %s. Continuing\r\n", mac, str_ip);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push policy ACL flow for MAC %s, IP %s. rc=%d\r\n", mac, str_ip, rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed policy ACL flow for for MAC %s, IP %s\r\n", mac, str_ip);
        }
    }
    
    /* Now do the bridging flow -- for non-IP packet delivery to ORC's RX func */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_BRIDGING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize bridging flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaBridgingFlowEntry_t));
        
        /* Matches */
        memcpy(&(flow.flowData.bridgingFlowEntry.match_criteria.destMac), hw_mac, OFDPA_MAC_ADDR_LEN);
        memset(&(flow.flowData.bridgingFlowEntry.match_criteria.destMacMask), 0xff, OFDPA_MAC_ADDR_LEN); /* exact MAC match */
        flow.flowData.bridgingFlowEntry.match_criteria.vlanId = compute_vlan_id(port->index); /* must match on VLAN w/o 'present' bit set */
        flow.flowData.bridgingFlowEntry.match_criteria.vlanIdMask = 0x0fff;
        
        /* Must also supply L2 interface group, even though we won't use it. */
        flow.flowData.bridgingFlowEntry.groupID = default_interface_group_id;
        
        /* Apply Actions Instruction */
        flow.flowData.bridgingFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt non-IP packets up to ORC's RX func */
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("Bridging flow already exists for MAC %s, VLAN %d. Continuing\r\n", mac, flow.flowData.bridgingFlowEntry.match_criteria.vlanId);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push bridging flow for MAC %s, VLAN %d. rc=%d\r\n", mac, flow.flowData.bridgingFlowEntry.match_criteria.vlanId, rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed bridging flow for for MAC %s, VLAN %d\r\n", mac, flow.flowData.bridgingFlowEntry.match_criteria.vlanId);
        }
    }
    
    /* Handle any ARP packets that we might need to answer in the kernel */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize ARP policy ACL flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
        
        /* Matches -- all ARP packets */
        flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
        flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
        flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0806;
        flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
        
        /* Apply Actions Instruction */
        flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt ARP packets up to ORC's RX func */
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("ARP policy flow already exists. Continuing\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push ARP policy ACL flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Pushed ARP policy ACL flow\r\n");
        }
    }
    
    /* If we get here, then all flows should be inserted w/o error */
    ofdpa_interface_t * iface = NULL;
    if (!is_update)
    {
        *l3_intf_id = generate_l3_intf_id();
        iface = (ofdpa_interface_t *) malloc(sizeof(ofdpa_interface_t));
        if (iface == NULL)
        {
            orc_fatal("Mallocing ofdpa_interface_t failed\r\n");
            return -1;
        }
    }
    else
    {
        iface = get_interface(*l3_intf_id);
        if (iface == NULL)
        {
            orc_fatal("Could not locate ofdpa_interface_t in local store for ID %d\r\n", *l3_intf_id);
            return -1;
        }
    }
    
    /* init/update interface structure (iface will not be NULL here) */
    iface->port = (ofdpa_port_t *) port; /* pointer already */
    memcpy(&(iface->mac), hw_mac, OFDPA_MAC_ADDR_LEN);
    iface->ipv4 = ipv4_addr;
    if (!is_update)
    {
        iface->id = *l3_intf_id;
        iface->next_hop = NULL;
        add_interface(iface);
    }
    
    return 0;
}

/******
 * ofdpa_update_l3_v4_interface: modify an existing IPv4 interface
 */
int ofdpa_update_l3_v4_interface(port_t * port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t l3_intf_id) {
    /*
     * To update an interface, we need to remove the flows
     * first, since a flow-modify will not allow us to
     * change the match of the flow (might be modified).
     */
    
    if (ofdpa_del_l3_v4_interface(port, l3_intf_id) < 0)
    {
        orc_err("Error removing interface %d on port %d\r\n", l3_intf_id, port->index);
        return -1;
    }
    
    if (get_interface(l3_intf_id) == NULL)
    {
        orc_err("Could not lookup L3 interface ID %d. Not updating.\r\n", l3_intf_id);
        return -1;
    }
    else
    {
        return ofdpa_add_or_update_l3_v4_interface(port, hw_mac, mtu, ipv4_addr, &l3_intf_id, true /* yes, this is an update */);
    }
}

/******
 * ofdpa_del_l3_v4_interface: delete an existing IPv4 interface
 */
int ofdpa_del_l3_v4_interface(port_t * port, l3_intf_id_t l3_intf_id) {
    OFDPA_ERROR_t rc;
    ofdpaFlowEntry_t flow;
    
    /* Lookup L3 interface to find the IP, MAC, etc. */
    ofdpa_interface_t * iface = get_interface(l3_intf_id);
    if (iface == NULL)
    {
        orc_err("Could not locate L3 interface ID %d in local store\r\n", l3_intf_id);
        return -1;
    }
    else if (port != (port_t *) iface->port) /* pointer address check should be sufficient */
    {
        orc_err("Port provided inconsistent with port in L3 interface. Not deleting L3 iface %d\r\n", l3_intf_id);
        return -1;
    }
    
    /* We found our interface, so get the info we need. */
    u8 hw_mac[6];
    u32 ipv4_addr;
    memcpy(&hw_mac, &(iface->mac), OFDPA_MAC_ADDR_LEN);
    uint32_t vlan_id = compute_vlan_id(port->index);
    ipv4_addr = iface->ipv4;
    
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize untagged VLAN %d flow. rc=%d\r\n", vlan_id, rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaVlanFlowEntry_t));
        
        /* Matches */
        flow.flowData.vlanFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.vlanFlowEntry.match_criteria.vlanId = 0x0000;
        flow.flowData.vlanFlowEntry.match_criteria.vlanIdMask = 0x0fff;
        
        /* Goto Table Instruction */
        flow.flowData.vlanFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_TERMINATION_MAC;
        
        /* Apply Actions Instruction */
        flow.flowData.vlanFlowEntry.setVlanIdAction = ACTION;
        flow.flowData.vlanFlowEntry.newVlanId = compute_vlan_id(port->index);
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("Untagged VLAN %d on port %d flow not found\r\n", vlan_id, flow.flowData.vlanFlowEntry.match_criteria.inPort);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete untagged VLAN %d flow on port %d. rc=%d\r\n", vlan_id, flow.flowData.vlanFlowEntry.match_criteria.inPort, rc);
            return -1;
        }
        else
        {
            orc_warn("Untagged VLAN %d flow on port %d deleted\r\n", vlan_id, flow.flowData.vlanFlowEntry.match_criteria.inPort);
        }
    }
    
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize VLAN %d flow. rc=%d\r\n", vlan_id, rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaVlanFlowEntry_t));

        /* Matches */
        flow.flowData.vlanFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.vlanFlowEntry.match_criteria.vlanId = vlan_id | 0x1000;
        flow.flowData.vlanFlowEntry.match_criteria.vlanIdMask = 0x1fff;
        
        /* Goto Table Instruction */
        flow.flowData.vlanFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_TERMINATION_MAC;
        
        /* Apply Actions Instruction */
        flow.flowData.vlanFlowEntry.setVlanIdAction = ACTION;
        flow.flowData.vlanFlowEntry.newVlanId = vlan_id;
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("VLAN %d flow on port %d not found\r\n", vlan_id, flow.flowData.vlanFlowEntry.match_criteria.inPort);
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete VLAN %d flow on port %d. rc=%d\r\n", vlan_id, flow.flowData.vlanFlowEntry.match_criteria.inPort, rc);
            return -1;
        }
        else
        {
            orc_warn("VLAN %d flow on port %d deleted\r\n", vlan_id, flow.flowData.vlanFlowEntry.match_criteria.inPort);
        }
    }
    
    /* Now do the termination MAC flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_TERMINATION_MAC, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize termination MAC flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaTerminationMacFlowEntry_t));
        
        /* Matches */
        flow.flowData.terminationMacFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.terminationMacFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_EXACT_MASK;
        flow.flowData.terminationMacFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        memcpy(&(flow.flowData.terminationMacFlowEntry.match_criteria.destMac), hw_mac, OFDPA_MAC_ADDR_LEN);
        memset(&(flow.flowData.terminationMacFlowEntry.match_criteria.destMacMask), 0xff, OFDPA_MAC_ADDR_LEN);
        
        /* Goto Table Instruction */
        flow.flowData.terminationMacFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING;
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("Termination MAC flow not found\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete termination MAC flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Termination MAC flow deleted\r\n");
        }
    }
    
    /* Now do the unicast routing flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize unicast routing flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaUnicastRoutingFlowEntry_t));
        
        /* Matches */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4 = ipv4_addr;
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4Mask = OFDPA_IPV4_ADDR_EXACT_MASK; /* exact IPv4 match */
        
        /* Default/bogus next hop */
        flow.flowData.unicastRoutingFlowEntry.groupID = default_next_hop_group_id;
        
        /* Goto Table Instruction */
        flow.flowData.unicastRoutingFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_ACL_POLICY;
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("Unicast routing flow not found\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete unicast routing flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Unicast routing flow deleted\r\n");
        }
    }
    
    /* Now do the policy ACL flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize policy ACL flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
        
        /* Matches */
        flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
        flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
        memcpy(&(flow.flowData.policyAclFlowEntry.match_criteria.destMac), hw_mac, OFDPA_MAC_ADDR_LEN);
        memset(&(flow.flowData.policyAclFlowEntry.match_criteria.destMacMask), 0xff, OFDPA_MAC_ADDR_LEN); /* exact MAC match -- TODO might be redundant? */
        flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
        flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ipv4_addr;
        flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = OFDPA_IPV4_ADDR_EXACT_MASK; /* exact IPv4 match */
        
        /* Apply Actions Instruction */
        flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt up to ORC (will pass into RX thread) */
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("Policy ACL flow not found\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete policy ACL flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Policy ACL flow deleted\r\n");
        }
    }
    
    /* Now do the bridging flow -- for non-IP packet delivery to ORC's RX func */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_BRIDGING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize bridging flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaBridgingFlowEntry_t));
        
        /* Matches */
        memcpy(&(flow.flowData.bridgingFlowEntry.match_criteria.destMac), hw_mac, OFDPA_MAC_ADDR_LEN);
        memset(&(flow.flowData.bridgingFlowEntry.match_criteria.destMacMask), 0xff, OFDPA_MAC_ADDR_LEN); /* exact MAC match */
        flow.flowData.bridgingFlowEntry.match_criteria.vlanId = compute_vlan_id(port->index); /* must match on VLAN w/o 'present' bit set */
        flow.flowData.bridgingFlowEntry.match_criteria.vlanIdMask = 0x0fff;
        
        /* Must also supply L2 interface group, even though we won't use it. */
        flow.flowData.bridgingFlowEntry.groupID = default_interface_group_id;
        
        /* Apply Actions Instruction */
        flow.flowData.bridgingFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt non-IP packets up to ORC's RX func */
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("Bridging flow not found\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete bridging flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Bridging flow deleted\r\n");
        }
    }
    
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize ARP policy ACL flow. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
        
        /* Matches -- all ARP packets */
        flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
        flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
        flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0806;
        flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
        
        /* Apply Actions Instruction */
        flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt ARP packets up to ORC's RX func */
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("ARP policy flow not found. Continuing\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete ARP policy ACL flow. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Deleted ARP policy ACL flow\r\n");
        }
    }
    
    /* If we get here, then all flows should be deleted w/o error */
    del_interface(l3_intf_id);
    
    return 0;
}

/******
 * ofdpa_add_l3_v4_next_hop: set the next hop of an interface. Will only permit adding the next hop if
 * the interface does not already have a next hop.
 * TODO what is the purpose of the port_t * port argument?
 * (l3_next_hop_id_t is an int)
 */
int ofdpa_add_l3_v4_next_hop(port_t * port, l3_intf_id_t l3_intf_id, u8 next_hop_hw_mac[6], l3_next_hop_id_t * l3_next_hop_id) {
    ofdpa_interface_t * iface = get_interface(l3_intf_id);
    if (iface == NULL)
    {
        orc_err("Could not lookup interface ID %d. Not adding next hop.\r\n", l3_intf_id);
        return -1;
    }
    else if (iface->next_hop != NULL)
    {
        orc_err("Tried adding a next hop to an interface that already has a next hop. Not adding next hop. Remove the existing next hop %d first.\r\n", iface->next_hop->id);
        return -1;
    }
    
    /* Verify next hop MAC is valid */
    char mac[OFDPA_MAC_ADDR_LEN * 2 * 2];
    mac_to_str(mac, next_hop_hw_mac);
    if (next_hop_hw_mac[0] == 0xff &&
        next_hop_hw_mac[1] == 0xff &&
        next_hop_hw_mac[2] == 0xff &&
        next_hop_hw_mac[3] == 0xff &&
        next_hop_hw_mac[4] == 0xff &&
        next_hop_hw_mac[5] == 0xff)
    {
        orc_err("Received invalid broadcast MAC %s as next hop. Expected unicast\r\n", mac);
        return -1;
    }
    else if (next_hop_hw_mac[0] == 0x00 &&
             next_hop_hw_mac[1] == 0x00 &&
             next_hop_hw_mac[2] == 0x00 &&
             next_hop_hw_mac[3] == 0x00 &&
             next_hop_hw_mac[4] == 0x00 &&
             next_hop_hw_mac[5] == 0x00)
    {
        orc_err("Received invalid zero MAC %s as next hop. Expected unicast\r\n", mac);
        return -1;
    }
    else if (next_hop_hw_mac[0] & 0x01)
    {
        orc_err("Received invalid multicast MAC %s as next hop. Expected unicast\r\n", mac);
        return -1;
    }
    
    ofdpa_next_hop_t * next_hop = (ofdpa_next_hop_t *) malloc(sizeof(ofdpa_next_hop_t));
    if (next_hop == NULL)
    {
        orc_fatal("ofdpa_next_hop_t malloc failed\r\n");
        return -1;
    }
    
    /* Fill in our next hop */
    *l3_next_hop_id = generate_l3_next_hop_id(); /* ID is guaranteed to be unique */
    next_hop->id = *l3_next_hop_id;
    iface->next_hop = next_hop;
    
    /*
     * IMPLEMENTATION NOTE:
     * Here, we set the next hop's port as the port supplied
     * as an argument to this function. We do not use the
     * port of the interface we looked up from the l3_intf_id_t
     * supplied, since that is the port of the interface, not
     * the port of the next hop. Different interfaces on different
     * switch ports can share the same next hop. It only makes
     * sense to use the interface-independent port supplied as an
     * argument to this function.
     */
    next_hop->port = (ofdpa_port_t *) port;
    memcpy(&(next_hop->mac), next_hop_hw_mac, OFDPA_MAC_ADDR_LEN);
    
    mac_to_str(mac, next_hop->mac);
    orc_warn("Creating next hop interface %d with next hop MAC %s\r\n", next_hop->id, mac);
    
    add_next_hop(next_hop);
    
    /*
     * Insert L3 Unicast group entry AND L2 Interface group entry
     * on port of the next hop.
     */
    uint32_t port_id = port->index;
    uint32_t vlan_id = compute_vlan_id(port->index);
    
    /*
     * First do the L2 Interface group, since we need to reference it
     * in the L3 Unicast group as a bucket.
     */
    uint32_t group_id = add_group_l2_interface(vlan_id, port_id);
    if (group_id == 0) {
        return -1;
    }

    /* Now, we can do the L3 Unicast group. */
    group_id = add_group_l3_unicast(next_hop->id,
                                    group_id,
                                    vlan_id,
                                    iface->mac,
                                    next_hop->mac);
    if (group_id == 0) {
        return -1;
    }
    
    return 0;
}

/******
 * ofdpa_del_l3_v4_next_hop: delete a next hop entry. Note that
 * due to a simple implementation choice, at least until all the
 * initial bugs are worked out, the caller must first delete all
 * routes that are using this next hop PRIOR to deleting the next
 * hop here. TODO Record route IP and subnet mask so we can remove
 * routes automatically when a next hop is removed.
 */
int ofdpa_del_l3_v4_next_hop(l3_next_hop_id_t l3_next_hop_id) {
    ofdpa_next_hop_t * next_hop = get_next_hop(l3_next_hop_id);
    if (next_hop == NULL && l3_next_hop_id == NEXT_HOP_KERNEL)
    {
        orc_warn("Ignoring deletion of next hop %d to kernel.\r\n", l3_next_hop_id);
        return 0;
    }
    else if (next_hop == NULL)
    {
        orc_err("Could not find next hop %d. Not deleting next hop.\r\n", l3_next_hop_id);
        return -1;
    }
    else
    {
        orc_warn("Found next hop %d. Proceeding to delete it\r\n", next_hop->id);
    }
    
    
    port_t * port = &(next_hop->port->port);
    
    uint32_t group_id = 0;
    uint32_t port_id;
    uint32_t vlan_id;
    uint32_t group_type;
    
    /* First do the L3 Unicast group. */
    port_id = port->index;
    vlan_id = compute_vlan_id(port->index);
    group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST;
    
    OFDPA_ERROR_t rc = ofdpaGroupTypeSet(&group_id, group_type); 
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L3 unicast group type. rc=%d\r\n", rc);
        return -1;
    }
    
    rc = ofdpaGroupIndexSet(&group_id, next_hop->id);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L3 unicast group index/ID. rc=%d\r\n", rc);
        return -1;
    }
    
    rc = ofdpaGroupBucketsDeleteAll(group_id);
    if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
    {
        orc_warn("L3 unicast group %#010x not found\r\n", group_id);
    }
    else if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to delete L3 unicast group %#010x buckets. rc=%d\r\n", group_id, rc);
        return -1;
    }
    else
    {
        orc_warn("L3 unicast group %#010x bucket deleted from port %d, VLAN %d\r\n", group_id, port_id, vlan_id);
    }
    
    rc = ofdpaGroupDelete(group_id);
    if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
    {
        orc_warn("L3 unicast group %#010x not found\r\n", group_id);
    }
    else if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to delete L3 unicast group %#010x. rc=%d\r\n", group_id, rc);
        return -1;
    }
    else
    {
        orc_warn("L3 unicast group entry %#010x deleted.\r\n", group_id);
    }
    
    /* Now, we can do the L2 Interface group. */
    group_id = 0;
    group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L2_INTERFACE;
    
    rc = ofdpaGroupTypeSet(&group_id, group_type); 
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group type. rc=%d\r\n", rc);
        return -1;
    }
    
    rc = ofdpaGroupVlanSet(&group_id, vlan_id); 
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group VLAN. rc=%d\r\n", rc);
        return -1;
    }
    
    rc = ofdpaGroupPortIdSet(&group_id, port_id); 
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group port. rc=%d\r\n", rc);
        return -1;
    }
    
    rc = ofdpaGroupBucketsDeleteAll(group_id);
    if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
    {
        orc_warn("L2 interface group %#010x buckets not found\r\n", group_id);
    }
    else if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to delete L2 interface group %#010x buckets. rc=%d\r\n", group_id, rc);
        return -1;
    }
    else
    {
        orc_warn("L2 interface group %#010x buckets deleted from port %d\r\n", group_id, port_id);
    }
    
    rc = ofdpaGroupDelete(group_id);
    if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
    {
        orc_warn("L2 interface group %#010x not found\r\n", group_id);
    }
    else if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to delete L2 interface group %#010x. rc=%d\r\n", group_id, rc);
        return -1;
    }
    else
    {
        orc_warn("L2 interface group %#010x deleted\r\n", group_id);
    }
    
    /*
     * Remove from our internal list of next hops.
     * This will remove the next hop from all
     * interfaces too.
     */
    del_next_hop(l3_next_hop_id);
    
    return 0;
}

/******
 * ofdpa_add_l3_v4_route: add a CIDR route provided a next hop. This involves
 * adding flows in the unicast routing table in order to match on the correct
 * subnet.
 *
 * Since a next hop must be provided, that means groups will already have been
 * installed. All we need to do is insert a masked-match flow in the unicast
 * routing table in order to send matches to the L3 unicast group (which will
 * then chain into the L2 interface group, which will then output the packet).
 */
int ofdpa_add_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id) {
    ofdpa_next_hop_t * next_hop = get_next_hop(l3_next_hop_id);
    
    if (next_hop == NULL && l3_next_hop_id == NEXT_HOP_KERNEL)
    {
        orc_warn("Adding route with next hop %d to kernel.\r\n", l3_next_hop_id);
    }
    else if (next_hop == NULL)
    {
        orc_err("Could not find next hop %d. Not adding route.\r\n", l3_next_hop_id);
        return -1;
    }
    else
    {
        orc_warn("Found next hop %d. Proceeding to add route\r\n", next_hop->id);
    }
    
    ofdpaFlowEntry_t flow;
    OFDPA_ERROR_t rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize unicast routing flow for masked-match route. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaUnicastRoutingFlowEntry_t));
        
        /* Matches */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4 = ip_dst;
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4Mask = netmask; /* exact IPv4 match */
        
        /*
         * Compute the correct L3 unicast group ID and use in write-actions.
         *
         * The L3 group ID is defined by merely the group type + next hop ID.
         */
        uint32_t group_id;
        if (next_hop != NULL)
        {
            uint32_t group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST;
            
            rc = ofdpaGroupTypeSet(&group_id, group_type); 
            if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to set L3 unicast group type for masked-match route. rc=%d\r\n", rc);
                return -1;
            }
            
            rc = ofdpaGroupIndexSet(&group_id, next_hop->id);
            if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to set L3 unicast group index/ID for masked-match route. rc=%d\r\n", rc);
                return -1;
            }
            
            /* Write Actions Instruction: Group ID should now be correctly set */
            flow.flowData.unicastRoutingFlowEntry.groupID = group_id;
        }
        else
        {
            orc_debug("Using default next hop L3 unicast group for kernel-bound route\r\n");
            flow.flowData.unicastRoutingFlowEntry.groupID = default_next_hop_group_id;
        }
        
        /*
         * Goto Table Instruction: Still need to send here or will drop.
         * Policy ACL will then do the saved write-actions and send to the
         * L3 unicast group.
         */
        flow.flowData.unicastRoutingFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_ACL_POLICY;
        
        rc = ofdpaFlowAdd(&flow);
        if (rc == OFDPA_E_EXISTS)
        {
            orc_warn("Unicast routing flow for masked-match route already exists\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push unicast routing flow for masked-match route. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Added unicast routing flow for masked-match route\r\n");
        }
    }
    
    if (next_hop == NULL)
    {
        /* Do policy ACL if we need to reroute to kernel (from bogus group) */
        rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to initialize policy ACL flow. rc=%d\r\n", rc);
            return -1;
        } else
        {
            memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
            
            /* Matches -- no MAC address; IPv4 w/mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
            flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ip_dst;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = netmask;
            
            /* Apply Actions Instruction */
            flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt up to ORC (will pass into RX thread) */
            
            rc = ofdpaFlowAdd(&flow);
            if (rc == OFDPA_E_EXISTS)
            {
                orc_warn("Policy ACL flow for masked-match, kernel-bound route already exists\r\n");
            }
            else if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to add policy ACL flow for masked-match, kernel-bound route. rc=%d\r\n", rc);
                return -1;
            }
            {
                orc_warn("Added policy ACL flow for masked-match, kernel-bound route\r\n");
            }
        }
    }
    else
    {
        /* DELETE existing kernel-bound flow for this same exact IP (remove old next hop) */
        rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to initialize policy ACL flow. rc=%d\r\n", rc);
            return -1;
        } else
        {
            memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
            
            /* Matches -- no MAC address; IPv4 w/mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
            flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ip_dst;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = netmask;
            
            /* Apply Actions Instruction */
            flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt up to ORC (will pass into RX thread) */
            
            rc = ofdpaFlowDelete(&flow);
            if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
            {
                orc_warn("Policy ACL flow for masked-match, kernel-bound route not found\r\n");
            }
            else if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to delete policy ACL flow for masked-match, kernel-bound route. rc=%d\r\n", rc);
                return -1;
            }
            else
            {
                orc_warn("Deleted policy ACL flow for masked-match, kernel-bound route\r\n");
            }
        }
        
        /* Now add in place the new next hop that avoids the kernel */
        rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to initialize hardware-routing policy ACL flow. rc=%d\r\n", rc);
            return -1;
        } else
        {
            memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
            
            flow.priority = 100;
            
            /* Matches -- no MAC address; IPv4 w/mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
            flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ip_dst;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = netmask;
            
            /* NO Apply Actions Instruction */
            
            rc = ofdpaFlowAdd(&flow);
            if (rc == OFDPA_E_EXISTS)
            {
                orc_warn("Policy ACL flow for hardware-routing route already exists\r\n");
            }
            else if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to add policy ACL flow for hardware-routing route. rc=%d\r\n", rc);
                return -1;
            }
            {
                orc_warn("Added policy ACL flow for hardware-routing route\r\n");
            }
        }
    }
    
    return 0;
}

/******
 * ofdpa_del_l3_v4_route: delete an existing CIDR route. The next hop should be
 * preserved, since other routes might rely on it.
 */
int ofdpa_del_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id) {
    ofdpa_next_hop_t * next_hop = get_next_hop(l3_next_hop_id);
    if (next_hop == NULL && l3_next_hop_id == NEXT_HOP_KERNEL)
    {
        orc_warn("Deleting route with next hop %d to kernel.\r\n", l3_next_hop_id);
    }
    else if (next_hop == NULL)
    {
        orc_err("Could not find next hop %d. Not deleting route.\r\n", l3_next_hop_id);
        return -1;
    }
    else
    {
        orc_warn("Found next hop %d. Proceeding to delete route\r\n", next_hop->id);
    }
    
    ofdpaFlowEntry_t flow;
    OFDPA_ERROR_t rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize unicast routing flow for masked-match route. rc=%d\r\n", rc);
        return -1;
    } else
    {
        memset(&(flow.flowData), 0, sizeof(ofdpaUnicastRoutingFlowEntry_t));
        
        /* Matches */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4 = ip_dst;
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4Mask = netmask; /* exact IPv4 match */
        
        uint32_t group_id;
        if (next_hop != NULL)
        {
            uint32_t group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST;
            
            rc = ofdpaGroupTypeSet(&group_id, group_type); 
            if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to set L3 unicast group type for masked-match route. rc=%d\r\n", rc);
                return -1;
            }
            
            rc = ofdpaGroupIndexSet(&group_id, next_hop->id);
            if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to set L3 unicast group index/ID for masked-match route. rc=%d\r\n", rc);
                return -1;
            }
            
            /* Write Actions Instruction: Group ID should now be correctly set */
            flow.flowData.unicastRoutingFlowEntry.groupID = group_id;
        }
        else
        {
            orc_debug("Using default next hop L3 unicast group for kernel-bound route\r\n");
            flow.flowData.unicastRoutingFlowEntry.groupID = default_next_hop_group_id;
        }
        
        /*
         * Goto Table Instruction: Still need to send here or will drop.
         * Policy ACL will then do the saved write-actions and send to the
         * L3 unicast group.
         */
        flow.flowData.unicastRoutingFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_ACL_POLICY;
        
        rc = ofdpaFlowDelete(&flow);
        if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
        {
            orc_warn("Unicast routing flow for masked-match, kernel-bound route not found\r\n");
        }
        else if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete unicast routing flow for masked-match, kernel-bound route. rc=%d\r\n", rc);
            return -1;
        }
        else
        {
            orc_warn("Unicast routing flow for masked-match, kernel-bound route deleted\r\n");
        }
    }
    
    if (next_hop == NULL)
    {
        /* Do policy ACL if we need to reroute to kernel (from bogus group) */
        rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to initialize policy ACL flow. rc=%d\r\n", rc);
            return -1;
        } else
        {
            memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
            
            /* Matches -- no MAC address; IPv4 w/mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
            flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ip_dst;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = netmask;
            
            /* Apply Actions Instruction */
            flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt up to ORC (will pass into RX thread) */
            
            rc = ofdpaFlowDelete(&flow);
            if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
            {
                orc_warn("Policy ACL flow for masked-match, kernel-bound route not found\r\n");
            }
            else if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to delete policy ACL flow for masked-match, kernel-bound route. rc=%d\r\n", rc);
                return -1;
            }
            else
            {
                orc_warn("Deleted policy ACL flow for masked-match, kernel-bound route\r\n");
            }
        }
    }
    else
    {
        rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to initialize hardware-routing policy ACL flow. rc=%d\r\n", rc);
            return -1;
        } else
        {
            memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));
            
            flow.priority = 100;
            
            /* Matches -- no MAC address; IPv4 w/mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPort = OFDPA_PORT_TYPE_PHYSICAL; /* Must explicitly specify ANY physical port + type_mask */
            flow.flowData.policyAclFlowEntry.match_criteria.inPortMask = OFDPA_INPORT_TYPE_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
            flow.flowData.policyAclFlowEntry.match_criteria.etherTypeMask = OFDPA_ETHERTYPE_EXACT_MASK;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ip_dst;
            flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = netmask;
            
            /* NO Apply Actions Instruction */
            
            rc = ofdpaFlowDelete(&flow);
            if (rc == OFDPA_E_NOT_FOUND || rc == OFDPA_E_EMPTY)
            {
                orc_warn("Policy ACL flow for hardware-routing route not found\r\n");
            }
            else if (rc != OFDPA_E_NONE)
            {
                orc_err("Failed to delete policy ACL flow for hardware-routing route. rc=%d\r\n", rc);
                return -1;
            }
            {
                orc_warn("Deleted policy ACL flow for hardware-routing route\r\n");
            }
        }
    }
    
    return 0;
}

/******
 * ofdpa_log_debug_info
 */
void ofdpa_log_debug_info() {
    orc_warn("Setting debug level\r\n"); //TODO
    ofdpaDebugLvl(3);
    return;
}

/*****
 * Actual hooks into this driver; DRIVER_HOOKS
 * is the symbol that the main program looks for
 */
orc_driver_t DRIVER_HOOKS = {
    .init_driver = ofdpa_init_driver,
    .discover_ports = ofdpa_discover_ports,
    .tx_pkt = ofdpa_tx_pkt,
    .start_rx = ofdpa_start_rx,
    .stop_rx = ofdpa_stop_rx,
    /* TODO raw_port_enable/disable? */
    .add_l3_v4_interface = ofdpa_add_l3_v4_interface,
    .update_l3_v4_interface = ofdpa_update_l3_v4_interface,
    .del_l3_interface = ofdpa_del_l3_v4_interface,
    .add_l3_v4_next_hop = ofdpa_add_l3_v4_next_hop,
    .del_l3_next_hop = ofdpa_del_l3_v4_next_hop, /* why _v4 missing? */
    .add_l3_v4_route = ofdpa_add_l3_v4_route,
    .del_l3_v4_route = ofdpa_del_l3_v4_route,
    .log_debug_info = ofdpa_log_debug_info
};

/********************
 * Helper functions *
 ********************/

/******
 * Determine the number of ports exposed through ofdpa.
 */
int get_num_ofdpa_ports() {
    /*
     * OpenFlow defines ports from 1+ (including special ports).
     * Starting at 1 should return 2 as the next port (if present).
     */
    uint32_t currPort = 0;
    uint32_t nextPort = 0;
    
    int ports = 0;
    OFDPA_ERROR_t rc;
    
    int iter = 0;
    while (ofdpaPortNextGet(currPort, &nextPort) == OFDPA_E_RPC)
    {
        orc_warn("Waiting for OF-DPA RPC server to respond while getting number of ports. Wait time: %ds\r\n", iter);
        usleep(1000000);
        iter++;
    }
    nextPort = 0; /* reset next port */
    do
    {
        rc = ofdpaPortNextGet(currPort, &nextPort);
        orc_debug("Got rc=%d fetching next port from current port %d\r\n", rc, currPort);
        currPort = nextPort;
        if (rc == OFDPA_E_NONE)
        {
            ports++;
        }
    }
    while (rc == OFDPA_E_NONE);
    
    orc_debug("Got %d ports returned from ofdpa\r\n", ports);
    return ports;
}

/******
 * port_rx_monitor: check ofdpa for port RX events and push the
 * packet received to the corresponding orc FD
 */
void * port_rx_monitor(void * args) {
    ofdpaPacket_t * pkt = malloc(sizeof(ofdpaPacket_t));
    char * buf = (char *) malloc(JUMBO_FRAME_BYTES);
    
    if (pkt == NULL || buf == NULL)
    {
        orc_fatal("Malloc failed during packet RX\r\n");
    }
    while(1) {
        //memset(buf, 0, JUMBO_FRAME_BYTES); /* TODO this probably isn't necessary; just changing the size to zero should do it */
        pkt->pktData.pstart = buf;
        pkt->pktData.size = 0;
        
        OFDPA_ERROR_t rc = ofdpaPktReceive(NULL /* no timeout -- block */, pkt);
        if (rc != OFDPA_E_NONE) {
            orc_err("Waiting on packet RX returned error rc=%d\r\n", rc);
            break;
        }
        
        /* We have a packet if we get here */
        sem_wait(&rx_lock);
        int i;
        for (i = 0; i < MAX_PORTS; i++)
        {
            /*
             * If the ingress port of this packet is registered
             * to receive RX events, dispatch the message; else,
             * ignore the packet from the unregistered port.
             */
            if (ofdpa_ports[i] != NULL &&
                ofdpa_ports[i]->port.index == pkt->inPortNum &&
                ofdpa_ports[i]->rx > 0
                )
            {
                /*
                 * Remove the VLAN tag from the packet prior to giving to kernel
                 */
                memmove(pkt->pktData.pstart + ETH_HEADER_VLAN_TAG_BYTES, pkt->pktData.pstart, ETH_HEADER_BEFORE_VLAN_TAG_BYTES);
                pkt->pktData.size = pkt->pktData.size - ETH_HEADER_VLAN_TAG_BYTES; /* minus VLAN tag */
                pkt->pktData.size = pkt->pktData.size - sizeof(uint32_t); /* minus removed CRC */
                
                int rc = write(ofdpa_ports[i]->port.fd, pkt->pktData.pstart + ETH_HEADER_VLAN_TAG_BYTES, pkt->pktData.size);
                orc_debug("Writing packet to kernel from port %d\r\n", ofdpa_ports[i]->port.index);
                
                if (rc < 0)
                {
                    orc_err("Write to FD failed during packet RX\r\n");
                }
                /* We found the port that we received the packet on, so we're done */
                break;
            } else if (ofdpa_ports[i] == NULL)
            {
                /* We've reached the last port */
                orc_debug("Reached last port in rx monitor thread. Index into port list is %d\r\n", i);
                break;
            } /* else keep looping and searching */
            else {
                orc_debug("Not sending packet up to kernel from port %d / index %d. RX flag set to %d.\r\n", pkt->inPortNum, ofdpa_ports[i]->port.index, ofdpa_ports[i]->rx);
            }
        }
        
        sem_post(&rx_lock);
    }
    
    free(pkt);
    free(buf);
    return NULL;
}

/******
 * free_ports: clean up any memory allocated for ports
 */
void free_ports() {
    int i;
    for (i = 0; i < MAX_PORTS; i++)
    {
        orc_trace("Freeing memory for port %d\r\n", ofdpa_ports[i]->port.index);
        free(ofdpa_ports[i]);
        ofdpa_ports[i] = NULL; /* NULL for reuse/detection of end of ports */
    }
    orc_debug("Freed memory for ofdpa_port_t's\r\n");
}

/******
 * add_interface: add an interface from the list of interfaces
 */
void add_interface(ofdpa_interface_t * iface) {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        if (ofdpa_interfaces[i] == NULL)
        {
            ofdpa_interfaces[i] = iface;
            orc_warn("Adding interface %d\r\n", iface->id);
            return;
        }
    }
    orc_fatal("Ran out of space in the interface list. Could not add interface %d\r\n", iface->id);
    return;
}

/******
 * get_interface
 */
ofdpa_interface_t * get_interface(l3_intf_id_t id) {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        if (ofdpa_interfaces[i] != NULL && ofdpa_interfaces[i]->id == id)
        {
            orc_debug("Found interface %d. Retrieving it.\r\n", id);
            return ofdpa_interfaces[i];
        }
    }
    orc_warn("Could not find interface %d to retrieve\r\n", id);
    return NULL;
}

/******
 * del_interface
 */
void del_interface(l3_intf_id_t id) {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        if (ofdpa_interfaces[i] != NULL && ofdpa_interfaces[i]->id == id)
        {
            orc_warn("Removing/freeing interface %d\r\n", id);
            free(ofdpa_interfaces[i]);
            ofdpa_interfaces[i] = NULL; /* reset */
            return;
        }
    }
    orc_warn("Could not find interface %d to remove\r\n", id);
    return;
}

/******
 * del_next_hop_from_interfaces: locate and remove (set NULL) next hop
 * from any interfaces that have it as their next hop.
 */
void del_next_hop_from_interfaces(ofdpa_next_hop_t * next_hop) {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        if (ofdpa_interfaces[i] != NULL && ofdpa_interfaces[i]->next_hop == next_hop) /* check if they point to the same place (shallow) */
        {
            orc_warn("Removing next hop %d from interface %d\r\n", next_hop->id, ofdpa_interfaces[i]->id);
            ofdpa_interfaces[i]->next_hop = NULL; /* reset */
        }
    }
    return;
}

/******
 * free_interfaces
 */
void free_interfaces() {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        orc_trace("Removing/freeing interface %d\r\n", ofdpa_interfaces[i]->id);
        free(ofdpa_interfaces[i]);
        ofdpa_interfaces[i] = NULL; /* reset */
    }
    orc_warn("Freed %d interfaces\r\n", i);
    return;
}

/******
 * add_next_hop: add a next hop to the list of next hops
 */
void add_next_hop(ofdpa_next_hop_t * next_hop) {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        if (ofdpa_next_hops[i] == NULL)
        {
            ofdpa_next_hops[i] = next_hop;
            orc_warn("Adding next hop %d\r\n", next_hop->id);
            return;
        }
    }
    orc_fatal("Ran out of space in next hop list. Could not add next hop %d\r\n", next_hop->id);
    return;
}

/******
 * get_next_hop
 */
ofdpa_next_hop_t * get_next_hop(l3_next_hop_id_t id) {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        if (ofdpa_next_hops[i] != NULL && ofdpa_next_hops[i]->id == id)
        {
            orc_debug("Got next hop %d. Retrieving it.\r\n", id);
            return ofdpa_next_hops[i];
        }
    }
    orc_warn("Could not find next hop %d to retrieve\r\n", id);
    return NULL;
}

/******
 * del_next_hop
 */
void del_next_hop(l3_next_hop_id_t id) {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        if (ofdpa_next_hops[i] != NULL && ofdpa_next_hops[i]->id == id)
        {
            orc_warn("Removing/freeing next hop %d\r\n", id);
            /* Remove any pointers that will break after freeing */
            del_next_hop_from_interfaces(ofdpa_next_hops[i]);
            free(ofdpa_next_hops[i]);
            ofdpa_next_hops[i] = NULL; /* reset */
            return;
        }
    }
    orc_warn("Could not find next hop %d to remove\r\n", id);
    return;
}

/******
 * free_next_hops
 */
void free_next_hops() {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        orc_trace("Removing/freeing next_hop %d\r\n", ofdpa_next_hops[i]->id);
        free(ofdpa_next_hops[i]);
        ofdpa_next_hops[i] = NULL; /* reset */
    }
    orc_warn("Freed %d next_hops\r\n", i);
    return;
}

/******
 * compute_vlan_id: from an ofdpa port index, determine the correct
 * VLAN ID to associate/assign to the port. The algorithm at present
 * uses the port index + 100.
 */
uint16_t compute_vlan_id(uint32_t port) {
    return (uint16_t) (port + 100);
}

/******
 * Generate unique l3_intf_id_t. Note that l3_intf_id_t is just an int (i.e. so we can add to it).
 */
l3_intf_id_t generate_l3_intf_id() {
    static l3_intf_id_t l3_intf_id; /* initially 0 */
    l3_intf_id_t tmp;
    
    sem_wait(&l3_intf_id_lock);
    l3_intf_id = l3_intf_id + 1;
    tmp = l3_intf_id;
    sem_post(&l3_intf_id_lock);
    
    orc_trace("Generated unique l3_intf_id_t of %d\r\n", tmp);
    return tmp;
}

/******
 * Generate unique l3_next_hop_id_t. Note that l3_next_hop_id_t is just an int (i.e. so we can add to it).
 */
l3_next_hop_id_t generate_l3_next_hop_id() {
    static l3_next_hop_id_t next_hop_id; /* initially 0 */
    l3_next_hop_id_t tmp;
    
    sem_wait(&next_hop_id_lock);
    next_hop_id = next_hop_id + 1;
    tmp = next_hop_id;
    sem_post(&next_hop_id_lock);
    
    orc_trace("Generated unique l3_next_hop_id_t of %d\r\n", tmp);
    return tmp;
}

/******
 * Clear a specific flow table
 * @return +1 for success; -1 for error
 */
int clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_t table_id)
{
    OFDPA_ERROR_t rc;
    ofdpaFlowEntry_t currentFlow;
    ofdpaFlowEntry_t nextFlow;
    memset(&nextFlow, 0, sizeof(ofdpaFlowEntry_t));
    memset(&currentFlow, 0, sizeof(ofdpaFlowEntry_t));
    
    currentFlow.tableId = table_id;
    
    while (1)
    {
        rc = ofdpaFlowNextGet(&currentFlow, &nextFlow);
        if (rc != OFDPA_E_NONE)
        {
            orc_warn("Reached end of flow table. No more flows to delete in table %d.\r\n", table_id);
            break;
        }
        
        memcpy(&currentFlow, &nextFlow, sizeof(ofdpaFlowEntry_t));
        
        rc = ofdpaFlowDelete(&currentFlow);
        if (rc != OFDPA_E_NONE)
        {
            orc_debug("Failed to delete flow from table %d. rc=%d\r\n", table_id, rc);
            return -1;
        }
        else {
            orc_warn("Deleted flow from table %d\r\n", table_id);
        }
    }
    
    return 1; /* positive 1 is success */
}

/******
 * Delete all flows and groups
 */
int clear_ofdpa_tables() {
    OFDPA_ERROR_t rc;
    uint32_t group_id = 0;
    
    if (!clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_INGRESS_PORT)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_PORT_DSCP_TRUST)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_PORT_PCP_TRUST)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_TUNNEL_DSCP_TRUST)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_TUNNEL_PCP_TRUST)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_VLAN)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_VLAN_1)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MAINTENANCE_POINT)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_L2_PORT)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_DSCP_TRUST)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_PCP_TRUST)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_TERMINATION_MAC)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_0)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_1)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_2)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_MAINTENANCE_POINT)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_BFD)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MULTICAST_ROUTING)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_BRIDGING)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_ACL_POLICY)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_EGRESS_VLAN)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_EGRESS_VLAN_1)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_EGRESS_MAINTENANCE_POINT)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_L2_INTF_DSCP_REMARK)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_QOS)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_VPN_LBL_EXP_REMARK)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_VPN_LBL_8021P_PRI_REMARK)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_TUNNEL_LBL_EXP_REMARK)
        || !clear_ofdpa_flow_table(OFDPA_FLOW_TABLE_ID_MPLS_TUNNEL_LBL_8021P_PRI_REMARK))
    {
        return -1;
    }
    
    /*
     * Remove all L3 groups first, since they depend on L2 groups,
     * and we cannot leave behind a reference to a group that no
     * longer exists.
     */
    while (1)
    {
        ofdpaGroupEntry_t group;
        rc = ofdpaGroupTypeNextGet(group_id, OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST, &group);
        if (rc != OFDPA_E_NONE)
        {
            orc_warn("Reached end of L3 unicast group table. No more groups to delete after %#010x.\r\n", group_id);
            break;
        }
        
        group_id = group.groupId;
        
        rc = ofdpaGroupBucketsDeleteAll(group_id);
        if (rc != OFDPA_E_NONE)
        {
            orc_debug("Failed to delete L3 unicast group %#010x buckets. rc=%d\r\n", group_id, rc);
            return -1;
        }
        else {
            orc_warn("Deleted buckets for L3 unicast group %#010x\r\n", group_id);
        }
        
        rc = ofdpaGroupDelete(group_id);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete L3 unicast group %#010x. rc=%d\r\n", group_id, rc);
            return -1;
        }
        else
        {
            orc_warn("Deleted L3 unicast group %#010x\r\n", group_id);
        }
    }
    
    /*
     * All groups that depend on L2 Interface are removed.
     * Now we can remove L2 Interface groups w/o error.
     */
    group_id = 0;
    while (1)
    {
        ofdpaGroupEntry_t group;
        rc = ofdpaGroupTypeNextGet(group_id, OFDPA_GROUP_ENTRY_TYPE_L2_INTERFACE, &group);
        if (rc != OFDPA_E_NONE)
        {
            orc_warn("Reached end of L2 interface group table. No more groups to delete after %#010x.\r\n", group_id);
            break;
        }
        
        group_id = group.groupId;
        
        rc = ofdpaGroupBucketsDeleteAll(group_id);
        if (rc != OFDPA_E_NONE)
        {
            orc_debug("Failed to delete L2 interface group %#010x buckets. rc=%d\r\n", group_id, rc);
            return -1;
        }
        else {
            orc_warn("Deleted buckets for L2 interface group %#010x\r\n", group_id);
        }
        
        rc = ofdpaGroupDelete(group_id);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to delete L2 interface group %#010x. rc=%d\r\n", group_id, rc);
            return -1;
        }
        else
        {
            orc_warn("Deleted L2 interface group %#010x\r\n", group_id);
        }
    }
    return 1;
}

/******
 * MAC to char[]; null terminated
 */
void mac_to_str(char * str, u8 * hw_mac) {
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            hw_mac[0],
            hw_mac[1],
            hw_mac[2],
            hw_mac[3],
            hw_mac[4],
            hw_mac[5]);
}

/******
 * MAC to char[]; null terminated
 */
void ipv4_to_str(char * str, uint32_t ipv4) {
    sprintf(str, "%d.%d.%d.%d",
            ipv4 >> 24 & 0xff,
            ipv4 >> 16 & 0xff,
            ipv4 >> 8 & 0xff,
            ipv4 >> 0 & 0xff);
}