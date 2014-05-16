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

#include <bloom_filter/bloom_filter.h>

/* Documented in bloom_filter.h */
bloom_filter_t *
bloom_filter_create(int size)
{
    AIM_ASSERT(aim_is_pow2_u32(size), "bloom filter size must be a power of 2");

    /*
     * We split the 32 bit hash value in two, so the largest index
     * we can use is UINT16_MAX.
     */
    if (size > UINT16_MAX+1) {
        size = UINT16_MAX+1;
    }

    bloom_filter_t *bloom = aim_zmalloc(sizeof(*bloom));

    aim_bitmap_alloc(&bloom->bitmap, size);
    bloom->refcounts = aim_zmalloc(size * sizeof(bloom->refcounts[0]));
    bloom->mask = size - 1;

    return bloom;
}

/* Documented in bloom_filter.h */
void
bloom_filter_destroy(bloom_filter_t *bloom)
{
    aim_bitmap_free(&bloom->bitmap);
    aim_free(bloom->refcounts);
    aim_free(bloom);
}

/* Documented in bloom_filter.h */
void
bloom_filter_add(bloom_filter_t *bloom, uint32_t hash)
{
    int i;
    for (i = 0; i < BLOOM_FILTER_NUM_PROBES; i++) {
        int idx = bloom_filter_probe_index(bloom, hash, i);

        if (bloom->refcounts[idx] == 0) {
            AIM_BITMAP_SET(&bloom->bitmap, idx);
        }

        /* Prevent overflow */
        if (bloom->refcounts[idx] != UINT16_MAX) {
            bloom->refcounts[idx]++;
        }
    }
}

/* Documented in bloom_filter.h */
void
bloom_filter_remove(bloom_filter_t *bloom, uint32_t hash)
{
    int i;
    for (i = 0; i < BLOOM_FILTER_NUM_PROBES; i++) {
        int idx = bloom_filter_probe_index(bloom, hash, i);

        /*
         * If the refcount is saturated at UINT16_MAX we
         * can't decrement it. Doing so would allow
         * a false negative.
         */
        if (bloom->refcounts[idx] != UINT16_MAX) {
            bloom->refcounts[idx]--;
        }

        if (bloom->refcounts[idx] == 0) {
            AIM_BITMAP_CLR(&bloom->bitmap, idx);
        }
    }
}
