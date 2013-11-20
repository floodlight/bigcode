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
#include <VPI/vpi.h>
#include "vpi_int.h"

#include <VPI/vpi_protocol.h>

#include <stdlib.h>
#include <string.h>

/* <auto.start.enum(ALL).source> */
aim_map_si_t vpi_protocol_opcode_map[] =
{
    { "packet", VPI_PROTOCOL_OPCODE_PACKET },
    { "hello", VPI_PROTOCOL_OPCODE_HELLO },
    { "register_recv_listener", VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER },
    { "unregister_recv_listener", VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER },
    { "register_send_listener", VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER },
    { "unregister_send_listener", VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER },
    { "echo", VPI_PROTOCOL_OPCODE_ECHO },
    { "register_sendrecv_listener", VPI_PROTOCOL_OPCODE_REGISTER_SENDRECV_LISTENER },
    { "unregister_sendrecv_listener", VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER },
    { NULL, 0 }
};

aim_map_si_t vpi_protocol_opcode_desc_map[] =
{
    { "None", VPI_PROTOCOL_OPCODE_PACKET },
    { "None", VPI_PROTOCOL_OPCODE_HELLO },
    { "None", VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER },
    { "None", VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER },
    { "None", VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER },
    { "None", VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER },
    { "None", VPI_PROTOCOL_OPCODE_ECHO },
    { "None", VPI_PROTOCOL_OPCODE_REGISTER_SENDRECV_LISTENER },
    { "None", VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER },
    { NULL, 0 }
};

const char*
vpi_protocol_opcode_name(vpi_protocol_opcode_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, vpi_protocol_opcode_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'vpi_protocol_opcode'";
    }
}

int
vpi_protocol_opcode_value(const char* str, vpi_protocol_opcode_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, vpi_protocol_opcode_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
vpi_protocol_opcode_desc(vpi_protocol_opcode_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, vpi_protocol_opcode_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'vpi_protocol_opcode'";
    }
}


aim_map_si_t vpi_log_flag_map[] =
{
    { "create", VPI_LOG_FLAG_CREATE },
    { "send", VPI_LOG_FLAG_SEND },
    { "recv", VPI_LOG_FLAG_RECV },
    { NULL, 0 }
};

aim_map_si_t vpi_log_flag_desc_map[] =
{
    { "None", VPI_LOG_FLAG_CREATE },
    { "None", VPI_LOG_FLAG_SEND },
    { "None", VPI_LOG_FLAG_RECV },
    { NULL, 0 }
};

const char*
vpi_log_flag_name(vpi_log_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, vpi_log_flag_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'vpi_log_flag'";
    }
}

int
vpi_log_flag_value(const char* str, vpi_log_flag_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, vpi_log_flag_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
vpi_log_flag_desc(vpi_log_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, vpi_log_flag_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'vpi_log_flag'";
    }
}

int
vpi_log_flag_valid(vpi_log_flag_t e)
{
    return aim_map_si_i(NULL, e, vpi_log_flag_map, 0) ? 1 : 0;
}

/* <auto.end.enum(ALL).source> */
