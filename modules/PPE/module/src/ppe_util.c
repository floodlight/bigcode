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
        if(*sdata != b) {
            sum += b<<8;
        }
        else {
            sum += (b);
        }
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
    if(header) {
        int csum = 0;
        uint32_t size;
        uint8_t pseudo_header[12];
        ppe_field_t checksum_field;

        if(protocol == PPE_IP_PROTOCOL_TCP) {
            /* Calculate Header+Payload Size */
            size = ppep->size;
            size -= (header-ppep->data);
            checksum_field = PPE_FIELD_TCP_CHECKSUM;
        }
        else {
            ppe_field_get(ppep, PPE_FIELD_UDP_LENGTH, &size);
            checksum_field = PPE_FIELD_UDP_CHECKSUM;
        }

        /* Pseudoheader in network order */
        PPE_MEMCPY(pseudo_header+0,
                   ppe_fieldp_get(ppep, PPE_FIELD_IP4_SRC_ADDR), 4);
        PPE_MEMCPY(pseudo_header+4,
                   ppe_fieldp_get(ppep, PPE_FIELD_IP4_DST_ADDR), 4);
        pseudo_header[8] = 0x00;
        pseudo_header[9] = protocol;
        pseudo_header[10] = (size >> 8);
        pseudo_header[11] = (size);

        /* Clear existing checksum and compute */
        ppe_field_set(ppep, checksum_field, 0);
        csum = ip_checksum__((uint8_t*)pseudo_header, sizeof(pseudo_header),
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
