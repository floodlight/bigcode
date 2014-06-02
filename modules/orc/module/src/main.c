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
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "orc/orc_driver.h"
#include "orc/orc_logger.h"
#include "orc/load_driver.h"
#include "orc/netlink_mon.h"
#include "orc/options.h"
#include "orc/tap_utils.h"
#include "orc/fwd_loop.h"


#ifndef MAX_PORTS
#define MAX_PORTS 256
#endif

static orc_options_t * Options = NULL;

/***
 * Walk the driver through its life cycle then go into a pkt handling loop
 */

int
run_driver(orc_options_t * options, int argc, char * argv[])
{
    port_t * ports[MAX_PORTS];
    int num_ports = MAX_PORTS;
    int i;
    int err = 0;
    int netlink_sock;
    orc_driver_t * drv = options->drv;

    /****
     * initialize driver
     */
    if (drv->init_driver != NULL)   /* init is optional for a driver */
       err = drv->init_driver(argc, argv);
    if (err)
        orc_fatal("Driver init_driver() function failed: err=%d\n",
                        err);

    /****
     * Get list of ports from driver
     */
    if (drv->discover_ports == NULL)    /* discover_ports is required */
        orc_fatal("Driver has no valid discover_ports() function!?!\nExiting\n");
    err = drv->discover_ports(ports, &num_ports);
    if (err)
        orc_fatal("Driver discover_ports() function failed: err=%d\n", err);

    /****
     * Create tap interfaces for each port
     */
    err = tap_interfaces_create(options->ifnam_prefix, ports, num_ports);
    if (err)
        orc_fatal("Function tap_interfaces_create() failed: err=%d\n", err);

    /******
     * Start up netlink monitor, if configured to do so...
     */

    netlink_sock = -1;
    if (options->ports_only)
    {
        /* enable raw mode on each port */
        orc_info("Not enabling netlink monitoring because --ports-only set\n");
        if (drv->raw_port_enable)
        {
            for (i=0; i<num_ports; i++)
            {
                orc_debug("Enabling raw_mode for port %s\n", ports[i]->name);
                drv->raw_port_enable(ports[i]);
            }
        }
        else
        {
            orc_warn("Driver does not support raw_port_enable() -- --ports-only mode probably won't work\n");
        }
    }
    else if (    drv->add_l3_v4_interface != NULL &&
            drv->del_l3_interface != NULL &&
            drv->add_l3_v4_next_hop != NULL &&
            drv->del_l3_next_hop != NULL &&
            drv->add_l3_v4_route != NULL &&
            drv->del_l3_v4_route != NULL
            )
    {
        netlink_sock = netlink_mon_create(options);
    }
    else
    {
        orc_warn("Disabling netlink monitoring because driver does not"
                " implement required L3 functions - this is probably bad\n");
    }

    /******
     * Tell the driver we are ready to receive packets
     */

    if (drv->start_rx == NULL || drv->tx_pkt == NULL)
        orc_fatal("Driver does not implement start_rx() or tx_pkt()\n");
    err = drv->start_rx(ports, num_ports);
    if (err)
        orc_fatal("Driver start_rx() function failed: err=%d\n", err);

    /*****
     * Now do packet forwarding loop (blocks until killed) and
     * monitor netlink updates
     */

    return packet_forwarding_loop(options, drv, num_ports, ports, netlink_sock);
}

void handle_sigusr1(int ignore)
{
    if (Options != NULL &&
            Options->drv != NULL &&
            Options->drv->log_debug_info != NULL)
        Options->drv->log_debug_info();
    else
        orc_log("Got SIGUSR1, but driver does not support log_debug_info()\n");
    signal( SIGUSR1, handle_sigusr1);
}

/**********
 * main program:
 *  - parse args
 *  - find and run driver
 */

int
orc_main(int argc, char * argv[])
{
    int count;
    orc_options_t options;

    options_init_default(&options);
    /** TODO: read options from /etc/asic-orc/asic-orc.conf */
    count = options_parse(&options, argv, argc);
    argc -= count;
    argv += count;

    options.drv = load_driver(&options);

    if (options.drv == NULL)
        orc_fatal( "Err: Failed to initialize driver at %s\nexiting...\n",
                        options.driver);
    Options = &options;  // for signal handler
    signal(SIGUSR1, handle_sigusr1);
    return run_driver(&options, argc, argv);
}

