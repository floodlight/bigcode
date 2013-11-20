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
 * @brief VPI Inband Protocol Definition and Processing.
 *
 * @addtogroup vpi-protocol
 * @{
 *
 ***************************************************************/
#ifndef __VPI_PROTOCOL_H__
#define __VPI_PROTOCOL_H__

#include "vpi_config.h"


/**************************************************************************//**
 *
 * This is the protocol header used for all packets sent over VPI channels.
 * All Fields must be uint32_t.
 *
 *****************************************************************************/
typedef struct vpi_header_s {

    /** The entire size of this message. */
    uint32_t message_size;

    /** Protocol operation opcode */
    uint32_t opcode;

    /** Size of the payload (only for PACKET messages) */
    uint32_t payload_size;

    /** Sequence number of the payload (only for PACKET messages). */
    uint32_t sequence_number;

} vpi_header_t;


/**
 * Encapsulating structure for VPI packet data.
 */
typedef struct vpi_packet_s {
    /** packet data pointer */
    uint8_t* data;
    /** packet data size */
    int size;
} vpi_packet_t;


/**
 * @brief Create a VPI protocol messages.
 * @param hdr The VPI protocol header information.
 * @param data The message data.
 * @param size The size of the message data.
 */
vpi_packet_t*
vpi_protocol_msg_create(vpi_header_t* hdr, uint8_t* data, int size);

/**
 * @brief Free a VPI packet structure.
 * @param p The packet structure.
 */
void
vpi_packet_free(vpi_packet_t* p);

/**
 * @brief Parse an incoming byte stream as a VPI protocol message.
 * @param msg The message stream.
 * @param msg_size The size of the incoming message.
 * @param hdr Receives the VPI protocol header data for this message.
 * @param packet Receives the packet data (for PACKET messages).
 */
int
vpi_protocol_msg_parse(uint8_t* msg, unsigned int msg_size,
                       vpi_header_t* hdr, vpi_packet_t* packet);


#endif /* __VPI_PROTOCOL_H__ */
/*@}*/

