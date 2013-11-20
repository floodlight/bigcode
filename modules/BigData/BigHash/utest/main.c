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

#include <BigHash/bighash_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <AIM/aim.h>

#include <BigHash/bighash.h>
#include <BigList/biglist.h>
#include <stdlib.h>
#include <murmur/murmur.h>
#include <AIM/aim_list.h> /* for container_of */

#define AIM_LOG_MODULE_NAME bighash_unittest
#include <AIM/aim_log.h>
AIM_LOG_STRUCT_DEFINE(
                      BIGHASH_CONFIG_LOG_OPTIONS_DEFAULT,
                      BIGHASH_CONFIG_LOG_BITS_DEFAULT,
                      NULL, /* Custom log map */
                      BIGHASH_CONFIG_LOG_CUSTOM_BITS_DEFAULT
                     );


BIGHASH_DEFINE_STATIC(static_table, 64);

typedef struct test_entry_s {
    uint32_t id;
    int found;
    bighash_entry_t hash_entry;
} test_entry_t;

static uint32_t
hash_id(uint32_t id)
{
    return murmur_hash(&id, sizeof(id), 0);
}

static test_entry_t *
find_by_id(bighash_table_t *table, uint32_t id)
{
    bighash_entry_t *e;
    for (e = bighash_first(table, hash_id(id)); e; e = bighash_next(e)) {
        test_entry_t *te = container_of(e, hash_entry, test_entry_t);
        if (te->id == id) {
            return te;
        }
    }
    return NULL;
}

static void
free_test_entry(bighash_entry_t *e)
{
    test_entry_t *te = container_of(e, hash_entry, test_entry_t);
    aim_free(te);
}

static int insert__(bighash_table_t *table, int count, biglist_t** entries)
{
    int c;
    for(c = 0; c < count; c++) {
        test_entry_t *te = aim_zmalloc(sizeof(*te));
        te->id = c;

        bighash_insert(table, &te->hash_entry, hash_id(te->id));
        if(entries) {
            *entries = biglist_prepend(*entries, te);
        }
        /** Make sure we can find it */
        test_entry_t *fe = find_by_id(table, te->id);
        if(fe == NULL) {
            AIM_DIE("inserted entry was not found, count=%d/%d", c, count);
        }
        if(fe != te) {
            AIM_DIE("Retreived pointer not equal.");
        }
        if(bighash_entry_count(table) != (c+1)) {
            AIM_DIE("Entry count mismatch: should be %d, reported as %d",
                    (c+1), bighash_entry_count(table));
        }
    }
    return 0;
}


int test_table_data__(bighash_table_t *table, biglist_t** entry_list)
{
    /* Make sure we can find all entries in the entry list */
    {
        biglist_t *ble;
        test_entry_t *te;
        BIGLIST_FOREACH_DATA(ble, *entry_list, test_entry_t*, te) {
            test_entry_t *fe = find_by_id(table, te->id);
            if(fe == NULL) {
                AIM_DIE("inserted entry (id=%d) was not found after filling table",
                        te->id);
            }
            else if(fe != te) {
                AIM_DIE("Retreived pointer does not match. (te=%p (%d), fe=%p (%d)",
                        te, te->id, fe, fe->id);
            }
        }
    }

    /* Make sure all elements in the list are enumerated */
    {
        bighash_iter_t iter;
        bighash_entry_t *e;
        int count = 0;
        for(e = bighash_iter_start(table, &iter); e; e = bighash_iter_next(&iter)) {
            test_entry_t *te = container_of(e, hash_entry, test_entry_t);

            if(te->found) {
                AIM_DIE("Entry %p:%d was already enumerated.", te, te->id);
            }
            te->found++;
            if(biglist_find(*entry_list, te) == NULL) {
                AIM_DIE("Entry %p:%d is not in the entry list.", te, te->id);
            }
            count++;
        }
        if(count != biglist_length(*entry_list)) {
            AIM_DIE("Enumeration error: count=%d, length=%d",
                    count, biglist_length(*entry_list));
        }
    }

    /* Remove from table */
    {
        biglist_t *ble;
        test_entry_t *te;
        int count = 1;
        BIGLIST_FOREACH_DATA(ble, *entry_list, test_entry_t*, te) {
            bighash_remove(table, &te->hash_entry);
            if(find_by_id(table, te->id) != NULL) {
                AIM_DIE("Removed entry was not removed.");
            }
            if(bighash_entry_count(table) != biglist_length(*entry_list)-count) {
                AIM_DIE("Entry count mismatch: hashcount=%d, listcount=%d",
                        bighash_entry_count(table), biglist_length(*entry_list)-count);
            }
            count++;
        }
    }

    /* Free entry list */
    biglist_free_all(*entry_list, aim_free);
    *entry_list = NULL;
    return 0;
}

#include "test_hashtable.h"

int
test_template(void)
{
    bighash_table_t *table = bighash_table_create(16);
    test_entry_t entry1a = { 1, 0 };
    test_entry_t entry1b = { 1, 0 };
    test_entry_t entry2a = { 2, 0 };
    test_entry_t *fe;
    uint32_t key;

    test_hashtable_insert(table, &entry1a);
    test_hashtable_insert(table, &entry1b);
    test_hashtable_insert(table, &entry2a);

    key = 1;
    fe = test_hashtable_first(table, &key);
    assert(fe == &entry1b);
    fe = test_hashtable_next(fe);
    assert(fe == &entry1a);
    fe = test_hashtable_next(fe);
    assert(fe == NULL);

    key = 2;
    fe = test_hashtable_first(table, &key);
    assert(fe == &entry2a);
    fe = test_hashtable_next(fe);
    assert(fe == NULL);

    key = 3;
    fe = test_hashtable_first(table, &key);
    assert(fe == NULL);

    return 0;
}

int main(int argc, char *argv[])
{
    biglist_t *entries = NULL;

    /** Basic Hashing -- statically allocated table */
    bighash_table_init_static(&static_table);
    insert__(&static_table, 10000, &entries);
    test_table_data__(&static_table, &entries);
    bighash_table_destroy(&static_table, NULL);

    /** Basic hashing -- dynamically allocated table */
    {
        bighash_table_t *dtable;
        dtable = bighash_table_create(64);
        insert__(dtable, 10000, &entries);
        test_table_data__(dtable, &entries);
        bighash_table_destroy(dtable, NULL);
    }

    /** Count and removal while iterating */
    {
        bighash_table_t *table;
        bighash_iter_t iter;
        bighash_entry_t *e;
        test_entry_t *te;
        int ecount=100;
        int count = 0;
        table = bighash_table_create(64);
        insert__(table, ecount, NULL);
        if(bighash_entry_count(table) != ecount) {
            AIM_DIE("Entry count mismatch: expected %d, got %d",
                    ecount, bighash_entry_count(table));
        }
        /* Iterate once, just counting */
        for(te = (test_entry_t*)bighash_iter_start(table, &iter);
            te; te = (test_entry_t*)bighash_iter_next(&iter)) {
            count++;
        }
        if(count != ecount) {
            AIM_DIE("Iteration count incorrect (%d)", count);
        }
        /* Iterate again, removing elements. Tests both iteration restart
         * and remove-while-iterating.
         */
        for(e = bighash_iter_start(table, &iter); e; e = bighash_iter_next(&iter)) {
            test_entry_t *te = container_of(e, hash_entry, test_entry_t);
            bighash_remove(table, &te->hash_entry);
            /* Clear the list links on this element to make sure the
             * removal was done properly. */
            memset(te, 0, sizeof(*te));
            aim_free(te);
        }
        if(bighash_entry_count(table) != 0) {
            AIM_DIE("Count is not zero after iterated removal (count=%d)",
                    bighash_entry_count(table));
        }
        bighash_table_destroy(table, NULL);
    }

    /** Migrating between tables */
    {
        bighash_table_t *src;
        bighash_table_t *dst;
        src = bighash_table_create(64);
        dst = bighash_table_create(128);
        insert__(src, 1000, &entries);
        bighash_entries_move(dst, src);
        if(bighash_entry_count(src) != 0) {
            AIM_DIE("Source table is not empty.");
        }
        bighash_table_destroy(src, NULL);
        test_table_data__(dst, &entries);
        bighash_table_destroy(dst, NULL);
    }

    /** Check utilization and automatic element destruction */
    bighash_table_init_static(&static_table);
    insert__(&static_table, 100000, 0);
    bighash_table_utilization_show(&static_table, &aim_pvs_stdout);
    bighash_table_destroy(&static_table, free_test_entry);

    test_template();

    return 0;
}

