/****************************************************************
 *
 *        Copyright 2014, Big Switch Networks, Inc.
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

/**
 * Bloom filter
 *
 * This datastructure supports the query "is this item possibly in the set?"
 * The advantage over using a hashtable to do the same thing is better cache
 * efficiency.
 *
 * See http://pages.cs.wisc.edu/~cao/papers/summary-cache/node8.html
 *
 * This implementation fixes the number of probes (K) at 2 for performance.
 * To adjust the false positive rate, change the ratio between the size of the
 * filter in bits (M) and the maximum number of items it contains (N).
 *
 * Common ratios are:
 *   M/N = 8 -> 5% false positive rate
 *   M/N = 16 -> 1.4% false positive rate
 *
 * The largest allowed value of M is 65536. Larger values will be silently
 * ignored.
 */

#ifndef __BLOOM_FILTER_H__
#define __BLOOM_FILTER_H__

#include <stdint.h>
#include <stdbool.h>
#include <AIM/aim.h>

typedef struct bloom_filter bloom_filter_t;

/**
 * Create a bloom filter
 *
 * @param m Size of the bloom filter, in bits. Must be a power of 2.
 */
bloom_filter_t *bloom_filter_create(int m);

/**
 * Destroy a bloom filter
 */
void bloom_filter_destroy(bloom_filter_t *bloom);

/**
 * Add an item to the set
 *
 * @param hash Hash of the item
 */
void bloom_filter_add(bloom_filter_t *b, uint32_t hash);

/**
 * Remove an item from the set
 *
 * @param hash Hash of the item
 */
void bloom_filter_remove(bloom_filter_t *b, uint32_t hash);

/**
 * Check whether an item might exist in the set
 *
 * @param hash Hash of the item
 */
static inline bool bloom_filter_lookup(bloom_filter_t *b, uint32_t hash);


/* Private inline functions */

#define BLOOM_FILTER_NUM_PROBES 2

struct bloom_filter {
    aim_bitmap_t bitmap;
    uint16_t *refcounts; /* Reference count for each bit in the bitmap */
    uint32_t mask; /* Size minus one */
};

/**
 * Return the bitmap index to use for a given hash and probe iteration.
 */
static inline int
bloom_filter_probe_index(bloom_filter_t *bloom, uint32_t hash, int i)
{
    switch (i) {
    case 0: return hash & bloom->mask;
    case 1: return (hash >> 16) & bloom->mask;
    default: AIM_TRUE_OR_DIE(0, "unsupported probe count");
    };
}

static inline bool
bloom_filter_lookup(bloom_filter_t *bloom, uint32_t hash)
{
    int i;
    for (i = 0; i < BLOOM_FILTER_NUM_PROBES; i++) {
        int idx = bloom_filter_probe_index(bloom, hash, i);

        if (AIM_BITMAP_GET(&bloom->bitmap, idx) == 0) {
            return false;
        }
    }

    return true;
}

#endif
