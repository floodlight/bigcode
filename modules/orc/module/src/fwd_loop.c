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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>


#include "orc/fwd_loop.h"
#include "orc/orc_logger.h"
#include "orc/netlink_mon.h"

#ifndef BUFLEN
#define BUFLEN 65536
#endif

/*****
 * Read a packet from the port, and call tx_pkt on the driver
 */
int
read_packet_from_port(orc_driver_t * drv, port_t * port)
{
    u8 buf[BUFLEN];
    int len;
    // read one packet per call, assumes level-triggered epoll()
    len = read(port->fd, buf, BUFLEN);
    if (len > 0)
    {
        drv->tx_pkt(port, buf, len);
    }
    else if (len < 0)
    {
        if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
           return 0;
        else
        {
            orc_err("read() for port %s returned : %s\n",
                            port->name, strerror(errno));
            return -1;
        }
    }
    else if (len == 0)
    {
        orc_err("read() for port %s returned EOF (!?) -- ignoring\n",
                        port->name);
    }
    return 0;
}

/**** needed for CLI ***/
int orc_els_callback(void *);

/*****
 * Loop infinitely, epoll() on the fds in ports[] and on read,
 * call read_packet_from_port()
 */

int
packet_forwarding_loop(orc_options_t * options,
                orc_driver_t * drv,
                int num_ports,
                port_t * ports[],
                int netlink_sock)
{
    int epoll_fd;
    struct epoll_event * events, ev;
    int i, nfds;
    int err;
    orc_log("Entering forwarding loop.\n");
    epoll_fd = epoll_create(num_ports
			    + 1 /* +1 for netlink_sock */
			    + (options->els != NULL ? 1 : 0) /* +1 for CLI, if configured */
			    );
    if (epoll_fd == -1)
    {
        orc_err("epoll_create() : %s\n", strerror(errno));
        return -1;
    }

    events = malloc(num_ports * sizeof(struct epoll_event));
    if (events == NULL)
    {
        orc_err("malloc() : %s\n", strerror(errno));
        return -1;
    }

    /* First add ports to epoll list */
    for (i=0; i<num_ports; i++)
    {
        ev.events = EPOLLIN;
        ev.data.ptr = ports[i];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ports[i]->fd, &ev) == -1)
        {
            orc_err("epoll_ct(ports) : %s\n", strerror(errno));
            return -1;
        }
    }
    /* Now add netlink sock */
    if (netlink_sock != -1)
    {
        ev.events = EPOLLIN;
        ev.data.ptr = &netlink_sock;
        num_ports ++;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, netlink_sock, &ev) == -1)
        {
            orc_err("epoll_ctl(netlink) : %s\n", strerror(errno));
            return -1;
        }
    }

    /* Add the CLI if configured */
    if (options->els)
    {
        ev.events = EPOLLIN;
        ev.data.ptr = &options->els->eventfd;
        num_ports ++;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, options->els->eventfd, &ev) == -1)
        {
            orc_err("epoll_ctl(cli) : %s\n", strerror(errno));
            return -1;
        }

    }


    orc_debug("Going into epoll() loop\n");
    err = 0;
    while (!err)
    {
        nfds = epoll_wait(epoll_fd, events, num_ports, -1);
        if (nfds < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            // else...
            orc_err("epoll_wait(): ret=%d : %s\n", nfds, strerror(errno));
            return -1;
        }
        for (i=0; i<nfds; i++)
        {
            /** TODO come back and add a thread poll here for efficency **/
            if ( events[i].data.ptr == &netlink_sock)
                err = netlink_mon_handler(options, drv, netlink_sock);
            else if ( events[i].data.ptr == &options->els->eventfd)
                err = orc_els_callback(options);
            else
                read_packet_from_port(drv, (port_t *) events[i].data.ptr);
        }
    }

    orc_log("Exiting forwarding loop.\n");
    return 0;
}

