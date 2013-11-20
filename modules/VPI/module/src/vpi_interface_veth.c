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

#if VPI_CONFIG_INCLUDE_INTERFACE_VETH == 1

#define VPI_LOG_PREFIX1 ".veth"
#include "vpi_log.h"

#include "vpi_interface_veth.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

static const char* veth_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: VETH \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI endpoints using VETH interfaces.\n"
    "\n"
    "Create format:\n"
    "    'veth|INTERFACE'\n"
    "\n"
    "     Example:\n"
    "     'veth|veth0'\n"
    "\n"
    "\n";

typedef struct vpi_interface_veth_s {

/**************************************************************************//**
 *
 * This must always be first.
 * This is the public inteface data.
 *
 *****************************************************************************/
    vpi_interface_t interface;


/**************************************************************************//**
 *
 * Private implementation data.
 *
 *
 *****************************************************************************/
    const char* log_string;

    /* Our raw socket descriptor */
    int fd;

    /* Interface index of our assigned network interface */
    int ifindex;

    /* Interface name */
    char interface_name[IFNAMSIZ];


} vpi_interface_veth_t;

#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_veth_t* _vi_name = (vpi_interface_veth_t*)(_vpi_name)


int
vpi_veth_interface_register(void)
{
    /* Register our module as handling type 'udp' */
    return vpi_interface_register("veth", vpi_veth_interface_create, NULL,
                                veth_interface_docstring__);
}


/**************************************************************************//**
 *
 * Send packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/
static int
sendto__(vpi_interface_veth_t* vi, char* data, int len)
{
    struct sockaddr_ll sockaddr;

    VPI_MEMSET(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sll_family  = AF_PACKET;
    sockaddr.sll_ifindex = vi->ifindex;

    if (sendto(vi->fd, data, len, 0, (struct sockaddr *)&sockaddr,
               sizeof(sockaddr)) < 0) {
        VPI_ERROR(vi, "sendto() failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

/**************************************************************************//**
 *
 * Receive packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/
static int
read__(vpi_interface_veth_t* vi, char* data, int len)
{
    int rv;
    if((rv = read(vi->fd, data, len)) < 0) {
        VPI_ERROR(vi, "read() failed: %s", strerror(errno));
        return -1;
    }
    else {
        VPI_INFO(vi, "packet received");
    }
    return rv;
}

/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/

int
vpi_veth_interface_create(vpi_interface_t** vi, char* args[], int flags,
                       const char* vpi_name_ptr)
{
    struct sockaddr_ll sockaddr;
    struct ifreq ifreq;
    vpi_interface_veth_t* nvi = aim_zmalloc(sizeof(*nvi));
    char** arg = args;

    AIM_REFERENCE(flags);

    if(nvi == NULL) {
        VPI_MERROR("interface allocation failed for %s.",
                   vpi_name_ptr);
        return -1;
    }

    /*
     * Point our log_string to our name so we can use it immediately
     * in log messages.
     */
    nvi->log_string = vpi_name_ptr;


    /*
     * The first argument is the type -- skip for now
     */
    arg++;

    aim_strlcpy(nvi->interface_name, *arg, sizeof(nvi->interface_name));

    /* Create RAW socket */
    if((nvi->fd = socket(PF_PACKET, SOCK_RAW, 0)) < 0) {
        VPI_ERROR(nvi, "socket() failed: %s\n", strerror(errno));
        aim_free(nvi);
        return -1;
    }

    /*
     * Get the interface index for the requested interface, as specified
     * in the current argument.
     */

    VPI_MEMSET(&ifreq, 0, sizeof(ifreq));
    aim_strlcpy(ifreq.ifr_name, nvi->interface_name, IFNAMSIZ);

    if(ioctl(nvi->fd, SIOCGIFINDEX, &ifreq) < 0) {
        VPI_ERROR(nvi, "ioctl() failed: %s", strerror(errno));
        close(nvi->fd);
        aim_free(nvi);
        return -1;
    }
    nvi->ifindex = ifreq.ifr_ifindex;

    VPI_INFO(nvi, "ifndex is %d", nvi->ifindex);

    VPI_MEMSET(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sll_family=AF_PACKET;
    sockaddr.sll_protocol = htons(ETH_P_ALL);
    sockaddr.sll_ifindex = ifreq.ifr_ifindex;

    if(bind(nvi->fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        VPI_ERROR(nvi, "bind() failed");
        return -1;
    }

    nvi->interface.send = vpi_veth_interface_send;
    nvi->interface.recv = vpi_veth_interface_recv;
    nvi->interface.recv_ready = vpi_veth_interface_recv_ready;
    nvi->interface.destroy = vpi_veth_interface_destroy;
    nvi->interface.descriptor = vpi_veth_interface_descriptor;

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

int
vpi_veth_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return sendto__(vi, (char*)data, len);
}

int
vpi_veth_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return read__(vi, (char*)data, len);
}

int
vpi_veth_interface_recv_ready(vpi_interface_t* _vi)
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
vpi_veth_interface_descriptor(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->fd;
}

int
vpi_veth_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    close(vi->fd);
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_VETH */
