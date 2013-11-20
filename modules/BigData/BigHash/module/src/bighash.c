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
#include <BigHash/bighash.h>
#include "bighash_log.h"

bighash_table_t *
bighash_table_create(int bucket_count)
{
    bighash_table_t *table = aim_zmalloc(sizeof(*table));
    table->flags |= BIGHASH_TABLE_F_TABLE_ALLOCATED;
    bighash_table_init(table, bucket_count);
    return table;
}

int
bighash_table_init_static(bighash_table_t *table)
{
    return bighash_table_init(table, table->bucket_count);
}

static void
bighash_table_init_buckets__(bighash_table_t *table)
{
    int i;
    for(i = 0; i < table->bucket_count; i++) {
        table->buckets[i] = NULL;
    }
}

int
bighash_table_init(bighash_table_t *table, int bucket_count)
{
    if(table->buckets == NULL) {
        table->buckets = aim_zmalloc(sizeof(table->buckets[0])*bucket_count);
        table->flags |= BIGHASH_TABLE_F_BUCKETS_ALLOCATED;
    }
    table->bucket_count = bucket_count;

    bighash_table_init_buckets__(table);

    return 0;
}

void
bighash_table_destroy(bighash_table_t *table, bighash_entry_free_f efree)
{
    /* Destroy or invalidate all entries */
    int b;
    for(b = 0; b < table->bucket_count; b++) {
        bighash_entry_t *cur, *next;
        cur = table->buckets[b];
        while (cur != NULL) {
            next = cur->next;
            cur->next = NULL;
            cur->hash = 0;
            if(efree) {
                efree(cur);
            }
            cur = next;
        }
    }

    if(table->flags & BIGHASH_TABLE_F_BUCKETS_ALLOCATED) {
        aim_free(table->buckets);
        table->buckets = NULL;
        table->bucket_count = 0;
    }

    if(table->flags & BIGHASH_TABLE_F_TABLE_ALLOCATED) {
        aim_free(table);
    }
}


bighash_entry_t **
bighash_bucket(bighash_table_t *table, uint32_t hash)
{
    int bucket = hash % table->bucket_count;
    return &table->buckets[bucket];
}

void
bighash_insert(bighash_table_t *table, bighash_entry_t *e, uint32_t hash)
{
    bighash_entry_t **bucket = bighash_bucket(table, hash);
    e->next = *bucket;
    e->hash = hash;
    *bucket = e;
    table->entry_count++;
}

bighash_entry_t *
bighash_first(bighash_table_t *table, uint32_t hash)
{
    bighash_entry_t *e = *bighash_bucket(table, hash);
    while (e != NULL) {
        if (e->hash == hash) {
            return e;
        }
        e = e->next;
    }
    return NULL;
}

bighash_entry_t *
bighash_next(bighash_entry_t *e)
{
    uint32_t hash = e->hash;
    e = e->next;
    while (e != NULL) {
        if (e->hash == hash) {
            return e;
        }
        e = e->next;
    }
    return NULL;
}

void
bighash_remove(bighash_table_t *table, bighash_entry_t *e)
{
    bighash_entry_t **prev_ptr = bighash_bucket(table, e->hash);
    bighash_entry_t *cur = *prev_ptr;
    while (cur != NULL) {
        if (cur == e) {
            *prev_ptr = cur->next;
            table->entry_count--;
            return;
        }
        prev_ptr = &cur->next;
        cur = *prev_ptr;
    }
}

void
bighash_table_utilization_show(bighash_table_t *table, aim_pvs_t *pvs)
{
    int i;
    int count = 0;

    aim_printf(pvs, "table: %d buckets\n", table->bucket_count);

    for(i = 0; i < table->bucket_count; i++) {
        int c = 0;
        bighash_entry_t *cur = table->buckets[i];
        while (cur != NULL) {
            c++;
            cur = cur->next;
        }
        aim_printf(pvs, "    %.4d: %d \n", i, c);
        count += c;
    }
}

static int
next_nonempty_bucket__(bighash_table_t *table, int current)
{
    int b;
    for(b = current+1; b < table->bucket_count; b++) {
        if(table->buckets[b] != NULL) {
            return b;
        }
    }
    return -1;
}

void *
bighash_iter_start(bighash_table_t *table, bighash_iter_t *iter)
{
    iter->table = table;
    iter->current_bucket = -1;
    iter->next_entry = NULL;
    return bighash_iter_next(iter);
}

void *
bighash_iter_next(bighash_iter_t *iter)
{
    bighash_entry_t *cur = iter->next_entry;

    if (cur == NULL) {
        /* Advance to next bucket */
        int next_bucket = next_nonempty_bucket__(iter->table, iter->current_bucket);
        if (next_bucket == -1) {
            /* Finished all buckets */
            return NULL;
        }
        iter->current_bucket = next_bucket;
        cur = iter->table->buckets[iter->current_bucket];
    }

    iter->next_entry = cur->next;

    return cur;
}

int
bighash_entry_count(bighash_table_t *table)
{
    return table->entry_count;
}

int
bighash_entries_move(bighash_table_t *dst, bighash_table_t *src)
{
    bighash_iter_t iter;
    bighash_entry_t *e;
    for(e = bighash_iter_start(src, &iter); e; e = bighash_iter_next(&iter)) {
        bighash_remove(src, e);
        bighash_insert(dst, e, e->hash);
    }
    return 0;
}
