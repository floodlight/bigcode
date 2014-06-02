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

#ifndef NEIGHBOR_H
#define NEIGHBOR_H

/******
 * This file contains the data structures and relevant access methods
 * to look up pending next_hop addresses.
 *
 * Basically, at the time we learn a 'direct attach' route, we
 * don't yet know the next_hop information for hosts in that route.
 *
 * So, this structure stores them for easy lookup once we get the
 * corresponding NEIGHBOR update and can then fully populate the route
 *
 * The implementation of the DB is intentionally hidden as we will
 * want to improve it over time.
 *
 * FIXME Explain these semantics more sanely.
 */

#include "orc_driver.h"
#include "utils.h"

typedef struct pending_next_hop_s pending_next_hop_t;

typedef struct next_hop_db_s next_hop_db;

#ifndef MAX_PENDING_NEXT_HOPS
#define MAX_PENDING_NEXT_HOPS   65536
#endif


/***
 * Initialize the in-memory DB that holds the pending next_hop entires.
 * The caller is only expected to provide a pointer to a pointer and
 * in fact cannot allocate this structure directly - by design
 *
 * @return 0 on success, -1 on failure
 */
int next_hop_db_init(next_hop_db ** db);

/***
 * Releases resources associated with the pending next_hop db
 */
int next_hop_db_free(next_hop_db * db);

/***
 * Add this pending next hop to the DB
 */

int pending_next_hop_add_gateway(next_hop_db * db,
        u32 gateway_ip,
        u32 route_ip,
        u32 route_netmask
        );
int pending_next_hop_add_direct(next_hop_db * db,
        u32 route_ip,
        u32 route_netmask
        );

/***
 * Del this pending next hop to the DB
 */

int pending_next_hop_del_gateway(next_hop_db * db,
        u32 gateway_ip,
        u32 route_ip,
        u32 route_netmask
        );
int pending_next_hop_del_direct(next_hop_db * db,
        u32 route_ip,
        u32 route_netmask
        );

/**
 * Lookup in the pending db to see if we have a route waiting on
 * this next_hop.
 *
 * If found, then ip_dst and netmask will be populated with the corresponding
 * routes needed for this next_hop.  There could be zero, one, or many
 * routes depending on this next_hop entry.
 *
 * The matching routes means if this is a gateway route,
 * ip_dst/netmask will correspond to the CIDR address of that route.
 *
 * If the route is a direct attach route, then the route will correspond to
 * a /32 host route.
 *
 * FIXME initial implementation is linear in the number of routes
 * Come back and replace with a TRIE for performance.
 *
 * @in param:
 *
 * @return 1 if found, 0 if not, -1 if error
 */

int pending_next_hop_lookup(
        next_hop_db * db,
        u32 neighbor,
        u32 * dst_ip,
        u32 * netmask,
        int * n_entries);


/**************
 * Lookup the mapping between this next_hop IP and the ID as returned from
 * the driver call add_l3_v4_next_hop()
 *
 * @param[in]   db -- as initialized by next_hop_db_create()
 * @param[in]   next_hop_ip -- an ipv4 address
 * @param[out] next_hop_id is populated with the valid next_hop_id if 1 is returned, else undefined
 *
 * @return 1 if found, 0 if not
 */

int next_hop_add(
       next_hop_db * db,
       u32 next_hop_ip,
       l3_next_hop_id_t next_hop_id
       );

int next_hop_del(
       next_hop_db * db,
       u32 next_hop_ip
       );

int next_hop_lookup(
       next_hop_db * db,
       u32 next_hop_ip,
       l3_next_hop_id_t * next_hop_id
       );

int unittest_next_hop(void);

#endif
