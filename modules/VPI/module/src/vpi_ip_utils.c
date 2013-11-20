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


#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1 || VPI_CONFIG_INCLUDE_INTERFACE_TCP == 1


#define VPI_LOG_PREFIX1 ".iputils"

#include "vpi_ip_utils.h"
#include "vpi_log.h"


/**************************************************************************//**
 *
 * Convert an abstract vpi_ip_endpoint to sock_addr_in
 *
 *
 *****************************************************************************/

int
vpi_sockaddr_endpoint(struct sockaddr_in* dst, vpi_ip_endpoint_t* src)
{
    struct hostent* he;
    he = gethostbyname(src->host);
    if(he == NULL) {
        return -1;
    }
    memcpy(&dst->sin_addr, he->h_addr_list[0], he->h_length);
    dst->sin_port = htons(src->port);
    return 0;
}

/**************************************************************************//**
 *
 * Human-readable address formatted from a sockaddr_in strcuture.
 *
 *
 *****************************************************************************/
char*
vpi_sockaddr_format(struct sockaddr_in* sa, char* dst)
{
    VPI_SPRINTF(dst, "%s:%d", inet_ntoa(sa->sin_addr), ntohs(sa->sin_port));
    return dst;
}


/**************************************************************************//**
 *
 * Get the first non-loopack local interface IP address.
 *
 *
 *****************************************************************************/
#ifndef NI_MAXHOST
#define NI_MAXHOST 256
#endif

int
vpi_get_ip_local_endpoint(char* address)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if(family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return -1;
            }

            if(VPI_STRCMP(host, "127.0.0.1")) {
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    VPI_STRCPY(address, host);
    return 0;
}


/**************************************************************************//**
 *
 * Get the interface address which will be used to connect to the
 * given destination.
 *
 *****************************************************************************/
int
vpi_get_ip_src_for_dst_addr(vpi_ip_endpoint_t* remote_endpoint, char* src_address)
{
    /*
     * We want to know what interface/src address
     * will be used to connect to the destination address.
     *
     * The most portable way at this point seems to be
     * to open up a UDP socket, connect it to the destination,
     * read the connection info, then close the socket.
     *
     * Anything else (for example, /proc/net/route, "ip route get", etc
     * is a little too system specific.
     */
    char remote[32];
    struct sockaddr_in sa_remote;
    struct sockaddr_in sa_local;
    unsigned int sa_local_size = sizeof(sa_local);
    int fd = -1;

    /* Resolve connection info */
    if(vpi_sockaddr_endpoint(&sa_remote, remote_endpoint) < 0) {
        return -1;
    }
    sa_remote.sin_family = AF_INET;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) {
        VPI_MERROR("socket() failed: %s", strerror(errno));
        return -1;
    }
    if(connect(fd, (struct sockaddr*)&sa_remote, sizeof(sa_remote)) == -1) {
        VPI_MERROR("connect(%s) failed: %s",
                   vpi_sockaddr_format(&sa_remote, remote),
                   strerror(errno));
        close(fd);
        return -1;
    }

    /* Get our local connection information */
    if(getsockname(fd, (struct sockaddr*)&sa_local, &sa_local_size) == -1) {
        VPI_MERROR("could not get local socket information: %s",
                   strerror(errno));
        close(fd);
        return -1;
    }

    VPI_STRCPY(src_address, inet_ntoa(sa_local.sin_addr));
    VPI_MINFO("local route from %s", src_address);
    return 0;
}


#endif
