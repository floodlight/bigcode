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

#include <timer_wheel/timer_wheel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <AIM/aim.h>

static void
test_empty(void)
{
    timer_wheel_t *tw = timer_wheel_create(8, 128, 0);
    assert(timer_wheel_next(tw, 0) == NULL);
    assert(timer_wheel_next(tw, 1) == NULL);
    timer_wheel_destroy(tw);
}

static void
test_simple(void)
{
    timer_wheel_t *tw = timer_wheel_create(8, 128, 0);
    timer_wheel_entry_t entry1, entry2;

    /* Insert two entries into different buckets */
    timer_wheel_insert(tw, &entry1, 128);
    timer_wheel_insert(tw, &entry2, 256);

    assert(timer_wheel_peek(tw, 0) == NULL);
    assert(timer_wheel_next(tw, 0) == NULL);
    assert(timer_wheel_peek(tw, 127) == NULL);
    assert(timer_wheel_next(tw, 127) == NULL);
    assert(timer_wheel_peek(tw, 128) == &entry1);
    assert(timer_wheel_next(tw, 128) == &entry1);
    assert(timer_wheel_peek(tw, 128) == NULL);
    assert(timer_wheel_next(tw, 128) == NULL);
    assert(timer_wheel_peek(tw, 255) == NULL);
    assert(timer_wheel_next(tw, 255) == NULL);
    assert(timer_wheel_peek(tw, 256) == &entry2);
    assert(timer_wheel_next(tw, 256) == &entry2);
    assert(timer_wheel_peek(tw, 256) == NULL);
    assert(timer_wheel_next(tw, 256) == NULL);
    assert(timer_wheel_peek(tw, 1024) == NULL);
    assert(timer_wheel_next(tw, 1024) == NULL);

    timer_wheel_destroy(tw);
}

static void
test_wrap(void)
{
    timer_wheel_t *tw = timer_wheel_create(8, 128, 0);
    timer_wheel_entry_t entry1, entry2;

    /* Insert two entries into the same bucket one rotation apart */
    timer_wheel_insert(tw, &entry1, 1000);
    timer_wheel_insert(tw, &entry2, 2000);

    assert(timer_wheel_next(tw, 0) == NULL);
    assert(timer_wheel_next(tw, 999) == NULL);
    assert(timer_wheel_next(tw, 1000) == &entry1);
    assert(timer_wheel_next(tw, 1000) == NULL);
    assert(timer_wheel_next(tw, 1999) == NULL);
    assert(timer_wheel_next(tw, 2000) == &entry2);
    assert(timer_wheel_next(tw, 2000) == NULL);
    assert(timer_wheel_next(tw, 10000) == NULL);

    timer_wheel_destroy(tw);
}

static void
test_cancel(void)
{
    timer_wheel_t *tw = timer_wheel_create(8, 128, 0);
    timer_wheel_entry_t entry1, entry2;

    timer_wheel_insert(tw, &entry1, 1000);
    timer_wheel_insert(tw, &entry2, 2000);

    assert(timer_wheel_next(tw, 0) == NULL);
    assert(timer_wheel_next(tw, 999) == NULL);

    /* Cancel and reinsert */
    timer_wheel_remove(tw, &entry1);
    timer_wheel_insert(tw, &entry1, 2001);

    assert(timer_wheel_next(tw, 1000) == NULL);
    assert(timer_wheel_next(tw, 1999) == NULL);

    /* Cancel immediately before expiring */
    assert(timer_wheel_next(tw, 2001) == &entry2);
    timer_wheel_remove(tw, &entry1);

    assert(timer_wheel_next(tw, 2001) == NULL);

    /* Add two entries to the same bucket, remove one */
    timer_wheel_insert(tw, &entry1, 3000);
    timer_wheel_insert(tw, &entry2, 3000);
    timer_wheel_remove(tw, &entry1);
    assert(timer_wheel_next(tw, 3000) == &entry2);
    assert(timer_wheel_next(tw, 3000) == NULL);

    /* Add two entries to the same bucket, remove the other */
    timer_wheel_insert(tw, &entry1, 4000);
    timer_wheel_insert(tw, &entry2, 4000);
    timer_wheel_remove(tw, &entry2);
    assert(timer_wheel_next(tw, 4000) == &entry1);
    assert(timer_wheel_next(tw, 4000) == NULL);

    assert(timer_wheel_next(tw, 10000) == NULL);

    timer_wheel_destroy(tw);
}

static void
test_insert_past(void)
{
    timer_wheel_t *tw = timer_wheel_create(8, 128, 1000);
    timer_wheel_entry_t entry1;

    /* Insert at the current time */
    timer_wheel_insert(tw, &entry1, 1000);
    assert(timer_wheel_next(tw, 1000) == &entry1);

    /* Insert slightly in the past, in the same bucket */
    timer_wheel_insert(tw, &entry1, 999);
    assert(timer_wheel_next(tw, 1000) == &entry1);

    /* Insert further in the past, in the same bucket */
    timer_wheel_insert(tw, &entry1, 896);
    assert(timer_wheel_next(tw, 1000) == &entry1);

    /* Insert further in the past, in the previous bucket */
    timer_wheel_insert(tw, &entry1, 895);
    assert(timer_wheel_next(tw, 1000) == &entry1);

    timer_wheel_destroy(tw);
}

/* Insert timers with random deadlines and check that they all expire */
static void
test_stress(void)
{
#define N 1000
    const int MAX_TIME = N*10;
    const int MAX_TICK = 50;
    int i;

    struct test_entry {
        timer_wheel_entry_t timer_entry;
        uint64_t deadline;
    };

    static struct test_entry entries[N];

    /* Deliberately far too few buckets */
    timer_wheel_t *tw = timer_wheel_create(32, 16, 0);

    for (i = 0; i < N; i++) {
        entries[i].deadline = rand() % MAX_TIME;
        timer_wheel_insert(tw, &entries[i].timer_entry, entries[i].deadline);
    }

    uint64_t current_time = 0;
    uint64_t last_time = 0;
    int count = 0;

    while (current_time < MAX_TIME) {
        current_time += rand() % MAX_TICK;
        struct test_entry *e;
        while ((e = (void *)timer_wheel_next(tw, current_time)) != NULL) {
            assert(e->deadline <= current_time);
            assert(e->deadline >= last_time);
            last_time = e->deadline;
            count++;
        }
    }

    assert(count == N);

    timer_wheel_destroy(tw);
}

int aim_main(int argc, char* argv[])
{
    test_empty();
    test_simple();
    test_wrap();
    test_cancel();
    test_insert_past();
    test_stress();
    return 0;
}
