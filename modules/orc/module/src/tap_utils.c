/************************************************************
 * <bsn.cl fy=2013 v=epl>
 * 
 *        Copyright 2013, 2014 Big Switch Networks, Inc.       
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
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <net/if.h>
#include <linux/if_tun.h>

#include "orc/orc_logger.h"
#include "orc/tap_utils.h"

port_t ** Driver_Index_Linux_Index_Map = NULL;
int Driver_Index_Linux_Max_Index;

int tap_interface_create(char * ifnam_prefix, port_t * port);

int tap_interfaces_create(char * ifname_prefix, port_t * ports[], int num_ports)
{
    int i;
    int err;
    int max_index = -1;
    int map_size;
    int if_index;

    orc_debug("Creating %d Tap Interfaces\n", num_ports);
    for (i=0; i<num_ports; i++)
    {
        err = tap_interface_create(ifname_prefix, ports[i]);
        if (err)
        {
            orc_err("Error creating tap_interface_create for port idx=%d\n",
                            ports[i]->index);
            return err;
        }
        interface_name_to_index(ports[i]->name, &if_index);
        ports[i]->if_index = if_index;
        if (max_index < if_index)
            max_index = if_index;
    }

    /** now create a mapping between driver index and linux interface index */
    if (Driver_Index_Linux_Index_Map != NULL)
        free(Driver_Index_Linux_Index_Map);
    map_size = sizeof(port_t *) * (max_index+1);
    Driver_Index_Linux_Index_Map = malloc(map_size);
    if (Driver_Index_Linux_Index_Map == NULL)
        orc_fatal("Failed to allocate memory: %d bytes\n", map_size);
    bzero(Driver_Index_Linux_Index_Map, map_size);
    for (i=0; i<num_ports; i++)
    {
        interface_name_to_index(ports[i]->name, &if_index);
        Driver_Index_Linux_Index_Map[if_index] = ports[i];
    }
    Driver_Index_Linux_Max_Index = max_index ;
    return 0;
}

/*********************************
 * utility function: enable non-block
 */

int
set_socket_non_blocking(int fd)
{
        int flags, s;

        flags = fcntl (fd, F_GETFL, 0);
        if (flags == -1)
        {
                orc_err("fcntl(): %d : %s", flags, strerror(errno));
                return -1;
        }

        flags |= O_NONBLOCK;
        s = fcntl (fd, F_SETFL, flags);
        if (s == -1)
        {
                orc_err("fcntl(): %d : %s", s, strerror(errno));
                return -1;
        }

        return 0;
}

/********
 * Create a tap interface:
 *  - fill in fd and interface name into port_t
 */
int tap_interface_create(char * ifname_prefix, port_t * port)
{
    int err;

    snprintf(port->name, IFNAMSIZ, "%s%.2d", ifname_prefix, port->index);
    orc_debug("Creating tap interface %s\n", port->name);
    err = create_tap_iface(port->name, &port->fd);
    if (err)
    {
        orc_err("create_tap_iface(%s) returned %d\n", port->name, err);
        return err;
    }

    return 0;
}


int
create_tap_iface(char * devname, int * fd)
{
    struct ifreq ifr;
    int err;

    if( (*fd = open("/dev/net/tun", O_RDWR)) < 0 )
    {
        orc_err("open(\"/dev/net/tun\") returned %d : %s\n",
                        *fd, strerror(errno));
        return *fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    /* Flags:
     * IFF_TUN   - TUN device (no Ethernet headers)
     * IFF_TAP   - TAP device
     * IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (devname)
        strncpy(ifr.ifr_name, devname, IFNAMSIZ);
    else
        snprintf(ifr.ifr_name, IFNAMSIZ, "orc%%d");

    if  ((err = ioctl(*fd, TUNSETIFF, (void *) &ifr)) < 0 )
    {
        orc_err("ioctl() returned %d : %s\n", err, strerror(errno));
        close(*fd);
        return err;
    }

    if (set_socket_non_blocking(*fd) != 0)
    {
        orc_err("set_socket_non_blocking() returned %d : %s\n", err, strerror(errno));
        close(*fd);
        return err;
    }

    return 0;
}


/*********************************
 * Convert an index of an interface in to the inteface name
 *
 * fd doesn't matter, as long as it's an AF_INET
 */

static int AF_INET_SOCK = -1;   /* so the caller doesn't need one */

int interface_name_to_index(char name[IFNAMSIZ], int * if_index)
{
    struct ifreq ifr;
    if (AF_INET_SOCK == -1)
        AF_INET_SOCK = socket(AF_INET, SOCK_DGRAM, 0);
    memcpy(&ifr.ifr_ifrn.ifrn_name, name, IFNAMSIZ);
    int err = ioctl(AF_INET_SOCK, SIOCGIFINDEX, &ifr);
    if (err == 0)
    {
        *if_index = ifr.ifr_ifindex;
        return 0;
    }
    else
    {
        orc_err("Failed to lookup inteface by name %s: %s\n",
                name, strerror(errno));
        return -1;
    }
}

char * interface_index_to_name(int index, char * buf, int buflen)
{
    struct ifreq ifr;
    if (AF_INET_SOCK == -1)
        AF_INET_SOCK = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_ifindex = index;
    int err = ioctl(AF_INET_SOCK, SIOCGIFNAME, &ifr);
    if (err == 0)
    {
        snprintf(buf, buflen, "%s", ifr.ifr_name);
        return buf;
    }
    else
    {
        orc_err("Failed to lookup inteface by index %d: %s\n",
                index, strerror(errno));
        return NULL;
    }
}

port_t * interface_index_to_port_t(int index)
{
    int i;
    // FIXME: linear search!
    // foreach if_index that we've seen
    for (i=0; i<Driver_Index_Linux_Max_Index; i++)
    {
        /* NOTE: map == NULL if this is not an ORC interface */
        if (Driver_Index_Linux_Index_Map[i] != NULL &&
                Driver_Index_Linux_Index_Map[i]->if_index == index)
            return Driver_Index_Linux_Index_Map[i];
    }
    return NULL;
}

int interface_name_to_hw_mac(char name[IFNAMSIZ], u8 hw_mac[6])
{
    struct ifreq ifr;
    if (AF_INET_SOCK == -1)
        AF_INET_SOCK = socket(AF_INET, SOCK_DGRAM, 0);
    memcpy(&ifr.ifr_ifrn.ifrn_name, name, IFNAMSIZ);
    int err = ioctl(AF_INET_SOCK, SIOCGIFHWADDR, &ifr);
    if (err == 0)
    {
        memcpy(hw_mac, &ifr.ifr_hwaddr.sa_data, 6);
        return 0;
    }
    else
    {
        orc_err("Failed to lookup hw_mac by name %s: %s\n",
                name, strerror(errno));
        return -1;
    }
}

int interface_name_to_mtu(char name[IFNAMSIZ], int * mtu)
{
    struct ifreq ifr;
    if (AF_INET_SOCK == -1)
        AF_INET_SOCK = socket(AF_INET, SOCK_DGRAM, 0);
    memcpy(&ifr.ifr_ifrn.ifrn_name, name, IFNAMSIZ);
    int err = ioctl(AF_INET_SOCK, SIOCGIFMTU, &ifr);
    if (err == 0)
    {
        *mtu = ifr.ifr_mtu;
        return 0;
    }
    else
    {
        orc_err("Failed to lookup mtu by name %s: %s\n",
                name, strerror(errno));
        return -1;
    }
}
