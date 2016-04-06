/****************************************************************
 *
 *        Copyright 2016, Big Switch Networks, Inc.
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

/*
 * Histogram module
 *
 * This module provides histograms with uint32_t keys. Each key is
 * mapped into a bucket (counter). Multiple keys can share a bucket,
 * in which case they will be indistinguishable in the resulting
 * histogram.
 *
 * Only a few of the most significant bits of the key, plus its logarithm,
 * are used to choose a bucket. This means that the number of keys per
 * bucket increases for larger keys. For example, keys 0-31 each have their
 * own buckets, but 128-135 all share one bucket. This is how we are able
 * to support a wide range of keys in a small amount of memory. The important
 * property is that a key differs from the other keys in its bucket by less
 * than 10%.
 *
 * These histograms will be used in datapath code and so must be efficient.
 * The size and shift are not configurable to avoid extra instructions in
 * histogram_inc. For most applications the higher buckets won't be used
 * and so won't be brought into cache, making them basically free.
 */

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdint.h>
#include <AIM/aim.h>

/* histogram_bucket(UINT32_MAX) == 463 */
#define HISTOGRAM_BUCKETS 464

/* Each power of 2 is divided into 16 buckets */
#define HISTOGRAM_SHIFT 4

struct histogram {
    uint32_t counts[HISTOGRAM_BUCKETS];
    const char *name;
    struct list_links links;
};

/*
 * Create a histogram
 *
 * Automatically registers the histogram.
 *
 * 'name' will be copied.
 */
struct histogram *histogram_create(const char *name);

/*
 * Register a histogram
 *
 * Provided for applications that want to manage the memory
 * themselves. The memory must be zeroed.
 */
void histogram_register(struct histogram *hist, const char *name);

/*
 * Destroy a histogram
 *
 * Automatically unregisters the histogram.
 */
void histogram_destroy(struct histogram *hist);

/*
 * Unregister a histogram
 *
 * Provided for applications that want to manage the memory
 * themselves.
 */
void histogram_unregister(struct histogram *hist);

/*
 * Return the histogram list head
 */
struct list_head *histogram_list(void);

/*
 * Find a histogram by name
 */
struct histogram *histogram_find(const char *name);

/*
 * Map 32-bit key to bucket index
 *
 * Each power of two is divided into 16 buckets. We first calculate
 * the log base 2 and multiply by 16 to get the first bucket for that
 * power. We then add the next 4 bits after the MSB to get the final bucket.
 */
static inline uint32_t
histogram_bucket(uint32_t k)
{
    if ((k >> HISTOGRAM_SHIFT) == 0) {
        return k;
    }

    /*
     * Exponent of the key. Biased by HISTOGRAM_SHIFT because
     * we don't support non-integer keys.
     */
    uint32_t e = 31 - HISTOGRAM_SHIFT - __builtin_clz(k);

    return (e << HISTOGRAM_SHIFT) + (k >> e);
}

static inline void
histogram_inc(struct histogram *hist, uint32_t k)
{
    hist->counts[histogram_bucket(k)]++;
}

#endif
