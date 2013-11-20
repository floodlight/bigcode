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
#include <AIM/aim.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

#define AIM_LOG_MODULE_NAME BigRingTest
#include <AIM/aim_log.h>

AIM_LOG_STRUCT_DEFINE(
                      AIM_LOG_OPTIONS_DEFAULT,
                      0xFFFF,
                      NULL,     /* Custom Log Map */
                      0
                      );

static void
bigring_int_test(int size, int count)
{
    bigring_t* br = bigring_create(size, NULL);
    intptr_t c;
    intptr_t start;
    intptr_t max;
    int i;

    i = bigring_size(br);
    if(i != size) {
        AIM_LOG_ERROR("size not correct -- should be %d, is %d\n", size, i);
        abort();
    }

    for(c = 0; c < count; c++) {

#define ASSERT_COUNT(_bc, _vc, _size)                                   \
        do {                                                            \
            if(_bc != ((_vc < _size) ? _vc : _size)) {                  \
                AIM_LOG_ERROR("size = %d, count is %d, should be %d", _size, _bc, _vc); \
                abort();                                                \
            }                                                           \
        } while(0)

        ASSERT_COUNT(bigring_count(br), c, bigring_size(br));
        bigring_push(br, (void*)c);
    }

    /** Make sure the entries are correct (module size) */
    start = (count <= size) ? 0 : count - size;
    max = (count <= size) ? count : size;

    for(i = 0, c = max-1; i < max; i++, start++, c--) {
        intptr_t v = (intptr_t)bigring_shift(br);
        if(v != start) {
            AIM_LOG_ERROR("entry mismatch @%d - expected %p, got %p",
                          i, start, v);
            abort();
        }

        /* Count should have decremented */
        ASSERT_COUNT(bigring_count(br), c, bigring_size(br));
    }
    /* Make sure additional shifts return null */
    if(bigring_count(br) != 0) {
        AIM_LOG_ERROR("count is %d but should be zero", bigring_count(br));
        abort();
    }
    c = (intptr_t)bigring_shift(br);
    if(c != 0) {
        AIM_LOG_ERROR("shift returned %p when it should have returned null", c);
        abort();
    }


    /** Add an entry, pop and entry */
    c = 1;
    bigring_push(br, (void*)c);
    ASSERT_COUNT(bigring_count(br), 1, bigring_size(br));
    c = (intptr_t)bigring_shift(br);
    if(c != 1) {
        AIM_LOG_ERROR("shift returned %p when it should have returned 1", c);
        abort();
    }
    ASSERT_COUNT(bigring_count(br), 0, bigring_size(br));


    /* Repopulate and iterate instead of shifting */
    for(c = 0; c < count; c++) {

#define ASSERT_COUNT(_bc, _vc, _size)                                   \
        do {                                                            \
            if(_bc != ((_vc < _size) ? _vc : _size)) {                  \
                AIM_LOG_ERROR("size = %d, count is %d, should be %d", _size, _bc, _vc); \
                abort();                                                \
            }                                                           \
        } while(0)

        ASSERT_COUNT(bigring_count(br), c, bigring_size(br));
        bigring_push(br, (void*)c);
    }

    {
        int j;
        /* Iterate twice */
        for(j = 0; j < 2; j++) {
            int iter = -42;
            bigring_lock(br);
            bigring_iter_start(br, &iter);

            /** Make sure the entries are correct (module size) */
            start = (count <= size) ? 0 : count - size;
            max = (count <= size) ? count : size;

            for(i = 0; i < max; i++, start++) {
                intptr_t v = (intptr_t)bigring_iter_next(br, &iter);
                if(v != start) {
                    AIM_LOG_ERROR("entry mismatch @%d - expected %p, got %p",
                                  i, start, v);
                }
                ASSERT_COUNT(bigring_count_locked(br), max, bigring_size(br));
            }

            c = (intptr_t)bigring_iter_next(br, &iter);
            if(c != 0) {
                AIM_LOG_MSG("bigring_iter_next() returned %d when it should have returned NULL",
                            c);
            }
            c = (intptr_t)bigring_iter_next(br, &iter);
            if(c != 0) {
                AIM_LOG_MSG("bigring_iter_next() returned %d when it should have returned NULL",
                            c);
            }
            bigring_unlock(br);
        }
    }



    bigring_destroy(br);
}

static int free_count__ = 0;
void
free_entry__(void* p)
{
    char verify[16];
    aim_snprintf(verify, sizeof(verify), "%d", free_count__);
    if(strcmp(verify, p)) {
        AIM_LOG_ERROR("free_entry__: expected %s, got %s",
                      verify, p);
    }
    AIM_FREE(p);
    free_count__++;
}

/* Just make sure elements get free properly */
void
bigring_str_test(int size, bigring_free_entry_f fef)
{
    bigring_t* br = bigring_create(size, fef);
    int i;
    char* p;
    char buf[16];

    free_count__ = 0;

    for(i = 0; i < size*2; i++) {
        bigring_push(br, aim_fstrdup("%d", i));
    }
    /*
     * At this point:
     * free_count should be size (or zero if using default free function)
     * next entry should be "size"
     * ringcount should be size
     */
    if(fef == free_entry__) {
        if(free_count__ != size) {
            AIM_LOG_ERROR("Free count is %d, should be %d", free_count__, size);
        }
    }
    if(fef == bigring_aim_free_entry) {
        if(free_count__ != 0) {
            AIM_LOG_ERROR("free_count is %d, should be zero", free_count__);
        }
    }
    ASSERT_COUNT(bigring_count(br), size, bigring_size(br));
    p = bigring_shift(br);
    ASSERT_COUNT(bigring_count(br), size-1, bigring_size(br));
    aim_snprintf(buf, sizeof(buf), "%d", size);
    if(strcmp(p, buf)) {
        AIM_LOG_ERROR("Next is '%s', should be '%s'", p, buf);
        abort();
    }
    AIM_FREE(p);
    /*
     * Verify the rest of the entries are free.
     * Note - we removed the current entry, so we need
     * to increment free_count__ to account for it or
     * the verification will be off by one.
     */
    free_count__++;
    bigring_destroy(br);
}


int aim_main(int argc, char* argv[])
{
    int size_min = 1, size_max = 65;
    int count_min = 1, count_max = 129;
    int s, c;

    bigring_config_show(&aim_pvs_stdout);
    for(s = size_min; s <= size_max; s++) {
        for(c = count_min; c <= count_max; c++) {
            AIM_LOG_MSG("IntTest(%d,%d)", s, c);
            bigring_int_test(s, c);
            AIM_LOG_MSG("IntTest(%d,%d)", c, s);
            bigring_int_test(c, s);
        }
    }
    for(s = 1; s <= 10; s++) {
        AIM_LOG_MSG("StrTest(%d, free_entry__)", s);
        bigring_str_test(s, free_entry__);
        AIM_LOG_MSG("StrTest(%d, default)", s);
        bigring_str_test(s, bigring_aim_free_entry);
    }
    bigring_config_show(&aim_pvs_stdout);
    return 0;
}

