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

#include <nwac/nwac_config.h>
#include <nwac/nwac.h>
#include "nwac_log.h"

#include <murmur/murmur.h>

static inline int
cache_config_validate__(uint32_t n, uint32_t key_size,
                        uint32_t entry_size, uint32_t entry_count)
{
    if(entry_size == 0 || entry_count == 0 || n == 0) {
        return 0;
    }
    if( (entry_count % n) != 0) {
        return 0;
    }
    if(key_size >= entry_size) {
        return 0;
    }
    return 1;
}


nwac_t*
nwac_create(uint32_t n, uint32_t key_size,
            uint32_t entry_size, uint32_t entry_count)
{
    nwac_t* rv;

    rv = aim_zmalloc(sizeof(*rv));
    rv->flags |= NWAC_F_NWAC_ALLOC;

    if(nwac_init(rv, n, key_size, entry_size, entry_count) < 0) {
        aim_free(rv);
        rv = NULL;
    }

    return rv;
}

int
nwac_init(nwac_t* nwac, uint32_t n, uint32_t key_size,
          uint32_t entry_size, uint32_t entry_count)
{
    if(!cache_config_validate__(n, key_size, entry_size, entry_count)) {
        AIM_LOG_ERROR("Cache configuration %d:%d:%d is invalid.",
                      n, entry_size, entry_count);
        return -1;
    }

    if(nwac->cache == NULL) {
        nwac->cache = aim_zmalloc(entry_size*entry_count);
        nwac->flags |= NWAC_F_CACHE_ALLOC;
    }
    else {
        NWAC_MEMSET(nwac->cache, 0, entry_size*entry_count);
    }
    nwac->entry_size = entry_size;
    nwac->entry_count = entry_count;
    nwac->block_size = n;
    nwac->block_count = entry_count/n;
    nwac->key_size = key_size;

#if NWAC_CONFIG_INCLUDE_LOCKING == 1
    nwac->lock = os_sem_create(1);
#endif
    return 0;
}

int
nwac_init_static(nwac_t* nwac)
{
    return nwac_init(nwac, nwac->block_size, nwac->key_size,
                     nwac->entry_size, nwac->entry_count);
}

void
nwac_lock(nwac_t* nwac)
{
#if NWAC_CONFIG_INCLUDE_LOCKING == 1
    os_sem_take(nwac->lock);
#endif
}

void
nwac_unlock(nwac_t* nwac)
{
#if NWAC_CONFIG_INCLUDE_LOCKING == 1
    os_sem_give(nwac->lock);
#endif
}


#define NWAC_ENTRY_FIRST(_nwac, _block)                                 \
    ( (nwac_entry_t*) ( (_nwac->cache + nwac->block_size*block*nwac->entry_size) ))
#define NWAC_ENTRY_NEXT(_nwac, _ptr)                    \
    ( (nwac_entry_t*) ( ( ((uint8_t*)_ptr) + nwac->entry_size)))


static inline int
nwac_block_first__(nwac_t* nwac, uint32_t block, nwac_entry_t** first)
{
    if(block >= nwac->block_count) {
        AIM_LOG_ERROR("block %d is invalid. Must be between 0 and %d",
                      block, nwac->block_count-1);
        return -1;
    }
    *first = NWAC_ENTRY_FIRST(nwac, block);
    return 0;
}

#define NWAC_BLOCK_ITER(_nwac, _counter, _first, _entry)        \
    for(_entry = _first, _counter = 0;                          \
        _counter < nwac->block_size;                            \
        _entry = NWAC_ENTRY_NEXT(_nwac, _entry), _counter++)

nwac_entry_t*
nwac_search_block(nwac_t* nwac, uint32_t block, uint8_t* key, uint64_t now)
{
    nwac_entry_t* empty = NULL;
    nwac_entry_t* entry;
    nwac_entry_t* first;
    nwac_entry_t* oldest = NULL;
    int count;

    if(nwac_block_first__(nwac, block, &first) < 0) {
        return NULL;
    }

    /**
     * Search all entries in this block.
     * Keep track of the first empty entry so we can return it
     * in case we miss.
     */
    oldest = first;
    NWAC_BLOCK_ITER(nwac, count, first, entry) {
        if(entry->hdr.valid) {
            if(NWAC_MEMCMP(entry->key, key, nwac->key_size) == 0) {
                /* Found */
                if(now) {
                    entry->hdr.timestamp = now;
                }
                return entry;
            }
            else {
                /* Keep track of the oldest entry in this block */
                if(entry->hdr.timestamp < oldest->hdr.timestamp) {
                    oldest = entry;
                }
            }
        }
        else {
            if(empty == NULL) {
                /* This is the first empty entry in this block */
                empty = entry;
            }
        }
    }
    if(empty) {
        /** We didn't find the entry, but there was a free slot */
        empty->hdr.timestamp = now;
        return empty;
    }

    if(now) {
        /** flush the oldest and return it */
        oldest->hdr.valid = 0;
        oldest->hdr.timestamp = now;
        oldest->hdr.evictions++;
        return oldest;
    }

    /** Full, but timestamp disabled. */
    return NULL;
}

nwac_entry_t*
nwac_search_hash(nwac_t* nwac, uint32_t hash, uint8_t* key, uint64_t now)
{
    uint32_t block = hash % (nwac->block_count);
    return nwac_search_block(nwac, block, key, now);
}

nwac_entry_t*
nwac_search(nwac_t* nwac, uint8_t* key, uint64_t now)
{
    uint32_t hash = murmur_hash(key, nwac->key_size, 0);
    return nwac_search_hash(nwac, hash, key, now);
}

void
nwac_block_show(nwac_t* nwac, uint32_t block, aim_pvs_t* pvs,
                nwac_entry_show_f custom)
{
    nwac_entry_t* entry;
    nwac_entry_t* first;
    int counter;

    if(nwac_block_first__(nwac, block, &first) < 0) {
        return;
    }
    aim_printf(pvs, "block %d:\n", block);
    NWAC_BLOCK_ITER(nwac, counter, first, entry) {
        aim_printf(pvs, "    valid=%d e=%d", entry->hdr.valid, entry->hdr.evictions);
        if(entry->hdr.valid) {
            aim_printf(pvs, " ts=%d key=[ %{data}]", entry->hdr.timestamp, entry->key, nwac->key_size);
            if(custom) {
                aim_printf(pvs, " :: ");
                custom(nwac, entry, pvs);
            }
            aim_printf(pvs, "\n");
        }
        else {
            aim_printf(pvs, "\n");
        }
    }
}

void
nwac_show(nwac_t* nwac, aim_pvs_t* pvs, nwac_entry_show_f custom)
{
    int block;
    for(block = 0; block < nwac->block_count; block++) {
        nwac_block_show(nwac, block, pvs, custom);
    }
}

void
nwac_destroy(nwac_t* nwac)
{
    if(nwac) {

#if NWAC_CONFIG_INCLUDE_LOCKING == 1
        os_sem_destroy(nwac->lock);
#endif
        if(nwac->flags & NWAC_F_CACHE_ALLOC) {
            aim_free(nwac->cache);
            nwac->cache = NULL;
        }
        if(nwac->flags & NWAC_F_NWAC_ALLOC) {
            aim_free(nwac);
        }
    }
}

void
nwac_clear(nwac_t* nwac)
{
    if(nwac) {
        NWAC_MEMSET(nwac->cache, 0, nwac->entry_size*nwac->entry_count);
    }
}
