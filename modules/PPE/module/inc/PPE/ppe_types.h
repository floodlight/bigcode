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
/************************************************************//**
 *
 * @file
 * @brief PPE Structures and Datatypes.
 *
 *
 * @addtogroup ppe-ppe
 * @{
 *
 ****************************************************************/

#ifndef __PPE_TYPES_H__
#define __PPE_TYPES_H__

/* <auto.start.enum(ALL).header> */
/** ppe_slow_protocol */
typedef enum ppe_slow_protocol_e {
    PPE_SLOW_PROTOCOL_LACP = 1,
} ppe_slow_protocol_t;

/** Enum names. */
const char* ppe_slow_protocol_name(ppe_slow_protocol_t e);

/** Enum values. */
int ppe_slow_protocol_value(const char* str, ppe_slow_protocol_t* e, int substr);

/** Enum descriptions. */
const char* ppe_slow_protocol_desc(ppe_slow_protocol_t e);

/** Enum validator. */
int ppe_slow_protocol_valid(ppe_slow_protocol_t e);

/** validator */
#define PPE_SLOW_PROTOCOL_VALID(_e) \
    (ppe_slow_protocol_valid((_e)))

/** ppe_slow_protocol_map table. */
extern aim_map_si_t ppe_slow_protocol_map[];
/** ppe_slow_protocol_desc_map table. */
extern aim_map_si_t ppe_slow_protocol_desc_map[];

/** ppe_field */
typedef enum ppe_field_e {
    PPE_FIELD_META_INGRESS_PORT,
    PPE_FIELD_META_PACKET_LENGTH,
    PPE_FIELD_META_PACKET_FORMAT,


    #if PPE_CONFIG_INCLUDE_CUSTOM_META_FIELDS == 1
    #define PPE_CUSTOM_META_FIELD_ENTRY(_name, _size, _offset, _shift) PPE_FIELD_META_##_name,
    #include <ppe_custom_fields.x>
    #endif

    PPE_FIELD_META_FIELD_COUNT,
    PPE_FIELD_ETHERNET_DST_MAC,
    PPE_FIELD_ETHERNET_SRC_MAC,
    PPE_FIELD_ETHER_TYPE,
    PPE_FIELD_8021Q_TPID,
    PPE_FIELD_8021Q_PRI,
    PPE_FIELD_8021Q_CFI,
    PPE_FIELD_8021Q_VLAN,
    PPE_FIELD_OUTER_8021Q_TPID,
    PPE_FIELD_OUTER_8021Q_PRI,
    PPE_FIELD_OUTER_8021Q_CFI,
    PPE_FIELD_OUTER_8021Q_VLAN,
    PPE_FIELD_INNER_8021Q_TPID,
    PPE_FIELD_INNER_8021Q_PRI,
    PPE_FIELD_INNER_8021Q_CFI,
    PPE_FIELD_INNER_8021Q_VLAN,
    PPE_FIELD_LLC_LENGTH,
    PPE_FIELD_LLC_DSAP,
    PPE_FIELD_LLC_SSAP,
    PPE_FIELD_LLC_CONTROL,
    PPE_FIELD_SNAP_OUI,
    PPE_FIELD_SNAP_PROTOCOL,
    PPE_FIELD_ARP_HTYPE,
    PPE_FIELD_ARP_PTYPE,
    PPE_FIELD_ARP_HLEN,
    PPE_FIELD_ARP_PLEN,
    PPE_FIELD_ARP_OPERATION,
    PPE_FIELD_ARP_SHA,
    PPE_FIELD_ARP_SPA,
    PPE_FIELD_ARP_THA,
    PPE_FIELD_ARP_TPA,
    PPE_FIELD_DHCP_OPCODE,
    PPE_FIELD_DHCP_HTYPE,
    PPE_FIELD_DHCP_HLEN,
    PPE_FIELD_DHCP_HOPS,
    PPE_FIELD_DHCP_XID,
    PPE_FIELD_DHCP_SECONDS,
    PPE_FIELD_DHCP_FLAGS,
    PPE_FIELD_DHCP_CIADDR,
    PPE_FIELD_DHCP_YIADDR,
    PPE_FIELD_DHCP_SIADDR,
    PPE_FIELD_DHCP_GIADDR,
    PPE_FIELD_DHCP_CHADDR,
    PPE_FIELD_DHCP_SNAME,
    PPE_FIELD_DHCP_BOOTF,
    PPE_FIELD_DHCP_MAGIC,
    PPE_FIELD_DHCP_OPTIONS,
    PPE_FIELD_ICMP_TYPE,
    PPE_FIELD_ICMP_CODE,
    PPE_FIELD_ICMP_CHECKSUM,
    PPE_FIELD_ICMP_HEADER_DATA,
    PPE_FIELD_ICMP_PAYLOAD,
    PPE_FIELD_IP4_VERSION,
    PPE_FIELD_IP4_HEADER_SIZE,
    PPE_FIELD_IP4_TOS,
    PPE_FIELD_IP4_TOTAL_LENGTH,
    PPE_FIELD_IP4_TTL,
    PPE_FIELD_IP4_PROTOCOL,
    PPE_FIELD_IP4_CHECKSUM,
    PPE_FIELD_IP4_SRC_ADDR,
    PPE_FIELD_IP4_DST_ADDR,
    PPE_FIELD_IP6_VERSION,
    PPE_FIELD_IP6_TRAFFIC_CLASS_U,
    PPE_FIELD_IP6_TRAFFIC_CLASS_L,
    PPE_FIELD_IP6_FLOW_LABEL_U,
    PPE_FIELD_IP6_FLOW_LABEL_L,
    PPE_FIELD_IP6_PAYLOAD_LENGTH,
    PPE_FIELD_IP6_NEXT_HEADER,
    PPE_FIELD_IP6_HOP_LIMIT,
    PPE_FIELD_IP6_SRC_ADDR,
    PPE_FIELD_IP6_DST_ADDR,
    PPE_FIELD_TCP_SRC_PORT,
    PPE_FIELD_TCP_DST_PORT,
    PPE_FIELD_TCP_SEQUENCE_NUM,
    PPE_FIELD_TCP_ACKNUM,
    PPE_FIELD_TCP_FLAGS,
    PPE_FIELD_TCP_WINDOW,
    PPE_FIELD_TCP_CHECKSUM,
    PPE_FIELD_TCP_URGENT,
    PPE_FIELD_TCP_OPTIONS,
    PPE_FIELD_UDP_SRC_PORT,
    PPE_FIELD_UDP_DST_PORT,
    PPE_FIELD_UDP_LENGTH,
    PPE_FIELD_UDP_CHECKSUM,
    PPE_FIELD_L4_SRC_PORT,
    PPE_FIELD_L4_DST_PORT,
    PPE_FIELD_SLOW_PROTOCOLS_SUBTYPE,
    PPE_FIELD_LACP_VERSION,
    PPE_FIELD_LACP_ACTOR_INFO,
    PPE_FIELD_LACP_ACTOR_INFO_LEN,
    PPE_FIELD_LACP_ACTOR_SYS_PRI,
    PPE_FIELD_LACP_ACTOR_SYS,
    PPE_FIELD_LACP_ACTOR_KEY,
    PPE_FIELD_LACP_ACTOR_PORT_PRI,
    PPE_FIELD_LACP_ACTOR_PORT,
    PPE_FIELD_LACP_ACTOR_STATE,
    PPE_FIELD_LACP_RSV0,
    PPE_FIELD_LACP_PARTNER_INFO,
    PPE_FIELD_LACP_PARTNER_INFO_LEN,
    PPE_FIELD_LACP_PARTNER_SYS_PRI,
    PPE_FIELD_LACP_PARTNER_SYS,
    PPE_FIELD_LACP_PARTNER_KEY,
    PPE_FIELD_LACP_PARTNER_PORT_PRI,
    PPE_FIELD_LACP_PARTNER_PORT,
    PPE_FIELD_LACP_PARTNER_STATE,
    PPE_FIELD_LACP_RSV1,
    PPE_FIELD_LACP_COLLECTOR_INFO,
    PPE_FIELD_LACP_COLLECTOR_INFO_LEN,
    PPE_FIELD_LACP_COLLECTOR_MAX_DELAY,
    PPE_FIELD_LACP_RSV2,
    PPE_FIELD_LACP_TERMINATOR_INFO,
    PPE_FIELD_LACP_TERMINATOR_LENGTH,
    PPE_FIELD_LACP_RSV4,
    PPE_FIELD_OF10_FIRST,
    PPE_FIELD_OF10_ETHER_DST_MAC,
    PPE_FIELD_OF10_ETHER_SRC_MAC,
    PPE_FIELD_OF10_TPID,
    PPE_FIELD_OF10_PRI,
    PPE_FIELD_OF10_CFI,
    PPE_FIELD_OF10_VLAN,
    PPE_FIELD_OF10_IP4_DST_ADDR,
    PPE_FIELD_OF10_IP4_SRC_ADDR,
    PPE_FIELD_OF10_L4_DST_PORT,
    PPE_FIELD_OF10_L4_SRC_PORT,
    PPE_FIELD_OF10_IP4_PROTO,
    PPE_FIELD_OF10_IP4_TOS,
    PPE_FIELD_OF10_ICMP_TYPE,
    PPE_FIELD_OF10_ICMP_CODE,
    PPE_FIELD_OF10_INGRESS_PORT,
    PPE_FIELD_OF10_PACKET_FORMAT,
    PPE_FIELD_OF10_ETHER_TYPE,
    PPE_FIELD_OF10_ARP_SPA,
    PPE_FIELD_OF10_ARP_TPA,
    PPE_FIELD_OF10_ARP_PTYPE,
    PPE_FIELD_OF10_ARP_OPERATION,
    PPE_FIELD_OF10_LAST,
    PPE_FIELD_LAST = PPE_FIELD_OF10_LAST,
    PPE_FIELD_COUNT,
    PPE_FIELD_INVALID = -1,
} ppe_field_t;

/** Enum names. */
const char* ppe_field_name(ppe_field_t e);

/** Enum values. */
int ppe_field_value(const char* str, ppe_field_t* e, int substr);

/** Enum descriptions. */
const char* ppe_field_desc(ppe_field_t e);

/** validator */
#define PPE_FIELD_VALID(_e) \
    ( (0 <= (_e)) && ((_e) <= PPE_FIELD_OF10_LAST))

/** ppe_field_map table. */
extern aim_map_si_t ppe_field_map[];
/** ppe_field_desc_map table. */
extern aim_map_si_t ppe_field_desc_map[];

/** ppe_header */
typedef enum ppe_header_e {
    PPE_HEADER_META,
    PPE_HEADER_ETHERNET,
    PPE_HEADER_ETHER,
    PPE_HEADER_8021Q,
    PPE_HEADER_OUTER_8021Q,
    PPE_HEADER_INNER_8021Q,
    PPE_HEADER_LLC,
    PPE_HEADER_ETHERII,
    PPE_HEADER_SNAP,
    PPE_HEADER_ARP,
    PPE_HEADER_LLDP,
    PPE_HEADER_IP4,
    PPE_HEADER_IP6,
    PPE_HEADER_L4,
    PPE_HEADER_TCP,
    PPE_HEADER_UDP,
    PPE_HEADER_ICMP,
    PPE_HEADER_SLOW_PROTOCOLS,
    PPE_HEADER_LACP,
    PPE_HEADER_DHCP,
    PPE_HEADER_ETHERTYPE_MISSING,
    PPE_HEADER_OF10,
    PPE_HEADER_LAST = PPE_HEADER_OF10,
    PPE_HEADER_COUNT,
    PPE_HEADER_INVALID = -1,
} ppe_header_t;

/** Strings macro. */
#define PPE_HEADER_STRINGS \
{\
    "META", \
    "ETHERNET", \
    "ETHER", \
    "8021Q", \
    "OUTER_8021Q", \
    "INNER_8021Q", \
    "LLC", \
    "ETHERII", \
    "SNAP", \
    "ARP", \
    "LLDP", \
    "IP4", \
    "IP6", \
    "L4", \
    "TCP", \
    "UDP", \
    "ICMP", \
    "SLOW_PROTOCOLS", \
    "LACP", \
    "DHCP", \
    "ETHERTYPE_MISSING", \
    "OF10", \
}
/** Enum names. */
const char* ppe_header_name(ppe_header_t e);

/** Enum values. */
int ppe_header_value(const char* str, ppe_header_t* e, int substr);

/** Enum descriptions. */
const char* ppe_header_desc(ppe_header_t e);

/** validator */
#define PPE_HEADER_VALID(_e) \
    ( (0 <= (_e)) && ((_e) <= PPE_HEADER_OF10))

/** ppe_header_map table. */
extern aim_map_si_t ppe_header_map[];
/** ppe_header_desc_map table. */
extern aim_map_si_t ppe_header_desc_map[];

/** ppe_ip_protocol */
typedef enum ppe_ip_protocol_e {
    PPE_IP_PROTOCOL_ICMP = 1,
    PPE_IP_PROTOCOL_TCP = 6,
    PPE_IP_PROTOCOL_UDP = 17,
} ppe_ip_protocol_t;

/** Enum names. */
const char* ppe_ip_protocol_name(ppe_ip_protocol_t e);

/** Enum values. */
int ppe_ip_protocol_value(const char* str, ppe_ip_protocol_t* e, int substr);

/** Enum descriptions. */
const char* ppe_ip_protocol_desc(ppe_ip_protocol_t e);

/** Enum validator. */
int ppe_ip_protocol_valid(ppe_ip_protocol_t e);

/** validator */
#define PPE_IP_PROTOCOL_VALID(_e) \
    (ppe_ip_protocol_valid((_e)))

/** ppe_ip_protocol_map table. */
extern aim_map_si_t ppe_ip_protocol_map[];
/** ppe_ip_protocol_desc_map table. */
extern aim_map_si_t ppe_ip_protocol_desc_map[];

/** ppe_pservice_port */
typedef enum ppe_pservice_port_e {
    PPE_PSERVICE_PORT_DHCP_CLIENT = 68,
    PPE_PSERVICE_PORT_DHCP_SERVER = 67,
} ppe_pservice_port_t;

/** Enum names. */
const char* ppe_pservice_port_name(ppe_pservice_port_t e);

/** Enum values. */
int ppe_pservice_port_value(const char* str, ppe_pservice_port_t* e, int substr);

/** Enum descriptions. */
const char* ppe_pservice_port_desc(ppe_pservice_port_t e);

/** Enum validator. */
int ppe_pservice_port_valid(ppe_pservice_port_t e);

/** validator */
#define PPE_PSERVICE_PORT_VALID(_e) \
    (ppe_pservice_port_valid((_e)))

/** ppe_pservice_port_map table. */
extern aim_map_si_t ppe_pservice_port_map[];
/** ppe_pservice_port_desc_map table. */
extern aim_map_si_t ppe_pservice_port_desc_map[];

/** ppe_icmp_typecode */
typedef enum ppe_icmp_typecode_e {
    PPE_ICMP_TYPECODE_ECHO_REPLY = 0,
    PPE_ICMP_TYPECODE_DEST_NETWORK_UNREACHABLE = ( ( 3 << 8) |  0 ),
    PPE_ICMP_TYPECODE_DEST_HOST_UNREACHABLE = ( ( 3 << 8) |  1 ),
    PPE_ICMP_TYPECODE_DEST_PROTOCOL_UNREACHABLE = ( ( 3 << 8) |  2 ),
    PPE_ICMP_TYPECODE_DEST_PORT_UNREACHABLE = ( ( 3 << 8) |  3 ),
    PPE_ICMP_TYPECODE_FRAGMENTATION_REQUIRED = ( ( 3 << 8) |  4 ),
    PPE_ICMP_TYPECODE_SOURCE_ROUTE_FAILED = ( ( 3 << 8) |  5 ),
    PPE_ICMP_TYPECODE_NETWORK_UNKNOWN = ( ( 3 << 8) |  6 ),
    PPE_ICMP_TYPECODE_HOST_UNKNOWN = ( ( 3 << 8) |  7 ),
    PPE_ICMP_TYPECODE_SOURCE_ISOLATED = ( ( 3 << 8) |  8 ),
    PPE_ICMP_TYPECODE_NETWORK_PROHIBITED = ( ( 3 << 8) |  9 ),
    PPE_ICMP_TYPECODE_HOST_PROHIBITED = ( ( 3 << 8) | 10 ),
    PPE_ICMP_TYPECODE_NETWORK_TOS = ( ( 3 << 8) | 11 ),
    PPE_ICMP_TYPECODE_HOST_TOS = ( ( 3 << 8) | 12 ),
    PPE_ICMP_TYPECODE_COM_PROHIBITED = ( ( 3 << 8) | 13 ),
    PPE_ICMP_TYPECODE_HOST_PRECEDENCE = ( ( 3 << 8) | 14 ),
    PPE_ICMP_TYPECODE_PRECEDENCE_CUTOFF = ( ( 3 << 8) | 15 ),
    PPE_ICMP_TYPECODE_SOURCE_QUENCH = ( ( 4 << 8) |  0 ),
    PPE_ICMP_TYPECODE_REDIRECT_NETWORK = ( ( 5 << 8) |  0 ),
    PPE_ICMP_TYPECODE_REDIRECT_HOST = ( ( 5 << 8) |  1 ),
    PPE_ICMP_TYPECODE_REDIRECT_NETWORK_TOS = ( ( 5 << 8) |  2 ),
    PPE_ICMP_TYPECODE_REDIRECT_HOST_TOS = ( ( 5 << 8) |  3 ),
    PPE_ICMP_TYPECODE_ALTERNATE_HOST_ADDRESS = ( ( 6 << 8) |  0 ),
    PPE_ICMP_TYPECODE_ECHO_REQUEST = ( ( 8 << 8) |  0 ),
    PPE_ICMP_TYPECODE_ROUTER_ADVERT = ( ( 9 << 8) |  0 ),
    PPE_ICMP_TYPECODE_ROUTER_SOL = ( (10 << 8) |  0 ),
    PPE_ICMP_TYPECODE_TIME_EXCEEDED = ( (11 << 8) |  0 ),
    PPE_ICMP_TYPECODE_PARAMETER_POINTER = ( (12 << 8) |  0 ),
    PPE_ICMP_TYPECODE_PARAMETER_MISSING = ( (12 << 8) |  1 ),
    PPE_ICMP_TYPECODE_PARAMETER_BAD_LENGTH = ( (12 << 8) |  2 ),
    PPE_ICMP_TYPECODE_TIMESTAMP = ( (13 << 8) |  0 ),
    PPE_ICMP_TYPECODE_TIMESTAMP_REPLY = ( (14 << 8) |  0 ),
    PPE_ICMP_TYPECODE_INFORMATION_REQUEST = ( (15 << 8) |  0 ),
    PPE_ICMP_TYPECODE_INFORMATION_REPLY = ( (16 << 8) |  0 ),
    PPE_ICMP_TYPECODE_ADDRESS_MASK_REQUEST = ( (17 << 8) |  0 ),
    PPE_ICMP_TYPECODE_ADDRESS_MASK_REPLY = ( (18 << 8) |  0 ),
    PPE_ICMP_TYPECODE_TRACEROUTE_INFO_REQUEST = ( (30 << 8) |  0 ),
    PPE_ICMP_TYPECODE_DATAGRAM_CONVERSION = ( (31 << 8) |  0 ),
    PPE_ICMP_TYPECODE_MOBILE_HOST_REDIRECT = ( (32 << 8) |  0 ),
    PPE_ICMP_TYPECODE_WHERE_ARE_YOU = ( (33 << 8) |  0 ),
    PPE_ICMP_TYPECODE_HERE_I_AM = ( (34 << 8) |  0 ),
    PPE_ICMP_TYPECODE_MOBILE_REG_REQUEST = ( (35 << 8) |  0 ),
    PPE_ICMP_TYPECODE_MOBILE_REG_REPLY = ( (36 << 8) |  0 ),
    PPE_ICMP_TYPECODE_DOMAIN_NAME_REQUEST = ( (37 << 8) |  0 ),
    PPE_ICMP_TYPECODE_DOMAIN_NAME_REPLY = ( (38 << 8) |  0 ),
    PPE_ICMP_TYPECODE_SKIP = ( (39 << 8) |  0 ),
    PPE_ICMP_TYPECODE_PHOTURIS = ( (40 << 8) |  0 ),
    PPE_ICMP_TYPECODE_EXPERIMENTAL = ( (41 << 8) |  0 ),
} ppe_icmp_typecode_t;

/** Enum names. */
const char* ppe_icmp_typecode_name(ppe_icmp_typecode_t e);

/** Enum values. */
int ppe_icmp_typecode_value(const char* str, ppe_icmp_typecode_t* e, int substr);

/** Enum descriptions. */
const char* ppe_icmp_typecode_desc(ppe_icmp_typecode_t e);

/** Enum validator. */
int ppe_icmp_typecode_valid(ppe_icmp_typecode_t e);

/** validator */
#define PPE_ICMP_TYPECODE_VALID(_e) \
    (ppe_icmp_typecode_valid((_e)))

/** ppe_icmp_typecode_map table. */
extern aim_map_si_t ppe_icmp_typecode_map[];
/** ppe_icmp_typecode_desc_map table. */
extern aim_map_si_t ppe_icmp_typecode_desc_map[];

/** ppe_ethertype */
typedef enum ppe_ethertype_e {
    PPE_ETHERTYPE_ARP = 2054,
    PPE_ETHERTYPE_IP4 = 2048,
    PPE_ETHERTYPE_IP6 = 34525,
    PPE_ETHERTYPE_LLDP = 35020,
    PPE_ETHERTYPE_SLOW_PROTOCOLS = 34825,
} ppe_ethertype_t;

/** Enum names. */
const char* ppe_ethertype_name(ppe_ethertype_t e);

/** Enum values. */
int ppe_ethertype_value(const char* str, ppe_ethertype_t* e, int substr);

/** Enum descriptions. */
const char* ppe_ethertype_desc(ppe_ethertype_t e);

/** Enum validator. */
int ppe_ethertype_valid(ppe_ethertype_t e);

/** validator */
#define PPE_ETHERTYPE_VALID(_e) \
    (ppe_ethertype_valid((_e)))

/** ppe_ethertype_map table. */
extern aim_map_si_t ppe_ethertype_map[];
/** ppe_ethertype_desc_map table. */
extern aim_map_si_t ppe_ethertype_desc_map[];

/** ppe_log_flag */
typedef enum ppe_log_flag_e {
    PPE_LOG_FLAG_PARSE,
    PPE_LOG_FLAG_FORMAT,
    PPE_LOG_FLAG_LAST = PPE_LOG_FLAG_FORMAT,
    PPE_LOG_FLAG_COUNT,
    PPE_LOG_FLAG_INVALID = -1,
} ppe_log_flag_t;

/** Strings macro. */
#define PPE_LOG_FLAG_STRINGS \
{\
    "parse", \
    "format", \
}
/** Enum names. */
const char* ppe_log_flag_name(ppe_log_flag_t e);

/** Enum values. */
int ppe_log_flag_value(const char* str, ppe_log_flag_t* e, int substr);

/** Enum descriptions. */
const char* ppe_log_flag_desc(ppe_log_flag_t e);

/** validator */
#define PPE_LOG_FLAG_VALID(_e) \
    ( (0 <= (_e)) && ((_e) <= PPE_LOG_FLAG_FORMAT))

/** ppe_log_flag_map table. */
extern aim_map_si_t ppe_log_flag_map[];
/** ppe_log_flag_desc_map table. */
extern aim_map_si_t ppe_log_flag_desc_map[];
/* <auto.end.enum(ALL).header> */



/**************************************************************************//**
 *
 * This structure defines the properties of every packet field.
 *
 *****************************************************************************/

typedef struct ppe_field_info_s {

    /** Field identifier */
    ppe_field_t field;

    /** Header in which this field resides. */
    ppe_header_t header;

    /** The size of this field, in bits. */
    uint32_t size_bits;

    /** The offset to the first byte (in network order) containing data
     * for this field. */
    uint16_t offset_bytes;

    /** The number of bits we must shift to access the field's LSB. */
    uint16_t shift_bits;

} ppe_field_info_t;


/**
 * The field information for all available fields is available
 * for public use in this table.
 */
extern ppe_field_info_t ppe_field_info_table[];


/**************************************************************************//**
 *
 * The base address of every header found in the packet
 * while we are parsing it is stored in this structure.
 *
 *****************************************************************************/
typedef struct ppe_header_data_t {
    /** The base address of this header */
    uint8_t* start;
} ppe_header_data_t;


/**************************************************************************//**
 *
 * Packet data is managed through this structure.
 *
 *****************************************************************************/
typedef struct ppe_packet_s {

    /** The raw packet data */
    uint8_t* data;
    /** The total size of the packet data. */
    uint32_t size;

    /** bitmask of all headers found in the packet while parsing it.
     * see ppe_header_t
     */
    uint32_t header_mask;

    /** Header information for every parsed header that exists in this
     * packet.
     */
    ppe_header_data_t headers[PPE_HEADER_COUNT];

    /** Previous data pointer in the case of buffer reallocations */
    uint8_t* _data;

    /** Internal - used for metaheader data storage */
    uint32_t mh[PPE_FIELD_META_FIELD_COUNT];

    /** Available for client header storage. */
    uint32_t _ch[16];

    /** Internal - used for data copy management. */
    int realloc;

} ppe_packet_t;


/******************************************************************************
 *
 * Dynamic Field Keys
 *
 *
 *****************************************************************************/

/** Dynamic Field Key Header */
typedef struct ppe_dfk_header_s {
    /** The fields that make up this key. */
    const ppe_field_t* fields;
    /** Field count */
    unsigned int fcount;
} ppe_dfk_header_t;

/** Dynamic Field Key instance.  */
typedef struct ppe_dfk_s {
    /** Header for this key */
    ppe_dfk_header_t header;
    /** Mask of valid fields in this key data */
    uint32_t mask;
    /** Key data */
    uint8_t* data;
    /** Key data size */
    uint8_t size;
} ppe_dfk_t;





/** Set the bit corresponding to the given header in the packet's headermask */
#define PPE_PACKET_HEADERBIT_SET(_ppe_packet, _header)  \
    _ppe_packet->header_mask |= (1 << _header)

/** Get the bit corresponding to the given header in the packet's headermask */
#define PPE_PACKET_HEADERBIT_GET(_ppe_packet, _header)  \
    ((_ppe_packet->header_mask >> _header) & 0x1)

/** Clear the bit corresponding to the given header in the packet's headermask */
#define PPE_PACKET_HEADERBIT_CLEAR(_ppe_packet, _header)        \
    (_ppe_packet->header_mask &= ~(1 << _header))

/** Set the base address for the given header in the packet. */
#define PPE_PACKET_HEADER_START_SET(_ppe_packet, _header, _start)       \
    _ppe_packet->headers[_header].start = _start

/** Set the header mask and base address for a given header in the packet. */
#define PPE_PACKET_HEADER_SET(_ppe_packet, _header, _start)             \
    do {                                                                \
        PPE_PACKET_HEADERBIT_SET(_ppe_packet, _header);                 \
        PPE_PACKET_HEADER_START_SET(_ppe_packet, _header, _start);      \
    } while(0)

/** Clear the header mask and base address for a given header in the packet. */
#define PPE_PACKET_HEADER_CLEAR(_ppe_packet, _header)                   \
    do {                                                                \
        PPE_PACKET_HEADERBIT_CLEAR(_ppe_packet, _header);               \
        PPE_PACKET_HEADER_START_SET(_ppe_packet, _header, NULL);        \
    } while(0)


#endif /* __PPE_TYPES_H__ */
/*@}*/
