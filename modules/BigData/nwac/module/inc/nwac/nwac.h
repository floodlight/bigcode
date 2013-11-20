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
 * @addtogroup nwac-nwac
 * @{
 *
 ***************************************************************/
#ifndef __NWAC_H__
#define __NWAC_H__

#include <nwac/nwac_config.h>

#if NWAC_CONFIG_INCLUDE_LOCKING == 1
#include <OS/os_sem.h>
#endif

/**
 * The beginning of all cache entries must be
 * compatible with these structures.
 *
 */
typedef struct nwac_entry_header_s {
    /** This entry is valid */
    uint32_t valid;
    /** Eviction count */
    uint32_t evictions;
    /** The last access time for this entry */
    uint64_t timestamp;
} nwac_entry_header_t;

/**
 * This structure is used to reference the entry and key data.
 */
typedef struct nwac_entry_s {
    /** Header data */
    nwac_entry_header_t hdr;
    /** The search key data for this entry */
    uint8_t key[];
} nwac_entry_t;


/**
 * NWAC object.
 */
typedef struct nwac_s {
    /** All cache entries */
    uint8_t* cache;
    /** The associativity of this cache */
    uint32_t block_size;
    /** The size in bytes of each entry in the cache */
    uint32_t entry_size;
    /** Total number of entries in this cache. */
    uint32_t entry_count;
    /** The key size for all entries */
    uint32_t key_size;

    /** This NWAC was allocated */
#define NWAC_F_NWAC_ALLOC   0x1
    /** The cache data was allocated */
#define NWAC_F_CACHE_ALLOC  0x2
    /** NWAC flags */
    uint32_t flags;

#if NWAC_CONFIG_INCLUDE_LOCKING == 1
    /** Cache lock. */
    os_sem_t lock;
#endif

    /** The number of blocks in the cache */
    uint32_t block_count;
} nwac_t;

/**
 * @brief Create an NWAC.
 * @param n The associativity of the cache.
 * @param key_size The size of each entrie's key data.
 * @param entry_size The size of each cache entry.
 * @param entry_count The total number of entries in the cache.
 */
nwac_t* nwac_create(uint32_t n, uint32_t key_size,
                    uint32_t entry_size, uint32_t entry_count);

/**
 * @brief Initialize an NWAC.
 * @param nwac The cache.
 * @param n The associativity of the cache.
 * @param key_size The size of each entrie's key data.
 * @param entry_size The size of each cache entry.
 * @param entry_count The total number of entries in the cache.
 */
int nwac_init(nwac_t* nwac, uint32_t n, uint32_t key_size,
              uint32_t entry_size, uint32_t entry_count);

/**
 * @brief Declare a static nwac.
 */
#define NWAC_DEFINE_STATIC(_name, _n, _entry_size, _entry_count, _key_size) \
    static uint8_t _name##_cache__[_entry_size*_entry_count];           \
    nwac_t _name = { _name##_cache__, _n, _entry_size, _entry_count,    \
                     _key_size, 0 };                                    \

/**
 * @brief Initialize a static nwac.
 * @param nwac The NWAC.
 */
int nwac_init_static(nwac_t* nwac);

/**
 * @brief Destroy an NWAC.
 * @param nwac The NWAC.
 */
void nwac_destroy(nwac_t* nwac);


/**
 * @brief Lock an NWAC.
 * @param nwac The NWAC.
 * @note This is a noop unless NWAC_CONFIG_INCLUDE_LOCKING == 1
 */
void nwac_lock(nwac_t* nwac);

/**
 * @brief Unlock an NWAC.
 * @param nwac The NWAC.
 * @note This is a noop unless NWAC_CONFIG_INCLUDE_LOCKING == 1
 */
void nwac_unlock(nwac_t* nwac);


#if NWAC_CONFIG_INCLUDE_LOCKING == 1
/** Lock macro */
#define NWAC_LOCK(_n) nwac_lock(_n)
/** Unlock macro */
#define NWAC_UNLOCK(_n) nwac_unlock(_n)
#else
/** Lock macro */
#define NWAC_LOCK(_n)
/** Unlock macro */
#define NWAC_UNLOCK(_n)
#endif


/**
 * @brief Search the NWAC for the given key starting in the given block.
 * @param nwac The NWAC.
 * @param block The block number.
 * @param key The key data.
 * @param now The current time. Entries will be evicted based on LRU. Set this value
 * to zero to disable eviction.
 * @returns The entry pointer, if found in the cache (valid = 1)
 * @returns A new entry pointer, if not found in the cache (valid = 0)
 * @returns NULL if full and eviction is disabled.
 */
nwac_entry_t* nwac_search_block(nwac_t* nwac, uint32_t block, uint8_t* key,
                                uint64_t now);


/**
 * @brief Search the NWAC for the given key using the given hash.
 * @param nwac The NWAC.
 * @param hash The hash value for the key.
 * @param key The key data.
 * @param now The current time. Entries will be evicted based on LRU. Set this value
 * to zero to disable eviction.
 * @note The block number will be calculated as (hash) % (block_count)
 * @returns See nwac_search_block()
 */
nwac_entry_t* nwac_search_hash(nwac_t* nwac, uint32_t hash, uint8_t* key,
                               uint64_t now);

/**
 * @brief Search the NWAC for the given key.
 * @param nwac The NWAC.
 * @param key The key data.
 * @param now The current time. Entries will be evicted based on LRU. Set this value
 * to zero to disable eviction.
 * @note A hash will be calculated over all data in the key.
 * @returns See nwac_search_block()
 */
nwac_entry_t* nwac_search(nwac_t* nwac, uint8_t* key, uint64_t now);


/**
 * Custom entry dumper
 */
typedef void (nwac_entry_show_f)(nwac_t* nwac, nwac_entry_t* entry, aim_pvs_t* pvs);


/**
 * @brief Dump an NWAC block.
 * @param nwac The NWAC.
 * @param block The block number.
 * @param pvs The output PVS.
 * @param custom Custom entry dumper (optional)
 *
 */
void nwac_block_show(nwac_t* nwac, uint32_t block, aim_pvs_t* pvs,
                     nwac_entry_show_f custom);

/**
 * @brief Dump an NWAC.
 * @param nwac The NWAC.
 * @param pvs The output PVS.
 * @param custom Custom entry dumper (optional)
 */
void nwac_show(nwac_t* nwac, aim_pvs_t* pvs, nwac_entry_show_f custom);


/**
 * @brief Clear all cache entries.
 * @param nwac The NWAC.
 */
void nwac_clear(nwac_t* nwac);

#endif /* __NWAC_H__ */
/* @} */
