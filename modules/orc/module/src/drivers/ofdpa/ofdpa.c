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
#define NO_ACTION 0
#define ACTION 1
#define DEFAULT_ZERO_MAC {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}

typedef enum
{
  DEFAULT_COUNT                 = 1,
  DEFAULT_VLANID                = 0x1001,
  DEFAULT_VLANID_MASK           = 0x1fff,
  DEFAULT_INPORT                = 1,
  DEFAULT_ETHERTYPE             = 0,
  DEFAULT_ETHERTYPE_MASK        = 0,

  DEFAULT_NEWVLANID             = 1,
  DEFAULT_VRF_ACT               = NO_ACTION,
  DEFAULT_VRF                   = 0,
  DEFAULT_VLAN2_ACT             = NO_ACTION,
  DEFAULT_VLAN2                 = 1,
  DEFAULT_PUSH_VLAN             = NO_ACTION,
  DEFAULT_TPID                  = 0,
  DEFAULT_POP_VLAN              = NO_ACTION,
  DEFAULT_OVID_ACT              = NO_ACTION,
  DEFAULT_OVID                  = 0,
  DEFAULT_MPLSL2PORT_ACT        = NO_ACTION,
  DEFAULT_MPLSL2PORT            = 0,
  DEFAULT_TUNNELID_ACT          = NO_ACTION,
  DEFAULT_TUNNELID              = 0,
  DEFAULT_LMEP_ID_ACT           = 0,
  DEFAULT_LMEP_ID               = 0,
  DEFAULT_OAM_LM_COUNT_ACTION   = 0,
  DEFAULT_GOTO                  = OFDPA_FLOW_TABLE_ID_TERMINATION_MAC,

  DEFAULT_DISCARD               = NO_ACTION,
  DEFAULT_DELETE                = NO_ACTION,
  DEFAULT_LIST                  = NO_ACTION,
} VLAN_FLOW_DEFAULT_VALUES;

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
 * These are our ports. Global for
 * monitoring by our RX thread. RX
 * thread will only read, not write.
 * So, we do not need to lock on
 * anything.
 */
#define MAX_PORTS 128
ofdpa_port_t * ofdpa_ports[MAX_PORTS];

/*
 * Maintain a hold on our RX thread
 */
pthread_t rx_thread = NULL;
pthread_mutex_t rx_lock = PTHREAD_MUTEX_INITIALIZER;

/***************
 * ofdpa_init_driver: start up ofdpa
 */
int ofdpa_init_driver(orc_options_t *options, int argc, char * argv[])
{
    /*
     * First initialize
     */
    OFDPA_ERROR_t rc = ofdpaClientInitialize(CLIENT_NAME);
    if (rc == OFDPA_E_NONE) {
        orc_debug("ofdpa initialized");
    } else {
        orc_debug("ofdpa initialization returned error %d", rc);
        return -1;
    }

    /*
     * Then, bind packet socket for receiving packets.
     * Only one PID can bind to this, so we'll do it in
     * initialization rather than on demand.
     */
    rc = ofdpaClientPktSockBind();
    if (rc == OFDPA_E_NONE) {
        orc_debug("ofdpa RX socket bound");
    } else {
        orc_debug("ofdpa RX socket bind returned error %d", rc);
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
    pthread_mutex_lock(&rx_lock);

    /* Clear all driver-local ports */
    free_ports();

    /* Now reallocate */
    for (i = 0; i < ofdpaPorts; i++)
    {
        ofport = malloc(sizeof(*ofport));
        if (ofport == NULL)
        {
            orc_err("Out of mem allocating ports\n");
            return -1;
        }
        ofport->rx = 0; /* we don't want to receive packets from this port at present; set in start_rx below */
        ofport->port.index = i+1; /* index starts at 1 */
        ports[i] = (port_t *) ofport; /* init to the array passed in */
        ofdpa_ports[i] = ofport; /* keep a driver-local copy for RX thread */
        orc_debug("Created port %s for port %s\n",
                        ofport->ofdpa_name, ofport->port.name);
    }
    *num = vports;

    /*
     * Don't forget to release the lock.
     */
    pthread_mutex_unlock(&rx_lock);

    return 0;
}

/*********
 * ofdpa_tx_packet: Send a packet-out message. The in-port will be set as the
 * special OpenFlow port CONTROLLER
 */
int ofdpa_tx_pkt(port_t *port, u8 *pkt, unsigned int len)
{
    struct ofdpa_port * ofdpa = (struct ofdpa_port *) port;
    orc_debug("TX %d byte packet %s\n", len, ofdpa->port.name);

    /* in-port ignored if flag is not set */
    OFDPA_ERROR_t rc = ofdpaPktSend(pkt, 0, port->index, OFDPA_PORT_CONTROLLER);
    orc_debug("TX packet out port %s returned code %d", ofdpa->port.name, rc);
    if (rc == OFDPA_E_NONE) {
        return 0;
    } else {
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
    pthread_mutex_lock(&rx_lock);

    int i;
    for (i = 0; i < num_ports; i++)
    {
        ((ofdpa_port_t *) ports[i]).rx = 1; /* set flag to RX packets on this port */
    }

    if (rx_thread == null)
    {
        orc_debug("Starting RX thread");
        int rc = pthread_create(&rx_thread, NULL /* default attributes */, port_rx_monitor, NULL /* no args to function ptr <-- */);
        if (rc != 0)
        {
            orc_debug("RX thread creation failed with error code %d", rc);
            return -1;
        }
    }
    pthread_mutex_unlock(&rx_lock);
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
        orc_debug("Closing RX socket file descriptor: %d", fd); /* seeing the FD probably isn't important, but we'll do it anyway (initially) */
        close(fd);
        return 0;
    } else {
        orc_debug("ofdpa did not return a valid file descriptor when attempting to close the RX socket: %d", fd);
        return -1;
    }

    /*
     * Now free all the memory we ate
     */
    free_ports();
}

/******
 * ofdpa_add_l3_v4_interface: associate an IPv4 address with a port. Anything destined for it will be sent to the Linux iface FD.
 * (1) add an entry to the VLAN table -- we'll assume the packet arrives untagged (TODO until I figure out how to get the VLAN of the iface)
 * (2) add an entry to the termination MAC table -- match on ethertype and dst MAC provided as argument (cannot match on exact IP, so we won't at all); output to CONTROLLER
 * (l3_intf_id_t is an int)
 */
int ofdpa_add_l3_v4_interface(port_t * port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t * l3_intf_id) {
    OFDPA_ERROR_t rc;
    ofdpaFlowEntry_t flow;

    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_log("Failed to initialize VLAN flow table. rc=%d", rc);
        return -1;
    } else
    {
        /* VLAN flow entry init successful */

        /*
         * flow instance of ofdpaFlowEntry_t
         * flowData union of flow types
         * vlanFlowEntry instance of ofDpaVlanFlowEntry_t
         * match_criteria instance of ofdpaVlanFlowMatch_t
         */
        /* Matches */
        flow.flowData.vlanFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.vlanFlowEntry.match_criteria.vlanId = DEFAULT_VLANID; /* defaults to 1; TODO learn the VLAN ID */
        flow.flowData.vlanFlowEntry.match_criteria.vlanIdMask = DEFAULT_VLANID_MASK;
        flow.flowData.vlanFlowEntry.match_criteria.etherType = DEFAULT_ETHERTYPE;
        flow.flowData.vlanFlowEntry.match_criteria.etherTypeMask = DEFAULT_ETHERTYPE_MASK;
        flow.flowData.vlanFlowEntry.match_criteria.destMac.addr = DEFAULT_ZERO_MAC;
        flow.flowData.vlanFlowEntry.match_criteria.destMacMask.addr = DEFAULT_ZERO_MAC;
        /* Goto Table Instruction */
        flow.flowData.vlanFlowEntry.gotoTableId = DEFAULT_GOTO;
        /* Apply Actions Instruction */
        flow.flowData.vlanFlowEntry.setVlanIdAction = ACTION;
        flow.flowData.vlanFlowEntry.newVlanId = DEFAULT_VLANID; /* defaults to 1; TODO learn the VLAN ID */
        flow.flowData.vlanFlowEntry.popVlanAction = NO_ACTION;
        flow.flowData.vlanFlowEntry.pushVlan2Action = NO_ACTION;
        flow.flowData.vlanFlowEntry.newTpid2 = 0;
        flow.flowData.vlanFlowEntry.setVlanId2Action = NO_ACTION;
        flow.flowData.vlanFlowEntry.newVlanId2 = 0;
        flow.flowData.vlanFlowEntry.brcmOvidAction = NO_ACTION;
        flow.flowData.vlanFlowEntry.brcmOvid = 0;
        flow.flowData.vlanFlowEntry.vrfAction = NO_ACTION;
        flow.flowData.vlanFlowEntry.vrf = 0;
        flow.flowData.vlanFlowEntry.mplsL2PortAction = NO_ACTION;
        flow.flowData.vlanFlowEntry.mplsL2Port = 0;
        flow.flowData.vlanFlowEntry.tunnelIdAction = NO_ACTION;
        flow.flowData.vlanFlowEntry.tunnelId = 0;
        flow.flowData.vlanFlowEntry.lmepIdAction = NO_ACTION;
        flow.flowData.vlanFlowEntry.lmepId = 0;
        flow.flowData.vlanFlowEntry.oamLmTxCountAction = NO_ACTION;

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_log("Failed to push VLAN flow. rc=%d", rc);
            return -1;
        }
    }

    /* Now do the termination MAC flow */
    rc = ofdpaFlowEntryInit(OFDPA_FLOW_TABLE_ID_VLAN, &flow);
    if (rc != OFDPA_E_NONE)
    {
        orc_log("Failed to initialize termination MAC flow table. rc=%d", rc);
        return -1;
    } else
    {
        /* Termination MAC flow entry init successful */

        /*
         * flow instance of ofdpaFlowEntry_t
         * flowData union of flow types
         * terminationMacFlowEntry instance of ofDpaTerminationMacFlowEntry_t
         * match_criteria instance of ofdpaTerminationMacFlowMatch_t
         */
        /* Matches */
        flow.flowData.terminationMacFlowEntry.match_criteria.inPort = port->index;
        flow.flowData.terminationMacFlowEntry.match_criteria.inPortMask =
        flow.flowData.terminationMacFlowEntry.match_criteria.ethertype =
        flow.flowData.terminationMacFlowEntry.match_criteria.destMac =
        flow.flowData.terminationMacFlowEntry.match_criteria.destMacMask =
        flow.flowData.terminationMacFlowEntry.match_criteria.vlanId =
        flow.flowData.terminationMacFlowEntry.match_criteria.vlanIdMask =
        /* Goto Table Instruction */
        /* Apply Actions Instruction */

        /* Now we're finally ready to add the flow */
        rc = ofdpaFlowAdd(flow);
        if (rc != OFDPA_E_NONE)
        {
            orc_log("Failed to push termination MAC flow. rc=%d", rc);
            return -1;
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
int ofdpa_del_l3_v4_interface(port_t * port, l3_intf_id_t * l3_intf_id) {

    return 0;
}

/******
 * ofdpa_add_l3_v4_next_hop: set the next hop of a route. This is the next destination MAC address of routed packets.
 * (l3_next_hop_id_t is an int)
 */
int ofdpa_add_l3_v4_next_hop(port_t * port, l3_intf_id_t l3_intf_id, u8 next_hop_hw_mac[6], l3_next_hop_id_t * l3_next_hop_id) {

    return 0;
}

/******
 * ofdpa_del_l3_v4_next_hop: delete a next hop entry
 */
int ofdpa_del_l3_v4_next_hop(l3_next_hop_id_t * l3_next_hop_id) {

    return 0;
}

/******
 * ofdpa_add_l3_v4_route: add a CIDR route provided a next hop
 */
int ofdpa_add_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t * l3_next_hop_id) {

    return 0;
}

/******
 * ofdpa_del_l3_v4_route: delete an existing CIDR route
 */
int ofdpa_del_l3_v4_route(u32 ip_dst, u32 netmask, l3_next_hop_id_t * l3_next_hop_id) {

    return 0;
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
    .stop_rx = ofdpa_stop_rx
    /* TODO raw_port_enable/disable? */
    .add_l3_v4_interface = ofdpa_add_l3_v4_interface,
    .update_l3_v4_interface = ofdpa_update_l3_v4_interface,
    .del_l3_v4_interface = ofdpa_del_l3_v4_interface,
    .add_l3_v4_next_hop = ofdpa_add_l3_v4_next_hop,
    .del_l3_next_hop = ofdpa_del_l3_v4_next_hop, /* why _v4 missing? */
    .add_l3_v4_route = ofdpa_add_l3_v4_route,
    .del_l3_v4_rotue = ofdpa_del_l3_v4_route,
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
    return ports;
}

/******
 * port_rx_monitor: check ofdpa for port RX events and push the
 * packet received to the corresponding orc FD
 */
void * port_rx_monitor() {
    while(1) {
        ofdpaPacket_t * pkt = malloc(sizeof(ofdpaPacket_t));
        if (pkt == NULL)
        {
            orc_debug("malloc failed during packet RX");
            exit(1); /* TODO what should the correct behavior be here? */
        }
        ofdpaPktReceive(NULL /* no timeout -- block */, pkt);

        /* We have a packet if we get here */
        pthread_mutex_lock(&rx_lock);
        int i;
        for (i = 0; i < MAX_PORTS; i++)
        {
            /*
             * If the ingress port of this packet is registered
             * to receive RX events, dispatch the message; else,
             * ignore the packet from the unregistered port.
             */
            if (ofdpa_ports[i] != NULL &&
                ofdpa_ports[i]->port.index == pkt.inPortNum &&
                ofdpa_ports[i]->rx == 1
               )
            {
                int rc = write(ofdpa_ports[i]->fd, pkt->pktData.pstart, pkt->pktData.size);

                /* Don't forget to free the packet we malloced -- we can do this after write() */

                free(pkt);

                if (rc < 0)
                {
                    orc_debug("write to FD failed during packet RX");
                }
                /* We found the port that we received the packet on, so we're done */
                break;
            } else if (ofdpa_ports[i] == NULL)
            {
                /* We've reached the last port */
                break;
            } /* else keep looping and searching */
        }
        pthread_mutex_unlock(&rx_lock);
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
        free(ofdpa_ports[i]);
        ofdpa_ports[i] = NULL; /* NULL for reuse/detection of end of ports */
    }
    orc_debug("freed memory for ofdpa_port_t's");
}

