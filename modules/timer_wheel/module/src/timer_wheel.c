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

/*
 * Timer wheel implementation
 */

#include <timer_wheel/timer_wheel.h>
#include <stddef.h>
#include <AIM/aim.h>

struct timer_wheel {
    uint64_t current;
    int num_buckets;
    int bucket_shift;
    timer_wheel_entry_t **buckets;
};

static uint32_t timer_wheel_index(timer_wheel_t *tw, uint64_t time);


/* Public interface */

timer_wheel_t *
timer_wheel_create(int num_buckets, int bucket_size, uint64_t now)
{
    AIM_TRUE_OR_DIE(aim_is_pow2_u32(num_buckets));
    AIM_TRUE_OR_DIE(aim_is_pow2_u32(bucket_size));
    timer_wheel_t *tw = aim_zmalloc(sizeof(*tw));
    tw->current = now;
    tw->num_buckets = num_buckets;
    tw->bucket_shift = aim_log2_u32(bucket_size);
    tw->buckets = aim_zmalloc(num_buckets * sizeof(timer_wheel_entry_t *));
    return tw;
}

void
timer_wheel_destroy(timer_wheel_t *tw)
{
    aim_free(tw->buckets);
    aim_free(tw);
}

void
timer_wheel_insert(timer_wheel_t *tw, timer_wheel_entry_t *entry, uint64_t deadline)
{
    deadline = deadline < tw->current ? tw->current : deadline;
    int bucket_index = timer_wheel_index(tw, deadline);
    timer_wheel_entry_t **prev_ptr = &tw->buckets[bucket_index];
    timer_wheel_entry_t *cur = *prev_ptr;

    /* Find insertion point */
    while (cur != NULL && cur->deadline < deadline) {
        prev_ptr = &cur->next;
        cur = *prev_ptr;
    }

    /* Add to list */
    *prev_ptr = entry;
    entry->next = cur;
    entry->deadline = deadline;
}

void
timer_wheel_remove(timer_wheel_t *tw, timer_wheel_entry_t *entry)
{
    int bucket_index = timer_wheel_index(tw, entry->deadline);
    timer_wheel_entry_t **prev_ptr = &tw->buckets[bucket_index];
    timer_wheel_entry_t *cur = *prev_ptr;

    /* Find previous entry */
    while (cur != entry) {
        AIM_TRUE_OR_DIE(cur != NULL);
        prev_ptr = &cur->next;
        cur = *prev_ptr;
    }

    /* Remove from list */
    *prev_ptr = entry->next;
    entry->next = NULL;
    entry->deadline = 0;
}

timer_wheel_entry_t *
timer_wheel_next(timer_wheel_t *tw, uint64_t now)
{
    int bucket_index = timer_wheel_index(tw, tw->current);
    uint64_t current = tw->current;
    int bucket_size = 1 << tw->bucket_shift;

    while (current <= now) {
        timer_wheel_entry_t *entry = tw->buckets[bucket_index];

        /* Check whether the first entry in this bucket is ready */
        if (entry != NULL &&
                entry->deadline < current + bucket_size &&
                entry->deadline <= now) {
            tw->buckets[bucket_index] = entry->next;
            entry->next = NULL;
            entry->deadline = 0;
            return entry;
        }

        /* Advance to next bucket */
        bucket_index = (bucket_index + 1) & (tw->num_buckets - 1);
        current += bucket_size;
    }

    /* Round current time to bucket start */
    tw->current = now & ~(bucket_size - 1);

    return NULL;
}

timer_wheel_entry_t *
timer_wheel_peek(timer_wheel_t *tw, uint64_t later)
{
    int bucket_index = timer_wheel_index(tw, tw->current);
    uint64_t current = tw->current;
    int bucket_size = 1 << tw->bucket_shift;

    while (current <= later) {
        timer_wheel_entry_t *entry = tw->buckets[bucket_index];

        /* Check whether the first entry in this bucket is ready */
        if (entry != NULL &&
                entry->deadline < current + bucket_size &&
                entry->deadline <= later) {
            return entry;
        }

        /* Advance to next bucket */
        bucket_index = (bucket_index + 1) & (tw->num_buckets - 1);
        current += bucket_size;
    }

    return NULL;
}

/* Private functions */

static uint32_t
timer_wheel_index(timer_wheel_t *tw, uint64_t time)
{
    return (time >> tw->bucket_shift) & (tw->num_buckets - 1);
}
