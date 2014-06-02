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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "orc/orc_logger.h"
#include "orc/neighbor.h"
#include "orc/unittest.h"

#ifndef MAX_PENDING_NEXT_HOPS
#define MAX_PENDING_NEXT_HOPS   65536
#endif

/***
 * FIXME: this critical piece of fastpath code is implemented as
 * with linked lists, when it really really should be faster.
 * Quick and simple first; avoid premature optimizations = good,
 * BUT someone needs to come back and speed up this code.
 *
 * Implementation is intentionally hidden from callers to facilitate
 * easy upgrade in the future.
 */


/***
 * A linked list of next_hop entries that we're waiting to hear
 * about, e.g., from new ARP entries
 *
 * FIXME: Replace this with a TRIE with route_ip and route_netmask
 * as keys.
 */
typedef struct pending_next_hop_s {
    char is_gateway;    // is the pending nh for a gateway or not?
    u32 pending_next_hop;
    u32 pending_netmask;
    u32 route_ip;           // this is only used if a gateway route
    u32 route_netmask;      // this is only used if a gateway route
    struct pending_next_hop_s * next;
} pending_next_hop_t;


/***
 * A list of next_hop entries that we've found out about and have
 * registered into the driver via next_hop_add()
 *
 * FIXME make me a hash table with IP as a key
 */
typedef struct next_hop_entry_s {
    u32 next_hop_ip;
    l3_next_hop_id_t next_hop_id;
    struct next_hop_entry_s * next;
} next_hop_entry_t;


/***
 * The opaque structure that holds pointers to both linked lists
 */

typedef struct next_hop_db_s {
    struct pending_next_hop_s * head;
    struct next_hop_entry_s * nh_head;
} next_hop_db;


/**********************
 * Forward declarations
 */

static int pending_next_hop_del(
        next_hop_db *db,
        u32 route_ip,
        u32 route_netmask);
static int pending_next_hop_cmp(
        u32 ip_a,
        u32 netmask_a,
        u32 ip_b,
        u32 netmask_b
        );

/*******************************
 * Initialize the in-memory DB
 */

int next_hop_db_init(next_hop_db ** db)
{
    *db = malloc(sizeof(**db));
    if ((*db) == NULL)
    {
        orc_err("Failed to alloc next_hop_db(): %s\n",
                strerror(errno));
        return -1;
    }
    (*db)->head = NULL;
    (*db)->nh_head = NULL;
    return 0;
}

int next_hop_db_free(next_hop_db * db)
{
    pending_next_hop_t * curr, * next;
    next_hop_entry_t * nh_curr, * nh_next;
    curr = db->head;
    while (curr != NULL)
    {
        next = curr->next;
        // this is a bit funky because it always deletes the first
        // element of the DB and fixes up the pointers
        pending_next_hop_del(db, curr->route_ip, curr->route_netmask);
        curr = next;
    }
    nh_curr = db->nh_head;
    while (nh_curr != NULL)
    {
        nh_next = nh_curr->next;
        free(nh_curr);
        nh_curr = nh_next;
    }

    free(db);
    return 0;
}



static int pending_next_hop_gateway_entry_init(
        pending_next_hop_t ** entry,
        u32 gateway_ip,
        u32 ip_dst,
        u32 netmask
        )
{
    (*entry) = malloc(sizeof(**entry));
    if ((*entry) == NULL)
    {
        orc_err("Failed to alloc pending_next_hop_entry(): %s\n",
                strerror(errno));
        return -1;
    }
    (*entry)->is_gateway = 1;
    (*entry)->pending_next_hop = gateway_ip;
    (*entry)->pending_netmask = 0xffffffff; /* match all bits on gateway */
    (*entry)->route_ip = ip_dst;
    (*entry)->route_netmask = netmask;

    return 0;
}
static int pending_next_hop_direct_entry_init(
        pending_next_hop_t ** entry,
        u32 ip_dst,
        u32 netmask
        )
{
    (*entry) = malloc(sizeof(**entry));
    if ((*entry) == NULL)
    {
        orc_err("Failed to alloc pending_next_hop_entry(): %s\n",
                strerror(errno));
        return -1;
    }
    (*entry)->is_gateway = 0;
    (*entry)->pending_next_hop = ip_dst;
    (*entry)->pending_netmask = netmask;

    return 0;
}

static int pending_next_hop_entry_free(pending_next_hop_t * entry)
{
    free(entry);
    return 0;
}

/**
 * Remove this entry from the linked list
 */
static int pending_next_hop_del(
        next_hop_db *db,
        u32 route_ip,
        u32 route_netmask)
{
    pending_next_hop_t * prev = NULL, * curr;
    curr = db->head;

    while ( (curr != NULL) && (pending_next_hop_cmp(
                        curr->route_ip,
                        curr->route_netmask,
                        route_ip,
                        route_netmask)!=0))
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL)
        return -1;      /* not found */
    if (prev == NULL)
        db->head = curr->next;  /* was begin of list */
    else
        prev->next = curr->next; /* was middle or end */

    pending_next_hop_entry_free(curr);
    return 0;
}

int pending_next_hop_del_gateway(next_hop_db *db,
        u32 gateway_ip,
        u32 route_ip,
        u32 route_netmask)
{
    return pending_next_hop_del(db, route_ip, route_netmask);
}

int pending_next_hop_del_direct(next_hop_db *db,
        u32 route_ip,
        u32 route_netmask)
{
    return pending_next_hop_del(db, route_ip, route_netmask);
}

/***
 * A comparison function between two CIDR masks.
 *
 * Note that with two overlapping CIDR prefixes,
 *  the shorter mask will always be "less than" the
 *  longer mask.
 *
 * @return  less than zero is a < b,
 *          greater than zero  is a > b
 *          equal is a == b
 */

static int pending_next_hop_cmp(
        u32 ip_a,
        u32 netmask_a,
        u32 ip_b,
        u32 netmask_b
        )
{
    int a_val = ip_a & netmask_a;
    int b_val = ip_b & netmask_b;
    return a_val - b_val;
}


/***********************
 * Add a pending next_hop to the DB
 *
 * We maintain sorted order for ease of searching
 */


static int pending_add(next_hop_db * db, pending_next_hop_t * entry)
{
    pending_next_hop_t * curr, *prev = NULL;
    int cmp;

    curr = db->head;

    /** look for place in sorted list **/
    while ((curr != NULL) && (
                (cmp = pending_next_hop_cmp(
                        curr->pending_next_hop,
                        curr->pending_netmask,
                        entry->pending_next_hop,
                        entry->pending_netmask)) < 0))
    {
        prev = curr;
        curr = curr->next;
    }

    if ( cmp == 0)
    {
        orc_warn("Tried to add a duplicate entry to pending next_hop db:"
                "asked: "IPV4_FORMAT "/" IPV4_FORMAT
                " existing:" IPV4_FORMAT "/" IPV4_FORMAT " -- ignoring\n",
                IPV4_ADDR_PRINT(entry->pending_next_hop),
                IPV4_ADDR_PRINT(entry->pending_netmask),
                IPV4_ADDR_PRINT(curr->pending_next_hop),
                IPV4_ADDR_PRINT(curr->pending_netmask));
        return -1;
    }
    // now add between prev and curr
    entry->next = curr;
    if (prev == NULL)       /* adding to begin of list */
        db->head = entry;
    else                    /* adding to middle or end */
        prev->next = entry;

    return 0;       /* success */
}

int pending_next_hop_add_gateway(
        next_hop_db * db,
        u32 gateway_ip,
        u32 route_ip,
        u32 route_netmask)
{
    pending_next_hop_t * entry;
    pending_next_hop_gateway_entry_init(
            &entry,
            gateway_ip,
            route_ip,
            route_netmask);

    return pending_add(db, entry);
}

int pending_next_hop_add_direct(
        next_hop_db * db,
        u32 route_ip,
        u32 route_netmask)
{
    pending_next_hop_t * entry;
    pending_next_hop_direct_entry_init(
            &entry,
            route_ip,
            route_netmask);

    return pending_add(db, entry);
}

int pending_next_hop_lookup(
        next_hop_db * db,
        u32 neighbor,
        u32 * dst_ip,
        u32 * netmask,
        int * n_entries)
{
    int max = *n_entries;
    *n_entries = 0;
    pending_next_hop_t *curr = db->head;

    /** look for place in sorted list **/
    while ((curr != NULL) && (
                pending_next_hop_cmp(
                        curr->pending_next_hop,
                        curr->pending_netmask,
                        neighbor,
                        0xffffffff) < 0)
            )
    {
        if ((neighbor & curr->pending_netmask) == curr->pending_next_hop)
        {
            if ( (*n_entries) >= max)
                return -2;      // return array full
            dst_ip[*n_entries] = curr->pending_next_hop;
            netmask[*n_entries] = curr->pending_netmask;
            (*n_entries) ++;
        }
        curr = curr->next;
    }

    return (*n_entries > 0)? 1 : 0;
}

/********************
 * Next_Hop management utilities: this is basically a Map() function from IP --> next_hop_id
 *
 * FIXME handle diplci
 *
 * Implemented as a linked list.
 */

/*******
 * Add an IP->id mapping
 *
 * @return 0 on success, -1 on error (ENOMEM), 1 on already exists
 */

int next_hop_add(
        next_hop_db * db,
        u32 next_hop_ip,
        l3_next_hop_id_t next_hop_id)
{
    next_hop_entry_t * curr;
    curr = db->nh_head;
    while (curr != NULL)
    {
        if (curr->next_hop_ip == next_hop_ip)
            return 1;
        curr = curr->next;
    }
    /* curr is currently NULL, just reuse */
    curr = malloc(sizeof(next_hop_entry_t));
    if (curr == NULL)
    {
        orc_err("Failed to allocate memory\n");
        return -1;
    }
    curr->next_hop_ip = next_hop_ip;
    curr->next_hop_id = next_hop_id;
    curr->next = db->nh_head;   // add to front of list
    db->nh_head = curr;

    return 0;
}


/*******
 * Del an IP->id mapping
 *
 * @return 0 on success, -1 on error (??), 1 if doesn't exist
 */

int next_hop_del(
        next_hop_db * db,
        u32 next_hop_ip
        )
{
    next_hop_entry_t * curr, *prev;
    prev = NULL;
    curr = db->nh_head;
    while (curr != NULL)
    {
        if (curr->next_hop_ip == next_hop_ip)
            break;
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL)
        return 1;   /* not found */

    if (prev != NULL)
        prev->next = curr->next;
    else
        db->nh_head = curr->next;
    free(curr);

    return 0;
}

/******
 * Lookup:
 * @param[out] next_hop_id is populated with the valid next_hop_id if 1 is returned, else undefined
 * @return 1 if found, 0 if not
 */

int next_hop_lookup(
        next_hop_db *db,
        u32 next_hop_ip,
        l3_next_hop_id_t * next_hop_id
        )
{
    next_hop_entry_t * curr;
    curr = db->nh_head;
    while (curr != NULL)
    {
        if (curr->next_hop_ip == next_hop_ip)
        {
            *next_hop_id = curr->next_hop_id;
            return 1;
        }
        curr = curr->next;
    }
    return 0;   /* not found */
}


int unittest_next_hop(void)
{
    next_hop_db *db;
    l3_next_hop_id_t test_id;
    u32 ip1 = 1; l3_next_hop_id_t id1 = 1;
    u32 ip2 = 2; l3_next_hop_id_t id2 = 2;
    u32 ip3 = 3; l3_next_hop_id_t id3 = 3;

    TEST("NH_DB_INIT", assert(next_hop_db_init(&db) == 0));
    TEST("NH_ADD1", assert(next_hop_add(db, ip1, id1) == 0));
    TEST("NH_ADD1_DUP", assert(next_hop_add(db, ip1, id1) == 1));
    TEST("NH_LOOKUP1", assert(next_hop_lookup(db, ip1, &test_id) == 1));
    TEST("NH_LOOKUP1-verify", assert(test_id == id1));
    TEST("NH_ADD2", assert(next_hop_add(db, ip2, id2) == 0));
    TEST("NH_LOOKUP2", assert(next_hop_lookup(db, ip2, &test_id) == 1));
    TEST("NH_LOOKUP2-verify", assert(test_id == id2));

    TEST("NH_DEL2", assert(next_hop_del(db, ip2) == 0));
    TEST("NH_DEL2-DUP", assert(next_hop_del(db, ip2) == 1));
    TEST("NH_LOOKUP2_FAIL", assert(next_hop_lookup(db, ip2, &test_id) == 0));

    TEST("NH_ADD3", assert(next_hop_add(db, ip3, id3) == 0));
    TEST("NH_LOOKUP1b", assert(next_hop_lookup(db, ip1, &test_id) == 1));
    TEST("NH_LOOKUP1b-verify", assert(test_id == id1));

    TEST("NH_DB_FREE", assert(next_hop_db_free(db) == 0));
    return 0;
}
