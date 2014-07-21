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

#ifndef SLOT_ALLOCATOR_H
#define SLOT_ALLOCATOR_H

/*
 * This module implements an allocator for "slots", which are integers in the
 * range [0, N). A common usecase is finding a free index in an array.
 *
 * Allocation is amortized constant time as long as the load factor stays below
 * a fixed fraction (0.99 is reasonable). Deallocation is constant time.
 *
 * The slot allocator can be used as a more efficient alternative to malloc for
 * a fixed maximum number of constant size objects. Here's an example:
 *
 * \code
 *   struct slot_allocator *my_object_allocator;
 *   struct my_object my_objects[MY_OBJECT_COUNT];
 *
 *   void
 *   my_object_allocator_init(void)
 *   {
 *       my_object_allocator = slot_allocator_create(MY_OBJECT_COUNT);
 *   }
 *
 *   struct my_object *
 *   my_object_alloc(void)
 *   {
 *       uint32_t slot = slot_allocator_alloc(my_object_allocator);
 *       if (slot == SLOT_INVALID) {
 *           return NULL;
 *       } else {
 *           return &my_objects[slot];
 *       }
 *   }
 *
 *   void
 *   my_object_free(struct my_object *obj)
 *   {
 *       uint32_t slot = obj - my_objects;
 *       slot_allocator_free(my_object_allocator, slot);
 *   }
 * \endcode
 *
 * A major advantage of the slot allocator over malloc/free is that it supports
 * iteration in address order. This can make iteration over the set of
 * allocated objects an order of magnitude faster by making better use of the
 * cache and prefetcher. Also, iteration doesn't require an extra set of list
 * links.
 *
 * An advantage of the slot allocator over a freelist is that it doesn't store
 * information in deallocated objects. This means the backing memory can be
 * lazily allocated from the kernel as objects are allocated, rather than
 * all at once, reducing memory consumption.
 */

#include <stdint.h>

/*
 * Returned by slot_allocator_alloc when allocation fails.
 */
#define SLOT_INVALID UINT32_MAX

struct slot_allocator;

/**
 * Iterator over allocated slots
 *
 * Treat as private.
 */
struct slot_allocator_iter {
    struct slot_allocator *allocator;

    /* Where to start searching for an allocated slot */
    uint32_t slot;

    /*
     * Bitmap starting at 'slot'
     *
     * This is shifted right each time the iterator is advanced.
     */
    uint32_t cur_bitmap_word;
};

/**
 * Create a slot allocator
 *
 * @param num_slots
 */
struct slot_allocator *slot_allocator_create(uint32_t num_slots);

/**
 * Destroy a slot allocator
 *
 * @param allocator
 */
void slot_allocator_destroy(struct slot_allocator *allocator);

/**
 * Allocate a slot
 *
 * @param allocator
 * @returns A previously free slot
 */
uint32_t slot_allocator_alloc(struct slot_allocator *allocator);

/**
 * Free a slot
 *
 * @param allocator
 * @param slot Must be currently allocated
 */
void slot_allocator_free(struct slot_allocator *allocator, uint32_t slot);

/**
 * Initialize an iterator
 *
 * @param allocator
 * @param iter Will be initialized by this function
 */
void slot_allocator_iter_init(struct slot_allocator *allocator, struct slot_allocator_iter *iter);

/**
 * Advance an iterator
 *
 * @param iter
 *
 * Slots are returned in ascending order to optimize memory accesses.
 * SLOT_INVALID is returned on the end of iteration.
 */
uint32_t slot_allocator_iter_next(struct slot_allocator_iter *iter);

#endif
