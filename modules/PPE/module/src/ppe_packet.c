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
    return 0;
}

int
ppe_packet_format_get(ppe_packet_t* ppep, ppe_header_t* rv)
{
    if(PPE_PACKET_HEADERBIT_GET(ppep, PPE_HEADER_8021Q)) {
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

    ppe_packet_format_get(ppep, &current);

    PPE_LOG_FORMAT("convert (data=%p,size=%d) from format %s to format %s",
                   ppep->data,
                   ppep->size, ppe_header_name(current), ppe_header_name(type));

    if(current == type) {
        /* Already in the requested format. */
        return 0;
    }
    else {
        switch(current)
            {
            case PPE_HEADER_8021Q:
                {
                    switch(type)
                        {
                        case PPE_HEADER_ETHERII:
                            {
                                PPE_MEMMOVE(ppep->data + 12,
                                            ppep->data + 16,
                                            ppep->size - 16);
                                ppep->size -= 4;
                                break;
                            }
                        default:
                            {
                                AIM_LOG_ERROR("invalid format set (%s)",
                                              ppe_header_name(type));
                                return -1;
                            }
                        }
                    break;
                }
            case PPE_HEADER_ETHERII:
                {
                    switch(type)
                        {
                        case PPE_HEADER_8021Q:
                            {
                                /* @fixme architecture */
                                ppep->size += 4;
                                ppep->_data = ppep->data;
                                ppep->data = aim_zmalloc(ppep->size);

                                PPE_MEMCPY(ppep->data, ppep->_data, 12);
                                PPE_MEMCPY(ppep->data+16, ppep->_data+12, ppep->size-16);

                                ppep->data[12] = 0x81;
                                ppep->data[13] = 0;
                                ppep->data[14] = 0;
                                ppep->data[15] = 0;
                                ppep->realloc = 1;

                                rv = 1;
                                break;
                            }
                        default:
                            {
                                AIM_LOG_ERROR("invalid format set(%s)",
                                              ppe_header_name(type));
                                return -1;
                            }
                        }
                    break;
                }
            default:
                {
                    AIM_LOG_ERROR("cannot convert from current format (%s)",
                              ppe_header_name(type));
                    return -1;
                }
            }

        /**
         * Need to reparse
         */
        ppe_parse(ppep);
        return rv;
    }
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
