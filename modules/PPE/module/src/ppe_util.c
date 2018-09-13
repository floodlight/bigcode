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

/* fixme */
#include <arpa/inet.h>

uint32_t
sum16__(uint8_t* data, int len)
{
    uint32_t sum = 0;
    uint16_t* sdata = (uint16_t*)data;
    int olen = len;

    while(len > 1) {
        sum += *(sdata++);
        if(sum & 0x80000000) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        len -= 2;
    }

    if(len) {
        uint16_t b = data[olen-1];
        sum += ntohs(b<<8);
    }
    return sum;
}

/**************************************************************************//**
 *
 * 1's compliment summation checksum used for various headers.
 *
 * Three different buffers can be specified, as many calculations
 * involve the checksum over several combined entities.
 *
 *
 *****************************************************************************/
static uint16_t
ip_checksum__(uint8_t *data0, int len0,
              uint8_t* data1, int len1,
              uint8_t* data2, int len2)
{
    uint32_t sum = 0;

    if(data0) {
        sum += sum16__(data0, len0);
    }
    if(data1) {
        sum += sum16__(data1, len1);
    }
    if(data2) {
        sum += sum16__(data2, len2);
    }

    while(sum>>16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    sum = ~sum;
    sum = sum & 0xFFFF;
    return htons(sum);
}

/**************************************************************************//**
 *
 * Recalculate the IP4 Header checksum and update the packet.
 * Returns the new checksum.
 *
 *
 *****************************************************************************/

uint32_t
ppe_ip_header_checksum_update(ppe_packet_t* ppep)
{
    uint8_t* ip_header = ppep->headers[PPE_HEADER_IP4].start;
    int csum = 0;
    /* Only for IP Packets */
    if(ip_header) {
        uint32_t hdr_size = ip_header[0] & 0xF;
        hdr_size *= 4;
        ppe_field_set(ppep, PPE_FIELD_IP4_CHECKSUM, 0);
        csum = ip_checksum__(ip_header, hdr_size,
                             NULL, 0, NULL, 0);
        ppe_field_set(ppep, PPE_FIELD_IP4_CHECKSUM, csum);
    }
    return csum;
}

/**************************************************************************//**
 *
 * Recalculate the TCP/UDP Header checksum and update the packet.
 * Returns the new checksum.
 *
 *
 *****************************************************************************/
int
ppe_pseudoheader_checksum_update(ppe_packet_t* ppep,
                                 uint8_t* header,
                                 uint8_t protocol)
{
    if (header) {
        int csum = 0;
        uint32_t size, pseudo_header_length;
        uint8_t pseudo_header[40] = {0};
        ppe_field_t checksum_field;
        uint8_t* ip_header = ppep->headers[PPE_HEADER_IP4].start;
        uint8_t* ipv6_header = ppep->headers[PPE_HEADER_IP6].start;

        if (protocol == PPE_IP_PROTOCOL_TCP) {
            /* Calculate Header+Payload Size */
            size = ppep->size;
            size -= (header-ppep->data);
            checksum_field = PPE_FIELD_TCP_CHECKSUM;
        }
        else {
            ppe_field_get(ppep, PPE_FIELD_UDP_LENGTH, &size);
            checksum_field = PPE_FIELD_UDP_CHECKSUM;
        }

        /* Only for IPv4 Packets */
        if (ip_header) {
            /* Pseudoheader in network order */
            PPE_MEMCPY(pseudo_header+0,
                       ppe_fieldp_get(ppep, PPE_FIELD_IP4_SRC_ADDR), 4);
            PPE_MEMCPY(pseudo_header+4,
                       ppe_fieldp_get(ppep, PPE_FIELD_IP4_DST_ADDR), 4);
            pseudo_header[8] = 0x00;
            pseudo_header[9] = protocol;
            pseudo_header[10] = (size >> 8);
            pseudo_header[11] = (size);
            pseudo_header_length = 12;
        }

        /* Only for IPv6 Packets */
        if (ipv6_header) {
            uint16_t plen;

            /* Pseudoheader in network order */
            PPE_MEMCPY(pseudo_header+0,
                       ppe_fieldp_get(ppep, PPE_FIELD_IP6_SRC_ADDR), 16);
            PPE_MEMCPY(pseudo_header+16,
                       ppe_fieldp_get(ppep, PPE_FIELD_IP6_DST_ADDR), 16);
            ppe_wide_field_get(ppep, PPE_FIELD_IP6_PAYLOAD_LENGTH, (uint8_t *)&plen);

            *(uint32_t *)(pseudo_header+32) = htonl(ntohs(plen));
            pseudo_header[39] = protocol;
            pseudo_header_length = 40;
        }

        /* Clear existing checksum and compute */
        ppe_field_set(ppep, checksum_field, 0);
        csum = ip_checksum__((uint8_t*)pseudo_header, pseudo_header_length,
                             header, size,
                             NULL, 0);
        ppe_field_set(ppep,
                      checksum_field,
                      csum);
        return csum;
    }
    return -1;
}

uint32_t
ppe_tcp_header_checksum_update(ppe_packet_t* ppep)
{
    return ppe_pseudoheader_checksum_update(ppep,
                                            ppep->headers[PPE_HEADER_TCP].start,
                                            PPE_IP_PROTOCOL_TCP);
}

uint32_t
ppe_udp_header_checksum_update(ppe_packet_t* ppep)
{
    return ppe_pseudoheader_checksum_update(ppep,
                                            ppep->headers[PPE_HEADER_UDP].start,
                                            PPE_IP_PROTOCOL_UDP);
}

uint32_t
ppe_pim_header_checksum_update(ppe_packet_t* ppep)
{
    uint8_t* pim_header = ppep->headers[PPE_HEADER_PIM].start;
    int csum = 0;
    uint32_t ip_hdr, ip_total_len;
    /* PIM HDR (4 bytes) + Variable Size Data */
    uint32_t size = 0;

    /* Only for PIM Packets */
    if(pim_header) {
        /* Calculate the length of PIM Header and Data */
        ppe_field_get(ppep, PPE_FIELD_IP4_HEADER_SIZE, &ip_hdr);
        ip_hdr *= 4;
        ppe_field_get(ppep, PPE_FIELD_IP4_TOTAL_LENGTH, &ip_total_len);
        size = ip_total_len - ip_hdr;

        ppe_field_set(ppep, PPE_FIELD_PIM_CHECKSUM, 0);
        csum = ip_checksum__(pim_header, size, NULL, 0, NULL, 0);
        ppe_field_set(ppep, PPE_FIELD_PIM_CHECKSUM, csum);
    }
    return csum;
}

/**************************************************************************//**
 *
 * Recalculate the ICMP Header checksum and update the packet.
 * Returns the new checksum.
 *
 *
 *****************************************************************************/
uint32_t
ppe_icmp_header_checksum_update(ppe_packet_t* ppep)
{
    uint8_t* icmp_header = ppep->headers[PPE_HEADER_ICMP].start;
    int csum = 0;
    uint32_t ip_hdr, ip_total_len;
    /* ICMP HDR (8 bytes) + Variable Size Data */
    uint32_t size = 0;

    /* Only for ICMP Packets */
    if(icmp_header) {
        /* Calculate the length of ICMP Data */
        ppe_field_get(ppep, PPE_FIELD_IP4_HEADER_SIZE, &ip_hdr);
        ip_hdr *= 4;
        ppe_field_get(ppep, PPE_FIELD_IP4_TOTAL_LENGTH, &ip_total_len);
        size = ip_total_len - ip_hdr;

        ppe_field_set(ppep, PPE_FIELD_ICMP_CHECKSUM, 0);
        csum = ip_checksum__(icmp_header, size, NULL, 0, NULL, 0);
        ppe_field_set(ppep, PPE_FIELD_ICMP_CHECKSUM, csum);
    }
    return csum;
}

int
ppe_build_ipv4_header(ppe_packet_t* ppep, uint32_t src_ip, uint32_t dest_ip,
                      uint32_t total_len, uint32_t proto, uint32_t ttl)
{
    if (!ppep) return -1;

    if (!ppe_header_get(ppep, PPE_HEADER_IP4)) return -1;

    /* Version = 4, HDR_LEN = 5, Type of Service = 0 */
    ppe_field_set(ppep, PPE_FIELD_IP4_VERSION, 4);
    ppe_field_set(ppep, PPE_FIELD_IP4_HEADER_SIZE, 5);
    ppe_field_set(ppep, PPE_FIELD_IP4_TOS, 0);

    ppe_field_set(ppep, PPE_FIELD_IP4_TTL, ttl);
    ppe_field_set(ppep, PPE_FIELD_IP4_TOTAL_LENGTH, total_len);
    ppe_field_set(ppep, PPE_FIELD_IP4_PROTOCOL, proto);
    ppe_field_set(ppep, PPE_FIELD_IP4_SRC_ADDR, src_ip);
    ppe_field_set(ppep, PPE_FIELD_IP4_DST_ADDR, dest_ip);

    /* Update the checksum */
    ppe_packet_update(ppep);

    /* Need to reparse to recognize proto */
    ppe_parse(ppep);

    return 0;
}

int
ppe_build_icmp_packet(ppe_packet_t* ppep, uint32_t type, uint32_t code,
                      uint32_t hdr_data, uint8_t *icmp_data,
                      uint32_t icmp_data_len)
{
    if (!ppep || !icmp_data) return -1;

    if (!ppe_header_get(ppep, PPE_HEADER_ICMP)) return -1;

    ppe_field_set(ppep, PPE_FIELD_ICMP_TYPE, type);
    ppe_field_set(ppep, PPE_FIELD_ICMP_CODE, code);
    ppe_field_set(ppep, PPE_FIELD_ICMP_HEADER_DATA, hdr_data);

    PPE_MEMCPY(ppe_fieldp_get(ppep, PPE_FIELD_ICMP_PAYLOAD), icmp_data,
               icmp_data_len);

    /* Update the checksum */
    ppe_packet_update(ppep);

    return 0;
}

uint32_t
ppe_icmpv6_header_checksum_update(ppe_packet_t* ppep)
{
    uint8_t* icmpv6_header = ppep->headers[PPE_HEADER_ICMPV6].start;
    uint8_t pseudo_header[40] = {0};
    int csum = 0;
    uint32_t ipv6_total_len;
    uint32_t size = 0;
    uint16_t plen;
    uint8_t nxt_hdr;

    /* Only for ICMPV6 Packets */
    if(icmpv6_header) {

        /* Get the length of ICMPV6 Data */
        ppe_field_get(ppep, PPE_FIELD_IP6_PAYLOAD_LENGTH, &ipv6_total_len);
        size = ipv6_total_len;

        /* Pseudoheader in network order */
        PPE_MEMCPY(pseudo_header+0,
                   ppe_fieldp_get(ppep, PPE_FIELD_IP6_SRC_ADDR), 16);
        PPE_MEMCPY(pseudo_header+16,
                   ppe_fieldp_get(ppep, PPE_FIELD_IP6_DST_ADDR), 16);
        ppe_wide_field_get(ppep, PPE_FIELD_IP6_PAYLOAD_LENGTH, (uint8_t *)&plen);
        ppe_wide_field_get(ppep, PPE_FIELD_IP6_NEXT_HEADER, &nxt_hdr);

        *(uint32_t *)(pseudo_header+32) = htonl(ntohs(plen));
        pseudo_header[39] = nxt_hdr;
        ppe_field_set(ppep, PPE_FIELD_ICMPV6_CHECKSUM, 0);

        csum = ip_checksum__((uint8_t *)pseudo_header, sizeof(pseudo_header),
                             icmpv6_header, size, NULL, 0);

        ppe_field_set(ppep, PPE_FIELD_ICMPV6_CHECKSUM, csum);
    }
    return csum;
}

int
ppe_build_ipv6_header(ppe_packet_t* ppep, uint8_t *src_ip, uint8_t *dest_ip,
                      uint32_t total_len, uint32_t hop_limit, uint8_t next_header,
                      uint8_t traffic_class, uint32_t flow_label)
{
    uint8_t traffic_class_u, traffic_class_l, flow_label_u;
    uint16_t flow_label_l;

    if (!ppep) return -1;

    if (!ppe_header_get(ppep, PPE_HEADER_IP6)) return -1;

    traffic_class_u = traffic_class >> 4;
    traffic_class_l = traffic_class & 0xF;
    flow_label_l = flow_label & 0xFFFF;
    flow_label_u = flow_label >> 16;

    /* Version = 6 */
    ppe_field_set(ppep, PPE_FIELD_IP6_VERSION, 6);
    ppe_field_set(ppep, PPE_FIELD_IP6_TRAFFIC_CLASS_U, traffic_class_u);
    ppe_field_set(ppep, PPE_FIELD_IP6_TRAFFIC_CLASS_L, traffic_class_l);
    ppe_field_set(ppep, PPE_FIELD_IP6_FLOW_LABEL_U, flow_label_u);
    ppe_field_set(ppep, PPE_FIELD_IP6_FLOW_LABEL_L, flow_label_l);

    ppe_field_set(ppep, PPE_FIELD_IP6_PAYLOAD_LENGTH, total_len);
    ppe_field_set(ppep, PPE_FIELD_IP6_NEXT_HEADER, next_header);
    ppe_field_set(ppep, PPE_FIELD_IP6_HOP_LIMIT, hop_limit);
    ppe_wide_field_set(ppep, PPE_FIELD_IP6_SRC_ADDR, src_ip);
    ppe_wide_field_set(ppep, PPE_FIELD_IP6_DST_ADDR, dest_ip);

    /* IPv6 header has no checksum */

    /* Need to reparse to recognize proto */
    ppe_parse(ppep);

    return 0;
}

int
ppe_build_icmpv6_packet(ppe_packet_t* ppep, uint32_t type, uint32_t code,
                      uint8_t *icmpv6_msg, uint32_t icmpv6_msg_len)
{
    if (!ppep || !icmpv6_msg) return -1;

    if (!ppe_header_get(ppep, PPE_HEADER_ICMPV6)) return -1;

    ppe_field_set(ppep, PPE_FIELD_ICMPV6_TYPE, type);
    ppe_field_set(ppep, PPE_FIELD_ICMPV6_CODE, code);

    PPE_MEMCPY(ppe_fieldp_get(ppep, PPE_FIELD_ICMPV6_MESSAGE_BODY), icmpv6_msg,
               icmpv6_msg_len);

    /* Update the checksum */
    ppe_packet_update(ppep);

    return 0;
}

int
ppe_build_udp_header(ppe_packet_t* ppep, uint32_t sport, uint32_t dport,
                     uint32_t length)
{
    if (!ppep) return -1;

    if (!ppe_header_get(ppep, PPE_HEADER_UDP)) return -1;

    ppe_field_set(ppep, PPE_FIELD_UDP_SRC_PORT, sport);
    ppe_field_set(ppep, PPE_FIELD_UDP_DST_PORT, dport);
    ppe_field_set(ppep, PPE_FIELD_UDP_LENGTH, length);
    ppe_field_set(ppep, PPE_FIELD_UDP_CHECKSUM, 0);

    /* Update the checksum */
    ppe_packet_update(ppep);

    return 0;
}
