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
 * Hashed timer wheel
 *
 * See http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf
 *
 * This datastructure sorts expiration times first on the middle bits into an
 * array of buckets, then on the full expiration time into the bucket's linked
 * list. If the number of buckets is greater than the number of timers then
 * insertion and removal are O(1). Finding the next expired timer is done by
 * iterating over the buckets covering the time interval between the last time
 * next() was called and the current time. Assuming a constant number of
 * buckets and interval between ticks this is O(1).
 *
 * The combination of the num_buckets and bucket_size parameters to
 * timer_wheel_create determine the bits used to bucket entries. bucket_size is
 * the number of time units covered by the bucket in one rotation of the wheel.
 * These values should be tuned such that each bucket has on average one entry
 * (num_buckets ~= num_entries) and a small number of buckets are traversed
 * per timer tick (timer_period < bucket_size <= 8*timer_period).
 * bucket_size*num_buckets should be smaller than the largest timeout, but not
 * less than half.
 */

#ifndef __TIMER_WHEEL_H__
#define __TIMER_WHEEL_H__

#include <stdint.h>

typedef struct timer_wheel timer_wheel_t;

/**
 * Timer wheel entry
 *
 * Should be embedded in another struct.
 */
typedef struct timer_wheel_entry {
    struct timer_wheel_entry *next;
    uint64_t deadline;
} timer_wheel_entry_t;

/**
 * Create a timer wheel
 *
 * @param num_buckets
 * @param bucket_size
 * @param now
 * @returns Timer wheel
 */
timer_wheel_t *timer_wheel_create(int num_buckets, int bucket_size, uint64_t now);

/**
 * Destroy a timer wheel
 *
 * @param tw
 */
void timer_wheel_destroy(timer_wheel_t *tw);

/**
 * Insert an entry into a timer wheel
 *
 * @param tw
 * @param entry Pointer to an uninitialized timer_wheel_entry_t
 * @param deadline Time in the future to expire this entry
 */
void timer_wheel_insert(timer_wheel_t *tw, timer_wheel_entry_t *entry, uint64_t deadline);

/**
 * Remove an entry from a timer wheel
 *
 * @param tw
 * @param entry Pointer to an entry currently in the timer wheel
 */
void timer_wheel_remove(timer_wheel_t *tw, timer_wheel_entry_t *entry);

/**
 * Remove and return the next expired entry from the timer wheel
 *
 * @param tw
 * @param now Current time
 */
timer_wheel_entry_t *timer_wheel_next(timer_wheel_t *tw, uint64_t now);

/**
 * Return the next expiring entry from the timer wheel
 *
 * @param tw
 * @param later Future time
 *
 * The intended use is with a tickless timer system. Before sleeping
 * this function should be called with a 'later' parameter of some time in
 * the future, to get a lower bound on how long to sleep. 'later' should
 * only be far enough in the future to reduce the overhead of unnecessary
 * timer wakeups, since this function takes time linear in (later - current).
 */
timer_wheel_entry_t *timer_wheel_peek(timer_wheel_t *tw, uint64_t now);

#endif /* __TIMER_WHEEL__ */
