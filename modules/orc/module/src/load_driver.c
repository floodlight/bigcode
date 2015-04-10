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

#include <dlfcn.h>
#include <stdio.h>


#include <sys/stat.h>
#include <sys/types.h>

#include "orc/load_driver.h"
#include "orc/orc_logger.h"
#include "orc/options.h"


int locate_driver(orc_options_t * options, char * filename, int len);

#ifndef ORC_STATIC_DRIVER
static void * Driver_Symbols_Handle;
#else
    /* prototypes for all of the ORC BRCM functions - for static linking for debugging */
     int brcm_init_driver(struct orc_options_s * options, int argc, char * argv[]);
     int brcm_discover_ports(port_t * ports[], int * num);
     int discover_ports(port_t * ports[], int * num);
     int brcm_start_rx(port_t * ports[], int num_ports);
     int brcm_stop_rx(void);
     int brcm_tx_pkt(port_t *port, u8 *pkt, unsigned int len);
     int brcm_raw_port_enable(port_t * port);
     int brcm_raw_port_disable(port_t * port);
     int brcm_add_l3_v4_interface( port_t *port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t *l3_intf_id);
     int brcm_update_l3_v4_interface( port_t *port, u8 hw_mac[6], int mtu, u32 ipv4_addr, l3_intf_id_t l3_intf_id); 
     int brcm_del_l3_interface(port_t * port, l3_intf_id_t l3_intf_id);
     int brcm_add_l3_v4_next_hop( port_t * port,                         l3_intf_id_t l3_intf_id,    u8 next_hop_hw_mac[6], l3_next_hop_id_t * l3_next_hop_id);
     int brcm_del_l3_next_hop(l3_next_hop_id_t l3_next_hop_id);
     int brcm_add_l3_v4_route( u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id);
     int brcm_del_l3_v4_route( u32 ip_dst, u32 netmask, l3_next_hop_id_t l3_next_hop_id);
     void brcm_log_debug_info();
#endif

orc_driver_t * load_driver(orc_options_t * options)
{
#ifndef ORC_STATIC_DRIVER
    void * sym;
    char filename[BUFLEN];
    if (!locate_driver(options, filename, BUFLEN))
            return NULL;

    Driver_Symbols_Handle = dlopen(filename, RTLD_NOW | RTLD_GLOBAL);
    if (Driver_Symbols_Handle == NULL) {
        orc_err(" Could not open Module %s -- %s\n",
                        filename, dlerror());
        return NULL;
    }

    sym = dlsym(Driver_Symbols_Handle, DRIVER_HOOK);

    if (sym == NULL) {
        orc_err(" Module %s contains no symbol '%s' -- %s\n",
                        filename, DRIVER_HOOK, dlerror());
        return NULL;
    }

    return (orc_driver_t *) sym;
#else
    orc_log("Using statically linked driver -- ignoring dynamic\n");


    static orc_driver_t driver;
    driver.init_driver = brcm_init_driver,
    driver.discover_ports = brcm_discover_ports;
    driver.tx_pkt = brcm_tx_pkt;
    driver.start_rx = brcm_start_rx;
    driver.stop_rx = brcm_stop_rx;
    driver.raw_port_enable = brcm_raw_port_enable;
    driver.add_l3_v4_interface = brcm_add_l3_v4_interface;
    driver.del_l3_interface = brcm_del_l3_interface;
    driver.add_l3_v4_next_hop = brcm_add_l3_v4_next_hop;
    driver.del_l3_next_hop = brcm_del_l3_next_hop;
    driver.add_l3_v4_route    = brcm_add_l3_v4_route;
    driver.del_l3_v4_route    = brcm_del_l3_v4_route;
    driver.log_debug_info = brcm_log_debug_info;
    return &driver;    /* can only have one defined at a time */
#endif
}



int locate_driver(orc_options_t * options, char * filename, int len) {
    int i;
    struct stat sbuf;
    for (i=0; i < options->num_driver_paths; i++)
    {
        snprintf(filename, len, "%s/%s",
                        options->driver_paths[i],
                        options->driver);
        orc_debug("Looking for driver at: %s\n", filename);
        if (!stat(filename, &sbuf) && S_ISREG(sbuf.st_mode))
        {
            orc_log("Found driver at: %s\n", filename);
            return 1;
        }
    }
    orc_err("Failed to find driver '%s' in search path\n", options->driver);
    return 0;
}
