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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

/*
 * Construct a "hash" such that we control which bits the
 * bloom filter uses. Very implementation-specific.
 */
static uint32_t
make_hash(int bit0, int bit1)
{
    return bit0 | (bit1 << 16);
}

static void
test_basic(void)
{
    const uint32_t h1 = make_hash(0, 1);
    const uint32_t h2 = make_hash(2, 3);
    const uint32_t h3 = make_hash(0, 3);

    /* Empty set */
    bloom_filter_t *bloom = bloom_filter_create(8);
    AIM_ASSERT(!bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h3));

    /* Add h1 */
    bloom_filter_add(bloom, h1);
    AIM_ASSERT(bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h3));

    /* Add h2. False positive for h3 */
    bloom_filter_add(bloom, h2);
    AIM_ASSERT(bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(bloom_filter_lookup(bloom, h3));

    /* Add h3 */
    bloom_filter_add(bloom, h3);
    AIM_ASSERT(bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(bloom_filter_lookup(bloom, h3));

    /* Remove h1 */
    bloom_filter_remove(bloom, h1);
    AIM_ASSERT(!bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(bloom_filter_lookup(bloom, h3));

    /* Remove h2 */
    bloom_filter_remove(bloom, h2);
    AIM_ASSERT(!bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(bloom_filter_lookup(bloom, h3));

    /* Remove h3 */
    bloom_filter_remove(bloom, h3);
    AIM_ASSERT(!bloom_filter_lookup(bloom, h1));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h2));
    AIM_ASSERT(!bloom_filter_lookup(bloom, h3));

    bloom_filter_destroy(bloom);
}

/*
 * It's unlikely but possible that the refcount of a bit overflows.
 * In this case we hold the refcount at its maximum value. Future
 * removals will not decrement the refcount, to avoid false negatives.
 */
static void
test_saturated(void)
{
    const uint32_t h = make_hash(0, 1);
    bloom_filter_t *bloom = bloom_filter_create(8);
    int i;

    for (i = 0; i < UINT16_MAX; i++) {
        bloom_filter_add(bloom, h);
        AIM_ASSERT(bloom_filter_lookup(bloom, h));
    }

    for (i = 0; i < UINT16_MAX; i++) {
        AIM_ASSERT(bloom_filter_lookup(bloom, h));
        bloom_filter_remove(bloom, h);
    }

    /* Due to saturation the refcounts were not decremented */
    AIM_ASSERT(bloom_filter_lookup(bloom, h));

    bloom_filter_destroy(bloom);
}

/*
 * Like test_saturated, but adds one fewer item to the set and
 * so doesn't hit the saturation condition.
 */
static void
test_unsaturated(void)
{
    const uint32_t h = make_hash(0, 1);
    bloom_filter_t *bloom = bloom_filter_create(8);
    int i;

    for (i = 0; i < UINT16_MAX-1; i++) {
        bloom_filter_add(bloom, h);
        AIM_ASSERT(bloom_filter_lookup(bloom, h));
    }

    for (i = 0; i < UINT16_MAX-1; i++) {
        AIM_ASSERT(bloom_filter_lookup(bloom, h));
        bloom_filter_remove(bloom, h);
    }

    AIM_ASSERT(!bloom_filter_lookup(bloom, h));

    bloom_filter_destroy(bloom);
}

/*
 * Create a bloom filter with an estimated 5% false positive rate
 * and run random queries to see if the actual false positive rate
 * is close (within 10%) to the theoretical.
 */
static void
test_false_positive_rate(void)
{
    const int num_items = 1024;
    const int num_queries = 1000000;
    const double expected_false_positive_rate = 0.05;
    bloom_filter_t *bloom = bloom_filter_create(num_items*8);
    int i;
    int hits = 0;

    for (i = 0; i < num_items; i++) {
        bloom_filter_add(bloom, random());
    }

    for (i = 0; i < num_queries; i++) {
        /*
         * It's very unlikely that random() will return a value
         * that we already added above. If it did, it would only
         * increase the observed false positive rate.
         */
        hits += bloom_filter_lookup(bloom, random()) ? 1 : 0;
    }

    double false_positive_rate = hits*1.0/num_queries;

    AIM_ASSERT(false_positive_rate > 0.9 * expected_false_positive_rate);
    AIM_ASSERT(false_positive_rate < 1.1 * expected_false_positive_rate);

    bloom_filter_destroy(bloom);
}

int aim_main(int argc, char* argv[])
{
    test_basic();
    test_saturated();
    test_unsaturated();
    test_false_positive_rate();
    return 0;
}
