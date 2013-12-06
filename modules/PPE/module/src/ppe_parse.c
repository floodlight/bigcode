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

#define PPE_LOG_PREFIX1 ".parse"
#include "ppe_log.h"


/**************************************************************************//**
 *
 * Generic data parsing
 *
 *
 *****************************************************************************/

static inline
uint8_t data8__(int skip, int advance, uint8_t** data, int* len)
{
    uint8_t rv;
    if(skip) {
        (*data) += skip;
        (*len) -= skip;
    }
    rv = (*data)[0];
    if(advance) {
        (*data)++;
        (*len)--;
    }
    return rv;
}

static inline
uint16_t data16__(int skip, int advance, uint8_t** data, int* len)
{
    uint16_t rv;
    if(skip) {
        (*data) += skip;
        (*len) -= skip;
    }
    rv = (*data)[0] << 8 | (*data)[1];
    if(advance) {
        (*data)+=2;
        (*len)-=2;
    }
    return rv;
}

static inline
uint32_t data32__(int skip, int advance, uint8_t** data, int* len)
{
    uint32_t rv;
    if(skip) {
        (*data) += skip;
        (*len) -= skip;
    }
    rv = (*data)[0] << 24 | (*data)[1] << 16 |
        (*data)[2] << 8 | (*data)[3];
    if(advance) {
        (*data) += 4;
        (*len)-=4;
    }
    return rv;
}

static inline
uint8_t* datap__(int skip, int advance, int size, uint8_t** data, int* len)
{
    uint8_t* rv;
    if(skip) {
        (*data) += skip;
        (*len) -= skip;
    }
    rv = *data;
    if(advance) {
        (*data)+=size;
        (*len)-=size;
    }
    return rv;
}


/**************************************************************************//**
 *
 * Parse packet headers.
 *
 *
 *****************************************************************************/
static inline int
ppe_parse_dhcp(ppe_packet_t* ppep, uint16_t sport, uint16_t dport,
               uint8_t* data, int size)
{
    uint32_t cookie;

    AIM_REFERENCE(sport);
    AIM_REFERENCE(dport);
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_DHCP, data);

    /*
     * Verify DHCP or BOOTP.
     */
#define DHCP_MAGIC_COOKIE 0x63825363

    ppe_field_get(ppep, PPE_FIELD_DHCP_MAGIC, &cookie);

    if(cookie == DHCP_MAGIC_COOKIE) {
        /* Possible options parse */
    }
    else {
        /* BOOTP */
    }
    return 0;
}


static inline int
ppe_parse_service_port_DHCP_CLIENT(ppe_packet_t* ppep,
                                   uint16_t sport, uint16_t dport,
                                   uint8_t* data, int size)
{
    return ppe_parse_dhcp(ppep, sport, dport, data, size);
}

static inline int
ppe_parse_service_port_DHCP_SERVER(ppe_packet_t* ppep,
                                   uint16_t sport, uint16_t dport,
                                   uint8_t* data, int size)
{
    return ppe_parse_dhcp(ppep, sport, dport, data, size);
}


static inline int
ppe_parse_service_ports(ppe_packet_t* ppep, uint8_t* data, int size)
{
    uint32_t sport;
    uint32_t dport;

    ppe_field_get(ppep, PPE_FIELD_L4_SRC_PORT, &sport);
    ppe_field_get(ppep, PPE_FIELD_L4_DST_PORT, &dport);

    switch(sport)
        {
#define PPE_PSERVICE_PORT_ENTRY(_name, _value)                          \
            case PPE_PSERVICE_PORT_##_name:                             \
                return ppe_parse_service_port_##_name(ppep, sport, dport, data, size);
#include <PPE/ppe.x>
        default:
            break;
        }

    return 0;
}

static inline int
ppe_parse_ip_protocol_TCP__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_TCP, data);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_L4, data);
    return 0;
}
static inline int
ppe_parse_ip_protocol_UDP__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_UDP, data);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_L4, data);
    return ppe_parse_service_ports(ppep, data+8, size-8);
}
static inline int
ppe_parse_ip_protocol_ICMP__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ICMP, data);
    return 0;
}

static inline int
ppe_parse_ip_protocol(ppe_packet_t* ppep, uint8_t protocol,
                      uint8_t* data, int size)
{
    switch(protocol)
        {
#define PPE_IP_PROTOCOL_ENTRY(_proto, _value)                           \
            case PPE_IP_PROTOCOL_##_proto:                              \
                return ppe_parse_ip_protocol_##_proto##__ (ppep, data, size);
#include <PPE/ppe.x>
        default: return 0;
        }
}

static inline int
ppe_parse_ethertype_ARP__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ARP, data);
    return 0;
}

static inline int
ppe_parse_ethertype_LLDP__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_LLDP, data);
    return 0;
}


static inline int
ppe_parse_slow_protocol_LACP__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    AIM_REFERENCE(size);
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_LACP, data);
    return 0;
}

static inline int
ppe_parse_ethertype_SLOW_PROTOCOLS__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    uint32_t subtype;

    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_SLOW_PROTOCOLS, data);
    ppe_field_get(ppep, PPE_FIELD_SLOW_PROTOCOLS_SUBTYPE, &subtype);

    switch(subtype)
        {
#define PPE_SLOW_PROTOCOL_ENTRY(_proto, _value) \
            case PPE_SLOW_PROTOCOL_##_proto:                            \
                return ppe_parse_slow_protocol_##_proto##__(ppep, data+1, size-1);
#include <PPE/ppe.x>
        default:
            {
                AIM_LOG_WARN("Slow Protocol Subtype 0x%x is not recognized.", subtype);
            }
        }
    return 0;
}

static inline int
ppe_parse_ethertype_IP4__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    int hsize = data[0] & 0xF;
    int hoffset;

    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_IP4, data);

    if(hsize < 5) {
        /* Invalid header size -- stop parsing */
        return 0;
    }

    /*
     * Protocol byte is at offset 9.
     * Next header offset is hsize*4 bytes
     */
    hoffset = hsize*4;
    return ppe_parse_ip_protocol(ppep, data[9], data+hoffset, size-hoffset);
}

static inline int
ppe_parse_ethertype_IP6__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_IP6, data);

    /* The nextheader is at offset 6 */
    return ppe_parse_ip_protocol(ppep, data[6], data+40, size-40);
}

static inline int
ppe_parse_ethertype__(ppe_packet_t* ppep, uint16_t etype,
                      uint8_t* data, int size)
{
    switch(etype)
        {

#define PPE_ETHERTYPE_ENTRY(_type, _value)                              \
            case PPE_ETHERTYPE_##_type: return ppe_parse_ethertype_##_type##__ (ppep, data, size);
#include <PPE/ppe.x>
        default:
            /* Unknown type */
            return 0;
        }
}

static inline int
ppe_parse_snap__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    uint32_t oui;
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_SNAP, data);
    ppe_field_get(ppep, PPE_FIELD_SNAP_OUI, &oui);
    if(oui == 0) {
        /* Protocol is Ethertype */
        uint32_t etype;
        ppe_field_get(ppep, PPE_FIELD_SNAP_PROTOCOL, &etype);
        PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ETHER, data+3);
        return ppe_parse_ethertype__(ppep, etype, data+5, size-5);
    }
    else {
        PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ETHERTYPE_MISSING, NULL);
    }
    return 0;
}
static inline int
ppe_parse_llc__(ppe_packet_t* ppep, uint8_t* data, int size)
{
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_LLC, data);
    if(data[2] == 0xAA || data[2] == 0xAB || data[3] == 0xAA || data[3] == 0xAB) {
        /* SNAP -- skip LLC header and continue */
        return ppe_parse_snap__(ppep, data+5, size-5);
    }
    else {
        /*
         * Indicate that this frame does not contain ethertype information.
         *
         * This is done explicitly so the headermask can be masked with
         * a value to determine this, rather than checking for the absence
         * of the PPE_HEADER_ETHER bit.
         */
        PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ETHERTYPE_MISSING, NULL);
    }
    return 0;
}

int
ppe_parse(ppe_packet_t* ppep)
{
    uint8_t* data;
    int size;
    uint16_t data16;

    if(ppep == NULL || ppep->data == NULL || ppep->size < 14) {
        return -1;
    }

    data = ppep->data;
    size = ppep->size;


    /*
     * Reset header information
     */
    ppep->header_mask = 0;
    PPE_MEMSET(&ppep->headers, 0, sizeof(ppep->headers));


    /*
     * All packets have meta information
     */
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_META, (uint8_t*)ppep->mh);
    ppe_field_set(ppep, PPE_FIELD_META_PACKET_LENGTH, ppep->size);

    /*
     * It is assumed the packet is ethernet...
     */
    PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ETHERNET, data);

    /* Skip 12 bytes and check the Ethertype/Len/TPID field */
    data16 = data16__(12, 0, &data, &size);

    if(data16 == 0x8100) {
        PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_8021Q, data);
        /* Skip the tag and get the ethertype/len field */
        data16 = data16__(4, 0, &data, &size);
        ppe_field_set(ppep, PPE_FIELD_META_PACKET_FORMAT, PPE_HEADER_8021Q);
    }
    else {
        /* @fixme */
        ppe_field_set(ppep, PPE_FIELD_META_PACKET_FORMAT, PPE_HEADER_ETHERII);
    }

    if(data16 >= 0x600) {
        /* EtherII */
        PPE_PACKET_HEADER_SET(ppep, PPE_HEADER_ETHER, data);
        return ppe_parse_ethertype__(ppep, data16, data+2, size-2);
    }
    else {
        /* 802.3, assumed LLC */
        /* Note -- our LLC header includes the 802.3 length field */
        return ppe_parse_llc__(ppep, data, size);
    }
}


