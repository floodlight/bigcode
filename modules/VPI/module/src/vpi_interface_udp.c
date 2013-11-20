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

#include "vpi_int.h"

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1

#define VPI_LOG_PREFIX1 ".udp"
#include "vpi_log.h"
#include "vpi_parser.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "vpi_interface_udp.h"
#include "vpi_ip_utils.h"


static const char* udp_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: UDP \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI endpoints using UDP sockets.\n"
    "\n"
    "Create format:\n"
    "    # Send and Recv endpoint:\n"
    "    'udp|recv:[LOCALADDRESS:]PORT|send:HOST:PORT'\n"
    "\n"
    "    # Recv-only endpoint:\n"
    "    'udp|recv:[LOCALADDRESS:]PORT'\n"
    "\n"
    "    # Send-only endpoint:\n"
    "    'udp|send:HOST:PORT'\n"
    "\n"
    "\n";


typedef struct vpi_interface_udp_s {

/**************************************************************************//**
 *
 * This must always be first.
 * This is the public interface data;
 *
 *
 *****************************************************************************/

    vpi_interface_t interface;


/**************************************************************************//**
 *
 * Private implementation data.
 *
 *
 *****************************************************************************/
    /* Our log_string -- will point to our name */
    const char* log_string;

    /* Our local port connection */
    struct sockaddr_in sa_local;
    unsigned int sa_local_size;

    /* Our peer port connection */
    struct sockaddr_in sa_remote;
    unsigned int sa_remote_size;

    /* Our datagram socket */
    int fd;

    /* Whether we should close our descriptor or not */
    int close_fd;

    /* Last recv */
    struct sockaddr_in sa_recv;

    /* Our send sequence number */
    int tx_count;

    /* Our recv sequence number */
    int rx_count;

} vpi_interface_udp_t;


#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_udp_t* _vi_name = (vpi_interface_udp_t*)(_vpi_name)



int
vpi_udp_interface_register(void)
{
    /* Register our module as handling type 'udp' */
    return vpi_interface_register("udp", vpi_udp_interface_create,
                                NULL, udp_interface_docstring__);
}

/**************************************************************************//**
 *
 * Send a packet to our peer
 *
 *
 *****************************************************************************/
static int
sendto__(vpi_interface_udp_t* vi, char* data, int len)
{
    int rv;
    char remote[32];

    {
        char local[32];
        char remote[32];
        VPI_MTRACE("send: %s --> %s", vpi_sockaddr_format(&vi->sa_local, local),
                   vpi_sockaddr_format(&vi->sa_remote, remote));
    }

    VPI_TRACE(vi, "sendto: %d bytes", len);
    rv = sendto(vi->fd, data, len, 0,
                (struct sockaddr*)&vi->sa_remote,
                sizeof(vi->sa_remote));
    VPI_TRACE(vi, "sendto(%s) = %d", vpi_sockaddr_format(&vi->sa_remote, remote),
              rv);
    return rv;
}

/**************************************************************************//**
 *
 * Recv a packet from our peer
 *
 *
 *****************************************************************************/
static int
recvfrom__(vpi_interface_udp_t* vi, char* data, int len)
{
    unsigned int socklen = sizeof(struct sockaddr_in);
    int rv = recvfrom(vi->fd, data, len, 0,
                      (struct sockaddr*)&vi->sa_recv,
                      &socklen);
    VPI_TRACE(vi, "Received packet, result=%d", rv, strerror(errno));
    if(rv < 0) {
        VPI_ERROR(vi, "Receive error: %s", strerror(errno));
    }

    {
        char local[32];
        char remote[32];
        VPI_MTRACE("recv: %s <-- %s", vpi_sockaddr_format(&vi->sa_local, local),
                   vpi_sockaddr_format(&vi->sa_recv, remote));
    }

    return rv;
}

int
vpi_udp_interface_create(vpi_interface_t** vi,
                         char* args[], int flags,
                         const char* vpi_name_ptr)
{
    char** arg = args;
    vpi_interface_udp_t* nvi = NULL;

    vpi_ip_endpoint_t* recv_endpoint = NULL;
    vpi_ip_endpoint_t* send_endpoint = NULL;

    /* For log messages */
    char local[32];
    char remote[32];

    int one = 1;
    int arg_fd = -1;

    AIM_REFERENCE(flags);


    /*
     * The first argument is always the type.
     * We don't register for more than one type so
     * we don't care at this point.
     */
    arg++;



    /*
     * We might get created from a shared file descriptor.
     * If the "fd:" option is set, we should use the specified
     * descriptor instead of creating our own socket.
     *
     * This option must come before the send: and recv: options.
     */
    if(VPI_SSCANF(*arg, "fd:%d", &arg_fd) == 1) {
        arg++;
    }

    /*
     * Parse incoming arguments -- must be in Send/Recv IP format.
     */

    if(vpi_parse_send_recv_ip_endpoints(arg, &recv_endpoint, &send_endpoint) < 0) {
        VPI_MERROR("invalid arguments.");
        goto vpi_udp_interface_create_Error;
    }

    /*
     * Allocate a new control structure.
     */
    if( (nvi = aim_zmalloc(sizeof(*nvi))) == NULL) {
        VPI_MERROR("interface allocation failed.");
        goto vpi_udp_interface_create_Error;
    }

    nvi->log_string = vpi_name_ptr;

    if(arg_fd != -1) {
        /*
         * Use the specified file descriptor, and don't close it
         * when we are destroyed.
         */
        nvi->fd = arg_fd;
        nvi->close_fd = 0;
    }
    else {
        nvi->fd = socket(AF_INET, SOCK_DGRAM, 0);
        nvi->close_fd = 1;
        if(nvi->fd < 0) {
            VPI_ERROR(nvi, "socket() failed: %s", strerror(errno));
            goto vpi_udp_interface_create_Error;
        }

        if(setsockopt(nvi->fd, SOL_SOCKET, SO_REUSEADDR, &one,
                      sizeof(one)) < 0) {
            VPI_WARN(nvi, "setsockopt() failed: %s", strerror(errno));
        }
    }

    if(recv_endpoint) {
        /*
         * Bind our socket
         */
        VPI_SPRINTF(nvi->interface.recv_spec, "udp|recv:%s:%d",
                    recv_endpoint->host, recv_endpoint->port);

        nvi->sa_local_size = sizeof(nvi->sa_local);
        nvi->sa_local.sin_family = AF_INET;

        if(vpi_sockaddr_endpoint(&nvi->sa_local, recv_endpoint) < 0) {
            VPI_ERROR(nvi, "Invalid address in local endpoint.");
            goto vpi_udp_interface_create_Error;
        }

        if(bind(nvi->fd, (struct sockaddr *)&nvi->sa_local,
                nvi->sa_local_size) == -1) {

            VPI_ERROR(nvi, "bind(%s:%d) failed: %s",
                      inet_ntoa(nvi->sa_local.sin_addr),
                      ntohs(nvi->sa_local.sin_port),
                      strerror(errno));
            goto vpi_udp_interface_create_Error;
        }


        if(nvi->sa_local.sin_port == 0) {
            /*
             * Bind to dynamic port. Need to retrieve the actual port
             * number.
             */
            if(getsockname(nvi->fd, (struct sockaddr*)&nvi->sa_local,
                           &nvi->sa_local_size) == -1) {
                VPI_ERROR(nvi, "getsockname() failed: %s", strerror(errno));
                goto vpi_udp_interface_create_Error;
            }
        }


        /*
         * Set our send_to address as UDP dynamic.
         *
         * Our destination endpoint will figure out
         * how to talk to us from the receive information.
         *
         * This is a special create_spec that only works
         * through Ioctl registration processing.
         * You can't use it explicitly.
         */
        if(send_endpoint) {
            VPI_SPRINTF(nvi->interface.send_to_spec, "udp|peer");
        }

        /* Recv interface is ready to go */
        nvi->interface.recv = vpi_udp_interface_recv;
        nvi->interface.recv_ready = vpi_udp_interface_recv_ready;
    }

    if(send_endpoint) {
        nvi->sa_remote_size = sizeof(nvi->sa_remote);
        nvi->sa_remote.sin_family = AF_INET;

        if(vpi_sockaddr_endpoint(&nvi->sa_remote, send_endpoint) < 0) {
            VPI_ERROR(nvi, "Invalid host in remote endpoint.");
            goto vpi_udp_interface_create_Error;
        }

        /* Send interface is ready to go */
        nvi->interface.send = vpi_udp_interface_send;
    }

    if(recv_endpoint && send_endpoint) {
        VPI_INFO(nvi, "send-recv: local(%s) <-> remote(%s)",
                 vpi_sockaddr_format(&nvi->sa_local, local),
                 vpi_sockaddr_format(&nvi->sa_remote, remote));
    }
    else if(send_endpoint) {
        VPI_INFO(nvi, "send-only: -> remote(%s)",
                 vpi_sockaddr_format(&nvi->sa_remote, remote));
    }
    else {
        VPI_INFO(nvi, "recv-only: local(%s) <-",
                 vpi_sockaddr_format(&nvi->sa_local,local));
    }

    nvi->interface.descriptor = vpi_udp_interface_descriptor;
    nvi->interface.destroy = vpi_udp_interface_destroy;
    nvi->interface.filter_create_spec = vpi_udp_interface_filter_create_spec;
    nvi->interface.flags = VPI_INTERFACE_FLAG_PROTOCOL;

    vpi_free_ip_endpoint(&recv_endpoint);
    vpi_free_ip_endpoint(&send_endpoint);
    *vi = (vpi_interface_t*)nvi;

    return 0;

 vpi_udp_interface_create_Error:
    vpi_free_ip_endpoint(&recv_endpoint);
    vpi_free_ip_endpoint(&send_endpoint);
    vpi_udp_interface_destroy((vpi_interface_t*)nvi);
    return -1;
}

int
vpi_udp_interface_descriptor(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->fd;
}

int
vpi_udp_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    int rv;
    VICAST(vi, _vi);

    rv = recvfrom__(vi, (char*)data, len);

    if(rv < 0) {
        VPI_ERROR(vi, "recv() returned error %d (%s)", rv,
                  strerror(errno));
    }
    return rv;
}

int
vpi_udp_interface_recv_ready(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);

    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(vi->fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return select(vi->fd+1, &rfds, NULL, NULL, &tv);
}

int
vpi_udp_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    int rv;
    rv = sendto__(vi, (char*)data, len);
    return rv;
}

int
vpi_udp_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    if(vi) {
        if(vi->fd > 0) {
            if(vi->close_fd) {
                close(vi->fd);
            }
        }
        aim_free(vi);
    }
    return 0;
}

/*
 * This function is called when the VPI library processes
 * a create spec for Register and Unregister ioctls that were
 * received on our VPI.
 *
 * This gives us the opportunity to provide connection-specific
 * information, like determine the create_spec necessary to
 * communicate with the peer that sent the ioctl message.
 */
char*
vpi_udp_interface_filter_create_spec(vpi_interface_t* _vi, const char* spec)
{
    VICAST(vi, _vi);
    if(vi) {
        /*
         * We only handle the special "udp|peer" specification.
         * "udp|peer" means "send to the guy I received it from".
         *
         * In this case, we get the information directly from our last
         * received packet.
         *
         * This may seem gigantically overly complicated when the goal is just
         * "talk to the guy talking to me", but in order to fit properly
         * into the generic {send/recv} listener architecture (which does
         * not require symmetric communication peers), populating another
         * VPI instance (of ourselves) that can be added and removed dynamically
         * without affecting ourselves as an endpoint is necessary, useful,
         * and cleaner.
         */
        if(!VPI_STRCMP(spec, "udp|peer")) {
            char remote[32];
            char ns[64];
            /*
             * Create a copy of ourselves, using our own file descriptor,
             * and the peer information for the current reception:
             */
            VPI_SPRINTF(ns, "udp|fd:%d|send:%s",
                        vi->fd, vpi_sockaddr_format(&vi->sa_recv, remote));
            return aim_strdup(ns);
        }
        else {
            /* No changes */
            return (char*)spec;
        }
    }
    return (char*)spec;
}


#endif /* INCLUDE_INTERFACE_UDP */
