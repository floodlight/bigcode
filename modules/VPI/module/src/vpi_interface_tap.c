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

#if VPI_CONFIG_INCLUDE_INTERFACE_TAP == 1

#define VPI_LOG_PREFIX1 ".tap"
#include "vpi_log.h"

#include "vpi_interface_tap.h"
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
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_arp.h>


static const char* tap_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: TAP \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI endpoints using TAP interfaces.\n"
    "\n"
    "Create format:\n"
    "    'tap|INTERFACE'\n"
    "\n"
    "     Example:\n"
    "     'tap|tap0'\n"
    "\n"
    "\n";

typedef struct vpi_interface_tap_s {

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

    /* Our tun file descriptor */
    int fd;

    /* Interface name */
    char interface_name[IFNAMSIZ];


} vpi_interface_tap_t;

#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_tap_t* _vi_name = (vpi_interface_tap_t*)(_vpi_name)


int
vpi_tap_interface_register(void)
{
    /* Register our module as handling type 'udp' */
    return vpi_interface_register("tap", vpi_tap_interface_create, NULL,
                                tap_interface_docstring__);
}


/**************************************************************************//**
 *
 * Send packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/
static int
write__(vpi_interface_tap_t* vi, char* data, int len)
{
    int rv;
    if((rv = write(vi->fd, data, len)) < 0) {
        VPI_ERROR(vi, "write() failed: %{errno}", errno);
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
read__(vpi_interface_tap_t* vi, char* data, int len)
{
    int rv;
    if((rv = read(vi->fd, data, len)) < 0) {
        VPI_ERROR(vi, "read() failed: %{errno}", errno);
        return -1;
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
set_mac_address__( const char *interface, char *mac_address )
{
    int s, i;
    struct ifreq ifr;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    strcpy(ifr.ifr_name, interface);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    for ( i = 0; i < 6; i++ )
        ifr.ifr_hwaddr.sa_data[i] = mac_address[i];

    if (ioctl(s, SIOCSIFHWADDR, &ifr)) {
        perror("SIOCSIFHWADDR");
        close( s );
        return -1;
    }

    close( s );

    return 0;
}

int
vpi_tap_interface_create(vpi_interface_t** vi, char* args[], int flags,
                       const char* vpi_name_ptr)
{
    struct ifreq ifr;
    int fd, rv;
    vpi_interface_tap_t* nvi;
    char** arg = args;
    int persist = 1;

    AIM_REFERENCE(flags);

    nvi = aim_zmalloc(sizeof(*nvi));
    nvi->log_string = vpi_name_ptr;

    if( (fd = open("/dev/net/tun", O_RDWR)) < 0) {
        AIM_LOG_ERROR(nvi, "open(/dev/net/tun): %{errno}", errno);
        goto error;
    }

    /* The first argument is the VPI type. */
    arg++;

    VPI_STRNCPY(nvi->interface_name, *arg, IFNAMSIZ);

    VPI_MEMSET(&ifr, 0, sizeof(ifr));
    VPI_STRNCPY(ifr.ifr_name, *arg, IFNAMSIZ);
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if( (rv = ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0) {
        AIM_LOG_ERROR(nvi, "ioctl(TUNSETIFF): %{errno}", errno);
        goto error;
    }

    arg++;

    while(*arg) {
        uint8_t mac[6];

        if(!VPI_STRCMP(*arg, "nopersist")) {
            persist = 0;
        }
        else if(!VPI_STRCMP(*arg, "persist")) {
            persist = 1;
        }
        else if(aim_sparse(arg, NULL, "{mac}", mac) == 0) {

            ifr.ifr_flags = 0;
            ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
            VPI_MEMCPY(ifr.ifr_hwaddr.sa_data, mac, 6);
            if(ioctl(fd, SIOCSIFHWADDR, &ifr) < 0) {
                AIM_LOG_ERROR(nvi, "ioctl(%s, SIOCSIFHWADDR,%{mac}): %{errno}",
                              ifr.ifr_name, ifr.ifr_hwaddr.sa_data, errno);
                goto error;
            }
        }
        else {
            AIM_LOG_ERROR(nvi, "argument '%s' cannot be parsed.", *arg);
            goto error;
        }
        arg++;
    }

    if( (rv = ioctl(fd, TUNSETPERSIST, persist)) < 0) {
        AIM_LOG_ERROR(nvi, "ioctl(TUNSETPERSIST, %d): %{errno}",
                      persist, errno);
        goto error;
    }

    nvi->fd = fd;
    nvi->interface.send = vpi_tap_interface_send;
    nvi->interface.recv = vpi_tap_interface_recv;
    nvi->interface.recv_ready = vpi_tap_interface_recv_ready;
    nvi->interface.destroy = vpi_tap_interface_destroy;
    nvi->interface.descriptor = vpi_tap_interface_descriptor;

    *vi = (vpi_interface_t*)nvi;
    return 0;

 error:
    if(fd > 0) {
        close(fd);
    }
    if(nvi) {
        aim_free(nvi);
    }
    return -1;
}

int
vpi_tap_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return write__(vi, (char*)data, len);
}

int
vpi_tap_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return read__(vi, (char*)data, len);
}

int
vpi_tap_interface_recv_ready(vpi_interface_t* _vi)
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
vpi_tap_interface_descriptor(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->fd;
}

int
vpi_tap_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    close(vi->fd);
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_TAP */
