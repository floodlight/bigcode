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

#include <PPE/ppe_config.h>
#include <PPE/ppe.h>
#include "ppe_int.h"

#include <AIM/aim_map.h>


#include <stdlib.h>
#include <string.h>

/* <auto.start.enum(ALL).source> */
aim_map_si_t ppe_slow_protocol_map[] =
{
    { "LACP", PPE_SLOW_PROTOCOL_LACP },
    { NULL, 0 }
};

aim_map_si_t ppe_slow_protocol_desc_map[] =
{
    { "None", PPE_SLOW_PROTOCOL_LACP },
    { NULL, 0 }
};

const char*
ppe_slow_protocol_name(ppe_slow_protocol_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_slow_protocol_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_slow_protocol'";
    }
}

int
ppe_slow_protocol_value(const char* str, ppe_slow_protocol_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_slow_protocol_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_slow_protocol_desc(ppe_slow_protocol_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_slow_protocol_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_slow_protocol'";
    }
}

int
ppe_slow_protocol_valid(ppe_slow_protocol_t e)
{
    return aim_map_si_i(NULL, e, ppe_slow_protocol_map, 0) ? 1 : 0;
}


aim_map_si_t ppe_field_map[] =
{
    { "META_INGRESS_PORT", PPE_FIELD_META_INGRESS_PORT },
    { "META_PACKET_LENGTH", PPE_FIELD_META_PACKET_LENGTH },
    { "META_PACKET_FORMAT", PPE_FIELD_META_PACKET_FORMAT },


    #if PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS == 1
    #define PPE_CUSTOM_META_FIELD_ENTRY(_name, _size, _offset, _shift) { "META_" #_name, PPE_FIELD_META_##_name },
    #include <ppe_custom_fields.x>
    #endif

    { "META_FIELD_COUNT", PPE_FIELD_META_FIELD_COUNT },
    { "ETHERNET_DST_MAC", PPE_FIELD_ETHERNET_DST_MAC },
    { "ETHERNET_SRC_MAC", PPE_FIELD_ETHERNET_SRC_MAC },
    { "ETHER_TYPE", PPE_FIELD_ETHER_TYPE },
    { "8021Q_TPID", PPE_FIELD_8021Q_TPID },
    { "8021Q_PRI", PPE_FIELD_8021Q_PRI },
    { "8021Q_CFI", PPE_FIELD_8021Q_CFI },
    { "8021Q_VLAN", PPE_FIELD_8021Q_VLAN },
    { "OUTER_8021Q_TPID", PPE_FIELD_OUTER_8021Q_TPID },
    { "OUTER_8021Q_PRI", PPE_FIELD_OUTER_8021Q_PRI },
    { "OUTER_8021Q_CFI", PPE_FIELD_OUTER_8021Q_CFI },
    { "OUTER_8021Q_VLAN", PPE_FIELD_OUTER_8021Q_VLAN },
    { "INNER_8021Q_TPID", PPE_FIELD_INNER_8021Q_TPID },
    { "INNER_8021Q_PRI", PPE_FIELD_INNER_8021Q_PRI },
    { "INNER_8021Q_CFI", PPE_FIELD_INNER_8021Q_CFI },
    { "INNER_8021Q_VLAN", PPE_FIELD_INNER_8021Q_VLAN },
    { "LLC_LENGTH", PPE_FIELD_LLC_LENGTH },
    { "LLC_DSAP", PPE_FIELD_LLC_DSAP },
    { "LLC_SSAP", PPE_FIELD_LLC_SSAP },
    { "LLC_CONTROL", PPE_FIELD_LLC_CONTROL },
    { "SNAP_OUI", PPE_FIELD_SNAP_OUI },
    { "SNAP_PROTOCOL", PPE_FIELD_SNAP_PROTOCOL },
    { "ARP_HTYPE", PPE_FIELD_ARP_HTYPE },
    { "ARP_PTYPE", PPE_FIELD_ARP_PTYPE },
    { "ARP_HLEN", PPE_FIELD_ARP_HLEN },
    { "ARP_PLEN", PPE_FIELD_ARP_PLEN },
    { "ARP_OPERATION", PPE_FIELD_ARP_OPERATION },
    { "ARP_SHA", PPE_FIELD_ARP_SHA },
    { "ARP_SPA", PPE_FIELD_ARP_SPA },
    { "ARP_THA", PPE_FIELD_ARP_THA },
    { "ARP_TPA", PPE_FIELD_ARP_TPA },
    { "DHCP_OPCODE", PPE_FIELD_DHCP_OPCODE },
    { "DHCP_HTYPE", PPE_FIELD_DHCP_HTYPE },
    { "DHCP_HLEN", PPE_FIELD_DHCP_HLEN },
    { "DHCP_HOPS", PPE_FIELD_DHCP_HOPS },
    { "DHCP_XID", PPE_FIELD_DHCP_XID },
    { "DHCP_SECONDS", PPE_FIELD_DHCP_SECONDS },
    { "DHCP_FLAGS", PPE_FIELD_DHCP_FLAGS },
    { "DHCP_CIADDR", PPE_FIELD_DHCP_CIADDR },
    { "DHCP_YIADDR", PPE_FIELD_DHCP_YIADDR },
    { "DHCP_SIADDR", PPE_FIELD_DHCP_SIADDR },
    { "DHCP_GIADDR", PPE_FIELD_DHCP_GIADDR },
    { "DHCP_CHADDR", PPE_FIELD_DHCP_CHADDR },
    { "DHCP_SNAME", PPE_FIELD_DHCP_SNAME },
    { "DHCP_BOOTF", PPE_FIELD_DHCP_BOOTF },
    { "DHCP_MAGIC", PPE_FIELD_DHCP_MAGIC },
    { "DHCP_OPTIONS", PPE_FIELD_DHCP_OPTIONS },
    { "ICMP_TYPE", PPE_FIELD_ICMP_TYPE },
    { "ICMP_CODE", PPE_FIELD_ICMP_CODE },
    { "ICMP_CHECKSUM", PPE_FIELD_ICMP_CHECKSUM },
    { "ICMP_HEADER_DATA", PPE_FIELD_ICMP_HEADER_DATA },
    { "ICMP_PAYLOAD", PPE_FIELD_ICMP_PAYLOAD },
    { "IP4_VERSION", PPE_FIELD_IP4_VERSION },
    { "IP4_HEADER_SIZE", PPE_FIELD_IP4_HEADER_SIZE },
    { "IP4_TOS", PPE_FIELD_IP4_TOS },
    { "IP4_TOTAL_LENGTH", PPE_FIELD_IP4_TOTAL_LENGTH },
    { "IP4_TTL", PPE_FIELD_IP4_TTL },
    { "IP4_PROTOCOL", PPE_FIELD_IP4_PROTOCOL },
    { "IP4_CHECKSUM", PPE_FIELD_IP4_CHECKSUM },
    { "IP4_SRC_ADDR", PPE_FIELD_IP4_SRC_ADDR },
    { "IP4_DST_ADDR", PPE_FIELD_IP4_DST_ADDR },
    { "IP6_VERSION", PPE_FIELD_IP6_VERSION },
    { "IP6_TRAFFIC_CLASS_U", PPE_FIELD_IP6_TRAFFIC_CLASS_U },
    { "IP6_TRAFFIC_CLASS_L", PPE_FIELD_IP6_TRAFFIC_CLASS_L },
    { "IP6_FLOW_LABEL_U", PPE_FIELD_IP6_FLOW_LABEL_U },
    { "IP6_FLOW_LABEL_L", PPE_FIELD_IP6_FLOW_LABEL_L },
    { "IP6_PAYLOAD_LENGTH", PPE_FIELD_IP6_PAYLOAD_LENGTH },
    { "IP6_NEXT_HEADER", PPE_FIELD_IP6_NEXT_HEADER },
    { "IP6_HOP_LIMIT", PPE_FIELD_IP6_HOP_LIMIT },
    { "IP6_SRC_ADDR", PPE_FIELD_IP6_SRC_ADDR },
    { "IP6_DST_ADDR", PPE_FIELD_IP6_DST_ADDR },
    { "TCP_SRC_PORT", PPE_FIELD_TCP_SRC_PORT },
    { "TCP_DST_PORT", PPE_FIELD_TCP_DST_PORT },
    { "TCP_SEQUENCE_NUM", PPE_FIELD_TCP_SEQUENCE_NUM },
    { "TCP_ACKNUM", PPE_FIELD_TCP_ACKNUM },
    { "TCP_FLAGS", PPE_FIELD_TCP_FLAGS },
    { "TCP_WINDOW", PPE_FIELD_TCP_WINDOW },
    { "TCP_CHECKSUM", PPE_FIELD_TCP_CHECKSUM },
    { "TCP_URGENT", PPE_FIELD_TCP_URGENT },
    { "TCP_OPTIONS", PPE_FIELD_TCP_OPTIONS },
    { "UDP_SRC_PORT", PPE_FIELD_UDP_SRC_PORT },
    { "UDP_DST_PORT", PPE_FIELD_UDP_DST_PORT },
    { "UDP_LENGTH", PPE_FIELD_UDP_LENGTH },
    { "UDP_CHECKSUM", PPE_FIELD_UDP_CHECKSUM },
    { "L4_SRC_PORT", PPE_FIELD_L4_SRC_PORT },
    { "L4_DST_PORT", PPE_FIELD_L4_DST_PORT },
    { "SLOW_PROTOCOLS_SUBTYPE", PPE_FIELD_SLOW_PROTOCOLS_SUBTYPE },
    { "LACP_VERSION", PPE_FIELD_LACP_VERSION },
    { "LACP_ACTOR_INFO", PPE_FIELD_LACP_ACTOR_INFO },
    { "LACP_ACTOR_INFO_LEN", PPE_FIELD_LACP_ACTOR_INFO_LEN },
    { "LACP_ACTOR_SYS_PRI", PPE_FIELD_LACP_ACTOR_SYS_PRI },
    { "LACP_ACTOR_SYS", PPE_FIELD_LACP_ACTOR_SYS },
    { "LACP_ACTOR_KEY", PPE_FIELD_LACP_ACTOR_KEY },
    { "LACP_ACTOR_PORT_PRI", PPE_FIELD_LACP_ACTOR_PORT_PRI },
    { "LACP_ACTOR_PORT", PPE_FIELD_LACP_ACTOR_PORT },
    { "LACP_ACTOR_STATE", PPE_FIELD_LACP_ACTOR_STATE },
    { "LACP_RSV0", PPE_FIELD_LACP_RSV0 },
    { "LACP_PARTNER_INFO", PPE_FIELD_LACP_PARTNER_INFO },
    { "LACP_PARTNER_INFO_LEN", PPE_FIELD_LACP_PARTNER_INFO_LEN },
    { "LACP_PARTNER_SYS_PRI", PPE_FIELD_LACP_PARTNER_SYS_PRI },
    { "LACP_PARTNER_SYS", PPE_FIELD_LACP_PARTNER_SYS },
    { "LACP_PARTNER_KEY", PPE_FIELD_LACP_PARTNER_KEY },
    { "LACP_PARTNER_PORT_PRI", PPE_FIELD_LACP_PARTNER_PORT_PRI },
    { "LACP_PARTNER_PORT", PPE_FIELD_LACP_PARTNER_PORT },
    { "LACP_PARTNER_STATE", PPE_FIELD_LACP_PARTNER_STATE },
    { "LACP_RSV1", PPE_FIELD_LACP_RSV1 },
    { "LACP_COLLECTOR_INFO", PPE_FIELD_LACP_COLLECTOR_INFO },
    { "LACP_COLLECTOR_INFO_LEN", PPE_FIELD_LACP_COLLECTOR_INFO_LEN },
    { "LACP_COLLECTOR_MAX_DELAY", PPE_FIELD_LACP_COLLECTOR_MAX_DELAY },
    { "LACP_RSV2", PPE_FIELD_LACP_RSV2 },
    { "LACP_TERMINATOR_INFO", PPE_FIELD_LACP_TERMINATOR_INFO },
    { "LACP_TERMINATOR_LENGTH", PPE_FIELD_LACP_TERMINATOR_LENGTH },
    { "LACP_RSV4", PPE_FIELD_LACP_RSV4 },
    { "OF10_FIRST", PPE_FIELD_OF10_FIRST },
    { "OF10_ETHER_DST_MAC", PPE_FIELD_OF10_ETHER_DST_MAC },
    { "OF10_ETHER_SRC_MAC", PPE_FIELD_OF10_ETHER_SRC_MAC },
    { "OF10_TPID", PPE_FIELD_OF10_TPID },
    { "OF10_PRI", PPE_FIELD_OF10_PRI },
    { "OF10_CFI", PPE_FIELD_OF10_CFI },
    { "OF10_VLAN", PPE_FIELD_OF10_VLAN },
    { "OF10_IP4_DST_ADDR", PPE_FIELD_OF10_IP4_DST_ADDR },
    { "OF10_IP4_SRC_ADDR", PPE_FIELD_OF10_IP4_SRC_ADDR },
    { "OF10_L4_DST_PORT", PPE_FIELD_OF10_L4_DST_PORT },
    { "OF10_L4_SRC_PORT", PPE_FIELD_OF10_L4_SRC_PORT },
    { "OF10_IP4_PROTO", PPE_FIELD_OF10_IP4_PROTO },
    { "OF10_IP4_TOS", PPE_FIELD_OF10_IP4_TOS },
    { "OF10_ICMP_TYPE", PPE_FIELD_OF10_ICMP_TYPE },
    { "OF10_ICMP_CODE", PPE_FIELD_OF10_ICMP_CODE },
    { "OF10_INGRESS_PORT", PPE_FIELD_OF10_INGRESS_PORT },
    { "OF10_PACKET_FORMAT", PPE_FIELD_OF10_PACKET_FORMAT },
    { "OF10_ETHER_TYPE", PPE_FIELD_OF10_ETHER_TYPE },
    { "OF10_ARP_SPA", PPE_FIELD_OF10_ARP_SPA },
    { "OF10_ARP_TPA", PPE_FIELD_OF10_ARP_TPA },
    { "OF10_ARP_PTYPE", PPE_FIELD_OF10_ARP_PTYPE },
    { "OF10_ARP_OPERATION", PPE_FIELD_OF10_ARP_OPERATION },
    { "OF10_LAST", PPE_FIELD_OF10_LAST },
    { NULL, 0 }
};

aim_map_si_t ppe_field_desc_map[] =
{
    { "None", PPE_FIELD_META_INGRESS_PORT },
    { "None", PPE_FIELD_META_PACKET_LENGTH },
    { "None", PPE_FIELD_META_PACKET_FORMAT },


    #if PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS == 1
    #define PPE_CUSTOM_META_FIELD_ENTRY(_name, _size, _offset, _shift) { "None", PPE_FIELD_META_##_name },
    #include <ppe_custom_fields.x>
    #endif

    { "None", PPE_FIELD_META_FIELD_COUNT },
    { "None", PPE_FIELD_ETHERNET_DST_MAC },
    { "None", PPE_FIELD_ETHERNET_SRC_MAC },
    { "None", PPE_FIELD_ETHER_TYPE },
    { "None", PPE_FIELD_8021Q_TPID },
    { "None", PPE_FIELD_8021Q_PRI },
    { "None", PPE_FIELD_8021Q_CFI },
    { "None", PPE_FIELD_8021Q_VLAN },
    { "None", PPE_FIELD_OUTER_8021Q_TPID },
    { "None", PPE_FIELD_OUTER_8021Q_PRI },
    { "None", PPE_FIELD_OUTER_8021Q_CFI },
    { "None", PPE_FIELD_OUTER_8021Q_VLAN },
    { "None", PPE_FIELD_INNER_8021Q_TPID },
    { "None", PPE_FIELD_INNER_8021Q_PRI },
    { "None", PPE_FIELD_INNER_8021Q_CFI },
    { "None", PPE_FIELD_INNER_8021Q_VLAN },
    { "None", PPE_FIELD_LLC_LENGTH },
    { "None", PPE_FIELD_LLC_DSAP },
    { "None", PPE_FIELD_LLC_SSAP },
    { "None", PPE_FIELD_LLC_CONTROL },
    { "None", PPE_FIELD_SNAP_OUI },
    { "None", PPE_FIELD_SNAP_PROTOCOL },
    { "None", PPE_FIELD_ARP_HTYPE },
    { "None", PPE_FIELD_ARP_PTYPE },
    { "None", PPE_FIELD_ARP_HLEN },
    { "None", PPE_FIELD_ARP_PLEN },
    { "None", PPE_FIELD_ARP_OPERATION },
    { "None", PPE_FIELD_ARP_SHA },
    { "None", PPE_FIELD_ARP_SPA },
    { "None", PPE_FIELD_ARP_THA },
    { "None", PPE_FIELD_ARP_TPA },
    { "None", PPE_FIELD_DHCP_OPCODE },
    { "None", PPE_FIELD_DHCP_HTYPE },
    { "None", PPE_FIELD_DHCP_HLEN },
    { "None", PPE_FIELD_DHCP_HOPS },
    { "None", PPE_FIELD_DHCP_XID },
    { "None", PPE_FIELD_DHCP_SECONDS },
    { "None", PPE_FIELD_DHCP_FLAGS },
    { "None", PPE_FIELD_DHCP_CIADDR },
    { "None", PPE_FIELD_DHCP_YIADDR },
    { "None", PPE_FIELD_DHCP_SIADDR },
    { "None", PPE_FIELD_DHCP_GIADDR },
    { "None", PPE_FIELD_DHCP_CHADDR },
    { "None", PPE_FIELD_DHCP_SNAME },
    { "None", PPE_FIELD_DHCP_BOOTF },
    { "None", PPE_FIELD_DHCP_MAGIC },
    { "None", PPE_FIELD_DHCP_OPTIONS },
    { "None", PPE_FIELD_ICMP_TYPE },
    { "None", PPE_FIELD_ICMP_CODE },
    { "None", PPE_FIELD_ICMP_CHECKSUM },
    { "None", PPE_FIELD_ICMP_HEADER_DATA },
    { "None", PPE_FIELD_ICMP_PAYLOAD },
    { "None", PPE_FIELD_IP4_VERSION },
    { "None", PPE_FIELD_IP4_HEADER_SIZE },
    { "None", PPE_FIELD_IP4_TOS },
    { "None", PPE_FIELD_IP4_TOTAL_LENGTH },
    { "None", PPE_FIELD_IP4_TTL },
    { "None", PPE_FIELD_IP4_PROTOCOL },
    { "None", PPE_FIELD_IP4_CHECKSUM },
    { "None", PPE_FIELD_IP4_SRC_ADDR },
    { "None", PPE_FIELD_IP4_DST_ADDR },
    { "None", PPE_FIELD_IP6_VERSION },
    { "None", PPE_FIELD_IP6_TRAFFIC_CLASS_U },
    { "None", PPE_FIELD_IP6_TRAFFIC_CLASS_L },
    { "None", PPE_FIELD_IP6_FLOW_LABEL_U },
    { "None", PPE_FIELD_IP6_FLOW_LABEL_L },
    { "None", PPE_FIELD_IP6_PAYLOAD_LENGTH },
    { "None", PPE_FIELD_IP6_NEXT_HEADER },
    { "None", PPE_FIELD_IP6_HOP_LIMIT },
    { "None", PPE_FIELD_IP6_SRC_ADDR },
    { "None", PPE_FIELD_IP6_DST_ADDR },
    { "None", PPE_FIELD_TCP_SRC_PORT },
    { "None", PPE_FIELD_TCP_DST_PORT },
    { "None", PPE_FIELD_TCP_SEQUENCE_NUM },
    { "None", PPE_FIELD_TCP_ACKNUM },
    { "None", PPE_FIELD_TCP_FLAGS },
    { "None", PPE_FIELD_TCP_WINDOW },
    { "None", PPE_FIELD_TCP_CHECKSUM },
    { "None", PPE_FIELD_TCP_URGENT },
    { "None", PPE_FIELD_TCP_OPTIONS },
    { "None", PPE_FIELD_UDP_SRC_PORT },
    { "None", PPE_FIELD_UDP_DST_PORT },
    { "None", PPE_FIELD_UDP_LENGTH },
    { "None", PPE_FIELD_UDP_CHECKSUM },
    { "None", PPE_FIELD_L4_SRC_PORT },
    { "None", PPE_FIELD_L4_DST_PORT },
    { "None", PPE_FIELD_SLOW_PROTOCOLS_SUBTYPE },
    { "None", PPE_FIELD_LACP_VERSION },
    { "None", PPE_FIELD_LACP_ACTOR_INFO },
    { "None", PPE_FIELD_LACP_ACTOR_INFO_LEN },
    { "None", PPE_FIELD_LACP_ACTOR_SYS_PRI },
    { "None", PPE_FIELD_LACP_ACTOR_SYS },
    { "None", PPE_FIELD_LACP_ACTOR_KEY },
    { "None", PPE_FIELD_LACP_ACTOR_PORT_PRI },
    { "None", PPE_FIELD_LACP_ACTOR_PORT },
    { "None", PPE_FIELD_LACP_ACTOR_STATE },
    { "None", PPE_FIELD_LACP_RSV0 },
    { "None", PPE_FIELD_LACP_PARTNER_INFO },
    { "None", PPE_FIELD_LACP_PARTNER_INFO_LEN },
    { "None", PPE_FIELD_LACP_PARTNER_SYS_PRI },
    { "None", PPE_FIELD_LACP_PARTNER_SYS },
    { "None", PPE_FIELD_LACP_PARTNER_KEY },
    { "None", PPE_FIELD_LACP_PARTNER_PORT_PRI },
    { "None", PPE_FIELD_LACP_PARTNER_PORT },
    { "None", PPE_FIELD_LACP_PARTNER_STATE },
    { "None", PPE_FIELD_LACP_RSV1 },
    { "None", PPE_FIELD_LACP_COLLECTOR_INFO },
    { "None", PPE_FIELD_LACP_COLLECTOR_INFO_LEN },
    { "None", PPE_FIELD_LACP_COLLECTOR_MAX_DELAY },
    { "None", PPE_FIELD_LACP_RSV2 },
    { "None", PPE_FIELD_LACP_TERMINATOR_INFO },
    { "None", PPE_FIELD_LACP_TERMINATOR_LENGTH },
    { "None", PPE_FIELD_LACP_RSV4 },
    { "None", PPE_FIELD_OF10_FIRST },
    { "None", PPE_FIELD_OF10_ETHER_DST_MAC },
    { "None", PPE_FIELD_OF10_ETHER_SRC_MAC },
    { "None", PPE_FIELD_OF10_TPID },
    { "None", PPE_FIELD_OF10_PRI },
    { "None", PPE_FIELD_OF10_CFI },
    { "None", PPE_FIELD_OF10_VLAN },
    { "None", PPE_FIELD_OF10_IP4_DST_ADDR },
    { "None", PPE_FIELD_OF10_IP4_SRC_ADDR },
    { "None", PPE_FIELD_OF10_L4_DST_PORT },
    { "None", PPE_FIELD_OF10_L4_SRC_PORT },
    { "None", PPE_FIELD_OF10_IP4_PROTO },
    { "None", PPE_FIELD_OF10_IP4_TOS },
    { "None", PPE_FIELD_OF10_ICMP_TYPE },
    { "None", PPE_FIELD_OF10_ICMP_CODE },
    { "None", PPE_FIELD_OF10_INGRESS_PORT },
    { "None", PPE_FIELD_OF10_PACKET_FORMAT },
    { "None", PPE_FIELD_OF10_ETHER_TYPE },
    { "None", PPE_FIELD_OF10_ARP_SPA },
    { "None", PPE_FIELD_OF10_ARP_TPA },
    { "None", PPE_FIELD_OF10_ARP_PTYPE },
    { "None", PPE_FIELD_OF10_ARP_OPERATION },
    { "None", PPE_FIELD_OF10_LAST },
    { NULL, 0 }
};

const char*
ppe_field_name(ppe_field_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_field_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_field'";
    }
}

int
ppe_field_value(const char* str, ppe_field_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_field_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_field_desc(ppe_field_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_field_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_field'";
    }
}


aim_map_si_t ppe_header_map[] =
{
    { "META", PPE_HEADER_META },
    { "ETHERNET", PPE_HEADER_ETHERNET },
    { "ETHER", PPE_HEADER_ETHER },
    { "8021Q", PPE_HEADER_8021Q },
    { "OUTER_8021Q", PPE_HEADER_OUTER_8021Q },
    { "INNER_8021Q", PPE_HEADER_INNER_8021Q },
    { "LLC", PPE_HEADER_LLC },
    { "ETHERII", PPE_HEADER_ETHERII },
    { "SNAP", PPE_HEADER_SNAP },
    { "ARP", PPE_HEADER_ARP },
    { "LLDP", PPE_HEADER_LLDP },
    { "IP4", PPE_HEADER_IP4 },
    { "IP6", PPE_HEADER_IP6 },
    { "L4", PPE_HEADER_L4 },
    { "TCP", PPE_HEADER_TCP },
    { "UDP", PPE_HEADER_UDP },
    { "ICMP", PPE_HEADER_ICMP },
    { "SLOW_PROTOCOLS", PPE_HEADER_SLOW_PROTOCOLS },
    { "LACP", PPE_HEADER_LACP },
    { "DHCP", PPE_HEADER_DHCP },
    { "ETHERTYPE_MISSING", PPE_HEADER_ETHERTYPE_MISSING },
    { "OF10", PPE_HEADER_OF10 },
    { NULL, 0 }
};

aim_map_si_t ppe_header_desc_map[] =
{
    { "None", PPE_HEADER_META },
    { "None", PPE_HEADER_ETHERNET },
    { "None", PPE_HEADER_ETHER },
    { "None", PPE_HEADER_8021Q },
    { "None", PPE_HEADER_OUTER_8021Q },
    { "None", PPE_HEADER_INNER_8021Q },
    { "None", PPE_HEADER_LLC },
    { "None", PPE_HEADER_ETHERII },
    { "None", PPE_HEADER_SNAP },
    { "None", PPE_HEADER_ARP },
    { "None", PPE_HEADER_LLDP },
    { "None", PPE_HEADER_IP4 },
    { "None", PPE_HEADER_IP6 },
    { "None", PPE_HEADER_L4 },
    { "None", PPE_HEADER_TCP },
    { "None", PPE_HEADER_UDP },
    { "None", PPE_HEADER_ICMP },
    { "None", PPE_HEADER_SLOW_PROTOCOLS },
    { "None", PPE_HEADER_LACP },
    { "None", PPE_HEADER_DHCP },
    { "None", PPE_HEADER_ETHERTYPE_MISSING },
    { "None", PPE_HEADER_OF10 },
    { NULL, 0 }
};

const char*
ppe_header_name(ppe_header_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_header_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_header'";
    }
}

int
ppe_header_value(const char* str, ppe_header_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_header_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_header_desc(ppe_header_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_header_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_header'";
    }
}


aim_map_si_t ppe_ip_protocol_map[] =
{
    { "ICMP", PPE_IP_PROTOCOL_ICMP },
    { "TCP", PPE_IP_PROTOCOL_TCP },
    { "UDP", PPE_IP_PROTOCOL_UDP },
    { NULL, 0 }
};

aim_map_si_t ppe_ip_protocol_desc_map[] =
{
    { "None", PPE_IP_PROTOCOL_ICMP },
    { "None", PPE_IP_PROTOCOL_TCP },
    { "None", PPE_IP_PROTOCOL_UDP },
    { NULL, 0 }
};

const char*
ppe_ip_protocol_name(ppe_ip_protocol_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_ip_protocol_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_ip_protocol'";
    }
}

int
ppe_ip_protocol_value(const char* str, ppe_ip_protocol_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_ip_protocol_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_ip_protocol_desc(ppe_ip_protocol_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_ip_protocol_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_ip_protocol'";
    }
}

int
ppe_ip_protocol_valid(ppe_ip_protocol_t e)
{
    return aim_map_si_i(NULL, e, ppe_ip_protocol_map, 0) ? 1 : 0;
}


aim_map_si_t ppe_pservice_port_map[] =
{
    { "DHCP_CLIENT", PPE_PSERVICE_PORT_DHCP_CLIENT },
    { "DHCP_SERVER", PPE_PSERVICE_PORT_DHCP_SERVER },
    { NULL, 0 }
};

aim_map_si_t ppe_pservice_port_desc_map[] =
{
    { "None", PPE_PSERVICE_PORT_DHCP_CLIENT },
    { "None", PPE_PSERVICE_PORT_DHCP_SERVER },
    { NULL, 0 }
};

const char*
ppe_pservice_port_name(ppe_pservice_port_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_pservice_port_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_pservice_port'";
    }
}

int
ppe_pservice_port_value(const char* str, ppe_pservice_port_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_pservice_port_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_pservice_port_desc(ppe_pservice_port_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_pservice_port_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_pservice_port'";
    }
}

int
ppe_pservice_port_valid(ppe_pservice_port_t e)
{
    return aim_map_si_i(NULL, e, ppe_pservice_port_map, 0) ? 1 : 0;
}


aim_map_si_t ppe_icmp_typecode_map[] =
{
    { "ECHO_REPLY", PPE_ICMP_TYPECODE_ECHO_REPLY },
    { "DEST_NETWORK_UNREACHABLE", PPE_ICMP_TYPECODE_DEST_NETWORK_UNREACHABLE },
    { "DEST_HOST_UNREACHABLE", PPE_ICMP_TYPECODE_DEST_HOST_UNREACHABLE },
    { "DEST_PROTOCOL_UNREACHABLE", PPE_ICMP_TYPECODE_DEST_PROTOCOL_UNREACHABLE },
    { "DEST_PORT_UNREACHABLE", PPE_ICMP_TYPECODE_DEST_PORT_UNREACHABLE },
    { "FRAGMENTATION_REQUIRED", PPE_ICMP_TYPECODE_FRAGMENTATION_REQUIRED },
    { "SOURCE_ROUTE_FAILED", PPE_ICMP_TYPECODE_SOURCE_ROUTE_FAILED },
    { "NETWORK_UNKNOWN", PPE_ICMP_TYPECODE_NETWORK_UNKNOWN },
    { "HOST_UNKNOWN", PPE_ICMP_TYPECODE_HOST_UNKNOWN },
    { "SOURCE_ISOLATED", PPE_ICMP_TYPECODE_SOURCE_ISOLATED },
    { "NETWORK_PROHIBITED", PPE_ICMP_TYPECODE_NETWORK_PROHIBITED },
    { "HOST_PROHIBITED", PPE_ICMP_TYPECODE_HOST_PROHIBITED },
    { "NETWORK_TOS", PPE_ICMP_TYPECODE_NETWORK_TOS },
    { "HOST_TOS", PPE_ICMP_TYPECODE_HOST_TOS },
    { "COM_PROHIBITED", PPE_ICMP_TYPECODE_COM_PROHIBITED },
    { "HOST_PRECEDENCE", PPE_ICMP_TYPECODE_HOST_PRECEDENCE },
    { "PRECEDENCE_CUTOFF", PPE_ICMP_TYPECODE_PRECEDENCE_CUTOFF },
    { "SOURCE_QUENCH", PPE_ICMP_TYPECODE_SOURCE_QUENCH },
    { "REDIRECT_NETWORK", PPE_ICMP_TYPECODE_REDIRECT_NETWORK },
    { "REDIRECT_HOST", PPE_ICMP_TYPECODE_REDIRECT_HOST },
    { "REDIRECT_NETWORK_TOS", PPE_ICMP_TYPECODE_REDIRECT_NETWORK_TOS },
    { "REDIRECT_HOST_TOS", PPE_ICMP_TYPECODE_REDIRECT_HOST_TOS },
    { "ALTERNATE_HOST_ADDRESS", PPE_ICMP_TYPECODE_ALTERNATE_HOST_ADDRESS },
    { "ECHO_REQUEST", PPE_ICMP_TYPECODE_ECHO_REQUEST },
    { "ROUTER_ADVERT", PPE_ICMP_TYPECODE_ROUTER_ADVERT },
    { "ROUTER_SOL", PPE_ICMP_TYPECODE_ROUTER_SOL },
    { "TIME_EXCEEDED", PPE_ICMP_TYPECODE_TIME_EXCEEDED },
    { "PARAMETER_POINTER", PPE_ICMP_TYPECODE_PARAMETER_POINTER },
    { "PARAMETER_MISSING", PPE_ICMP_TYPECODE_PARAMETER_MISSING },
    { "PARAMETER_BAD_LENGTH", PPE_ICMP_TYPECODE_PARAMETER_BAD_LENGTH },
    { "TIMESTAMP", PPE_ICMP_TYPECODE_TIMESTAMP },
    { "TIMESTAMP_REPLY", PPE_ICMP_TYPECODE_TIMESTAMP_REPLY },
    { "INFORMATION_REQUEST", PPE_ICMP_TYPECODE_INFORMATION_REQUEST },
    { "INFORMATION_REPLY", PPE_ICMP_TYPECODE_INFORMATION_REPLY },
    { "ADDRESS_MASK_REQUEST", PPE_ICMP_TYPECODE_ADDRESS_MASK_REQUEST },
    { "ADDRESS_MASK_REPLY", PPE_ICMP_TYPECODE_ADDRESS_MASK_REPLY },
    { "TRACEROUTE_INFO_REQUEST", PPE_ICMP_TYPECODE_TRACEROUTE_INFO_REQUEST },
    { "DATAGRAM_CONVERSION", PPE_ICMP_TYPECODE_DATAGRAM_CONVERSION },
    { "MOBILE_HOST_REDIRECT", PPE_ICMP_TYPECODE_MOBILE_HOST_REDIRECT },
    { "WHERE_ARE_YOU", PPE_ICMP_TYPECODE_WHERE_ARE_YOU },
    { "HERE_I_AM", PPE_ICMP_TYPECODE_HERE_I_AM },
    { "MOBILE_REG_REQUEST", PPE_ICMP_TYPECODE_MOBILE_REG_REQUEST },
    { "MOBILE_REG_REPLY", PPE_ICMP_TYPECODE_MOBILE_REG_REPLY },
    { "DOMAIN_NAME_REQUEST", PPE_ICMP_TYPECODE_DOMAIN_NAME_REQUEST },
    { "DOMAIN_NAME_REPLY", PPE_ICMP_TYPECODE_DOMAIN_NAME_REPLY },
    { "SKIP", PPE_ICMP_TYPECODE_SKIP },
    { "PHOTURIS", PPE_ICMP_TYPECODE_PHOTURIS },
    { "EXPERIMENTAL", PPE_ICMP_TYPECODE_EXPERIMENTAL },
    { NULL, 0 }
};

aim_map_si_t ppe_icmp_typecode_desc_map[] =
{
    { "None", PPE_ICMP_TYPECODE_ECHO_REPLY },
    { "None", PPE_ICMP_TYPECODE_DEST_NETWORK_UNREACHABLE },
    { "None", PPE_ICMP_TYPECODE_DEST_HOST_UNREACHABLE },
    { "None", PPE_ICMP_TYPECODE_DEST_PROTOCOL_UNREACHABLE },
    { "None", PPE_ICMP_TYPECODE_DEST_PORT_UNREACHABLE },
    { "None", PPE_ICMP_TYPECODE_FRAGMENTATION_REQUIRED },
    { "None", PPE_ICMP_TYPECODE_SOURCE_ROUTE_FAILED },
    { "None", PPE_ICMP_TYPECODE_NETWORK_UNKNOWN },
    { "None", PPE_ICMP_TYPECODE_HOST_UNKNOWN },
    { "None", PPE_ICMP_TYPECODE_SOURCE_ISOLATED },
    { "None", PPE_ICMP_TYPECODE_NETWORK_PROHIBITED },
    { "None", PPE_ICMP_TYPECODE_HOST_PROHIBITED },
    { "None", PPE_ICMP_TYPECODE_NETWORK_TOS },
    { "None", PPE_ICMP_TYPECODE_HOST_TOS },
    { "None", PPE_ICMP_TYPECODE_COM_PROHIBITED },
    { "None", PPE_ICMP_TYPECODE_HOST_PRECEDENCE },
    { "None", PPE_ICMP_TYPECODE_PRECEDENCE_CUTOFF },
    { "None", PPE_ICMP_TYPECODE_SOURCE_QUENCH },
    { "None", PPE_ICMP_TYPECODE_REDIRECT_NETWORK },
    { "None", PPE_ICMP_TYPECODE_REDIRECT_HOST },
    { "None", PPE_ICMP_TYPECODE_REDIRECT_NETWORK_TOS },
    { "None", PPE_ICMP_TYPECODE_REDIRECT_HOST_TOS },
    { "None", PPE_ICMP_TYPECODE_ALTERNATE_HOST_ADDRESS },
    { "None", PPE_ICMP_TYPECODE_ECHO_REQUEST },
    { "None", PPE_ICMP_TYPECODE_ROUTER_ADVERT },
    { "None", PPE_ICMP_TYPECODE_ROUTER_SOL },
    { "None", PPE_ICMP_TYPECODE_TIME_EXCEEDED },
    { "None", PPE_ICMP_TYPECODE_PARAMETER_POINTER },
    { "None", PPE_ICMP_TYPECODE_PARAMETER_MISSING },
    { "None", PPE_ICMP_TYPECODE_PARAMETER_BAD_LENGTH },
    { "None", PPE_ICMP_TYPECODE_TIMESTAMP },
    { "None", PPE_ICMP_TYPECODE_TIMESTAMP_REPLY },
    { "None", PPE_ICMP_TYPECODE_INFORMATION_REQUEST },
    { "None", PPE_ICMP_TYPECODE_INFORMATION_REPLY },
    { "None", PPE_ICMP_TYPECODE_ADDRESS_MASK_REQUEST },
    { "None", PPE_ICMP_TYPECODE_ADDRESS_MASK_REPLY },
    { "None", PPE_ICMP_TYPECODE_TRACEROUTE_INFO_REQUEST },
    { "None", PPE_ICMP_TYPECODE_DATAGRAM_CONVERSION },
    { "None", PPE_ICMP_TYPECODE_MOBILE_HOST_REDIRECT },
    { "None", PPE_ICMP_TYPECODE_WHERE_ARE_YOU },
    { "None", PPE_ICMP_TYPECODE_HERE_I_AM },
    { "None", PPE_ICMP_TYPECODE_MOBILE_REG_REQUEST },
    { "None", PPE_ICMP_TYPECODE_MOBILE_REG_REPLY },
    { "None", PPE_ICMP_TYPECODE_DOMAIN_NAME_REQUEST },
    { "None", PPE_ICMP_TYPECODE_DOMAIN_NAME_REPLY },
    { "None", PPE_ICMP_TYPECODE_SKIP },
    { "None", PPE_ICMP_TYPECODE_PHOTURIS },
    { "None", PPE_ICMP_TYPECODE_EXPERIMENTAL },
    { NULL, 0 }
};

const char*
ppe_icmp_typecode_name(ppe_icmp_typecode_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_icmp_typecode_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_icmp_typecode'";
    }
}

int
ppe_icmp_typecode_value(const char* str, ppe_icmp_typecode_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_icmp_typecode_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_icmp_typecode_desc(ppe_icmp_typecode_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_icmp_typecode_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_icmp_typecode'";
    }
}

int
ppe_icmp_typecode_valid(ppe_icmp_typecode_t e)
{
    return aim_map_si_i(NULL, e, ppe_icmp_typecode_map, 0) ? 1 : 0;
}


aim_map_si_t ppe_ethertype_map[] =
{
    { "ARP", PPE_ETHERTYPE_ARP },
    { "IP4", PPE_ETHERTYPE_IP4 },
    { "IP6", PPE_ETHERTYPE_IP6 },
    { "LLDP", PPE_ETHERTYPE_LLDP },
    { "SLOW_PROTOCOLS", PPE_ETHERTYPE_SLOW_PROTOCOLS },
    { NULL, 0 }
};

aim_map_si_t ppe_ethertype_desc_map[] =
{
    { "None", PPE_ETHERTYPE_ARP },
    { "None", PPE_ETHERTYPE_IP4 },
    { "None", PPE_ETHERTYPE_IP6 },
    { "None", PPE_ETHERTYPE_LLDP },
    { "None", PPE_ETHERTYPE_SLOW_PROTOCOLS },
    { NULL, 0 }
};

const char*
ppe_ethertype_name(ppe_ethertype_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_ethertype_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_ethertype'";
    }
}

int
ppe_ethertype_value(const char* str, ppe_ethertype_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_ethertype_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_ethertype_desc(ppe_ethertype_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_ethertype_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_ethertype'";
    }
}

int
ppe_ethertype_valid(ppe_ethertype_t e)
{
    return aim_map_si_i(NULL, e, ppe_ethertype_map, 0) ? 1 : 0;
}


aim_map_si_t ppe_log_flag_map[] =
{
    { "parse", PPE_LOG_FLAG_PARSE },
    { "format", PPE_LOG_FLAG_FORMAT },
    { NULL, 0 }
};

aim_map_si_t ppe_log_flag_desc_map[] =
{
    { "None", PPE_LOG_FLAG_PARSE },
    { "None", PPE_LOG_FLAG_FORMAT },
    { NULL, 0 }
};

const char*
ppe_log_flag_name(ppe_log_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_log_flag_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_log_flag'";
    }
}

int
ppe_log_flag_value(const char* str, ppe_log_flag_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, ppe_log_flag_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
ppe_log_flag_desc(ppe_log_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, ppe_log_flag_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'ppe_log_flag'";
    }
}

/* <auto.end.enum(ALL).source> */
