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



/* <auto.start.xmacro(ALL).define> */
#ifdef PPE_FIELD_ENTRY
PPE_FIELD_ENTRY(META, INGRESS_PORT, 32, 0, 0)
PPE_FIELD_ENTRY(META, PACKET_LENGTH, 32, 4, 0)
PPE_FIELD_ENTRY(META, PACKET_FORMAT, 32, 8, 0)

#if PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS == 1
#define PPE_CUSTOM_META_FIELD_ENTRY(_name, _size, _offset, _shift) \
PPE_FIELD_ENTRY(META, _name, _size, _offset + 12, _shift)
#include <ppe_custom_fields.x>
#endif

PPE_FIELD_ENTRY(META, FIELD_COUNT, 0, 0, 0)
PPE_FIELD_ENTRY(ETHERNET, DST_MAC, 48, 0, 0)
PPE_FIELD_ENTRY(ETHERNET, SRC_MAC, 48, 6, 0)
PPE_FIELD_ENTRY(ETHER, TYPE, 16, 0, 0)
PPE_FIELD_ENTRY(8021Q, TPID, 16, 0, 0)
PPE_FIELD_ENTRY(8021Q, PRI, 3, 2, 5)
PPE_FIELD_ENTRY(8021Q, CFI, 1, 2, 4)
PPE_FIELD_ENTRY(8021Q, VLAN, 12, 2, 0)
PPE_FIELD_ENTRY(8021Q1, TPID, 16, 0, 0)
PPE_FIELD_ENTRY(8021Q1, PRI, 3, 2, 5)
PPE_FIELD_ENTRY(8021Q1, CFI, 1, 2, 4)
PPE_FIELD_ENTRY(8021Q1, VLAN, 12, 2, 0)
PPE_FIELD_ENTRY(8021Q2, TPID, 16, 0, 0)
PPE_FIELD_ENTRY(8021Q2, PRI, 3, 2, 5)
PPE_FIELD_ENTRY(8021Q2, CFI, 1, 2, 4)
PPE_FIELD_ENTRY(8021Q2, VLAN, 12, 2, 0)
PPE_FIELD_ENTRY(LLC, LENGTH, 16, 0, 0)
PPE_FIELD_ENTRY(LLC, DSAP, 8, 2, 0)
PPE_FIELD_ENTRY(LLC, SSAP, 8, 3, 0)
PPE_FIELD_ENTRY(LLC, CONTROL, 8, 4, 0)
PPE_FIELD_ENTRY(SNAP, OUI, 24, 0, 0)
PPE_FIELD_ENTRY(SNAP, PROTOCOL, 16, 3, 0)
PPE_FIELD_ENTRY(ARP, HTYPE, 16, 0, 0)
PPE_FIELD_ENTRY(ARP, PTYPE, 16, 2, 0)
PPE_FIELD_ENTRY(ARP, HLEN, 8, 4, 0)
PPE_FIELD_ENTRY(ARP, PLEN, 8, 5, 0)
PPE_FIELD_ENTRY(ARP, OPERATION, 16, 6, 0)
PPE_FIELD_ENTRY(ARP, SHA, 48, 8, 0)
PPE_FIELD_ENTRY(ARP, SPA, 32, 14, 0)
PPE_FIELD_ENTRY(ARP, THA, 48, 18, 0)
PPE_FIELD_ENTRY(ARP, TPA, 32, 24, 0)
PPE_FIELD_ENTRY(DHCP, OPCODE, 8, 0, 0)
PPE_FIELD_ENTRY(DHCP, HTYPE, 8, 1, 0)
PPE_FIELD_ENTRY(DHCP, HLEN, 8, 2, 0)
PPE_FIELD_ENTRY(DHCP, HOPS, 8, 3, 0)
PPE_FIELD_ENTRY(DHCP, XID, 32, 4, 0)
PPE_FIELD_ENTRY(DHCP, SECONDS, 16, 8, 0)
PPE_FIELD_ENTRY(DHCP, FLAGS, 16, 10, 0)
PPE_FIELD_ENTRY(DHCP, CIADDR, 32, 12, 0)
PPE_FIELD_ENTRY(DHCP, YIADDR, 32, 16, 0)
PPE_FIELD_ENTRY(DHCP, SIADDR, 32, 20, 0)
PPE_FIELD_ENTRY(DHCP, GIADDR, 32, 24, 0)
PPE_FIELD_ENTRY(DHCP, CHADDR, 128, 28, 0)
PPE_FIELD_ENTRY(DHCP, SNAME, 512, 44, 0)
PPE_FIELD_ENTRY(DHCP, BOOTF, 1024, 108, 0)
PPE_FIELD_ENTRY(DHCP, MAGIC, 32, 236, 0)
PPE_FIELD_ENTRY(DHCP, OPTIONS, 8, 240, 0)
PPE_FIELD_ENTRY(ICMP, TYPE, 8, 0, 0)
PPE_FIELD_ENTRY(ICMP, CODE, 8, 1, 0)
PPE_FIELD_ENTRY(ICMP, CHECKSUM, 16, 2, 0)
PPE_FIELD_ENTRY(ICMP, HEADER_DATA, 32, 4, 0)
PPE_FIELD_ENTRY(ICMP, PAYLOAD, 8, 8, 0)
PPE_FIELD_ENTRY(ICMPV6, TYPE, 8, 0, 0)
PPE_FIELD_ENTRY(ICMPV6, CODE, 8, 1, 0)
PPE_FIELD_ENTRY(ICMPV6, CHECKSUM, 16, 2, 0)
PPE_FIELD_ENTRY(ICMPV6, MESSAGE_BODY, 8, 4, 0)
PPE_FIELD_ENTRY(IGMP, TYPE, 8, 0, 0)
PPE_FIELD_ENTRY(IGMP, MAX_RESP_TIME, 8, 1, 0)
PPE_FIELD_ENTRY(IGMP, CHECKSUM, 16, 2, 0)
PPE_FIELD_ENTRY(IGMP, GROUP_ADDRESS, 32, 4, 0)
PPE_FIELD_ENTRY(IP4, VERSION, 4, 0, 4)
PPE_FIELD_ENTRY(IP4, HEADER_SIZE, 4, 0, 0)
PPE_FIELD_ENTRY(IP4, TOS, 8, 1, 0)
PPE_FIELD_ENTRY(IP4, TOTAL_LENGTH, 16, 2, 0)
PPE_FIELD_ENTRY(IP4, FLAGS, 3, 6, 5)
PPE_FIELD_ENTRY(IP4, FRAG_OFFSET, 13, 6, 0)
PPE_FIELD_ENTRY(IP4, TTL, 8, 8, 0)
PPE_FIELD_ENTRY(IP4, PROTOCOL, 8, 9, 0)
PPE_FIELD_ENTRY(IP4, CHECKSUM, 16, 10, 0)
PPE_FIELD_ENTRY(IP4, SRC_ADDR, 32, 12, 0)
PPE_FIELD_ENTRY(IP4, DST_ADDR, 32, 16, 0)
PPE_FIELD_ENTRY(IP6, VERSION, 4, 0, 4)
PPE_FIELD_ENTRY(IP6, TRAFFIC_CLASS_U, 4, 0, 0)
PPE_FIELD_ENTRY(IP6, TRAFFIC_CLASS_L, 4, 1, 4)
PPE_FIELD_ENTRY(IP6, FLOW_LABEL_U, 4, 1, 0)
PPE_FIELD_ENTRY(IP6, FLOW_LABEL_L, 16, 2, 0)
PPE_FIELD_ENTRY(IP6, PAYLOAD_LENGTH, 16, 4, 0)
PPE_FIELD_ENTRY(IP6, NEXT_HEADER, 8, 6, 0)
PPE_FIELD_ENTRY(IP6, HOP_LIMIT, 8, 7, 0)
PPE_FIELD_ENTRY(IP6, SRC_ADDR, 128, 8, 0)
PPE_FIELD_ENTRY(IP6, DST_ADDR, 128, 24, 0)
PPE_FIELD_ENTRY(TCP, SRC_PORT, 16, 0, 0)
PPE_FIELD_ENTRY(TCP, DST_PORT, 16, 2, 0)
PPE_FIELD_ENTRY(TCP, SEQUENCE_NUM, 32, 4, 0)
PPE_FIELD_ENTRY(TCP, ACKNUM, 32, 8, 0)
PPE_FIELD_ENTRY(TCP, FLAGS, 6, 13, 0)
PPE_FIELD_ENTRY(TCP, WINDOW, 16, 14, 0)
PPE_FIELD_ENTRY(TCP, CHECKSUM, 16, 16, 0)
PPE_FIELD_ENTRY(TCP, URGENT, 16, 18, 0)
PPE_FIELD_ENTRY(TCP, OPTIONS, 32, 20, 0)
PPE_FIELD_ENTRY(UDP, SRC_PORT, 16, 0, 0)
PPE_FIELD_ENTRY(UDP, DST_PORT, 16, 2, 0)
PPE_FIELD_ENTRY(UDP, LENGTH, 16, 4, 0)
PPE_FIELD_ENTRY(UDP, CHECKSUM, 16, 6, 0)
PPE_FIELD_ENTRY(L4, SRC_PORT, 16, 0, 0)
PPE_FIELD_ENTRY(L4, DST_PORT, 16, 2, 0)
PPE_FIELD_ENTRY(GRE, FLAGS, 13, 0, 3)
PPE_FIELD_ENTRY(GRE, VERSION, 3, 1, 0)
PPE_FIELD_ENTRY(GRE, PROTOCOL, 16, 2, 0)
PPE_FIELD_ENTRY(GRE, KEY, 32, 4, 0)
PPE_FIELD_ENTRY(VXLAN, FLAGS, 8, 0, 0)
PPE_FIELD_ENTRY(VXLAN, RESERVED1, 24, 1, 0)
PPE_FIELD_ENTRY(VXLAN, VNID, 24, 4, 0)
PPE_FIELD_ENTRY(VXLAN, RESERVED2, 8, 7, 0)
PPE_FIELD_ENTRY(SLOW_PROTOCOLS, SUBTYPE, 8, 0, 0)
PPE_FIELD_ENTRY(LACP, VERSION, 8, 0, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_INFO, 8, 1, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_INFO_LEN, 8, 2, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_SYS_PRI, 16, 3, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_SYS, 48, 5, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_KEY, 16, 11, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_PORT_PRI, 16, 13, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_PORT, 16, 15, 0)
PPE_FIELD_ENTRY(LACP, ACTOR_STATE, 8, 17, 0)
PPE_FIELD_ENTRY(LACP, RSV0, 24, 18, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_INFO, 8, 21, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_INFO_LEN, 8, 22, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_SYS_PRI, 16, 23, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_SYS, 48, 25, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_KEY, 16, 31, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_PORT_PRI, 16, 33, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_PORT, 16, 35, 0)
PPE_FIELD_ENTRY(LACP, PARTNER_STATE, 8, 37, 0)
PPE_FIELD_ENTRY(LACP, RSV1, 24, 38, 0)
PPE_FIELD_ENTRY(LACP, COLLECTOR_INFO, 8, 41, 0)
PPE_FIELD_ENTRY(LACP, COLLECTOR_INFO_LEN, 8, 42, 0)
PPE_FIELD_ENTRY(LACP, COLLECTOR_MAX_DELAY, 16, 43, 0)
PPE_FIELD_ENTRY(LACP, RSV2, 96, 45, 0)
PPE_FIELD_ENTRY(LACP, TERMINATOR_INFO, 8, 57, 0)
PPE_FIELD_ENTRY(LACP, TERMINATOR_LENGTH, 8, 58, 0)
PPE_FIELD_ENTRY(LACP, RSV4, 384, 59, 0)
PPE_FIELD_ENTRY(OF10, FIRST, 0, 0, 0)
PPE_FIELD_ENTRY(OF10, ETHER_DST_MAC, 48, 0, 0)
PPE_FIELD_ENTRY(OF10, ETHER_SRC_MAC, 48, 6, 0)
PPE_FIELD_ENTRY(OF10, TPID, 16, 12, 0)
PPE_FIELD_ENTRY(OF10, PRI, 3, 14, 5)
PPE_FIELD_ENTRY(OF10, CFI, 1, 14, 4)
PPE_FIELD_ENTRY(OF10, VLAN, 12, 14, 0)
PPE_FIELD_ENTRY(OF10, IP4_DST_ADDR, 32, 16, 0)
PPE_FIELD_ENTRY(OF10, IP4_SRC_ADDR, 32, 20, 0)
PPE_FIELD_ENTRY(OF10, L4_DST_PORT, 16, 24, 0)
PPE_FIELD_ENTRY(OF10, L4_SRC_PORT, 16, 26, 0)
PPE_FIELD_ENTRY(OF10, IP4_PROTO, 8, 28, 0)
PPE_FIELD_ENTRY(OF10, IP4_TOS, 8, 29, 0)
PPE_FIELD_ENTRY(OF10, ICMP_TYPE, 8, 30, 0)
PPE_FIELD_ENTRY(OF10, ICMP_CODE, 8, 31, 0)
PPE_FIELD_ENTRY(OF10, INGRESS_PORT, 32, 32, 0)
PPE_FIELD_ENTRY(OF10, PACKET_FORMAT, 16, 36, 0)
PPE_FIELD_ENTRY(OF10, ETHER_TYPE, 16, 38, 0)
PPE_FIELD_ENTRY(OF10, ARP_SPA, 32, 40, 0)
PPE_FIELD_ENTRY(OF10, ARP_TPA, 32, 44, 0)
PPE_FIELD_ENTRY(OF10, ARP_PTYPE, 16, 48, 0)
PPE_FIELD_ENTRY(OF10, ARP_OPERATION, 16, 50, 0)
PPE_FIELD_ENTRY(OF10, LAST, 0, 52, 0)
PPE_FIELD_ENTRY(PIM, VERSION, 4, 0, 4)
PPE_FIELD_ENTRY(PIM, TYPE, 4, 0, 0)
PPE_FIELD_ENTRY(PIM, CHECKSUM, 16, 2, 0)
#undef PPE_FIELD_ENTRY
#endif

#ifdef PPE_SLOW_PROTOCOL_ENTRY
PPE_SLOW_PROTOCOL_ENTRY(LACP, 1)
#undef PPE_SLOW_PROTOCOL_ENTRY
#endif

#ifdef PPE_ICMPV6_TYPECODE_ENTRY
PPE_ICMPV6_TYPECODE_ENTRY(ROUTER_SOLICITATION, 133)
PPE_ICMPV6_TYPECODE_ENTRY(ROUTER_ADVERTISEMENT, 134)
PPE_ICMPV6_TYPECODE_ENTRY(NEIGHBOR_SOLICITATION, 135)
PPE_ICMPV6_TYPECODE_ENTRY(NEIGHBOR_ADVERTISEMENT, 136)
PPE_ICMPV6_TYPECODE_ENTRY(ECHO_REQUEST, 128)
PPE_ICMPV6_TYPECODE_ENTRY(ECHO_REPLY, 129)
#undef PPE_ICMPV6_TYPECODE_ENTRY
#endif

#ifdef PPE_HEADER_ENTRY
PPE_HEADER_ENTRY(META)
PPE_HEADER_ENTRY(ETHERNET)
PPE_HEADER_ENTRY(ETHER)
PPE_HEADER_ENTRY(8021Q)
PPE_HEADER_ENTRY(8021Q1)
PPE_HEADER_ENTRY(8021Q2)
PPE_HEADER_ENTRY(LLC)
PPE_HEADER_ENTRY(ETHERII)
PPE_HEADER_ENTRY(SNAP)
PPE_HEADER_ENTRY(ARP)
PPE_HEADER_ENTRY(LLDP)
PPE_HEADER_ENTRY(IP4)
PPE_HEADER_ENTRY(IP6)
PPE_HEADER_ENTRY(L4)
PPE_HEADER_ENTRY(TCP)
PPE_HEADER_ENTRY(UDP)
PPE_HEADER_ENTRY(GRE)
PPE_HEADER_ENTRY(VXLAN)
PPE_HEADER_ENTRY(ICMP)
PPE_HEADER_ENTRY(ICMPV6)
PPE_HEADER_ENTRY(SLOW_PROTOCOLS)
PPE_HEADER_ENTRY(LACP)
PPE_HEADER_ENTRY(DHCP)
PPE_HEADER_ENTRY(ETHERTYPE_MISSING)
PPE_HEADER_ENTRY(OF10)
PPE_HEADER_ENTRY(IGMP)
PPE_HEADER_ENTRY(PIM)
#undef PPE_HEADER_ENTRY
#endif

#ifdef PPE_IP_PROTOCOL_ENTRY
PPE_IP_PROTOCOL_ENTRY(ICMP, 1)
PPE_IP_PROTOCOL_ENTRY(IGMP, 2)
PPE_IP_PROTOCOL_ENTRY(TCP, 6)
PPE_IP_PROTOCOL_ENTRY(UDP, 17)
PPE_IP_PROTOCOL_ENTRY(GRE, 47)
PPE_IP_PROTOCOL_ENTRY(ICMPV6, 58)
PPE_IP_PROTOCOL_ENTRY(PIM, 103)
#undef PPE_IP_PROTOCOL_ENTRY
#endif

#ifdef PPE_ICMP_TYPECODE_ENTRY
PPE_ICMP_TYPECODE_ENTRY(ECHO_REPLY, 0)
PPE_ICMP_TYPECODE_ENTRY(DEST_NETWORK_UNREACHABLE, ( ( 3 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(DEST_HOST_UNREACHABLE, ( ( 3 << 8) |  1 ))
PPE_ICMP_TYPECODE_ENTRY(DEST_PROTOCOL_UNREACHABLE, ( ( 3 << 8) |  2 ))
PPE_ICMP_TYPECODE_ENTRY(DEST_PORT_UNREACHABLE, ( ( 3 << 8) |  3 ))
PPE_ICMP_TYPECODE_ENTRY(FRAGMENTATION_REQUIRED, ( ( 3 << 8) |  4 ))
PPE_ICMP_TYPECODE_ENTRY(SOURCE_ROUTE_FAILED, ( ( 3 << 8) |  5 ))
PPE_ICMP_TYPECODE_ENTRY(NETWORK_UNKNOWN, ( ( 3 << 8) |  6 ))
PPE_ICMP_TYPECODE_ENTRY(HOST_UNKNOWN, ( ( 3 << 8) |  7 ))
PPE_ICMP_TYPECODE_ENTRY(SOURCE_ISOLATED, ( ( 3 << 8) |  8 ))
PPE_ICMP_TYPECODE_ENTRY(NETWORK_PROHIBITED, ( ( 3 << 8) |  9 ))
PPE_ICMP_TYPECODE_ENTRY(HOST_PROHIBITED, ( ( 3 << 8) | 10 ))
PPE_ICMP_TYPECODE_ENTRY(NETWORK_TOS, ( ( 3 << 8) | 11 ))
PPE_ICMP_TYPECODE_ENTRY(HOST_TOS, ( ( 3 << 8) | 12 ))
PPE_ICMP_TYPECODE_ENTRY(COM_PROHIBITED, ( ( 3 << 8) | 13 ))
PPE_ICMP_TYPECODE_ENTRY(HOST_PRECEDENCE, ( ( 3 << 8) | 14 ))
PPE_ICMP_TYPECODE_ENTRY(PRECEDENCE_CUTOFF, ( ( 3 << 8) | 15 ))
PPE_ICMP_TYPECODE_ENTRY(SOURCE_QUENCH, ( ( 4 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(REDIRECT_NETWORK, ( ( 5 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(REDIRECT_HOST, ( ( 5 << 8) |  1 ))
PPE_ICMP_TYPECODE_ENTRY(REDIRECT_NETWORK_TOS, ( ( 5 << 8) |  2 ))
PPE_ICMP_TYPECODE_ENTRY(REDIRECT_HOST_TOS, ( ( 5 << 8) |  3 ))
PPE_ICMP_TYPECODE_ENTRY(ALTERNATE_HOST_ADDRESS, ( ( 6 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(ECHO_REQUEST, ( ( 8 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(ROUTER_ADVERT, ( ( 9 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(ROUTER_SOL, ( (10 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(TIME_EXCEEDED, ( (11 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(PARAMETER_POINTER, ( (12 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(PARAMETER_MISSING, ( (12 << 8) |  1 ))
PPE_ICMP_TYPECODE_ENTRY(PARAMETER_BAD_LENGTH, ( (12 << 8) |  2 ))
PPE_ICMP_TYPECODE_ENTRY(TIMESTAMP, ( (13 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(TIMESTAMP_REPLY, ( (14 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(INFORMATION_REQUEST, ( (15 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(INFORMATION_REPLY, ( (16 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(ADDRESS_MASK_REQUEST, ( (17 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(ADDRESS_MASK_REPLY, ( (18 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(TRACEROUTE_INFO_REQUEST, ( (30 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(DATAGRAM_CONVERSION, ( (31 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(MOBILE_HOST_REDIRECT, ( (32 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(WHERE_ARE_YOU, ( (33 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(HERE_I_AM, ( (34 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(MOBILE_REG_REQUEST, ( (35 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(MOBILE_REG_REPLY, ( (36 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(DOMAIN_NAME_REQUEST, ( (37 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(DOMAIN_NAME_REPLY, ( (38 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(SKIP, ( (39 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(PHOTURIS, ( (40 << 8) |  0 ))
PPE_ICMP_TYPECODE_ENTRY(EXPERIMENTAL, ( (41 << 8) |  0 ))
#undef PPE_ICMP_TYPECODE_ENTRY
#endif

#ifdef PPE_ETHERFRAMES
PPE_ETHERFRAMES(ETHERII)
PPE_ETHERFRAMES(8023)
#undef PPE_ETHERFRAMES
#endif

#ifdef PPE_ETHERTYPE_ENTRY
PPE_ETHERTYPE_ENTRY(ARP, 2054)
PPE_ETHERTYPE_ENTRY(IP4, 2048)
PPE_ETHERTYPE_ENTRY(IP6, 34525)
PPE_ETHERTYPE_ENTRY(LLDP, 35020)
PPE_ETHERTYPE_ENTRY(SLOW_PROTOCOLS, 34825)
#undef PPE_ETHERTYPE_ENTRY
#endif

#ifdef PPE_PSERVICE_PORT_ENTRY
PPE_PSERVICE_PORT_ENTRY(DHCP_CLIENT, 68)
PPE_PSERVICE_PORT_ENTRY(DHCP_SERVER, 67)
PPE_PSERVICE_PORT_ENTRY(VXLAN, 4789)
#undef PPE_PSERVICE_PORT_ENTRY
#endif
/* <auto.end.xmacro(ALL).define> */

/* <auto.start.xenum(ALL).define>*/
#ifdef PPE_ENUMERATION_ENTRY
PPE_ENUMERATION_ENTRY(ppe_slow_protocol, "PPE slow protocol.")
PPE_ENUMERATION_ENTRY(ppe_pim_type, "PPE PIM Type.")
PPE_ENUMERATION_ENTRY(ppe_ip4_flags, "PPE IP4 Flags.")
PPE_ENUMERATION_ENTRY(ppe_field, "PPE field identifier.")
PPE_ENUMERATION_ENTRY(ppe_header, "PPE header identifier.")
PPE_ENUMERATION_ENTRY(ppe_ip_protocol, "PPE IP protocol.")
PPE_ENUMERATION_ENTRY(ppe_icmpv6_typecode, "PPE Icmpv6 Typecode.")
PPE_ENUMERATION_ENTRY(ppe_igmp_type, "PPE Igmp Type.")
PPE_ENUMERATION_ENTRY(ppe_pservice_port, "PPE Parsable Service Port.")
PPE_ENUMERATION_ENTRY(ppe_icmp_typecode, "PPE Icmp Typecode.")
PPE_ENUMERATION_ENTRY(ppe_ethertype, "PPE ethertype.")
PPE_ENUMERATION_ENTRY(ppe_log_flag, "PPE Log Flag")
#undef PPE_ENUMERATION_ENTRY
#endif
/* <auto.end.xenum(ALL).define>*/


