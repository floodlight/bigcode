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

#if VPI_CONFIG_INCLUDE_INTERFACE_TCP == 1

#define VPI_LOG_PREFIX1 ".tcp"
#include "vpi_log.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "vpi_interface_tcp.h"
#include "vpi_parser.h"
#include "vpi_ip_utils.h"

static const char* tcp_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: TCP \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI endpoints using TCP sockets.\n"
    "\n"
    "Create format:\n"
    "    # Send and Recv endpoint:\n"
    "    'tcp|recv:[LOCALADDRESS:]PORT|send:HOST:PORT'\n"
    "\n"
    "    # Recv-only endpoint:\n"
    "    'tcp|recv:[LOCALADDRESS:]PORT'\n"
    "\n"
    "    # Send-only endpoint:\n"
    "    'tcp|send:HOST:PORT'\n"
    "\n"
    "\n";


typedef struct vpi_interface_tcp_s {

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

    /* Our local listening socket */
    int listen_fd;

    /* Our send sequence number */
    int tx_count;

    /* Our recv sequence number */
    int rx_count;

} vpi_interface_tcp_t;


#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_tcp_t* _vi_name = (vpi_interface_tcp_t*)(_vpi_name)


int
vpi_tcp_interface_register(void)
{
    /* Register our module as handling type 'tcp' */
    return vpi_interface_register("tcp", vpi_tcp_interface_create, NULL,
                                tcp_interface_docstring__);
}

int
vpi_tcp_interface_create(vpi_interface_t** vi,
                      char* args[], int flags,
                      const char* vpi_name_ptr)
{
    char** arg = args;
    vpi_interface_tcp_t* nvi = NULL;

    vpi_ip_endpoint_t* recv_endpoint = NULL;
    vpi_ip_endpoint_t* send_endpoint = NULL;

    /* For log messages */
    char local[32];
    char remote[32];

    AIM_REFERENCE(flags);


    /*
     * The first argument is always the type.
     * We don't register for more than one type so
     * we don't care at this point.
     */
    arg++;

    /*
     * Parse incoming arguments -- must be in Send/Recv IP format.
     */
    if(vpi_parse_send_recv_ip_endpoints(arg, &recv_endpoint, &send_endpoint) < 0) {
        VPI_MERROR("invalid arguments.");
        goto vpi_tcp_interface_create_Error;
    }

    /*
     * Allocate a new control structure.
     */
    if( (nvi = aim_zmalloc(sizeof(*nvi))) == NULL) {
        VPI_MERROR("interface allocation failed.");
        goto vpi_tcp_interface_create_Error;
    }

    nvi->log_string = vpi_name_ptr;


    if(recv_endpoint) {
        int one = 1;

        VPI_SPRINTF(nvi->interface.recv_spec, "tcp|recv:%s:%d",
                    recv_endpoint->host, recv_endpoint->port);

        nvi->sa_local_size = sizeof(nvi->sa_local);
        nvi->sa_local.sin_family = AF_INET;

        if(vpi_sockaddr_endpoint(&nvi->sa_local, recv_endpoint) < 0) {
            /* Could not resolve the endpoint */
            VPI_ERROR(nvi, "Invalid address in local endpoint.");
            goto vpi_tcp_interface_create_Error;
        }

        /*
         * Create a TCP socket on which we will receive packets.
         */
        nvi->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(nvi->listen_fd < 0) {
            VPI_ERROR(nvi, "socket() failed: %s", strerror(errno));
            goto vpi_tcp_interface_create_Error;
        }

        if(setsockopt(nvi->listen_fd, SOL_SOCKET, SO_REUSEADDR, &one,
                      sizeof(one)) < 0) {
            VPI_WARN(nvi, "setsockopt() failed: %s", strerror(errno));
        }

        if(bind(nvi->listen_fd, (struct sockaddr *)&nvi->sa_local,
                nvi->sa_local_size) == -1) {

            VPI_ERROR(nvi, "bind(%s:%d) failed: %s",
                      inet_ntoa(nvi->sa_local.sin_addr),
                      ntohs(nvi->sa_local.sin_port),
                      strerror(errno));
            goto vpi_tcp_interface_create_Error;
        }

        if(nvi->sa_local.sin_port == 0) {
            /*
             * Bind to a dynamic port. Need to retrieve the actual port
             * number.
             */
            if(getsockname(nvi->listen_fd, (struct sockaddr*)&nvi->sa_local,
                           &nvi->sa_local_size) == -1) {
                VPI_ERROR(nvi, "getsockname() failed: %s", strerror(errno));
                goto vpi_tcp_interface_create_Error;
            }
        }

        if(send_endpoint) {
            if(vpi_get_ip_src_for_dst_addr(send_endpoint, local) < 0) {
                VPI_ERROR(nvi, "Could not resolve route to destination.");
                goto vpi_tcp_interface_create_Error;
            }
            VPI_SPRINTF(nvi->interface.send_to_spec, "tcp|send:%s:%d",
                        local, ntohs(nvi->sa_local.sin_port));
        }

        if(listen(nvi->listen_fd, 1) < 0) {
            VPI_ERROR(nvi, "listen() failed: %s", strerror(errno));
            goto vpi_tcp_interface_create_Error;
        }

        /* Recv interface is ready to go */
        nvi->interface.recv = vpi_tcp_interface_recv;
        nvi->interface.recv_ready = vpi_tcp_interface_recv_ready;
        vpi_free_ip_endpoint(&recv_endpoint);
    }

    if(send_endpoint) {

        nvi->sa_remote_size = sizeof(nvi->sa_remote);
        nvi->sa_remote.sin_family = AF_INET;

        if(vpi_sockaddr_endpoint(&nvi->sa_remote, send_endpoint) < 0) {
            /* Could not resolve the endpoint */
            VPI_ERROR(nvi, "Invalid host in remote endpoint.");
            goto vpi_tcp_interface_create_Error;
        }

        /* Send interface is ready to go */
        nvi->interface.send = vpi_tcp_interface_send;
        vpi_free_ip_endpoint(&send_endpoint);
    }

    /*
     * Ready to go
     */
    if(nvi->sa_local.sin_port > 0 && nvi->sa_remote.sin_port > 0) {
        VPI_INFO(nvi, "send-recv: local(%s) <-> remote(%s)",
                 vpi_sockaddr_format(&nvi->sa_local, local),
                 vpi_sockaddr_format(&nvi->sa_remote, remote));
    }
    else if(nvi->sa_remote.sin_port > 0) {
        VPI_INFO(nvi, "send-only: -> remote(%s)",
                 vpi_sockaddr_format(&nvi->sa_remote, remote));
    }
    else {
        VPI_INFO(nvi, "recv-only: local(%s) <-",
                 vpi_sockaddr_format(&nvi->sa_local, local));
    }

    nvi->interface.descriptor = vpi_tcp_interface_descriptor;
    nvi->interface.destroy = vpi_tcp_interface_destroy;
    nvi->interface.flags = VPI_INTERFACE_FLAG_PROTOCOL;

    *vi = (vpi_interface_t*)nvi;
    return 0;


 vpi_tcp_interface_create_Error:
    vpi_free_ip_endpoint(&send_endpoint);
    vpi_free_ip_endpoint(&recv_endpoint);
    vpi_tcp_interface_destroy((vpi_interface_t*)nvi);
    return -1;
}

int
vpi_tcp_interface_descriptor(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->listen_fd;
}


/**************************************************************************//**
 *
 * PACKET protocol
 *
 *
 *****************************************************************************/

typedef struct vpi_tcp_packet_s {

    /* Incoming packet information */
    char* packet;
    int length;


    /* Actual packet payload data */
    char* payload;
    int payload_length;


    /* Protocol Data */
    int size;
    int sequence_number;

} vpi_tcp_packet_t;


static int
descriptor_ready__(int fd, int tv_usec)
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = tv_usec;
    return select(fd+1, &rfds, NULL, NULL, &tv);
}


int
vpi_tcp_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    int fd;
    int net_len;
    int rv;

    VPI_TRACE(vi, "Accepting connection...");
    /* Accept a connection on our listening socket */
    if( (fd = accept(vi->listen_fd, NULL, NULL)) < 0) {
        VPI_ERROR(vi, "accept() failed: %s", strerror(errno));
        return -1;
    }
    VPI_TRACE(vi, "Accepted. Reading packet size...");

    /*
     * Now that we have accepted the connection, if all works properly
     * we should expect our connected client to forward its packet.
     *
     * In reality, we can't check the descriptor immediately for data
     * since the connection is still being negotiated and there will be
     * some latency before the packet data arrives.
     *
     * We also don't want to block forever here, since we told the upper
     * layer that data was available (because of the select on listen_fd),
     * so we cheat and select() on the descriptor for a short period to make
     * sure we never block forever on a read that won't succeed.
     */
    if(descriptor_ready__(fd, 3000000) == 0) {
        VPI_ERROR(vi, "Accepted connection did not send data after 3 seconds.");
        close(fd);
        return 0;
    }

    if( (rv = read(fd, &net_len, 4)) != 4) {
        VPI_ERROR(vi, "read() returned %d: %s", rv, strerror(errno));
        close(fd);
        return -1;
    }
    net_len = ntohl(net_len);
    VPI_INFO(vi, "packet length specified as %d", net_len);

    if(net_len > len) {
        net_len = len;
    }
    VPI_TRACE(vi, "Reading packet data...");
    if( (rv = read(fd, data, net_len)) != net_len) {
        /* Todo -- multiple reads */
        VPI_ERROR(vi, "read() returned %d when we wanted %d",
                  rv, net_len);
        close(fd);
        return -1;
    }
    close(fd);
    VPI_TRACE(vi, "returning %d", net_len);
    return net_len;
}

int
vpi_tcp_interface_recv_ready(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    /*
     * We assume that a packet is waiting if we have a peer attempting
     * to connect to our listening socket. We can abort later on in
     * Recv() if something goes wrong.
     */
    return descriptor_ready__(vi->listen_fd, 0);
}

int
vpi_tcp_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    int rv;
    int fd;
    int net_len;

    VPI_TRACE(vi, "creating socket.");
    /* Create a new socket for this transmit */
    fd = socket(AF_INET, SOCK_STREAM, 0);


    if(fd < 0) {
        VPI_ERROR(vi, "socket() failed: %s", strerror(errno));
        return -1;
    }

    VPI_TRACE(vi, "connecting socket..");
    /* Attempt connection to our peer */
    if( (rv = connect(fd, (struct sockaddr*) &vi->sa_remote, vi->sa_remote_size)) < 0) {
        VPI_ERROR(vi, "connect() failed: %s", strerror(errno));
        close(fd);
        return -1;
    }
    VPI_TRACE(vi, "Connected. Writing %d", len);

    /* Write the total packet size */
    net_len = htonl(len);
    if( (rv = write(fd, &net_len, 4)) != 4) {
        VPI_ERROR(vi, "write() failed: %s", strerror(errno));
        close(fd);
        return -1;
    }
    VPI_TRACE(vi, "writing packet data");
    /* Write the packet data */
    if( (rv = write(fd, data, len)) != len) {
        VPI_ERROR(vi, "write() return %d: %s", rv, strerror(errno));
        close(fd);
        return -1;
    }
    VPI_TRACE(vi, "done.");
    close(fd);
    return len;
}

int
vpi_tcp_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    if(vi) {
        if(vi->listen_fd > 0) {
            close(vi->listen_fd);
        }
        aim_free(vi);
    }
    return 0;
}

#endif /* INCLUDE_INTERFACE_TCP */
