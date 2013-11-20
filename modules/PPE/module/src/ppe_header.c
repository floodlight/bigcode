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

int
ppe_header_exists(ppe_packet_t* ppep, ppe_header_t header)
{
    return ppep->headers[header].start != NULL;
}

uint8_t*
ppe_header_get(ppe_packet_t* ppep, ppe_header_t header)
{
    return ppep->headers[header].start;
}

int
ppe_header_set(ppe_packet_t* ppep, ppe_header_t header,
               uint8_t* start)
{
    if(start) {
        PPE_PACKET_HEADER_SET(ppep, header, start);
    }
    else {
        PPE_PACKET_HEADER_CLEAR(ppep, header);
    }
    return 0;
}

int
ppe_field_dump(ppe_packet_t* ppep, ppe_field_t field, aim_pvs_t* pvs)
{
    ppe_field_info_t* fi = ppe_field_info_table+field;
    return ppe_field_dump_header(ppep->headers[fi->header].start,
                                 field, pvs);
}

int
ppe_field_dump_header(uint8_t* start, ppe_field_t field, aim_pvs_t* pvs)
{
    ppe_field_info_t* fi = ppe_field_info_table+field;
    if(start == NULL) {
        return -1;
    }
    else {
        aim_printf(pvs, "%s=", ppe_field_name(field));
        if(fi->size_bits <= 32) {
            uint32_t v;
            ppe_field_get_header(start, field, &v);
            if(fi->size_bits <= 8) {
                aim_printf(pvs, "0x%.2x", v);
            }
            else if(fi->size_bits <= 16) {
                aim_printf(pvs, "0x%.4x", v);
            }
            else {
                aim_printf(pvs, "0x%.8x", v);
            }
        }
        else {
            unsigned int i;
            uint8_t* p = ppe_fieldp_get_header(start, field);
            for(i = 0; i < fi->size_bits/8; i++) {
                aim_printf(pvs, "0x%.2x ", p[i]);
            }
        }
        aim_printf(pvs, " ");
    }
    return 0;
}


int
ppe_header_dump(ppe_packet_t* ppep, ppe_header_t header, aim_pvs_t* pvs)
{
    return ppe_header_dump_header(ppep->headers[header].start, header, pvs);
}

int
ppe_header_dump_header(uint8_t* p, ppe_header_t header, aim_pvs_t* pvs)
{
    int i;
    ppe_field_info_t* fi;

    if(p == NULL) {
        return -1;
    }

    for(i = 0, fi = ppe_field_info_table; i < PPE_FIELD_COUNT; i++, fi++) {
        if(fi->header == header) {
            ppe_field_dump_header(p, fi->field, pvs);
        }
    }
    aim_printf(pvs, "\n");
    return 0;
}

