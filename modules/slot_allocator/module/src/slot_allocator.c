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

#include <slot_allocator/slot_allocator.h>
#include <assert.h>
#include <AIM/aim_memory.h>
#include <AIM/aim_bitmap.h>

/*
 * Fraction of slots allowed to be holes
 *
 * A hole is defined as a slot that is both free and smaller than alloc_start.
 */
#define HOLE_LIMIT_RATIO 0.01

#define likely(expr) __builtin_expect(!!(expr), 1)

struct slot_allocator {
    /* Total number of slots */
    uint32_t num_slots;

    /*
     * Starting slot to search for a free slot. Always a multiple of the number
     * of bits in a bitmap word.
     */
    uint32_t alloc_start;

    /* Maximum number of holes allowed */
    uint32_t max_holes;

    /* Current number of holes */
    uint32_t num_holes;

    /* Bits are set if the corresponding slot is allocated */
    aim_bitmap_t bitmap;
};

static uint32_t
bitmap_word_for_slot(struct slot_allocator *allocator, uint32_t slot)
{
    return AIM_BITMAP_WORD_GET32(&allocator->bitmap, slot/AIM_BITMAP_BITS_PER_WORD);
}

struct slot_allocator *
slot_allocator_create(uint32_t num_slots)
{
    struct slot_allocator *allocator = aim_zmalloc(sizeof(*allocator));
    allocator->num_slots = num_slots;
    allocator->alloc_start = 0;
    allocator->max_holes = num_slots * HOLE_LIMIT_RATIO;
    allocator->num_holes = 0;
    aim_bitmap_alloc(&allocator->bitmap, num_slots);
    return allocator;
}

void
slot_allocator_destroy(struct slot_allocator *allocator)
{
    aim_bitmap_free(&allocator->bitmap);
    free(allocator);
}

uint32_t
slot_allocator_alloc(struct slot_allocator *allocator)
{
    /* Search for a free slot */
    uint32_t slot;
    for (slot = allocator->alloc_start;
            slot < allocator->num_slots;
            slot += AIM_BITMAP_BITS_PER_WORD) {
        uint32_t w = bitmap_word_for_slot(allocator, slot);
        if (~w) {
            /* Bitmap word is not all ones, find the first zero bit starting
             * from the lsb */
            int shift = __builtin_ctz(~w);
            slot += shift;
            break;
        }
    }

    if (slot >= allocator->num_slots) {
        /*
         * We've hit the end of the bitmap. Search from the beginning if we
         * haven't already.
         */
        if (allocator->alloc_start != 0) {
            allocator->alloc_start = 0;
            allocator->num_holes = 0;
            return slot_allocator_alloc(allocator);
        }
        return SLOT_INVALID;
    }

    /* Maintain the invariant that alloc_start points to the beginning of a
     * bitmap word */
    allocator->alloc_start = slot & ~(AIM_BITMAP_BITS_PER_WORD - 1);

    AIM_BITMAP_SET(&allocator->bitmap, slot);
    return slot;
}

void
slot_allocator_free(struct slot_allocator *allocator, uint32_t slot)
{
    AIM_BITMAP_CLR(&allocator->bitmap, slot);

    /* This free may create a hole */
    allocator->num_holes += slot < allocator->alloc_start;
    if (allocator->num_holes > allocator->max_holes) {
        allocator->alloc_start = 0;
        allocator->num_holes = 0;
    }
}

void
slot_allocator_iter_init(struct slot_allocator *allocator, struct slot_allocator_iter *iter)
{
    iter->allocator = allocator;
    iter->slot = 0;
    iter->cur_bitmap_word = bitmap_word_for_slot(allocator, 0);
}

uint32_t
slot_allocator_iter_next(struct slot_allocator_iter *iter)
{
    /* Fastpath: a bit is set in cur_bitmap_word */
    if (likely(iter->cur_bitmap_word != 0)) {
        /* Advance to next set bit */
        int shift = __builtin_ctz(iter->cur_bitmap_word);
        iter->slot += shift;
        iter->cur_bitmap_word >>= shift;
        assert(AIM_BITMAP_GET(&iter->allocator->bitmap, iter->slot));

        uint32_t slot = iter->slot;

        /* Advance to next bit */
        iter->slot++;
        iter->cur_bitmap_word >>= 1;

        return slot;
    }

    /* Advance to next bitmap word */
    iter->slot += AIM_BITMAP_BITS_PER_WORD-1;
    iter->slot &= ~(AIM_BITMAP_BITS_PER_WORD-1);

    /* Find the next nonzero bitmap word */
    while (iter->slot < iter->allocator->num_slots) {
        uint32_t w = bitmap_word_for_slot(iter->allocator, iter->slot);
        if (w) {
            iter->cur_bitmap_word = w;
            /* Bitmap word is now nonzero, call the fastpath */
            return slot_allocator_iter_next(iter);
        } else {
            iter->slot += AIM_BITMAP_BITS_PER_WORD;
        }
    }

    return SLOT_INVALID;
}
