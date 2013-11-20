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
#include "vpi_parser.h"
#include <VPI/vpi_protocol.h>
#include <BigList/biglist.h>
#include <BigList/biglist_locked.h>
#include "vpi_interface_queue.h"
#include "vpi_interface_udp.h"
#include "vpi_interface_tcp.h"
#include "vpi_interface_veth.h"
#include "vpi_interface_bpp.h"
#include "vpi_interface_loopback.h"
#include "vpi_interface_pcap.h"
#include "vpi_interface_pcapdump.h"
#include "vpi_interface_tap.h"
#include "vpi_interface_dump.h"
#include "vpi_log.h"

static vpi_t vpi_create__(const char* create_spec);
static int vpi_destroy__(vpi_t vpi);

/**************************************************************************//**
 *
 * VPI Module Initialization
 *
 *
 *****************************************************************************/

/*
 * List of all active VPI instances.
 *
 * Managed by vpi_create, vpi_destroy, and vpi_close.
 */
biglist_locked_t* vpi_instances__;

int
vpi_init(void)
{
    AIM_RUN_ONCE(return 0);

    AIM_LOG_STRUCT_REGISTER();

    /* Initialize and register all builtin interface types */
#if VPI_CONFIG_INCLUDE_INTERFACE_QUEUE == 1
    vpi_queue_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1
    vpi_udp_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_VETH == 1
    vpi_veth_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_BIGPATCHPANEL == 1
    vpi_bpp_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK == 1
    vpi_loopback_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_TCP == 1
    vpi_tcp_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_PCAP == 1
    vpi_pcap_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP == 1
    vpi_pcapdump_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_BPP == 1
    vpi_bpp_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_DUMP == 1
    vpi_dump_interface_register();
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_TAP == 1
    vpi_tap_interface_register();
#endif

    vpi_instances__ = biglist_locked_create();

#if VPI_CONFIG_INCLUDE_ATEXIT == 1
    atexit(vpi_close);
#endif



    return 0;
}

static int
vpi_destroy_locked_list__(biglist_locked_t* ll)
{
    /*
     * Destroy all VPI interfaces in the list
     */
    biglist_t* ble;
    biglist_lock(ll);
    for(ble = ll->list; ble; ble = biglist_next(ble)) {
        BIGLIST_GET_DATA(vpi_t, vpi, ble);
        vpi_destroy__(vpi);
    }
    biglist_free(ll->list);
    ll->list = NULL;
    biglist_unlock(ll);
    return 0;
}

/**************************************************************************//**
 *
 * VPI Interface Management
 *
 *
 *****************************************************************************/
typedef struct vpi_interface_ctrl_s {

    /** Registered Type */
    char type[32];

    /** Registered creator */
    vpi_interface_create_f creator;

    /** Preprocessor */
    vpi_interface_preprocess_f preprocessor;

    /** Interface doc string */
    const char* doc_string;

} vpi_interface_ctrl_t;


static biglist_t* vpi_interface_list__ = NULL;

static vpi_interface_ctrl_t*
vpi_interface_find__(const char* type)
{
    biglist_t* ble;
    for(ble = vpi_interface_list__; ble; ble = biglist_next(ble)) {
        BIGLIST_GET_DATA(vpi_interface_ctrl_t*, c, ble);
        if(!VPI_STRCMP(c->type, type)) {
            return c;
        }
    }
    return NULL;
}


int
vpi_interface_register(const char* type,
                     vpi_interface_create_f creator,
                     vpi_interface_preprocess_f preprocessor,
                     const char* doc_string)


{
    vpi_interface_ctrl_t* c = vpi_interface_find__(type);

    if(c) {
        /*
         * Already registered. If this is a trivial re-registration of
         * the same creator then return success, otherwise failure.
         */
        if(c->creator == creator && c->preprocessor == preprocessor) {
            return 0;
        }
        else {
            VPI_MWARN("Failing registration for '%s' because it already exists"
                     " with a different provider.", type);
            return -1;
        }
    }
    else {
        /*
         * Register new provider
         */
        if(creator == NULL && preprocessor == NULL) {
            VPI_MWARN("Attempt to register NULL creator/preprocessor for type '%s'",
                     type);
            return -1;
        }

        c = aim_zmalloc(sizeof(*c));
        aim_strlcpy(c->type, type, sizeof(c->type));
        c->creator = creator;
        c->preprocessor = preprocessor;
        c->doc_string = doc_string;
        if(c->doc_string == NULL) {
            VPI_MWARN("type '%s' did not register a doc_string. Please fix.",
                      type);
        }
        vpi_interface_list__ = biglist_prepend(vpi_interface_list__, c);
        return 0;
    }
}

int
vpi_interface_unregister(const char* type)
{
    vpi_interface_ctrl_t* c = vpi_interface_find__(type);

    if(c == NULL) {
        VPI_MWARN("Attempt to deregister type '%s' but it is not currently register.", type);
        return 0;
    }

    vpi_interface_list__ = biglist_remove(vpi_interface_list__, c);

    if(vpi_interface_find__(type) != NULL) {
        VPI_MERROR("Interface removal failed.");
        return -1;
    }

    aim_free(c);
    return 0;
}





/**************************************************************************//**
 *
 * Create VPI Instance Objects.
 *
 *
 *****************************************************************************/

static int
vpi_process_directives__(vpi_t vpi, const char* directives)
{
    int rv;
    const char* cp;
    char c;

    if(directives == NULL) {
        return 0;
    }

    for(cp = directives; (c = *cp); cp++) {

        /* Common error conditions */
        if(c == 's' || c == 'r' || c == 'R' || c == 'S') {

            /*
             * Some directives require protocol processing
             */
            if(!(vpi->interface->flags & VPI_INTERFACE_FLAG_PROTOCOL)) {
                VPI_ERROR(vpi, "Create directive '%c' cannnot be performed "
                          "because this VPI instance does not support "
                          "the VPI protocol.");
                return -1;
            }

            /*
             * Some directives require the 'send_to_spec' on the
             * interface.
             */
            if(vpi->interface->send_to_spec[0] == 0) {
                VPI_ERROR(vpi, "Create directive '%c' cannot be performed "
                          "because thie VPI instance does not support "
                          "a return specification.");
                return -1;
            }
        }


        /* Individual directives */

        if(c == 's' || c == 'S') {
            /*
             * Register as a send listener with the remote
             * endpoint.
             */
            VPI_INFO(vpi, "Registering Send Listener '%s'",
                     vpi->interface->send_to_spec);

            rv = vpi_ioctl(vpi,
                          VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER,
                          (uint8_t*)vpi->interface->send_to_spec,
                          VPI_STRLEN(vpi->interface->send_to_spec)+1);
            if(rv < 0) {
                VPI_ERROR(vpi, "vpi_ioctl() returned %d", rv);
                return rv;
            }

            vpi->flags |= (1<<VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER);

            if(c == 's') {
                vpi->flags |= (1<<VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER);
            }
            else {
                /*
                 * 'S' instead of 's' means "don't unregister on
                 * destroy".
                 */
            }
        }
        else if(c == 'r' || c == 'R') {
            /*
             * Register as a recv listener with the remote
             * endpoint.
             */
            VPI_INFO(vpi, "Registering Recv Listener '%s'",
                     vpi->interface->send_to_spec);

            rv = vpi_ioctl(vpi,
                          VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER,
                          (uint8_t*)vpi->interface->send_to_spec,
                          VPI_STRLEN(vpi->interface->send_to_spec)+1);

            if(rv < 0) {
                VPI_ERROR(vpi, "vpi_ioctl(RecvListener) returned %d",
                          rv);
                return rv;
            }

            vpi->flags |= (1<<VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER);
            if(c == 'r') {
                vpi->flags |= (1<<VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER);
            }
            else {
                /*
                 * 'R' instead of 'r' means "dont unregister on
                 * destroy".
                 */
            }
        }
        else if(c == 'x' || c == 'X') {
            /*
             * Register as a sendrecv listener with the remote endpoint.
             */
            VPI_INFO(vpi, "Registering SendRecv Listener '%s'",
                     vpi->interface->send_to_spec);
            rv = vpi_ioctl(vpi,
                           VPI_PROTOCOL_OPCODE_REGISTER_SENDRECV_LISTENER,
                           (uint8_t*)vpi->interface->send_to_spec,
                           VPI_STRLEN(vpi->interface->send_to_spec)+1);
            if(rv < 0) {
                VPI_ERROR(vpi, "vpi_ioctl(SendRecvListener) returnd %d",
                          rv);
                return rv;
            }
            vpi->flags |= (1<<VPI_PROTOCOL_OPCODE_REGISTER_SENDRECV_LISTENER);
            if(c == 'x') {
                vpi->flags |= (1<<VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER);
            }
            else {
                /*
                 * 'X' instead of 'x' means "don't unregister on destroy".
                 */
            }
        }
        else {
            VPI_ERROR(vpi, "unknown directive '%c'", c);
            return -1;
        }
    }

    return 0;
}

vpi_t
vpi_create(const char* create_spec)
{
    vpi_t vpi = vpi_create__(create_spec);
    if (vpi != NULL) {
        biglist_locked_prepend(vpi_instances__, vpi);
    }
    return vpi;
}

static vpi_t
vpi_create__(const char* create_spec)
{
    int rv;
    vpi_interface_ctrl_t* ic;
    vpi_t nvpi = NULL;
    char** args = NULL;
    char** parse_array = NULL;
    char* arg;
    char* directives = NULL;
    char* name = NULL;

    VPI_LOG_MOD_CREATE("%s", create_spec);

    /*
     * Tokenize the create specification
     */
    parse_array = vpi_parse_array(create_spec, "|", NULL);
    args = parse_array;

    if (!args || args[0] == NULL) {
        VPI_MERROR("Failed to parse spec");
        goto vpi_create_Error;
    }

    /*
     * Is the first argument a VPI type or a directive?
     */
    arg = args[0];

    if(arg[0] == '*') {
        directives = arg+1;
        args++;
        arg = args[0];
    }

    if(arg[0] == '%') {
        name = arg+1;
        args++;
        arg = args[0];
    }

    if( (ic = vpi_interface_find__(arg)) == NULL) {
        VPI_MERROR("No interface available for type '%s'", arg);
        goto vpi_create_Error;
    }

    /* Is this a preprocessor interface? */
    if(ic->preprocessor) {
        char* new_spec = ic->preprocessor(args, create_spec);
        if(new_spec) {
            vpi_t rv = vpi_create__(new_spec);
            aim_free(new_spec);
            vpi_parse_arrayFree(parse_array);
            return rv;
        }
        else {
            VPI_MERROR("Preprocessor for type '%s' failed.",
                       arg);
        }
    }

    if( (nvpi = aim_zmalloc(sizeof(*nvpi))) == NULL) {
        VPI_MERROR("VPI allocation failed.");
        goto vpi_create_Error;
    }

    nvpi->log_string = nvpi->create_spec;

    VPI_STRCPY(nvpi->create_spec, create_spec);

    /*
     * Initialize the name of this VPI to something useful:
     */
    aim_snprintf(nvpi->name, sizeof(nvpi->name), "%s", nvpi->create_spec);

    nvpi->recv_listeners = biglist_locked_create();
    nvpi->send_listeners = biglist_locked_create();
    nvpi->sendrecv_listeners = biglist_locked_create();

    if ((rv = ic->creator(&nvpi->interface, args,
                          0, nvpi->name)) == 0) {

        if(nvpi->interface == NULL) {
            /* Creator returned success, but did not provide
             * the interface pointer
             */
            VPI_ERROR(nvpi, "creator for type '%s' did not return an "
                      "interface pointer.", arg);
            goto vpi_create_Error;
        }

        VPI_STRCPY(nvpi->type, arg);
        nvpi->interface->parent = nvpi;

        if(vpi_process_directives__(nvpi, directives) < 0) {
            /* Error already logged */
            goto vpi_create_Error;
        }
    }
    else {
        goto vpi_create_Error;
    }

    if(name) {
        vpi_name_set(nvpi, name);
    }

    vpi_parse_arrayFree(parse_array);
    nvpi->ref_count = 1;

    VPI_LOG_MOD_CREATE("name=%s created.", vpi_name_get(nvpi));
    return nvpi;

 vpi_create_Error:
    vpi_parse_arrayFree(parse_array);
    if (nvpi != NULL) {
        VPI_ERROR(nvpi, "VPI creation failed.");
        vpi_destroy(nvpi);
    }
    return NULL;
}

/**************************************************************************//**
 *
 * Send to a list of VPIs
 *
 *
 *****************************************************************************/
static int
vpi_send_list__(biglist_locked_t* ll, uint8_t* data, uint32_t len, int flag)
{
    biglist_t* ble;
    biglist_lock(ll);
    for(ble = ll->list; ble; ble = biglist_next(ble)) {
        BIGLIST_GET_DATA(vpi_t, vpi, ble);
        vpi->interface->flags |= flag;
        vpi_send(vpi, data, len);
        vpi->interface->flags &= ~flag;
    }
    biglist_unlock(ll);
    return 0;
}

/******************************************************************************
 *
 * Send a packet on a VPI
 *
 *
 *****************************************************************************/
int
vpi_send(vpi_t vpi, uint8_t* data, uint32_t len)
{
    int rv = 0;
    if(vpi) {

        if(AIM_LOG_CUSTOM_ENABLED(VPI_LOG_FLAG_SEND)) {
            char* s = aim_bytes_to_string(data, len, 0);
            VPI_LOG_SEND(vpi, "%s", s);
            AIM_FREE(s);
        }

        if(vpi->interface->send) {

            if(vpi->interface->flags & VPI_INTERFACE_FLAG_PROTOCOL) {

                vpi_packet_t* p = NULL;
                vpi_header_t hdr;

                VPI_INFO(vpi, "Protocol Send.");

                /*
                 * This VPI interface implementation supports
                 * the VPI protocol.
                 */

                VPI_MEMSET(&hdr, 0, sizeof(hdr));
                hdr.opcode = VPI_PROTOCOL_OPCODE_PACKET;
                hdr.payload_size = len;
                hdr.sequence_number = 0;

                p = vpi_protocol_msg_create(&hdr, data, len);
                rv = vpi->interface->send(vpi->interface, p->data, p->size);
                vpi_packet_free(p);
            }
            else {

                VPI_INFO(vpi, "Raw Send.");
                /*
                 * This VPI interface does not support the VPI protocol
                 * header. We can only send raw packet data.
                 */
                rv = vpi->interface->send(vpi->interface, data, len);
            }
        }
        /*
         * Sending to listeners even if this is a recv-only endpoint
         * is intentionally supported. Allows permanent recv-only endpoints
         * with transient peers.
         */
        if(rv >= 0 && vpi->send_listeners->list) {
            vpi_send_list__(vpi->send_listeners, data, len,
                            VPI_INTERFACE_FLAG_SEND_LISTENING);
        }
        if(rv >= 0 && vpi->sendrecv_listeners->list) {
            vpi_send_list__(vpi->sendrecv_listeners, data, len,
                            VPI_INTERFACE_FLAG_SEND_LISTENING);
        }
        return rv;
    }
    else {
        return -1;
    }
}

/******************************************************************************
 *
 * Send an ioctl message on interfaces that support it.
 *
 *
 *****************************************************************************/
int
vpi_ioctl(vpi_t vpi, vpi_protocol_opcode_t cmd, uint8_t* data, int len)
{
    int rv;

    if(vpi == NULL || vpi->interface == NULL) {
        rv = -1;
    }
    else if( (vpi->interface->flags & VPI_INTERFACE_FLAG_PROTOCOL) == 0) {
        /* Interface does not support the VPI protocol. */
        VPI_WARN(vpi, "vpi_ioctl() not supported.");
        rv = -1;
    }
    else {
        vpi_packet_t* p = NULL;
        vpi_header_t hdr;

        VPI_MEMSET(&hdr, 0, sizeof(hdr));
        hdr.opcode = cmd;
        p = vpi_protocol_msg_create(&hdr, data, len);
        rv = vpi->interface->send(vpi->interface, p->data, p->size);
        vpi_packet_free(p);
    }

    return rv;
}


static int
vpi_handle_ioctl__(vpi_t vpi, vpi_header_t* hdr, vpi_packet_t* packet)
{
    int rv = -1;
    const char* create_spec = NULL;

    VPI_INFO(vpi, "ioctl: %s", vpi_protocol_opcode_name(hdr->opcode));


    if(hdr->opcode == VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER ||
       hdr->opcode == VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER ||
       hdr->opcode == VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER ||
       hdr->opcode == VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER) {

        /*
         * Processing a registration. Give the
         * interface a gander at it.
         */
        if(vpi->interface->filter_create_spec) {
            create_spec = vpi->interface->filter_create_spec(vpi->interface,
                                                          (const char*)packet->data);
        }
        else {
            /* no changes */
            create_spec = (const char*)packet->data;
        }
        VPI_TRACE(vpi, "ListenerSpec is '%s'", create_spec);
    }


    switch(hdr->opcode)
        {
        case VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER:
            {
                /*
                 * The data contains the create_spec.
                 */
                rv = vpi_add_recv_listener_spec(vpi, create_spec);

                if(rv < 0) {
                    VPI_ERROR(vpi, "could not add recv listener '%s'",
                              create_spec);
                }
                break;
            }
        case VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER:
            {
                /*
                 * The data contains the create_spec.
                 */
                rv = vpi_remove_recv_listener_spec(vpi, create_spec);
                if(rv < 0) {
                    VPI_ERROR(vpi, "could not remove recv listener '%s'",
                              create_spec);
                }
                break;
            }
        case VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER:
            {
                /* The data contains the create spec */
                rv = vpi_add_send_listener_spec(vpi, create_spec);
                if(rv < 0) {
                    VPI_ERROR(vpi, "could not add send listener '%s'",
                              create_spec);
                }
                break;
            }
        case VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER:
            {
                /*
                 * The data contains the create_spec.
                 */
                rv = vpi_remove_send_listener_spec(vpi, create_spec);
                if(rv < 0) {
                    VPI_ERROR(vpi, "could not remove send listener '%s'",
                              create_spec);
                }
                break;
            }
        case VPI_PROTOCOL_OPCODE_ECHO:
            {
                /*
                 * The packet data should be returned to the caller.
                 */
                rv = vpi_send(vpi, packet->data, packet->size);
                break;
            }
        case VPI_PROTOCOL_OPCODE_HELLO:
            {
                if(packet->data) {
                    VPI_INFO(vpi, "HELLO received from '%s'",
                             packet->data);
                }
                else {
                    VPI_INFO(vpi, "HELLO received, no data.");
                }
                rv = 0;
                break;
            }

        default:
            {
                VPI_WARN(vpi, "Unhandled ioctl: %s",
                         vpi_protocol_opcode_name(hdr->opcode));
                break;
            }
        }


    if(create_spec && (void*)create_spec != (void*)packet->data) {
        aim_free((void*)create_spec);
    }

    return rv;
}


/******************************************************************************
 *
 * Recv a packet on a VPI
 *
 *
 *
 *****************************************************************************/
int
vpi_recv(vpi_t vpi, uint8_t* data, uint32_t len, int block)
{
    int rv;
    if(vpi && vpi->interface->recv) {

        if(block == 0 && vpi->interface->recv_ready(vpi->interface) == 0) {
            /* No data, don't block */
            return 0;
        }

        rv = vpi->interface->recv(vpi->interface, data, len);

        if(rv > 0) {
            if(vpi->interface->flags & VPI_INTERFACE_FLAG_PROTOCOL) {

                vpi_packet_t packet;
                vpi_header_t hdr;

                VPI_INFO(vpi, "Protocol Recv.");
                /*
                 * This VPI interface implementation supports
                 * the VPI protocol.
                 */

                if(vpi_protocol_msg_parse(data, rv, &hdr, &packet) < 0) {
                    VPI_ERROR(vpi, "Protocol packet error.");
                    return -1;
                }

                VPI_INFO(vpi, "Protocol: %s, %d, %d",
                          vpi_protocol_opcode_name(hdr.opcode),
                          hdr.payload_size, hdr.sequence_number);

                if(hdr.opcode == VPI_PROTOCOL_OPCODE_PACKET) {
                    VPI_MEMMOVE(data, packet.data, packet.size);
                    rv = packet.size;
                }
                else {
                    vpi_handle_ioctl__(vpi, &hdr, &packet);
                    return 0;
                }
            }
            else {
                /*
                 * This VPI interface implementation supports raw packets only.
                 *
                 * Data is already in the buffer -- drop through
                 */
                VPI_INFO(vpi, "Raw Recv.");
            }
        }

        if(rv > 0 && vpi->recv_listeners->list) {
            vpi_send_list__(vpi->recv_listeners, data, rv,
                            VPI_INTERFACE_FLAG_RECV_LISTENING);
        }
        if(rv > 0 && vpi->sendrecv_listeners->list) {
            vpi_send_list__(vpi->sendrecv_listeners, data, rv,
                            VPI_INTERFACE_FLAG_RECV_LISTENING);
        }

        if(AIM_LOG_CUSTOM_ENABLED(VPI_LOG_FLAG_RECV)) {
            char* s = aim_bytes_to_string(data, rv, 0);
            VPI_LOG_RECV(vpi, "%s", s);
            AIM_FREE(s);
        }

        return rv;
    }
    else {
        return -1;
    }
}

int
vpi_drain(vpi_t vpi)
{
    int count=0;
    uint8_t p[VPI_CONFIG_MAX_PACKET];

    while(vpi_recv(vpi, p, sizeof(p), 0) > 0) {
        count++;
    }
    return count;
}


int
vpi_descriptor_get(vpi_t vpi)
{
    int fd = -1;
    if(vpi && vpi->interface->descriptor) {
        fd = vpi->interface->descriptor(vpi->interface);
    }
    return fd;
}

/******************************************************************************
 *
 * Destroy a VPI instance.
 *
 *
 *****************************************************************************/
static int
vpi_destroy__(vpi_t vpi)
{
    if(vpi) {

        vpi->ref_count--;

        if(vpi->ref_count > 0) {
            return vpi->ref_count;
        }

        if(vpi->flags & (1<<VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER)) {

            VPI_INFO(vpi, "Unregistering SendListener '%s'",
                     vpi->interface->send_to_spec);

            vpi_ioctl(vpi, VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER,
                     (uint8_t*)vpi->interface->send_to_spec,
                     VPI_STRLEN(vpi->interface->send_to_spec)+1);
        }
        if(vpi->flags & (1<<VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER)) {

            VPI_INFO(vpi, "Unregistering RecvListener '%s'",
                     vpi->interface->send_to_spec);

            vpi_ioctl(vpi, VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER,
                     (uint8_t*)vpi->interface->send_to_spec,
                     VPI_STRLEN(vpi->interface->send_to_spec)+1);
        }
        if(vpi->flags & (1<<VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER)) {
            VPI_INFO(vpi, "Unregistering SendRecvListener '%s'",
                     vpi->interface->send_to_spec);
            vpi_ioctl(vpi, VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER,
                      (uint8_t*)vpi->interface->send_to_spec,
                      VPI_STRLEN(vpi->interface->send_to_spec)+1);
        }

        if (vpi->interface) {
            if(vpi->interface->destroy) {
                vpi->interface->destroy(vpi->interface);
            }
        }

        biglist_locked_free_all(vpi->recv_listeners,
                                (biglist_free_f)vpi_destroy__);
        biglist_locked_free_all(vpi->send_listeners,
                                (biglist_free_f)vpi_destroy__);
        biglist_locked_free_all(vpi->sendrecv_listeners,
                                (biglist_free_f)vpi_destroy__);
        aim_free(vpi);
    }
    return 0;
}

int
vpi_destroy(vpi_t vpi)
{
    int refcount = vpi_destroy__(vpi);
    if(refcount == 0) {
        biglist_locked_remove(vpi_instances__, vpi);
    }
    return refcount;
}


int
vpi_name_set(vpi_t vpi, const char* name)
{
    if(vpi) {
        aim_strlcpy(vpi->name, name, sizeof(vpi->name));
        return 0;
    }
    return -1;
}

const char*
vpi_name_get(vpi_t vpi)
{
    return (vpi) ? vpi->name : NULL;
}

/******************************************************************************
 *
 *
 *
 *
 *****************************************************************************/
const char*
vpi_get_create_spec(vpi_t vpi)
{
    return (vpi) ? vpi->create_spec : NULL;
}

const char*
vpi_get_recv_spec(vpi_t vpi)
{
    return (vpi && vpi->interface->recv_spec[0]) ?
        vpi->interface->recv_spec : NULL;
}

const char*
vpi_get_send_spec(vpi_t vpi)
{
    return (vpi && vpi->interface->send_spec[0]) ?
        vpi->interface->send_spec : NULL;
}

const char*
vpi_get_send_to_spec(vpi_t vpi)
{
    return (vpi && vpi->interface->send_to_spec[0]) ?
        vpi->interface->send_to_spec : NULL;
}


const char*
vpi_type_get(vpi_t vpi)
{
    return (vpi) ? vpi->type : NULL;
}


static vpi_t
vpi_find_spec_locked__(biglist_t* vpi_list, const char* create_spec)
{
    biglist_t* ble;
    for(ble = vpi_list; ble; ble = biglist_next(ble)) {
        BIGLIST_GET_DATA(vpi_t, vpi, ble);
        if(!VPI_STRCMP(create_spec, vpi->create_spec)) {
            return vpi;
        }
    }
    return NULL;
}

static int
vpi_add_listener_spec__(vpi_t vpi, biglist_locked_t* ll,
                        const char* create_spec)
{
    AIM_REFERENCE(vpi);
    biglist_lock(ll);

    /*
     * Listeners can only be added once.
     * Multiple adds of the same listener are allowed to refresh
     * timeouts.
     */
    if(vpi_find_spec_locked__(ll->list, create_spec) == NULL) {
        ll->list = biglist_prepend(ll->list, vpi_create__(create_spec));
    }
    biglist_unlock(ll);
    return 0;
}

static int
vpi_remove_listener_spec__(vpi_t vpi, biglist_locked_t* ll,
                        const char* create_spec)
{
    int rv = 0;
    vpi_t listener;

    AIM_REFERENCE(vpi);

    biglist_lock(ll);

    listener = vpi_find_spec_locked__(ll->list, create_spec);
    if(listener) {
        ll->list = biglist_remove(ll->list, listener);
        rv = vpi_destroy(listener);
    }
    biglist_unlock(ll);

    return rv;
}

/**
 * Receive Listeners
 */
int
vpi_add_recv_listener(vpi_t vpi, vpi_t listener)
{
    return (vpi) ? biglist_locked_prepend(vpi->recv_listeners, listener) : -1;
}

int
vpi_add_recv_listener_spec(vpi_t vpi, const char* spec)
{
    return (vpi) ? vpi_add_listener_spec__(vpi, vpi->recv_listeners, spec) : -1;
}

int
vpi_remove_recv_listener(vpi_t vpi, vpi_t listener)
{
    return (vpi) ? biglist_locked_remove(vpi->recv_listeners, listener) : -1;
}

int
vpi_remove_recv_listener_spec(vpi_t vpi, const char* spec)
{
    return (vpi) ? vpi_remove_listener_spec__(vpi, vpi->recv_listeners, spec) : -1;
}

int
vpi_recv_listener_count(vpi_t vpi)
{
    return (vpi) ? biglist_locked_length(vpi->recv_listeners) : -1;
}

int
vpi_recv_listeners_drop(vpi_t vpi)
{
    return vpi_destroy_locked_list__(vpi->recv_listeners);
}

/**
 * Send Listeners
 */
int
vpi_add_send_listener(vpi_t vpi, vpi_t listener)
{
    return (vpi) ? biglist_locked_prepend(vpi->send_listeners, listener) : -1;
}

int
vpi_add_send_listener_spec(vpi_t vpi, const char* create_spec)
{
    return (vpi) ? vpi_add_listener_spec__(vpi, vpi->send_listeners, create_spec) : -1;
}

int
vpi_remove_send_listener_spec(vpi_t vpi, const char* create_spec)
{
    return (vpi) ? vpi_remove_listener_spec__(vpi, vpi->send_listeners, create_spec) : -1;
}

int
vpi_remove_send_listener(vpi_t vpi, vpi_t listener)
{
    return (vpi) ? biglist_locked_remove(vpi->send_listeners, listener) : -1;
}

int
vpi_send_listener_count(vpi_t vpi)
{
    return (vpi) ? biglist_locked_length(vpi->send_listeners) : -1;
}

int
vpi_send_listeners_drop(vpi_t vpi)
{
    return vpi_destroy_locked_list__(vpi->send_listeners);
}

/**
 * SendRecv Listeners
 */
int
vpi_add_sendrecv_listener(vpi_t vpi, vpi_t listener)
{
    return (vpi) ? biglist_locked_prepend(vpi->sendrecv_listeners, listener) : -1;
}

int
vpi_add_sendrecv_listener_spec(vpi_t vpi, const char* create_spec)
{
    return (vpi) ? vpi_add_listener_spec__(vpi, vpi->sendrecv_listeners, create_spec) : -1;
}

int
vpi_remove_sendrecv_listener_spec(vpi_t vpi, const char* create_spec)
{
    return (vpi) ? vpi_remove_listener_spec__(vpi, vpi->sendrecv_listeners, create_spec) : -1;
}

int
vpi_remove_sendrecv_listener(vpi_t vpi, vpi_t listener)
{
    return (vpi) ? biglist_locked_remove(vpi->sendrecv_listeners, listener) : -1;
}

int
vpi_sendrecv_listener_count(vpi_t vpi)
{
    return (vpi) ? biglist_locked_length(vpi->sendrecv_listeners) : -1;
}

int
vpi_sendrecv_listeners_drop(vpi_t vpi)
{
    return vpi_destroy_locked_list__(vpi->sendrecv_listeners);
}

void
vpi_show(vpi_t vpi, aim_pvs_t* pvs)
{
    aim_printf(pvs, "VPI @ %p: NAME=%s SPEC=%s\n",
               vpi, vpi_name_get(vpi), vpi_get_create_spec(vpi));
}

/******************************************************************************
 *
 *
 *
 *
 *****************************************************************************/


void
vpi_close(void)
{
    if(vpi_instances__) {
        VPI_MINFO("vpi_close: %d outstanding VPIs.",
                  biglist_locked_length(vpi_instances__));
        biglist_locked_free_all(vpi_instances__, (biglist_free_f)vpi_destroy__);
        vpi_instances__ = NULL;
    }

    while(vpi_interface_list__) {
        BIGLIST_GET_DATA(vpi_interface_ctrl_t*, vi, vpi_interface_list__);
        vpi_interface_unregister(vi->type);
    }
}

int
vpi_ref(vpi_t vpi)
{
    if(vpi) {
        return ++vpi->ref_count;
    }
    return -1;
}

int
vpi_unref(vpi_t vpi)
{
    return vpi_destroy(vpi);
}

