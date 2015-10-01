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
 * Much of this code will make more sense after reading:
 *  - The iproute2 source, particularly for the `ip monitor`ing function
 *  - `man 7 rtnetlink`
 *  - `man 7 netdevice`
 *  - This tutorial -- http://www.linuxjournal.com/article/8498
 *
 ***********************************************************/

#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "orc/orc_driver.h"
#include "orc/orc_logger.h"
#include "orc/neighbor.h"
#include "orc/options.h"
#include "orc/tap_utils.h"
#include "orc/utils.h"

#ifndef MAX_NEXT_HOPS
#define MAX_NEXT_HOPS 256       /* used with pending_next_hop_lookup() */
#endif

next_hop_db * Next_Hop_DB = NULL;

enum monitor_opp {
    OP_ADD,
    OP_DEL
};

/**
 * Wrapper functions for driver calls
 */

int
call_add_l3_v4_route(
        orc_options_t * options,
        u32 ip_dst,
        u32 netmask,
        l3_next_hop_id_t next_hop_id)
{
    int err;
    orc_debug("Calling driver add_l3_v4_route(dst="IPV4_FORMAT",netmask="
            IPV4_FORMAT",next_hop=%d)\n",
            IPV4_ADDR_PRINT(ip_dst),
            IPV4_ADDR_PRINT(netmask),
            next_hop_id
           );
    err = options->drv->add_l3_v4_route(ip_dst, netmask, next_hop_id);
    if (err)
        orc_err("call_add_l3_v4_route() returned %d\n", err);
    return err;
}

int
call_add_l3_v4_next_hop(
        orc_options_t * options,
        port_t * port,
        l3_intf_id_t l3_intf_id,                    /* egress interface */
        u8 next_hop_hw_mac[6],                      /* of the next_hop */
        l3_next_hop_id_t * l3_next_hop_id)
{
    int err;
    err = options->drv->add_l3_v4_next_hop(
            port, l3_intf_id, next_hop_hw_mac, l3_next_hop_id);
    orc_debug("Called driver add_l3_v4_next_hop(port=%s,idx=%d,mac="
            ETH_FORMAT") == returned next_hop_id=%d\n",
            port->name,
            l3_intf_id,
            ETH_ADDR_PRINT(next_hop_hw_mac),
            *l3_next_hop_id);
    if (err)
        orc_err("options->drv->del_l3_next_hop() returned %d\n", err);
    return err;
}



/**
 * Used by RTM_NEWADDR, RTM_DELADDR
 *
 * @return 1 on success, 0 on failure
 */

static int parse_v4_addr_msg(orc_options_t * options, struct nlmsghdr * nl_msg,
        u32 * ipv4_addr, int * if_index, char ifnam[IFNAMSIZ])
{
    struct ifaddrmsg * ifaddr_msg;
    struct rtattr * attr;
    int rtattr_len;
    int found_addr = 0;
    int found_ifnam = 0;


    ifaddr_msg = (struct ifaddrmsg *) NLMSG_DATA(nl_msg);
    /* extract the first attribute */
    attr = (struct rtattr *) IFA_RTA(ifaddr_msg);
    rtattr_len = IFA_PAYLOAD(nl_msg);

    *if_index = ifaddr_msg->ifa_index;
    if (ifaddr_msg->ifa_family == AF_INET6)
    {
        /* TODO: add IPv6 support */
        orc_debug("Ignoring new IPv6 address: IPv6 unsupported\n");
        return 0;
    }

    /** note: the RTA_NEXT() macro decrements rtattr_len each time */
    for(; RTA_OK(attr, rtattr_len); attr = RTA_NEXT(attr, rtattr_len))
    {
        switch (attr->rta_type)
        {
            case IFA_ADDRESS:
                memcpy(ipv4_addr, RTA_DATA(attr), sizeof(*ipv4_addr));
                found_addr = 1;     /* probably always there, but... */
                break;
            case IFA_LABEL:
                snprintf(ifnam, IFNAMSIZ, "%s", (char *) RTA_DATA(attr));
                found_ifnam = 1;
                break;
            case IFA_LOCAL:
            case IFA_BROADCAST:
                orc_trace("Intentionally ignoring address update %s - not needed\n",
                    attr->rta_type == IFA_LOCAL? "IFA_LOCAL" : "IFA_BROADCAST");
                break;
            default:
                orc_warn("Skipping unhandled ipv4 address attr: %d\n", attr->rta_type);
        }
    }

    return found_addr && found_ifnam;
}

/********
 * Tell the driver there is a new IP address on an interface
 */

static int handle_v4_addr(
        enum monitor_opp opp,
        orc_options_t * options,
        struct nlmsghdr * nl_msg)
{
    u32 ipv4_addr;
    int if_index;
    char ifnam[IFNAMSIZ];
    port_t * port;
    u8 hw_mac[6];
    int mtu;
    int success = parse_v4_addr_msg(options, nl_msg, &ipv4_addr, &if_index, ifnam);
    if (success)
    {
        orc_log("%s new IP address " IPV4_FORMAT " on iface %s idx=%d\n",
                ((opp == OP_ADD)? "Adding" : "Deleting"),
                IPV4_ADDR_PRINT(ipv4_addr), ifnam, if_index);
    }
    else
    {
        orc_err("Failed to parse IPv4 address netlink message\n");
        return success;
    }

    /* lookup port */
    port = interface_index_to_port_t(if_index);
    if (port == NULL)
    {
        orc_debug("Ignoring new IP address for non-ORC port %s idx=%d;"
                " couldn't map if_index to port structure\n",
                ifnam, if_index);
        return 0;
    }

    /* lookup mac addr */
    if (interface_name_to_hw_mac(ifnam, hw_mac))
    {
        orc_warn("Couldn't get the HW MAC for interface %s idx=%d\n",
                ifnam, if_index);
        return -1;
    }

    /* lookup mtu */
    if (interface_name_to_mtu(ifnam, &mtu))
    {
        orc_warn("Couldn't get the MTU for interface %s idx=%d\n",
                ifnam, if_index);
        return -1;
    }

    if ( opp == OP_ADD)
    {
        orc_debug("Calling drv->add_l3_v4_inferface(%s,"ETH_FORMAT",%d,"
                IPV4_FORMAT "intf);\n",
                ifnam,
                ETH_ADDR_PRINT(hw_mac),
                mtu,
                IPV4_ADDR_PRINT(ipv4_addr)
            );
        return options->drv->add_l3_v4_interface(
                port,
                hw_mac,
                mtu,
                ipv4_addr,
                &port->l3_intf_id       /* TODO allow multiple L3 addrs on a phy port */
            );
    }
    else
    {
        orc_debug("Calling drv->del_l3_interface(\"%s\", %d)\n", port->name, port->l3_intf_id);
        return options->drv->del_l3_interface(port, port->l3_intf_id);
    }
}

/************************** routing updates ***********************/


/*****
 * Routes need:
 *  - src       MAC + VLAN
 *  - NextHop   MAC + egress port
 *  - CIDR prefix
 *  - TODO VRF ID
 */

typedef struct orc_v4_route_s {
    char src_route_valid;       /* does ->src* contain a real value */
    char dst_route_valid;       /* does ->dst* contain a real value */
    char gateway_valid;         /* does ->gateway contain a real value */
    u32 src_if_index;
    u32 src_ip;
    u8  src_mask_len;   /* 16 not 255.255.0.0 */
    u32 dst_if_index;
    u32 dst_ip;
    u8  dst_mask_len;   /* 16 not 255.255.0.0 */
    u32 gateway;    /* gateway == 0 if no gateway/we are last hop */
} orc_v4_route_t;

/***
 * Populate as much of the orc_v4_route_t struct as we can from the update message.
 *
 * @return 1 if a useful route is found, 0 if we should ignore this and -1 if an error
 */

static int parse_v4_route(
        orc_options_t * options,
        struct nlmsghdr * nl_msg,
        orc_v4_route_t * entry)
{
    struct rtmsg * rt_msg;
    struct rtattr * attr;
    int rtattr_len;
    int found_dst = 0;
    int found_dst_if = 0;


    rt_msg = (struct rtmsg *) NLMSG_DATA(nl_msg);
    /* extract the first attribute */
    attr = (struct rtattr *) RTM_RTA(rt_msg);
    rtattr_len = RTM_PAYLOAD(nl_msg);

    if (rt_msg->rtm_family == AF_INET6)
    {
        /* TODO: add IPv6 support */
        orc_debug("Ignoring IPv6 route update: IPv6 unsupported\n");
        return 0;
    }

    /** FIXME: figure out if we should qualify on
     * rt_msg -> rtm_table == RT_TABLE_DEFAULT _or_
     * rt_msg -> rtm_protocol == ???  _or_
     * rt_msg -> rtm_scope == ???
     */


    /** ignore BROADCAST, LOCAL, ANYCAST, MULTICAST routes updates;
     * for now, just accept unicast routes
     */
    if ( rt_msg -> rtm_type != RTN_UNICAST)
        return 0;

    orc_debug("Parsed IPv4 route Update: \n"
            "       rtm_family      = 0x%x\n"
            "       rtm_dst_len     = %d\n"
            "       rtm_src_len     = %d\n"
            "       rtm_tos         = 0x%x\n"
            "       rtm_table       = 0x%x\n"
            "       rtm_protocol    = 0x%x\n"
            "       rtm_scope       = 0x%x\n"
            "       rtm_type        = 0x%x\n"
            "       rtm_flags       = 0x%x\n",
            rt_msg -> rtm_family,
            rt_msg -> rtm_dst_len,
            rt_msg -> rtm_src_len,
            rt_msg -> rtm_tos,
            rt_msg -> rtm_table,
            rt_msg -> rtm_protocol,
            rt_msg -> rtm_scope,
            rt_msg -> rtm_type,
            rt_msg -> rtm_flags);

    bzero(entry, sizeof(*entry));
    /* get the dest mask from main msg */
    entry->dst_mask_len = rt_msg->rtm_dst_len;
    entry->src_mask_len = rt_msg->rtm_src_len;

    /** note: the RTA_NEXT() macro decrements rtattr_len each time */
    for(; RTA_OK(attr, rtattr_len); attr = RTA_NEXT(attr, rtattr_len))
    {
        switch (attr->rta_type)
        {
            case RTA_DST:
                memcpy( &entry->dst_ip, RTA_DATA(attr), sizeof(entry->dst_ip));
                found_dst = 1;
                orc_debug("Parsed IPv4 route dst: " IPV4_FORMAT "/%d\n",
                        IPV4_ADDR_PRINT(entry->dst_ip), entry->dst_mask_len);
                break;
            case RTA_SRC:
                memcpy( &entry->src_ip, RTA_DATA(attr), sizeof(entry->src_ip));
                entry->src_route_valid = 1; /* FIXME: check RTA_IIF as well */
                orc_debug("Parsed IPv4 route src: " IPV4_FORMAT "/%d\n",
                        IPV4_ADDR_PRINT(entry->src_ip), entry->src_mask_len);
                break;
            case RTA_IIF:
                memcpy( &entry->src_if_index, RTA_DATA(attr),
                        sizeof(entry->src_if_index));
                orc_debug("Parsed IPv4 route IIF index: %d\n",
                        entry->src_if_index);
                break;
            case RTA_OIF:
                memcpy( &entry->dst_if_index, RTA_DATA(attr),
                        sizeof(entry->dst_if_index));
                found_dst_if = 1;
                orc_debug("Parsed IPv4 route OIF index: %d\n",
                        entry->dst_if_index);
                break;
            case RTA_GATEWAY:
                memcpy( &entry->gateway, RTA_DATA(attr),
                        sizeof(entry->gateway));
                entry->gateway_valid = 1;
                orc_debug("Parsed IPv4 route gateway: " IPV4_FORMAT "\n",
                    IPV4_ADDR_PRINT(entry->gateway));
                break;
            case RTA_PREFSRC:
                orc_trace("Skipping unhandled RTA_PREFSRC route attr\n");
                break;
            case RTA_TABLE:
                orc_trace("Skipping unhandled RTA_TABLE route attr\n");
                break;
            default:
            /** TODO: decide if we actually need anything else from here */
                orc_warn("Skipping unhandled ipv4 route attr: %d\n",
                        attr->rta_type);
        }
    }
    if (found_dst && found_dst_if)
        entry->dst_route_valid = 1;

    return entry->dst_route_valid;
}


static int handle_v4_route(
        enum monitor_opp opp,
        orc_options_t * options,
        struct nlmsghdr * nl_msg)
{
    int err;
    char buf[BUFLEN];
    char intfnam[IFNAMSIZ];
    l3_next_hop_id_t next_hop;
    u32 netmask;
    orc_v4_route_t entry;

    int success = parse_v4_route(options, nl_msg, &entry);
    if (success == 0)
    {
        orc_trace("Ignoring irrelevant(?) route update\n");
        return 0;
    }
    else if (success == -1)
    {
        orc_warn("Error parsing v4 route update - ignoring\n");
        return -1;
    }

    if (entry.gateway_valid)
        snprintf(buf, BUFLEN, "via gateway "IPV4_FORMAT,
                IPV4_ADDR_PRINT(entry.gateway));
    else
        snprintf(buf, BUFLEN, "direct");
    orc_info("%s IPv4 Route: " IPV4_FORMAT "/%d %s on intf %s (%d)\n",
            ((opp == OP_ADD)? "Adding" : "Deleting"),
            IPV4_ADDR_PRINT(entry.dst_ip),
            entry.dst_mask_len,
            buf,
            interface_index_to_name(entry.dst_if_index, intfnam, IFNAMSIZ),
            entry.dst_if_index);

    netmask = ~((1<<(32-entry.dst_mask_len))-1);
    if (next_hop_lookup( Next_Hop_DB, entry.gateway, &next_hop))
    {
        orc_debug("Found next_hop_id=%d for gateway " IPV4_FORMAT "\n",
                    next_hop, IPV4_ADDR_PRINT(entry.gateway));
    }
    else
        next_hop = NEXT_HOP_KERNEL; /** software forward until we learn
                                     * next_hop via ARP
                                     **/

    if (opp == OP_ADD)
    {

        /* Call the driver to actually add the route IF it has a gateway */
        if (entry.gateway_valid)
        {
            /** even if we know the next hop, register
             * as pending anyway because the next hop could
             * expire and we need to be able to look it up for the lifetime of
             * the route
             */
            if ((err = pending_next_hop_add_gateway(
                    Next_Hop_DB,
                    entry.gateway,
                    entry.dst_ip,
                    netmask)) != 0)
            {
                orc_debug("pending_next_hop_add_gateway() failed: %d\n",
                        err);
            }
            call_add_l3_v4_route(options,
                    entry.dst_ip,
                    netmask,
                    next_hop
                );
        } else {
            if ((err = pending_next_hop_add_direct(
                    Next_Hop_DB,
                    entry.dst_ip,
                    netmask)) != 0)
            {
                orc_debug("pending_next_hop_add_direct() failed: %d\n",
                        err);
            }
        }
    }
    else    /* opp == OP_DEL  */
    {
        if (entry.gateway_valid)
        {
            orc_debug("Calling drv->del_l3_v4_route(" IPV4_FORMAT "/"
                    IPV4_FORMAT",next_hop=%d)\n",
                    IPV4_ADDR_PRINT(entry.dst_ip),
                    IPV4_ADDR_PRINT(netmask),
                    next_hop
                );
            /* call the driver to delete the route */
            options->drv->del_l3_v4_route(
                    entry.dst_ip,
                    netmask,
                    next_hop
                );
            /* delete the pending next_hop */
            if ((err = pending_next_hop_del_gateway(
                    Next_Hop_DB,
                    entry.gateway,
                    entry.dst_ip,
                    netmask)) != 0)
            {
                orc_debug("pending_next_hop_del_gateway() failed: %d\n",
                        err);
            }
        } else {    /* direct route */
            if ((err = pending_next_hop_del_direct(
                    Next_Hop_DB,
                    entry.dst_ip,
                    netmask)) != 0)
            {
                orc_debug("pending_next_hop_del_gateway() failed: %d\n",
                        err);
            }
        }
    }

    return success;
}

/***********
 * Used by RTM_NEWNEIGH, and RTM_DELNEIGH -- ARP management functions
 */

typedef struct orc_v4_neighbor_s {
    int if_index;           /* index of interface where we discovered them */
    u32 ip;                 /* ip addres of neighbor */
    u8 mac[6];              /* mac address of neighbor if mac_valid=1 */
    char mac_valid;         /* 1 iff mac address field is valid */
} orc_v4_neighbor_t;

/****
 * Parse out relevant information from a neigbor update
 *
 * @return 1 on success, 0 to ignore return value, -1 on parse failure
 */


/** silly Linux kernel; doesn't define these for some reason */
#ifndef NDA_RTA
#define NDA_RTA(r) \
        ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#endif
#ifndef NDA_PAYLOAD
#define NDA_PAYLOAD(n)  NLMSG_PAYLOAD(n,sizeof(struct ndmsg))
#endif


static int parse_v4_neighbor(
        orc_options_t * options,
        struct nlmsghdr * nl_msg,
        orc_v4_neighbor_t * neigh)
{
    struct ndmsg * nd_msg;
    struct rtattr * attr;
    int rtattr_len;
    int found_l3_addr = 0;

    nd_msg = (struct ndmsg *) NLMSG_DATA(nl_msg);
    /* extract first attribute */
    attr = (struct rtattr *) NDA_RTA(nd_msg);
    rtattr_len = NDA_PAYLOAD(nl_msg);

    if (nd_msg->ndm_family != AF_INET)
    {
        /* TODO: add IPv6 support */
        orc_debug("Ignoring IPv6 neighbor update: IPv6 unsupported\n");
        return 0;
    }

    neigh->if_index = nd_msg->ndm_ifindex;

    /** TODO: ignore neighbor updates on non-orc interfaces
     */

    if (nd_msg->ndm_state != NUD_STALE &&
            nd_msg->ndm_state != NUD_PERMANENT &&
            nd_msg->ndm_state != NUD_FAILED &&
            nd_msg->ndm_state != NUD_REACHABLE)
    {
        orc_trace("Ignoring irrelevant neighbor update:"
                    " not STALE/PERM/REACHABLE/FAILED\n");
        return 0;
    }

    /** note: the RTA_NEXT() macro decrements rtattr_len each time */
    for(; RTA_OK(attr, rtattr_len); attr = RTA_NEXT(attr, rtattr_len))
    {
        switch (attr->rta_type)
        {
            case NDA_DST:
                found_l3_addr = 1;
                memcpy(&neigh->ip, RTA_DATA(attr), sizeof(neigh->ip));
                orc_debug("Parsed IPv4 neighbor L3 addr: " IPV4_FORMAT "\n",
                        IPV4_ADDR_PRINT(neigh->ip));
                break;
            case NDA_LLADDR:
                neigh->mac_valid = 1;
                memcpy(neigh->mac, RTA_DATA(attr), 6);
                orc_debug("Parsed IPv4 neighbor L2 addr: " ETH_FORMAT "\n",
                        ETH_ADDR_PRINT(neigh->mac));
                break;
            case NDA_CACHEINFO:
                orc_trace("Ignoring irrelevant neighbor attr NDA_CACHEINFO\n");
                break;
            case NDA_PROBES:
                orc_trace("Ignoring irrelevant neighbor attr NDA_PROBES\n");
                break;
            default:
            /** TODO: decide if we actually need anything from here */
                orc_warn("Skipping unhandled ipv4 neighbor attr: %d\n", attr->rta_type);
        };
    }
    /** we found a complete/valid entry iff it has a mac and an IP **/
    return (found_l3_addr && neigh->mac_valid);
}


int handle_v4_neighbor(
        enum monitor_opp opp,
        orc_options_t *options,
        struct nlmsghdr *nl_msg)
{
    char buf[BUFLEN];
    char intfnam[IFNAMSIZ];
    orc_v4_neighbor_t neigh;
    l3_next_hop_id_t next_hop_id;
    int err, i;
    u32 route_ips[MAX_NEXT_HOPS];
    u32 route_netmasks[MAX_NEXT_HOPS];
    int n_next_hops = MAX_NEXT_HOPS;

    int success = parse_v4_neighbor(options, nl_msg, &neigh);
    if (success == 0)
    {
        orc_trace("Ignoring irrelevant(?) neighbor update\n");
        return 0;
    }
    else if (success != 1)
    {
        orc_warn("Got unparsable IPv4 neighbor update\n");
        return success;
    }
    if (neigh.mac_valid)
        snprintf(buf, BUFLEN, ETH_FORMAT, ETH_ADDR_PRINT(neigh.mac));
    else
        snprintf(buf, BUFLEN,"???");
    orc_debug("start: %s IPv4 Neighbor %s --> " IPV4_FORMAT
            " on %s idx=%d\n",
            ((opp == OP_ADD)? "Adding" : "Deleting"),
            buf,
            IPV4_ADDR_PRINT(neigh.ip),
            interface_index_to_name(neigh.if_index, intfnam, IFNAMSIZ),
            neigh.if_index);

    if (next_hop_lookup(Next_Hop_DB, neigh.ip, &next_hop_id) == 0)
        next_hop_id = NEXT_HOP_KERNEL; /* no next_hop_id allocated yet */

    if (pending_next_hop_lookup(
            Next_Hop_DB,
            neigh.ip,
            route_ips,
            route_netmasks,
            &n_next_hops
            ) == -1)
    {
        orc_warn("pending_next_hop_lookup() returned an error\n");
        return -1;
    }

    if (opp == OP_ADD)
    {
        /* create a next_hop_id if none exists */
        if (next_hop_id == NEXT_HOP_KERNEL)
        {
            port_t *port = interface_index_to_port_t(neigh.if_index);
            if ( port == NULL)
            {
                orc_err("Failed to find port_t structure from this"
                        " route's if_index=%d\n", neigh.if_index);
                return -1;
            }
            call_add_l3_v4_next_hop(options,
                    port,
                    port->l3_intf_id,
                    neigh.mac,
                    & next_hop_id);
            next_hop_add(Next_Hop_DB, neigh.ip, next_hop_id);
            orc_log("%s new IPv4 Neighbor %s --> " IPV4_FORMAT
                    " on %s idx=%d : next_hop_id=%d\n",
                    ((opp == OP_ADD)? "Adding" : "Deleting"),
                    buf,
                    IPV4_ADDR_PRINT(neigh.ip),
                    interface_index_to_name(neigh.if_index, intfnam, IFNAMSIZ),
                    neigh.if_index, next_hop_id);
        }
        /* update each route with the newly allocated next_hop_id */
        for (i = 0; i < n_next_hops; i++)
        {
            if ( (neigh.ip & route_netmasks[i]) == route_ips[i])
            {
                /* this is a direct attach/host route -- insert a /32 */
                if ((err = call_add_l3_v4_route(options,
                        neigh.ip,
                        0xffffffff, /* /32 */
                        next_hop_id
                        )) == -1)
                {
                    continue;
                }
            } else {
                /* this is a gateway route -- insert actual netmask */
                /* this assumes that this route will overwrite the
                 * a pre-existing route in the driver, if it exists */
                if ((err = call_add_l3_v4_route(options,
                        route_ips[i],
                        route_netmasks[i],
                        next_hop_id
                        )) == -1)
                {
                    continue;
                }
            }
        }
    } else {    /* opp == OP_DEL */
        if (next_hop_id != NEXT_HOP_KERNEL)
        {
            /* remove old next hop id */
            next_hop_del(Next_Hop_DB, next_hop_id);
            if ((err = options->drv->del_l3_next_hop(next_hop_id)) == -1)
            {
                orc_err("options->drv->del_l3_next_hop() returned -1\n");
                return -1;
            }
        }
        /* now delete each route that depended on this next_hop */
        for (i =0; i < n_next_hops; i++)
        {
            if ( (neigh.ip & route_netmasks[i]) == route_ips[i])
            {
                /* this was a direct attach/host route; just delete it */
                if ((err = options->drv->del_l3_v4_route(
                        neigh.ip,
                        0xffffffff, /* /32 */
                        next_hop_id
                        )) == -1)
                {
                    orc_err("options->drv->del_l3_v4_route() returned -1\n");
                    continue;
                }
            } else {
                /* this is a gateway route -- replace fast path route with slow */
                /* this assumes that this route will overwrite the
                 * a pre-existing route in the driver, if it exists */
                /* this happens when the ARP entry for the next hop gateway
                 * has timed out so we insert a slow path route so that when
                 * a new packet wants to traverse this route, the linux kernel
                 * will arp for it and then we will re-insert the fast path route
                 */
                if ((err = call_add_l3_v4_route(options,
                        route_ips[i],
                        route_netmasks[i],
                        NEXT_HOP_KERNEL
                        )) == -1)
                {
                    continue;
                }
            }
        }
    }

    return 0;
}

/***************************** public functions *******************/

/****
 * Create a new netlink socket
 */
int netlink_mon_create(orc_options_t * options)
{
    int err;
    struct sockaddr_nl saddr;

    /* Create socket */
    int sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
    if (sock < 0)
        orc_err(" Create socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE) failed: %s\n",
                    strerror(errno));

    /* Make it non-blocking */
    err = set_socket_non_blocking(sock);
    if (err)
    {
        orc_err("Failed to set netlink sock non-blocking: %d\n", err);
        return -1;
    }

    /* Bind it to the sockaddr_nl PID=$$ */
    bzero(&saddr, sizeof(saddr));
    saddr.nl_family = AF_NETLINK;
    saddr.nl_pid = getpid();
    /* Register for rtnetlink notificatons: Awesome sauce HERE */
    saddr.nl_groups =
            RTMGRP_LINK |            /* interfaces */
            RTMGRP_IPV4_ROUTE |      /* IPv4 routes */
            RTMGRP_NEIGH |           /* ARP entires */
            RTMGRP_IPV4_IFADDR;      /* IPv4 Address changes */
    if ((err = bind(sock, (struct sockaddr *) &saddr, sizeof(saddr))))
    {
        orc_err("Failed to bind netlink sock to pid\n");
        return -1;
    }

    /**
     * Create the next hop DB
     */
    err = next_hop_db_init(&Next_Hop_DB);
    if (err)
    {
        orc_fatal("Failed to initialize next_hop database: %d\n", err);
        return 0;
    }

    /* done! */
    return sock;
}



int netlink_mon_handler(orc_options_t * options,
                orc_driver_t * drv,
                int netlink_sock)
{
    char buf[BUFLEN];
    int msg_len;
    struct nlmsghdr * nl_msg;

    msg_len = recv(netlink_sock, buf, BUFLEN, 0);
    nl_msg = (struct nlmsghdr *) buf;
    for (; NLMSG_OK(nl_msg, msg_len); nl_msg=NLMSG_NEXT(nl_msg, msg_len))
    {
        switch(nl_msg->nlmsg_type)
        {
            case RTM_NEWLINK:
               orc_debug("RTNETLINK: add interface\n");
               /** TODO **/
               break;
            case RTM_DELLINK:
               orc_debug("RTNETLINK: del interface\n");
               /** TODO **/
               break;
            case RTM_NEWADDR:
               orc_trace("RTNETLINK: add IPv4 address\n");
               handle_v4_addr(OP_ADD, options, nl_msg);
               break;
            case RTM_DELADDR:
               orc_trace("RTNETLINK: del IPv4 address\n");
               handle_v4_addr(OP_DEL, options, nl_msg);
               break;
            case RTM_NEWROUTE:
               orc_trace("RTNETLINK: add IPv4 route\n");
               handle_v4_route(OP_ADD, options, nl_msg);
               break;
            case RTM_DELROUTE:
               orc_trace("RTNETLINK: del IPv4 route\n");
               handle_v4_route(OP_DEL, options, nl_msg);
               break;
            case RTM_NEWNEIGH:
               orc_trace("RTNETLINK: add IPv4 neighbor\n");
               handle_v4_neighbor(OP_ADD, options, nl_msg);
               break;
            case RTM_DELNEIGH:
               orc_trace("RTNETLINK: del IPv4 neighbor\n");
               handle_v4_neighbor(OP_DEL, options, nl_msg);
               break;
            case NLMSG_DONE:
               orc_log("RTNETLINK: ignoring an NLM_DONE msg\n");
               break;
            default:
               orc_log("RTNETLINK: unknown msg of type %d -- ignoring\n",
                       nl_msg->nlmsg_type);
        };

    }
    return 0;
}
