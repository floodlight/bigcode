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

/***********
 * Example orc driver for vethging
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/ethernet.h>

#include <netinet/in.h>

#include "orc/utils.h"
#include "orc/orc_logger.h"
#include "orc/orc_driver.h"
#include "orc/tap_utils.h"

#define PREFIX "VETH_DRV: "

int vports = 4;

typedef struct veth_port {
    port_t port;
    char veth_name[IFNAMSIZ];
    int veth_fd;
} veth_port_t;

/***************
 * init: just print args
 */

int
veth_init_driver(orc_options_t *options, int argc, char * argv[])
{
    int i;
    for (i=0; i<argc; i++)
        printf(PREFIX " arg %d = %s\n", i, argv[i]);

    return 0;
}

/******************
 * discover_ports: generate some fake ports
 */

int
veth_discover_ports(port_t * ports[], int * num)
{
    int i, err;
    veth_port_t * vport;

    if (vports > *num)
    {
        orc_err(" need to pre-allocate more ports: want %d got %d\n",
                        vports, *num);
        return -1;
    }
    for (i=0; i<vports; i++)
    {
        vport = malloc(sizeof(*vport));
        if (vport == NULL)
        {
            orc_err("Out of mem allocating ports\n");
            return -1;
        }
        vport->port.index = i+1;
        err = create_tap_iface(NULL, &vport->veth_fd);
        if (err)
        {
            orc_err(" failed to create veth tap interface: %d\n", err);
            return -1;
        }
        ports[i] = (port_t *) vport;
        orc_debug(" created port %s for port %s\n",
                        vport->veth_name, vport->port.name);
    }
    *num = vports;
    return 0;
}

/*********
 * tx: just print to veth fd
 */

int
veth_tx_pkt(port_t *port, u8 *pkt, unsigned int len)
{
    struct veth_port * veth = (struct veth_port *) port;
    orc_debug(" TX %d byte packet %s -> %s\n",
                    len, veth->port.name, veth->veth_name);
    return write(veth->veth_fd, pkt, len);
}

int veth_start_rx(port_t * ports[], int num_ports)
{
    printf(PREFIX "Fake starting RX\n");

    return 0;
}

int veth_stop_rx()
{
    printf(PREFIX "Fake stoping RX\n");

    return 0;
}

/*****
 * Actual hooks into this driver; DRIVER_HOOKS
 * is the symbol that the main program looks for
 */


orc_driver_t DRIVER_HOOKS = {
    .init_driver = veth_init_driver,
    .discover_ports = veth_discover_ports,
    .tx_pkt = veth_tx_pkt,
    .start_rx = veth_start_rx,
    .stop_rx = veth_stop_rx

};

