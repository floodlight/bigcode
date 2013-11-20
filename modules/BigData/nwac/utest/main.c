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

#include <nwac/nwac_config.h>
#include <nwac/nwac.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

#include <OS/os_time.h>

#define AIM_LOG_MODULE_NAME nwac_unittest
#include <AIM/aim_log.h>
AIM_LOG_STRUCT_DEFINE(
                      NWAC_CONFIG_LOG_OPTIONS_DEFAULT,
                      NWAC_CONFIG_LOG_BITS_DEFAULT,
                      NULL,
                      NWAC_CONFIG_LOG_CUSTOM_BITS_DEFAULT);


#define KEY_SIZE 64

typedef struct test_entry_s {
    nwac_entry_header_t hdr;
    uint8_t key[KEY_SIZE];
    uint32_t index;
} test_entry_t;

void
test_entry_show(nwac_t* nwac, nwac_entry_t* entry, aim_pvs_t* pvs)
{
    test_entry_t* te = (test_entry_t*)(entry);
    aim_printf(pvs, "index=%d", te->index);
}

int
test_nwac__(nwac_t* nwac, int search_count)
{
    int i;
    uint8_t key[KEY_SIZE];
    int time = 1;
    int collisions = 0;
    int rv = 0;
    int insert_count = 0;

    for(i = 0; i < search_count; i++) {
        test_entry_t* te;
        AIM_LOG_VERBOSE("i=%d", i);
        memset(key, i, sizeof(key));
        te = (test_entry_t*)nwac_search(nwac, key, 0);
        if(te == NULL) {
            AIM_LOG_VERBOSE("collision -- reinserting with eviction");
            te = (test_entry_t*)nwac_search(nwac, key, time);
            if(te == NULL) {
                AIM_LOG_ERROR("eviction failed.");
                return -1;
            }
            collisions++;
        }
        if(te->hdr.valid) {
            AIM_LOG_VERBOSE("found in cache at i=%d, index=%d",
                        i, te->index);
        }
        else {
            te->hdr.valid = 1;
            memcpy(te->key, key, sizeof(key));
            te->index = i;
            AIM_LOG_VERBOSE("Initializing entry at i=%d, index=%d", i, te->index);
            insert_count++;
        }
        /* Make sure the entry is now found */
        {
            test_entry_t* fe;
            fe = (test_entry_t*)nwac_search(nwac, key, 0);
            if(fe == NULL) {
                AIM_LOG_ERROR("inserted entry not found.");
                return -1;
            }
            if(fe != te) {
                AIM_LOG_ERROR("inserted entry not returned.");
                return -1;
            }
        }
    }

    /* Verify that no index is duplicated */
    {
        char* bits = aim_zmalloc(search_count);
        test_entry_t* te;
        int c = 0;
        int entries = 0;

        for(te = (test_entry_t*)nwac->cache;
            c < nwac->entry_count; te++, c++) {
            if(te->hdr.valid) {
                bits[te->index]++;
                if(bits[te->index] != 1) {
                    AIM_LOG_ERROR("index %d found multiple times.", te->index);
                    return -1;
                }
                entries++;
            }
        }

        if(aim_log_enabled(&AIM_LOG_STRUCT, AIM_LOG_FLAG_VERBOSE)) {
            nwac_show(nwac, aim_log_pvs_get(&AIM_LOG_STRUCT), test_entry_show);
        }
        /* There should be exactly (count-collisions) bits set */
        AIM_LOG_MSG("n=%d size=%d count=%d col=%d ic=%d",
                    nwac->block_size, nwac->entry_count, search_count, collisions, insert_count);
        AIM_LOG_MSG("utilization: %d/%d", (search_count-collisions), search_count);
        if(entries != (insert_count-collisions)) {
            AIM_LOG_ERROR("Number of collisions and number of entries doe not match: \
entries=%d, cc=%d", entries, (insert_count-collisions));
            rv = -1;
        }
        aim_free(bits);
    }

    return rv;
}

int
perftest__(nwac_t* nwac, int search_count)
{
    int i;
    uint8_t key[KEY_SIZE];
    uint64_t start, end;
    double seconds;

    start = os_time_thread();
    for(i = 0; i < search_count; i++) {
        test_entry_t* te;
        memset(key, i, sizeof(key));
        te = (test_entry_t*)nwac_search(nwac, key, 1);
        if(te->hdr.valid == 0) {
            te->hdr.valid = 1;
            memcpy(te->key, key, sizeof(key));
            te->index = i;
        }
    }
    end = os_time_thread();
    seconds = (end - start) / (1000.0*1000);
    if(aim_log_enabled(&AIM_LOG_STRUCT, AIM_LOG_FLAG_VERBOSE)) {
        nwac_show(nwac, aim_log_pvs_get(&AIM_LOG_STRUCT), test_entry_show);
    }
    AIM_LOG_MSG("nwac(%d,%d): %d searches in %f seconds (%f searches/sec)\n",
                nwac->block_size, nwac->entry_count, search_count, seconds, search_count/seconds);
    return 0;
}

int
perftest(int n, int entry_count, int search_count)
{
    int rv;
    nwac_t* nwac = nwac_create(n, KEY_SIZE, sizeof(test_entry_t), entry_count);
    rv = perftest__(nwac, search_count);
    nwac_destroy(nwac);
    return rv;
}

int
test_nwac(int n, int size, int iterations)
{
    int rv;
    nwac_t* nwac = nwac_create(n, KEY_SIZE, sizeof(test_entry_t), size);
    rv = test_nwac__(nwac, iterations);
    nwac_destroy(nwac);
    return rv;
}

NWAC_DEFINE_STATIC(static_nwac, 8, sizeof(test_entry_t), 256, KEY_SIZE);

int
aim_main(int argc, char* argv[])
{
    int n;
    int size;
    int iterations;

    if(argc > 1) {
        if(!strcmp(argv[1], "perf")) {
            if(argc == 2) {
                int iterations = 16*1024*1024;
                perftest(8, 512, iterations);
            }
            else {
                if(!strcmp(argv[2], "all")) {
                    int iterations = 16*1024*1024;
                    perftest(4, 128, iterations);
                    perftest(4, 256, iterations);
                    perftest(4, 512, iterations);
                    perftest(4, 1024, iterations);
                    perftest(8, 128, iterations);
                    perftest(8, 256, iterations);
                    perftest(8, 512, iterations);
                    perftest(8, 1024, iterations);
                }
                else if(argc == 5) {
                    int n = atoi(argv[2]);
                    int size = atoi(argv[3]);
                    int iterations = atoi(argv[4]);
                    perftest(n, size, iterations);
                }
                else {
                    fprintf(stderr, "unknown option '%s'\n", argv[2]);
                    return -1;
                }
            }
            return 0;
        }

        if(argc == 4) {
            n = atoi(argv[1]);
            size = atoi(argv[2]);
            iterations = atoi(argv[3]);
            return test_nwac(n, size, iterations);
        }
        fprintf(stderr, "unknown option '%s'\n", argv[1]);
        return -1;
    }

    /* Default Unit Test */
    for(n = 2; n <= 8; n <<= 1) {
        for(size=n*2; size < n*256; size <<= 1) {
            for(iterations = size; iterations < size*16; iterations <<= 1) {
                AIM_LOG_MSG("==");
                if(test_nwac(n, size, iterations) != 0) {
                    AIM_LOG_ERROR("failed: n=%d size=%d count=%d",
                                  n, size, iterations);
                    return 1;
                }
                if(test_nwac(n, size, iterations+1) != 0) {
                    AIM_LOG_ERROR("failed: n=%d size=%d iterations=%d",
                                  n, size, iterations+1);
                    return 1;
                }
                if(test_nwac(n, size, iterations-1) != 0) {
                    AIM_LOG_ERROR("failed: n=%d size=%d iterations=%d",
                           n, size, iterations-1);
                    return 1;
                }
                AIM_LOG_MSG("==");
            }
        }
    }
    {
        /* Static definition test */
        nwac_init_static(&static_nwac);
        test_nwac__(&static_nwac, 1024);
        nwac_destroy(&static_nwac);
    }
    return 0;
}


