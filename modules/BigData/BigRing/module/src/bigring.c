/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
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

#include <BigRing/bigring_config.h>
#include <BigRing/bigring.h>


#if BIGRING_CONFIG_INCLUDE_LOCKING == 1
#include <OS/os_sem.h>
#endif


struct bigring_s {
    /** The ring buffer size */
    int size;
    /** The ring buffer */
    void** ring;
    /** head slot */
    int head;
    /** tail write slot */
    int tail;

    /** Entry deallocator - optional */
    bigring_free_entry_f free_entry;

#if BIGRING_CONFIG_INCLUDE_LOCKING == 1
    os_sem_t lock;
#define BIGRING_LOCK(_br) bigring_lock(_br)
#define BIGRING_UNLOCK(_br) bigring_unlock(br)

#else
    /* Don't even bother with the empty calls */
#define BIGRING_LOCK(_br)
#define BIGRING_UNLOCK(_br)

#endif

}; /* bigring_t */






#define BIGRING_ITERATION_TERMINATED -2

bigring_t*
bigring_create(int size, bigring_free_entry_f free_entry)
{
    bigring_t* br = aim_zmalloc(sizeof(*br));

    /*
     * We keep one empty slot to distinguish full from empty.
     * We'll increase the size by one to account for this slot
     * so the user gets the requested number of entries.
     */
    size++;

    br->ring = aim_zmalloc(sizeof(void*)*size);
    br->size = size;
    br->free_entry = free_entry;
    br->head = 0;
    br->tail = 0;

#if BIGRING_CONFIG_INCLUDE_LOCKING == 1
    br->lock = os_sem_create(1);
#endif
    return br;
}


/**
 * Must be called under lock.
 */
static void
bigring_entry_free_locked__(bigring_t* br, int index)
{
    if(br->ring[index]) {
        if(br->free_entry) {
            br->free_entry(br->ring[index]);
        }
        br->ring[index] = NULL;
    }
}

void
bigring_destroy(bigring_t* br)
{
    int i;
    BIGRING_LOCK(br);
    for(i = 0; i < br->size; i++) {
        bigring_entry_free_locked__(br, i);
    }
    BIGRING_UNLOCK(br);

#if BIGRING_CONFIG_INCLUDE_LOCKING == 1
    os_sem_destroy(br->lock);
#endif

    AIM_FREE(br->ring);
    AIM_FREE(br);
}

void
bigring_lock(bigring_t* br)
{
#if BIGRING_CONFIG_INCLUDE_LOCKING == 1
    os_sem_take(br->lock);
#endif
}

void
bigring_unlock(bigring_t* br)
{
#if BIGRING_CONFIG_INCLUDE_LOCKING == 1
    os_sem_give(br->lock);
#endif
}

int
bigring_count_locked(bigring_t* br)
{
    if(br->head == br->tail) {
        return 0;
    }
    else if(br->head < br->tail) {
        return br->tail - br->head;
    }
    else {
        return br->size - (br->head - br->tail);
    }
}

int
bigring_count(bigring_t* br)
{
    int rv;
    BIGRING_LOCK(br);
    rv = bigring_count_locked(br);
    BIGRING_UNLOCK(br);
    return rv;
}


#define RING_INCREMENT(_br, _index) (_index = (_index + 1) % (_br->size))

static void
bigring_push_locked__(bigring_t* br, void* entry)
{
    br->ring[br->tail] = entry;
    br->tail = (br->tail + 1) % br->size;
    if (br->tail == br->head) {
        /*
         * Full -- free current and advance head
         */
        bigring_entry_free_locked__(br, br->head);
        RING_INCREMENT(br, br->head);
    }
}

void
bigring_push(bigring_t* br, void* entry)
{
    BIGRING_LOCK(br);
    bigring_push_locked__(br, entry);
    BIGRING_UNLOCK(br);
}


static void*
bigring_shift_locked__(bigring_t* br)
{
    void* rv = NULL;
    if(br->head != br->tail) {
        rv = br->ring[br->head];
        br->ring[br->head] = NULL;
        RING_INCREMENT(br, br->head);
    }
    return rv;
}
void*
bigring_shift(bigring_t* br)
{
    void* rv;
    BIGRING_LOCK(br);
    rv = bigring_shift_locked__(br);
    BIGRING_UNLOCK(br);
    return rv;
}


/**
 * These assume the ring is already locked.
 */
void
bigring_iter_start(bigring_t* br, int* iter)
{
    *iter = br->head;
}

void*
bigring_iter_next(bigring_t* br, int* iter)
{
    void* rv = NULL;
    if(*iter != br->tail) {
        rv = br->ring[*iter];
        RING_INCREMENT(br, *iter);
    }
    return rv;
}

void
bigring_aim_free_entry(void* entry)
{
    AIM_FREE(entry);
}

int
bigring_size(bigring_t* br)
{
    /* Account for empty slot */
    return br->size-1;
}
