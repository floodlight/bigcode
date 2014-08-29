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
 ****************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <slot_allocator/slot_allocator.h>

void test_example(void);

static void
test_basic(void)
{
    const int n = 32;
    uint32_t slots[n];
    uint32_t values[n];
    struct slot_allocator *m = slot_allocator_create(n);

    uint32_t i;
    for (i = 0; i < n; i++) {
        uint32_t slot = slot_allocator_alloc(m);
        assert(slot != SLOT_INVALID);
        slots[i] = slot;
        values[slot] = i;
    }

    assert(slot_allocator_alloc(m) == SLOT_INVALID);

    for (i = 0; i < n; i++) {
        assert(values[slots[i]] == i);
    }

    for (i = 0; i < n; i++) {
        slot_allocator_free(m, slots[i]);
    }

    i = slot_allocator_alloc(m);
    assert(i != SLOT_INVALID);
    slot_allocator_free(m, i);

    slot_allocator_destroy(m);
}

static int
compare_u32(const void *_a, const void *_b)
{
    uint32_t a = *(const uint32_t *)_a;
    uint32_t b = *(const uint32_t *)_b;
    return (int)(a - b);
}

static void
test_iteration(void)
{
    const int n = 4;
    uint32_t slots[n];
    int i;
    struct slot_allocator *m = slot_allocator_create(n);

    for (i = 0; i < n; i++) {
        slots[i] = slot_allocator_alloc(m);
    }

    qsort(slots, n, sizeof(uint32_t), compare_u32);

    struct slot_allocator_iter iter;
    slot_allocator_iter_init(m, &iter);
    assert(slot_allocator_iter_next(&iter) == slots[0]);
    assert(slot_allocator_iter_next(&iter) == slots[1]);
    assert(slot_allocator_iter_next(&iter) == slots[2]);
    assert(slot_allocator_iter_next(&iter) == slots[3]);
    assert(slot_allocator_iter_next(&iter) == SLOT_INVALID);

    for (i = 0; i < n; i++) {
        slot_allocator_free(m, slots[i]);
    }

    slot_allocator_destroy(m);
}

static void
test_stress(void)
{
    const int n = 16;
    uint32_t slots[n];
    int i, j;
    struct slot_allocator *m = slot_allocator_create(n*3/4);

    for (i = 0; i < n; i++) {
        if (rand() % 2 == 0) {
            slots[i] = slot_allocator_alloc(m);
        } else {
            slots[i] = SLOT_INVALID;
        }
    }

    for (j = 0; j < 10000; j++) {
        int i = rand() % n;
        if (slots[i] == SLOT_INVALID) {
            slots[i] = slot_allocator_alloc(m);
        } else {
            slot_allocator_free(m, slots[i]);
            slots[i] = SLOT_INVALID;
        }

        struct slot_allocator_iter iter;
        slot_allocator_iter_init(m, &iter);
        qsort(slots, n, sizeof(uint32_t), compare_u32);
        for (i = 0; i < n; i++) {
            if (slots[i] != SLOT_INVALID) {
                assert(slot_allocator_iter_next(&iter) == slots[i]);
            }
        }
        assert(slot_allocator_iter_next(&iter) == SLOT_INVALID);
    }

    for (i = 0; i < n; i++) {
        if (slots[i] != SLOT_INVALID) {
            slot_allocator_free(m, slots[i]);
        }
    }

    slot_allocator_destroy(m);
}

static void
test_holes(void)
{
    const int n = 1000;
    struct slot_allocator *m = slot_allocator_create(n);
    uint32_t slot;

    uint32_t i;
    for (i = 0; i < n/2; i++) {
        slot = slot_allocator_alloc(m);
        assert(slot == i);
    }

    /* Free one slot, not exceeding the hole limit */
    slot_allocator_free(m, 0);

    /* Allocation should continue where it left off */
    slot = slot_allocator_alloc(m);
    assert(slot == n/2);

    /* Free 99 slots, exceeding the hole limit */
    for (i = 1; i < 100; i++) {
        slot_allocator_free(m, i);
    }

    /* Allocation should restart at 0 */
    slot = slot_allocator_alloc(m);
    assert(slot == 0);

    slot_allocator_destroy(m);
}

int
aim_main(int argc, char **argv)
{
    test_basic();
    test_iteration();
    test_stress();
    test_example();
    test_holes();

    return 0;
}
