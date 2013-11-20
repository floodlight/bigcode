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

#include <VPI/vpi_config.h>
#include <VPI/vpi_protocol.h>

#define VPI_LOG_PREFIX1 ".protocol"

#include "vpi_int.h"
#include "vpi_log.h"

#include <arpa/inet.h>

static void
vpi_hdr_ntohl__(vpi_header_t* dst, vpi_header_t* src)
{
    unsigned int i;
    uint32_t* p_dst = (uint32_t*)dst;
    uint32_t* p_src = (uint32_t*)src;

    for(i = 0; i < sizeof(vpi_header_t)/sizeof(uint32_t); i++) {
        p_dst[i] = ntohl(p_src[i]);
    }
}

static void
vpi_hdr_htonl__(vpi_header_t* dst, vpi_header_t* src)
{
    unsigned int i;
    uint32_t* p_dst = (uint32_t*)dst;
    uint32_t* p_src = (uint32_t*)src;

    for(i = 0; i < sizeof(vpi_header_t)/sizeof(uint32_t); i++) {
        p_dst[i] = htonl(p_src[i]);
    }
}


vpi_packet_t*
vpi_protocol_msg_create(vpi_header_t* hdr, uint8_t* data, int data_size)
{
    vpi_packet_t* p;

    if( (p = aim_zmalloc(sizeof(*p))) == NULL) {
        VPI_MERROR("msgCreate allocation failed.");
        return NULL;
    }

    if( (p->data = aim_zmalloc(data_size + sizeof(*hdr))) == NULL) {
        VPI_MERROR("msgCreate data allocation failed.");
        aim_free(p);
        return NULL;
    }

    hdr->payload_size = data_size;
    hdr->message_size = data_size + sizeof(*hdr);
    vpi_hdr_htonl__((vpi_header_t*)p->data, hdr);

    if(data_size) {
        VPI_MEMCPY(p->data + sizeof(vpi_header_t), data, data_size);
    }
    p->size = data_size + sizeof(vpi_header_t);
    return p;
}

void
vpi_packet_free(vpi_packet_t* p)
{
    if(p) {
        if(p->data) {
            aim_free(p->data);
        }
        aim_free(p);
    }
}


int
vpi_protocol_msg_parse(uint8_t* msg, unsigned int msg_size,
                    vpi_header_t* hdr, vpi_packet_t* packet)
{
    if(hdr == NULL) {
        VPI_MERROR("header is null.");
        return -1;
    }

    if(msg_size < sizeof(*hdr)) {
        unsigned int i;
        VPI_MERROR("runt protocol msg.");
        for(i = 0; i < msg_size; i++) {
            VPI_MERROR("byte%d: 0x%.2x (%d)", i, msg[i], msg[i]);
        }
        return -1;
    }


    vpi_hdr_ntohl__(hdr, (vpi_header_t*)msg);

    /*
     * Correct message size?
     */
    if(hdr->message_size != msg_size) {
        VPI_MERROR("Message size mismatch. specified size was %d, actual size "
                  "was %d.", hdr->message_size, msg_size);
        return -1;
    }

    /*
     * Correct header and payload size?
     */
    if( (msg_size - sizeof(*hdr)) != hdr->payload_size) {
        VPI_MERROR("payload size mismatch. Specified=%d, calculated=%d",
                   hdr->payload_size, (msg_size - sizeof(*hdr)));
        return -1;
    }

    packet->data = msg+sizeof(*hdr);
    packet->size = hdr->payload_size;

    return 0;
}
