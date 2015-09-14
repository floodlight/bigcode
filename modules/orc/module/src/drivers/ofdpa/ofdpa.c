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
#include <semaphore.h> /* not sure if we're going inter-process or inter-thread, so we'll use sem_t insead of pthread locks */
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
 * Default values for fields we might not
 * care about in a VLAN flow.
 */
#define ACTION 1
#define DEFAULT_ZERO_MAC {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}

/*
 * At present, we just wrap a port_t,
 * but, we could in theory append
 * beyond the intial port_t field to
 * include ofdpa-specific information.
 * The port_t must come first, since
 * the driver user will treat
 * ofdpa_port_t pointers as port_t.
 */
typedef struct ofdpa_port {
    port_t port;
    int rx; /* rx <= 0, don't respond to RX events; else, push packet to orc interface */
} ofdpa_port_t;

/*
 * Struct to define a next hop.
 * Same as above, we need to be
 * able to associate an ID with
 * the MAC used when creating
 * that ID.
 */
typedef struct ofdpa_next_hop {
    l3_next_hop_id_t id;
    ofdpa_port_t * port;
    u8 mac[6];
} ofdpa_next_hop_t;

/*
 * Struct to define an interface.
 * When we return an ID, we need
 * to be able to take that ID later
 * and associate it with the address
 * information.
 */
typedef struct ofdpa_interface {
    l3_intf_id_t id;
    ofdpa_port_t * port;
    u8 mac[6];
    u32 ipv4;
    ofdpa_next_hop_t * next_hop;
} ofdpa_interface_t;

/*
 * Our interfaces.
 * TODO implement dynamic list (w/realloc)
 */
#define MAX_INTERFACES 128
ofdpa_interface_t * ofdpa_interfaces[MAX_INTERFACES];

/*
 * Our next hops.
 * TODO implement dynamic list
 */
#define MAX_NEXT_HOPS 128
ofdpa_next_hop_t * ofdpa_next_hops[MAX_NEXT_HOPS];

/*
 * These are our ports. Global for
 * monitoring by our RX thread. No real
 * need for an expandable list here. Ports
 * are physical with the exception of tunnel
 * ports. If there are a ton of tunnel ports,
 * then I suppose we can look into a dynamic
 * list here too.
 */
#define MAX_PORTS 128
ofdpa_port_t * ofdpa_ports[MAX_PORTS];

/*
 * Maintain a hold on our RX thread
 */
pthread_t rx_thread;
sem_t rx_lock;

/*
 * Unique ID generator locks
 */
sem_t next_hop_id_lock;
sem_t l3_intf_id_lock;

/*
 * TODO need locks for next hop and interface list traversing
 */

/*********
 * Function prototypes
 */
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

/***************
 * ofdpa_init_driver: start up ofdpa
 */
int ofdpa_init_driver(orc_options_t *options, int argc, char * argv[])
{
    /*
     * Our mutexes
     */
    int c;
    if ((c = sem_init(&rx_lock, 1 /* shared b/t threads and processes */, 1 /* count from 1 (i.e. only one at a time) */)) < 0)
    {
        orc_fatal("semaphore init for rx_lock failed: %d", c);
        return -1;
    }
    if ((c = sem_init(&l3_intf_id_lock, 1, 1)) < 0)
    {
        orc_fatal("semaphore init for l3_intf_id_lock failed: %d", c);
        return -1;
    }
    if ((c = sem_init(&next_hop_id_lock, 1, 1)) < 0)
    {
        orc_fatal("semaphore init for next_hop_id_lock failed: %d", c);
        return -1;
    }

    /*
     * First initialize
     */
    OFDPA_ERROR_t rc = ofdpaClientInitialize(CLIENT_NAME);
    if (rc == OFDPA_E_NONE) {
        orc_warn("ofdpa initialized");
    } else {
        orc_fatal("ofdpa initialization returned error %d", rc);
        return -1;
    }

    /*
     * Then, bind packet socket for receiving packets.
     * Only one PID can bind to this, so we'll do it in
     * initialization rather than on demand. (There is no
     * guarantee the same PID will invoke the rx function.)
     */
    rc = ofdpaClientPktSockBind();
    if (rc == OFDPA_E_NONE) {
        orc_warn("ofdpa RX socket bound");
    } else {
        orc_fatal("ofdpa RX socket bind returned error %d", rc);
        return -1;
    }
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

    if ((ofdpaPorts = get_num_ofdpa_ports()) > *num)
    {
        orc_err("Need to pre-allocate more ports: want %d got %d\n",
                        ofdpaPorts, *num);
        return -1;
    }

    /*
     * In case we call this more than once,
     * make sure we aren't RXing now.
     */
    sem_wait(&rx_lock);

    /* Clear all driver-local ports */
    free_ports();

    /* Now reallocate */
    for (i = 0; i < ofdpaPorts; i++)
    {
        ofport = malloc(sizeof(*ofport));
        if (ofport == NULL)
        {
            orc_err("Malloc failed allocating ports\n");
            return -1;
        }
        ofport->rx = 0; /* we don't want to receive packets from this port at present; set in start_rx below */
        ofport->port.index = i+1; /* index starts at 1 */
        ports[i] = (port_t *) ofport; /* init to the array passed in */
        ofdpa_ports[i] = ofport; /* keep a driver-local copy for RX thread */
        orc_debug("Created port %d for port %s\n",
                        ofport->port.index, ofport->port.name);
    }
    *num = ofdpaPorts;

    /*
     * Don't forget to release the lock.
     */
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

    /* Wrap data and size in an ofdpa_buffdesc */
    ofdpa_buffdesc buf;
    buf.size = len;
    buf.pstart = (char *) pkt;
    /* in-port ignored if flag is not set */
    OFDPA_ERROR_t rc = ofdpaPktSend(&buf, 0, port->index, OFDPA_PORT_CONTROLLER);
    if (rc == OFDPA_E_NONE) {
        orc_debug("TX packet out port %s, ID %d returned success code %d", ofdpa->port.name, ofdpa->port.index, rc);
        return 0;
    } else {
        orc_err("TX packet out port %s, ID %d, returned error %d", ofdpa->port.name, ofdpa->port.index, rc);
        return -1;
    }
}

/******
 * ofpda_start_rx: spawn our RX thread that will wait on packet RX events
 * as triggered by ofdpa
 */
int ofdpa_start_rx(port_t * ports[], int num_ports)
{
    /*
     * We bind to the RX socket in init.
     * This is not done here, because we can only
     * do it once whereas this function can in
     * theory be called multiple times for different
     * sets of ports each time. With ofdpa, there is
     * no need to start receiving on a per-port
     * basis. The single RX socket bound in init
     * will cover all the ports.
     *
     * We will however set the rx flag so that we'll
     * know on a packet-in event whether or not we
     * should receive from the port indicated in the
     * packet-in message.
     */
    sem_wait(&rx_lock);

    orc_warn("Setting RX on %d ports", num_ports);

    int i;
    for (i = 0; i < num_ports; i++)
    {
        orc_trace("Starting RX on port %d", ports[i]->index);
        ((ofdpa_port_t *) ports[i])->rx = 1; /* set flag to RX packets on this port */
    }

    if (rx_thread == 0)
    {
        int rc = pthread_create(&rx_thread, NULL /* default attributes */, port_rx_monitor, NULL /* no args to function ptr <-- */);
        if (rc != 0)
        {
            orc_fatal("RX thread creation failed with error code %d", rc);
            return -1;
        }
        orc_warn("Started RX thread");
    }
    sem_post(&rx_lock);

    return 0;
}

/******
 * ofdpa_stop_rx: clean up resources used to RX packets
 */
int ofdpa_stop_rx()
{
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
        orc_warn("Closing RX socket file descriptor: %d", fd); /* seeing the FD probably isn't important, but we'll do it anyway (initially) */
        close(fd);
        return 0;
    } else { /* Just log error so that we can continue killing stuff */
        orc_err("ofdpa did not return a valid file descriptor when attempting to close the RX socket: %d", fd);
        return -1;
    }

    /*
     * Kill thread 
     */
    pthread_cancel(rx_thread);

    /*
     * Now free all the memory we ate
     */
    free_ports();
    free_interfaces();
    free_next_hops();

    /*
     * And don't forget to destroy our locks/semaphores
     */
    sem_destroy(&rx_lock);
    sem_destroy(&l3_intf_id_lock);
    sem_destroy(&next_hop_id_lock);
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
    OFDPA_ERROR_t rc;
    ofdpaFlowEntry_t flow;

    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize VLAN flow. rc=%d", rc);
        return -1;
    } else
    {
        /* 
         * Clear the entire structure to avoid having to set
         * "default" values that we do not care about.
         */
        memset(&(flow.flowData), 0, sizeof(ofdpaVlanFlowEntry_t));

        /*
         * flow instance of ofdpaFlowEntry_t
         * flowData union of flow types
         * vlanFlowEntry instance of ofDpaVlanFlowEntry_t
         * match_criteria instance of ofdpaVlanFlowMatch_t
         */
        
        /* Matches */
        flow.flowData.vlanFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.vlanFlowEntry.match_criteria.vlanId = compute_vlan_id(port->index);
        flow.flowData.vlanFlowEntry.match_criteria.vlanIdMask = 0x1fff;

        /* Goto Table Instruction */
        flow.flowData.vlanFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_TERMINATION_MAC;
        
        /* Apply Actions Instruction */
        flow.flowData.vlanFlowEntry.setVlanIdAction = ACTION;
        flow.flowData.vlanFlowEntry.newVlanId = compute_vlan_id(port->index);

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push VLAN flow. rc=%d", rc);
            return -1;
        }
    }

    /* Now do the termination MAC flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize termination MAC flow. rc=%d", rc);
        return -1;
    } else
    {
        /* 
         * Clear the entire structure to avoid having to set
         * "default" values that we do not care about.
         */
        memset(&(flow.flowData), 0, sizeof(ofdpaTerminationMacFlowEntry_t));

        /* Matches */
        flow.flowData.terminationMacFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.terminationMacFlowEntry.match_criteria.inPortMask = 0xffff;
        flow.flowData.terminationMacFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        memcpy(&(flow.flowData.terminationMacFlowEntry.match_criteria.destMac), &hw_mac, 6);
        memset(&(flow.flowData.terminationMacFlowEntry.match_criteria.destMacMask), 0xff, 6);

        /* Goto Table Instruction */
        flow.flowData.terminationMacFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING;

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push termination MAC flow. rc=%d", rc);
            return -1;
        }
    }
    
    /* Now do the unicast routing flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize unicast routing flow. rc=%d", rc);
        return -1;
    } else
    {
        /* 
         * Clear the entire structure to avoid having to set
         * "default" values that we do not care about.
         */
        memset(&(flow.flowData), 0, sizeof(ofdpaUnicastRoutingFlowEntry_t));

        /* Matches */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4 = ipv4_addr;
        flow.flowData.unicastRoutingFlowEntry.match_criteria.dstIp4Mask = 0xffFFffFF; /* exact IPv4 match */
        
        /*
         * Note we do not specify an L3 group as a write-actions. This is
         * because we're matching on the router's IP. In the next table
         * (policy ACL), we'll perform the match again and punt it up to
         * the CONTROLLER. For some reason we can't send to controller
         * here unless the TTL expires... Why? (We can in bridging.)
         */

        /* Goto Table Instruction */
        flow.flowData.unicastRoutingFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_ACL_POLICY;

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push unicast routing flow. rc=%d", rc);
            return -1;
        }
    }
    
    /* Now do the policy ACL flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_ACL_POLICY, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize policy ACL flow. rc=%d", rc);
        return -1;
    } else
    {
        /* 
         * Clear the entire structure to avoid having to set
         * "default" values that we do not care about.
         */
        memset(&(flow.flowData), 0, sizeof(ofdpaPolicyAclFlowEntry_t));

        /* Matches */
        memcpy(&(flow.flowData.policyAclFlowEntry.match_criteria.destMac), &hw_mac, 6);
        memset(&(flow.flowData.policyAclFlowEntry.match_criteria.destMacMask), 0xff, 6); /* exact MAC match -- TODO might be redundant? */
        flow.flowData.policyAclFlowEntry.match_criteria.etherType = 0x0800; /* we assume IPv4 */
        flow.flowData.policyAclFlowEntry.match_criteria.destIp4 = ipv4_addr;
        flow.flowData.policyAclFlowEntry.match_criteria.destIp4Mask = 0xffFFffFF; /* exact IPv4 match */

        /* Apply Actions Instruction */
        flow.flowData.policyAclFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt up to ORC (will pass into RX thread) */

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push policy ACL flow. rc=%d", rc);
            return -1;
        }
    }

    /* Now do the bridging flow -- for non-IP packet delivery to ORC's RX func */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_BRIDGING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize bridging flow. rc=%d", rc);
        return -1;
    } else
    {
        /* 
         * Clear the entire structure to avoid having to set
         * "default" values that we do not care about.
         */
        memset(&(flow.flowData), 0, sizeof(ofdpaBridgingFlowEntry_t));

        /* Matches */
        memcpy(&(flow.flowData.bridgingFlowEntry.match_criteria.destMac), &hw_mac, 6);
        memset(&(flow.flowData.bridgingFlowEntry.match_criteria.destMacMask), 0xff, 6); /* exact MAC match */
        
        /* Apply Actions Instruction */
        flow.flowData.bridgingFlowEntry.outputPort = OFDPA_PORT_CONTROLLER; /* punt non-IP packets up to ORC's RX func */

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push bridging flow. rc=%d", rc);
            return -1;
        }
    }

    /*
     * If we get here, then all flows should be inserted w/o error
     */
    *l3_intf_id = generate_l3_intf_id();
    ofdpa_interface_t * iface = (ofdpa_interface_t *) malloc(sizeof(ofdpa_interface_t));
    if (iface == NULL)
    {
        orc_fatal("Mallocing ofdpa_interface_t failed");
        return -1;
    } else
    {
        /*
         * init interface structure
         */
        iface->id = *l3_intf_id;
        iface->port = (ofdpa_port_t *) port; /* pointer already */
        memcpy(&(iface->mac), &hw_mac, 6);
        iface->ipv4 = ipv4_addr;
        iface->next_hop = NULL;
        add_interface(iface); /* insert into the list */
    }

    return 0;
}

/******
 * ofdpa_update_l3_v4_interface: modify an existing IPv4 interface
 */
int ofdpa_update_l3_v4_interface(port_t * port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t l3_intf_id) {

    return 0;
}

/******
 * ofdpa_del_l3_v4_interface: delete an existing IPv4 interface
 */
int ofdpa_del_l3_v4_interface(port_t * port, l3_intf_id_t l3_intf_id) {

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
        orc_err("Could not lookup interface ID %d. Not adding next hop.", l3_intf_id);
        return -1;
    }
    else if (iface->next_hop != NULL)
    {
        orc_err("Tried adding a next hop to an interface that already has a next hop. Not adding next hop. Remove the existing next hop %d first.", iface->next_hop->id);
        return -1;
    }

    ofdpa_next_hop_t * next_hop = (ofdpa_next_hop_t *) malloc(sizeof(ofdpa_next_hop_t));
    if (next_hop == NULL)
    {
        orc_fatal("ofdpa_next_hop_t malloc failed");
        return -1;
    }

    /* Fill in our next hop */
    *l3_next_hop_id = generate_l3_next_hop_id(); /* ID is guaranteed to be unique */
    next_hop->id = *l3_next_hop_id;
    iface->next_hop = next_hop; /* Give interface access to next hop info */
    /*
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
    memcpy(&(next_hop->mac), next_hop_hw_mac, 6); /* Last but not least, the MAC itself */
    
    /* Add to our internal list of next hops */
    add_next_hop(next_hop);

    /*
     * Insert L3 Unicast group entry AND L2 Interface group entry
     * on port of the next hop.
     */
    ofdpaGroupEntry_t group;
    ofdpaGroupBucketEntry_t group_bucket;
    uint32_t group_id; /* used for both */
    uint32_t l2_group_id; /* save L2 Interface here to reference in L3 group */
    uint32_t port_id; /* only for L2 Interface */
    uint32_t vlan_id; /* ditto */
    uint32_t group_type;

    /* 
     * First do the L2 Interface group, since we need to reference it
     * in the L3 Unicast group as a bucket.
     */
    port_id = port->index;
    vlan_id = compute_vlan_id(port->index);
    group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L2_INTERFACE;
    
    OFDPA_ERROR_t rc = ofdpaGroupTypeSet(&group_id, group_type); /* encodes type in ID */
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group type. rc=%d", rc);
        return -1;
    }

    rc = ofdpaGroupVlanSet(&group_id, vlan_id); /* encodes VLAN in ID */
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group VLAN. rc=%d", rc);
        return -1;
    }

    rc = ofdpaGroupPortIdSet(&group_id, port_id); /* encodes port in ID */
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L2 interface group port. rc=%d", rc);
        return -1;
    }

    rc = ofdpaGroupEntryInit(group_type, &group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L2 interface group. rc=%d", rc);
        return -1;
    }

    group.groupId = group_id; /* this is the only thing to set */
    l2_group_id = group_id; /* save for reference by L3 group below */    
    rc = ofdpaGroupAdd(&group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to push L2 interface group. rc=%d", rc);
        return -1;
    }
    orc_warn("L2 interface group %d added", group_id);

    /* Add the bucket */
    memset(&group_bucket, 0, sizeof(ofdpaGroupBucketEntry_t));
    rc = ofdpaGroupBucketEntryInit(group_type, &group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L2 interface bucket. rc=%d", rc);
        return -1;
    }
    
    group_bucket.groupId = group_id;
    group_bucket.bucketIndex = 0; /* the only bucket, so 0 */
    group_bucket.bucketData.l2Interface.outputPort = port_id; /* union, just like the flows; only need output port here */

    rc = ofdpaGroupBucketEntryAdd(&group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to add L2 interface bucket. rc=%d", rc);
        return -1;
    }
    orc_warn("L2 interface group %d bucket added out port %d", group_id, port_id);

    /*
     * Now, we can do the L3 Unicast group.
     */
    memset(&group, 0, sizeof(ofdpaGroupEntry_t));
    group_id = 0;
    group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST;
    
    rc = ofdpaGroupTypeSet(&group_id, group_type); /* encodes type in ID */
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L3 unicast group type. rc=%d", rc);
        return -1;
    }

    rc = ofdpaGroupIndexSet(&group_id, next_hop->id); /* encodes index (next hop ID) in ID */
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to set L3 unicast group index/ID. rc=%d", rc);
        return -1;
    }

    rc = ofdpaGroupEntryInit(group_type, &group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L3 unicast group. rc=%d", rc);
        return -1;
    }

    /* Now we're finally ready to add the group */
    group.groupId = group_id; /* this is the only thing to set */
    rc = ofdpaGroupAdd(&group);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to push L3 unicast group. rc=%d", rc);
        return -1;
    }
    orc_warn("L3 unicast group entry %d added.", group_id);

    /* Add the bucket */
    memset(&group_bucket, 0, sizeof(ofdpaGroupBucketEntry_t));
    rc = ofdpaGroupBucketEntryInit(group_type, &group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize L3 unicast bucket. rc=%d", rc);
        return -1;
    }
    
    group_bucket.groupId = group_id;
    group_bucket.referenceGroupId = l2_group_id; /* retrieve saved ID from L2 group inserted above */
    group_bucket.bucketIndex = 0; /* the only bucket, so 0 */
    group_bucket.bucketData.l3Unicast.vlanId = vlan_id; /* set VLAN to VLAN out next hop's port */
    memcpy(&(group_bucket.bucketData.l3Unicast.srcMac), &(iface->mac), 6); /* source is the switch port MAC */
    memcpy(&(group_bucket.bucketData.l3Unicast.dstMac), &(next_hop->mac), 6); /* dest is the next hop MAC */

    rc = ofdpaGroupBucketEntryAdd(&group_bucket);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to add L3 unicast bucket. rc=%d", rc);
        return -1;
    }
    orc_warn("L3 unicast group %d bucket added out port %d, VLAN %d", group_id, port_id, vlan_id);

    return 0;
}

/******
 * ofdpa_del_l3_v4_next_hop: delete a next hop entry
 */
int ofdpa_del_l3_v4_next_hop(l3_next_hop_id_t l3_next_hop_id) {

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
    if (next_hop == NULL)
    {
        orc_err("Could not find next hop %d. Not adding route.", l3_next_hop_id);
        return -1;
    }

    ofdpaFlowEntry_t flow;
    OFDPA_ERROR_t rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_UNICAST_ROUTING, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_err("Failed to initialize unicast routing flow for masked-match route. rc=%d", rc);
        return -1;
    } else
    {
        /* 
         * Clear the entire structure to avoid having to set
         * "default" values that we do not care about.
         */
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
        uint32_t group_type = (uint32_t) OFDPA_GROUP_ENTRY_TYPE_L3_UNICAST;
    
        rc = ofdpaGroupTypeSet(&group_id, group_type); /* encodes type in ID */
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to set L3 unicast group type for masked-match route. rc=%d", rc);
            return -1;
        }

        rc = ofdpaGroupIndexSet(&group_id, next_hop->id); /* encodes index (next hop ID) in ID */
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to set L3 unicast group index/ID for masked-match route. rc=%d", rc);
            return -1;
        }

        /* Write Actions Instruction: Group ID should now be correctly set */
        flow.flowData.unicastRoutingFlowEntry.groupID = group_id;

        /* 
         * Goto Table Instruction: Still need to send here or will drop. 
         * Policy ACL will then do the saved write-actions and send to the
         * L3 unicast group.
         */
        flow.flowData.unicastRoutingFlowEntry.gotoTableId = OFDPA_FLOW_TABLE_ID_ACL_POLICY;

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(&flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_err("Failed to push unicast routing flow for masked-match route. rc=%d", rc);
            return -1;
        }
    }

    return 0;
}

/******
 * ofdpa_del_l3_v4_route: delete an existing CIDR route
 */
int ofdpa_del_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id) {

    return 0;
}

/******
 * ofdpa_log_debug_info
 */
void ofdpa_log_debug_info() {
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
    uint32_t currPort = 1;

    int ports = 0;
    while (ofdpaPortNextGet(currPort, &currPort) == OFDPA_E_NONE)
    {
        ports++;
    }

    orc_debug("Got %d ports returned from ofdpa", ports);
    return ports;
}

/******
 * port_rx_monitor: check ofdpa for port RX events and push the
 * packet received to the corresponding orc FD
 */
void * port_rx_monitor(void * args) {
    while(1) {
        ofdpaPacket_t * pkt = malloc(sizeof(ofdpaPacket_t));
        if (pkt == NULL)
        {
            orc_fatal("Malloc failed during packet RX");
        }
        ofdpaPktReceive(NULL /* no timeout -- block */, pkt);

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
                int rc = write(ofdpa_ports[i]->port.fd, pkt->pktData.pstart, pkt->pktData.size);
                orc_debug("Writing packet out port %d", ofdpa_ports[i]->port.index);

                /* Don't forget to free the packet we malloced -- we can do this after write() */
                free(pkt);

                if (rc < 0)
                {
                    orc_err("Write to FD failed during packet RX");
                }
                /* We found the port that we received the packet on, so we're done */
                break;
            } else if (ofdpa_ports[i] == NULL)
            {
                /* We've reached the last port */
                orc_trace("Reached last port in rx monitor thread. Index into port list is %d", i);
                break;
            } /* else keep looping and searching */
            else {
                orc_trace("Not sending packet out for port %d. RX flag set to %d", ofdpa_ports[i]->port.index, ofdpa_ports[i]->rx);
            }
        }
        sem_post(&rx_lock);
    }
}

/******
 * free_ports: clean up any memory allocated for ports
 */
void free_ports() {
    /* Clear all driver-local ports */
    int i;
    for (i = 0; i < MAX_PORTS; i++)
    {
        orc_trace("Freeing memory for port %d", ofdpa_ports[i]->port.index);
        free(ofdpa_ports[i]);
        ofdpa_ports[i] = NULL; /* NULL for reuse/detection of end of ports */
    }
    orc_debug("Freed memory for ofdpa_port_t's");
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
            orc_warn("Adding interface %d", iface->id);
            return;
        }
    }
    orc_fatal("Ran out of space in the interface list. Could not add interface %d", iface->id);
    return;
}

/******
 * get_interface
 */
ofdpa_interface_t * get_interface(l3_intf_id_t id) {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        if (ofdpa_interfaces[i]->id == id)
        {
            orc_debug("Found interface %d. Retrieving it.", id);
            return ofdpa_interfaces[i];
        }
    }
    orc_err("Could not find interface %d to retrieve", id);
    return NULL;
}

/******
 * del_interface
 */
void del_interface(l3_intf_id_t id) {
    int i;
    for (i = 0; i < MAX_INTERFACES; i++)
    {
        if (ofdpa_interfaces[i]->id == id)
        {
            orc_warn("Removing/freeing interface %d", id);
            free(ofdpa_interfaces[i]);
            ofdpa_interfaces[i] = NULL; /* reset */
            return;
        }
    }
    orc_err("Could not find interface %d to remove", id);
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
        if (ofdpa_interfaces[i]->next_hop == next_hop) /* check if they point to the same place (shallow) */
        {
            orc_warn("Removing next hop %d from interface %d", next_hop->id, ofdpa_interfaces[i]->id);
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
        orc_trace("Removing/freeing interface %d", ofdpa_interfaces[i]->id);
        free(ofdpa_interfaces[i]);
        ofdpa_interfaces[i] = NULL; /* reset */
    }
    orc_warn("Freed %d interfaces", i);
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
            orc_warn("Adding next hop %d", next_hop->id);
            return;
        }
    }
    orc_fatal("Ran out of space in next hop list. Could not add next hop %d", next_hop->id);
    return;
}

/******
 * get_next_hop
 */
ofdpa_next_hop_t * get_next_hop(l3_next_hop_id_t id) {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        if (ofdpa_next_hops[i]->id == id)
        {
            orc_debug("Got next hop %d. Retrieving it.", id);
            return ofdpa_next_hops[i];
        }
    }
    orc_err("Could not find next hop %d to retrieve", id);
    return NULL;
}

/******
 * del_next_hop
 */
void del_next_hop(l3_next_hop_id_t id) {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        if (ofdpa_next_hops[i]->id == id)
        {
            orc_warn("Removing/freeing next hop %d", id);
            /* Remove any pointers that will break after freeing */
            del_next_hop_from_interfaces(ofdpa_next_hops[i]);
            free(ofdpa_next_hops[i]);
            ofdpa_next_hops[i] = NULL; /* reset */
            return;
        }
    }
    orc_err("Could not find next hop %d to remove", id);
    return;
}

/******
 * free_next_hops
 */
void free_next_hops() {
    int i;
    for (i = 0; i < MAX_NEXT_HOPS; i++)
    {
        orc_trace("Removing/freeing next_hop %d", ofdpa_next_hops[i]->id);
        free(ofdpa_next_hops[i]);
        ofdpa_next_hops[i] = NULL; /* reset */
    }
    orc_warn("Freed %d next_hops", i);
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
    tmp = l3_intf_id; /* store to thread/process local copy so that we can unlock and know what we're returning */
    sem_post(&l3_intf_id_lock);

    orc_trace("Generated unique l3_intf_id_t of %d", tmp);
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
    tmp = next_hop_id; /* store to thread/process local copy so that we can unlock and know what we're returning */
    sem_post(&next_hop_id_lock);

    orc_trace("Generated unique l3_next_hop_id_t of %d", tmp);
    return tmp;
}
