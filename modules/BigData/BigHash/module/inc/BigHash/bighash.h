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
/************************************************************//**
 *
 * @file
 * @addtogroup bighash-bighash
 * @{
 *
 ***************************************************************/
#ifndef __BIGHASH_H__
#define __BIGHASH_H__

#include <BigHash/bighash_config.h>

struct bighash_entry_s;

/**
 * All hash tables are arrays of list_head structures.
 * These can be statically or dynamically allocated.
 */
typedef struct bighash_table_s {
    /** All buckets */
    struct bighash_entry_s **buckets;
    /** Number of buckets */
    int bucket_count;

    /** The table structure itself was allocated */
#define BIGHASH_TABLE_F_TABLE_ALLOCATED   0x1
    /** The hash bucket array was allocated */
#define BIGHASH_TABLE_F_BUCKETS_ALLOCATED 0x2

    /** Table Flags */
    uint32_t flags;

    /** Current number of entries in this table. */
    int entry_count;
} bighash_table_t;


/**
 * Iterator over hastable entries
 */
typedef struct bighash_iter_s {
    /** Hashtable. Must not be freed during iteration */
    bighash_table_t *table;
    /** Current bucket in the iteration */
    int current_bucket;
    /** Next entry in the iteration, or NULL if at end of bucket */
    struct bighash_entry_s *next_entry;
} bighash_iter_t;


/**
 * Hashtable entry
 *
 * Should be embedded in another struct.
 */
typedef struct bighash_entry_s {
    /** next entry */
    struct bighash_entry_s *next;
    /** hash value */
    uint32_t hash;
} bighash_entry_t;


/**
 * @brief Create a hash table.
 * @param bucket_count Number of buckets to allocation.
 * @returns The new hash table.
 */
bighash_table_t *bighash_table_create(int bucket_count);

/**
 * @brief Initialize a hash table structure.
 * @param table The table to initialize.
 * @param bucket_count The bucket count;
 */
int bighash_table_init(bighash_table_t *table, int bucket_count);

/**
 * @brief Declare a static hash table.
 */
#define BIGHASH_DEFINE_STATIC(_name, _bucket_count)                     \
    static bighash_entry_t *_name##_buckets__[_bucket_count];           \
    bighash_table_t _name = { _name##_buckets__, _bucket_count, 0 }

/**
 * @brief Initialize a static hash table.
 * @param table The static hash table.
 */
int bighash_table_init_static(bighash_table_t *table);

/**
 * Callback for entry destruction.
 */
typedef void (bighash_entry_free_f)(bighash_entry_t *entry);

/**
 * @brief Destroy a hash table.
 * @param table The table to destroy.
 * @param free The entry free function (optional)
 */
void bighash_table_destroy(bighash_table_t *table, bighash_entry_free_f free);

/**
 * @brief Begin iterating over entries with the given hash code.
 * @param table The hash table.
 * @param hash The hash code.
 * @returns The pointer to the first entry with the given hash code.
 * @returns NULL if no entries exist with the given hash code.
 */
bighash_entry_t *bighash_first(bighash_table_t *table, uint32_t hash);

/**
 * @brief Continue iterating over entries with a given hash code.
 * @param entry The previous entry.
 * @returns The pointer to the next entry with the same hash code.
 * @returns NULL if no more entries exist with the same hash code.
 */
bighash_entry_t *bighash_next(bighash_entry_t *entry);

/**
 * @brief Insert an entry into the hash table.
 * @param table The hash table.
 * @param entry The entry (does not need to be initialized).
 * @param hash The hash code.
 */
void bighash_insert(bighash_table_t *table, bighash_entry_t *entry, uint32_t hash);

/**
 * @brief Remove an entry from the hash table.
 * @param table The hash table.
 * @param entry The entry.
 */
void bighash_remove(bighash_table_t *table, bighash_entry_t *entry);


/**
 * @brief Dump the utilization information.
 * @param table The hash table.
 * @param pvs The output pvs.
 */
void bighash_table_utilization_show(bighash_table_t *table, aim_pvs_t *pvs);

/**
 * @brief Start iteration over all entries in the table.
 * @param table The hash table.
 * @param iter The iterator to initialize.
 * @returns The first element in the iteration, or NULL if empty.
 */
void *bighash_iter_start(bighash_table_t *table, bighash_iter_t *iter);

/**
 * @brief Get the next element in the current iteration.
 * @param iter The iterator.
 * @returns The next element, or NULL if the end has been reached.
 */
void *bighash_iter_next(bighash_iter_t *iter);


/**
 * @brief Get the number of entries in the table.
 * @param table The hash table.
 * @returns The count.
 * @note This is O(1)
 */
int bighash_entry_count(bighash_table_t *table);

/**
 * @brief Move all entries from one table to another.
 * @param dst The destination table.
 * @param src The source table.
 * @note This performs a remove from the src table and an insertion
 * into the dst table. This can be used to dynamically migrate
 * table sizes and hash functions.
 */
int bighash_entries_move(bighash_table_t *dst, bighash_table_t *src);

#endif /* __BIGHASH__ */
/* @} */
