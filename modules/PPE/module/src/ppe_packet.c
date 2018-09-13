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
#include "ppe_util.h"
#include "ppe_log.h"

int
ppe_packet_init(ppe_packet_t* ppep, uint8_t* data, int size)
{
    PPE_MEMSET(ppep, 0, sizeof(*ppep));
    ppep->data = data;
    ppep->size = size;
    return 0;
}

void
ppe_packet_denit(ppe_packet_t* ppep)
{
    if(ppep->realloc) {
        aim_free(ppep->data);
        ppep->data = ppep->_data;
    }
}

int
ppe_packet_dup(ppe_packet_t* dst, ppe_packet_t* src)
{
    ppe_packet_init(dst, aim_memdup(src->data, src->size), src->size);
    PPE_MEMCPY(dst->mh, src->mh, sizeof(dst->mh));
    ppe_parse(dst);
    dst->realloc = 1;
    return 0;
}

/**************************************************************************//**
 *
 * Update all known dynamic fields.
 *
 *
 *****************************************************************************/
int
ppe_packet_update(ppe_packet_t* ppep)
{
    /*
     * Attempt to update everything we know about.

     * Each call will simply fail immediately if
     * attempted on an irrelevant packet.
     */
    ppe_ip_header_checksum_update(ppep);
    ppe_tcp_header_checksum_update(ppep);
    ppe_udp_header_checksum_update(ppep);
    ppe_icmp_header_checksum_update(ppep);
    ppe_icmpv6_header_checksum_update(ppep);
    ppe_pim_header_checksum_update(ppep);

    /* FIXME: handle v6 packets */
    return 0;
}

int
ppe_packet_format_get(ppe_packet_t* ppep, ppe_header_t* rv)
{
    if(PPE_PACKET_HEADERBIT_GET(ppep, PPE_HEADER_8021Q2)) {
        *rv = PPE_HEADER_8021Q2;
    }
    else if(PPE_PACKET_HEADERBIT_GET(ppep, PPE_HEADER_8021Q1)) {
        *rv = PPE_HEADER_8021Q1;
    }
    else if(PPE_PACKET_HEADERBIT_GET(ppep, PPE_HEADER_8021Q)) {
        *rv = PPE_HEADER_8021Q;
    }
    else {
        *rv = PPE_HEADER_ETHERII;
    }
    return 0;
}

/*
 * This is clearly an unscaleable approach.
 * If the combinations increase this should be
 * rewritten properly.
 */
int
ppe_packet_format_set(ppe_packet_t* ppep, ppe_header_t type)
{
    int rv = 0;
    ppe_header_t current;
    uint8_t header_data[4];

    ppe_packet_format_get(ppep, &current);

    PPE_LOG_FORMAT("convert (data=%p,size=%d) from format %s to format %s",
                   ppep->data,
                   ppep->size, ppe_header_name(current), ppe_header_name(type));

    if(current == type) {
        /* Already in the requested format. */
        return 0;
    }

    /* convert to base (PPE_HEADER_ETHERII) first */
    switch(current)
        {
        case PPE_HEADER_ETHERII:
            {
                header_data[0] = 0x00;
                header_data[1] = 0x00;
                header_data[2] = 0x00;
                header_data[3] = 0x00;
                break;
            }
        case PPE_HEADER_8021Q:  /* fall-through */
        case PPE_HEADER_8021Q1: /* fall-through */
        case PPE_HEADER_8021Q2:
            {
                int offset;

                offset = (current == PPE_HEADER_8021Q) ? 4 :
                         (current == PPE_HEADER_8021Q1) ? 8 : 12;
                /* save the outer pcp/tag */
                header_data[0] = ppep->data[14];
                header_data[1] = ppep->data[15];
                if(current == PPE_HEADER_8021Q) {
                    header_data[2] = 0x00;
                    header_data[3] = 0x00;
                } else {
                    /* save the 2nd pcp/tag */
                    header_data[2] = ppep->data[18];
                    header_data[3] = ppep->data[19];
                }
                PPE_MEMMOVE(ppep->data + 12,
                            ppep->data + (12 + offset),
                            ppep->size - (12 + offset));
                ppep->size -= offset;
                break;
            }
        default:
            {
                AIM_LOG_ERROR("cannot convert from current format (%s)",
                              ppe_header_name(type));
                return -1;
            }
        }

    /* convert to requested type */
    switch(type)
        {
        case PPE_HEADER_ETHERII:
            {
                /* no-op */
                break;
            }
        case PPE_HEADER_8021Q:  /* fall-through */
        case PPE_HEADER_8021Q1: /* fall-through */
        case PPE_HEADER_8021Q2:
            {
                int offset;
                uint8_t* old_data;

                offset = (type == PPE_HEADER_8021Q) ? 4 :
                         (type == PPE_HEADER_8021Q1) ? 8 : 12;
                ppep->size += offset;
                old_data = ppep->data;
                if(!ppep->realloc) {
                    /* save original */
                    ppep->_data = ppep->data;
                }
                ppep->data = aim_zmalloc(ppep->size);

                PPE_MEMCPY(ppep->data, old_data, 12);
                PPE_MEMCPY(ppep->data + (12 + offset),
                           old_data + 12,
                           ppep->size - (12 + offset));
                /* restore outer pcp/tag */
                ppep->data[12] = 0x81;
                ppep->data[13] = 0x00;
                ppep->data[14] = header_data[0];
                ppep->data[15] = header_data[1];
                if((type == PPE_HEADER_8021Q1) ||
                   (type == PPE_HEADER_8021Q2)) {
                    /* restore 2nd pcp/tag */
                    ppep->data[16] = 0x81;
                    ppep->data[17] = 0x00;
                    ppep->data[18] = header_data[2];
                    ppep->data[19] = header_data[3];

                    if(type == PPE_HEADER_8021Q2) {
                        /* zero-out 3rd pcp/tag */
                        ppep->data[20] = 0x81;
                        ppep->data[21] = 0x00;
                        ppep->data[22] = 0x00;
                        ppep->data[23] = 0x00;
                    }
                }

                if(ppep->realloc) {
                    /* free only if not original */
                    aim_free(old_data);
                }
                ppep->realloc = 1;
                rv = 1;
                break;
            }
        default:
            {
                /* FIXME: no roll-back */
                AIM_LOG_ERROR("invalid format set (%s)",
                              ppe_header_name(type));
                return -1;
            }
        }

    /* Need to reparse */
    ppe_parse(ppep);
    return rv;
}

int
ppe_packet_dump(ppe_packet_t* ppep, aim_pvs_t* pvs)
{
    ppe_header_t h;
    for(h = 0; h < PPE_HEADER_COUNT; h++) {
        if(ppep->headers[h].start) {
            ppe_header_dump(ppep, h, pvs);
        }
    }
    return 0;
}
