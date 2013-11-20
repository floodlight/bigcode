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

#include <pimu/pimu_config.h>
#include <pimu/pimu.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>
#include <stdlib.h>

#include <OS/os_time.h>
#include <OS/os_sleep.h>


#define AIM_LOG_MODULE_NAME pimu_utest
#include <AIM/aim_log.h>
AIM_LOG_STRUCT_DEFINE(
                      PIMU_CONFIG_LOG_OPTIONS_DEFAULT,
                      PIMU_CONFIG_LOG_BITS_DEFAULT,
                      NULL, /* Custom log map */
                      PIMU_CONFIG_LOG_CUSTOM_BITS_DEFAULT
                      );

typedef struct packet_s {
    uint8_t data[1024];
    int size;
} packet_t;

#define PACKET_COUNT 32

packet_t packets__[PACKET_COUNT];

void
init_packets__(int vid, int ethertype, uint8_t* da)
{
    int i;
    for(i = 0; i < AIM_ARRAYSIZE(packets__); i++) {
        int j;
        packets__[i].size = rand() % AIM_ARRAYSIZE(packets__[i].data);
        if(packets__[i].size < 60) {
            packets__[i].size = 60;
        }
        for(j = 0; j < packets__[i].size; j++) {
            packets__[i].data[j] = rand();
        }
        if(vid) {
            packets__[i].data[12] = 0x81;
            packets__[i].data[13] = 0x00;
            packets__[i].data[14] = 0x00;
            packets__[i].data[15] = vid;
            packets__[i].data[16] = (ethertype >> 8) & 0xFF;
            packets__[i].data[17] = (ethertype) & 0xFF;
        }
        else if(ethertype) {
            packets__[i].data[12] = (ethertype >> 8) & 0xFF;
            packets__[i].data[13] = (ethertype) & 0xFF;
        }
        if(da) {
            memcpy(packets__[i].data, da, 6);
        }
    }
};

static uint64_t __now = 0;

int
verify_flow_pps__(pimu_t* pimu, int pps, int gid)
{
    int count[PACKET_COUNT] = {0};
    int t;
    int p;
    uint64_t now = __now; __now += 50000000;
    int stop_time = 1000000+now;
    /* Run all packets through for delta-T */
    for(t = now; t < stop_time; t+=10) {
        for(p = 0; p < PACKET_COUNT; p++) {
            pimu_action_t action = pimu_packet_in(pimu, 0, gid,
                                                  packets__[p].data,
                                                  packets__[p].size,
                                                  t);
            if(action != PIMU_ACTION_DROP) {
                count[p]++;
            }
        }
    }
    /* Verify the accept rate */
    for(p = 0; p < PACKET_COUNT; p++) {
        /* Verify within 2% of rate */
        uint64_t expected = pps;
        int lower = expected - expected/50;
        int upper = expected + expected/50;

        if(count[p] < lower || count[p] > upper) {
            AIM_LOG_ERROR("pps=%d count[%d] = %d (should be %d)\n", pps, p, count[p],
                          expected);
            exit(1);
        }
    }
    AIM_LOG_MSG("verified at %d pps.", pps);
    return 0;
}

int
verify_total_pps__(pimu_t* pimu, int pps, int gid)
{
    int count = 0;
    int t;
    int p;
    uint64_t now = __now; __now += 50000000;
    int stop_time = now + 1000000;
    /* Run all packets through for delta-T */
    for(t = now; t < stop_time; t+=10) {
        for(p = 0; p < PACKET_COUNT; p++) {
            pimu_action_t action = pimu_packet_in(pimu, 0, gid,
                                                  packets__[p].data,
                                                  packets__[p].size,
                                                  t);
            if(action != PIMU_ACTION_DROP) {
                count++;
            }
            if(pps == 0 && action != PIMU_ACTION_FORWARD_PRIORITY) {
                /** Assumption that pps==0 is the priority test */
                AIM_LOG_ERROR("FORWARD_PRIORITY was not returned.");
                exit(1);
            }
        }
    }
    if(pps != 0) {
        /* Verify within 2% of rate */
        uint64_t expected = pps;
        int lower = expected - expected/50;
        int upper = expected + expected/50;

        if(count < lower || count  > upper) {
            AIM_LOG_ERROR("pps=%d count = %d (should be %d)\n", pps, count,
                          expected);
            exit(1);
        }
    }
    AIM_LOG_MSG("verified at %d pps.", pps);
    return 0;
}

int
perf_test(pimu_t* pimu, int iterations)
{
    int i;
    uint64_t start, end;
    double seconds;
    int count = 0;
    int fcount = 0;
    int dcount = 0;
    start = os_time_thread();
    for(i = 0; i < iterations; i++) {
        int p;
        for(p = 0; p < PACKET_COUNT; p++) {
            if(pimu_packet_in(pimu, 0, -1, packets__[p].data,
                              packets__[p].size, os_time_monotonic()) == PIMU_ACTION_DROP) {
                dcount++;
            }
            else {
                fcount++;
            }
            count++;
        }
    }
    end = os_time_thread();

    seconds = (end - start) / (1000.0*1000);
    AIM_LOG_MSG("%d packets in %f seconds (%f searches/sec)\n",
                count, seconds, count/seconds);
    AIM_LOG_MSG("%d packets in %f seconds (%f forwards/sec)\n",
                fcount, seconds, fcount/seconds);
    AIM_LOG_MSG("%d packets in %f seconds (%f drops/sec)\n",
                dcount, seconds, dcount/seconds);
    return 0;
}

#include <AIM/aim_rl.h>

int aim_main(int argc, char* argv[])
{
    pimu_t* pimu;
    init_packets__(0, 0, NULL);
    pimu = pimu_create(8, 256);

    if(aim_valgrind_status() != 0) {
        /* Abbreviated tests under valgrind due to running time */
        AIM_LOG_MSG("Running abbreviated tests under valgrind.");
        pimu_cache_clear(pimu);
        pimu_flow_pps_set(pimu, 1);
        verify_flow_pps__(pimu, 1, -1);
        pimu_destroy(pimu);
        return 0;
    }

    /* Verify flows at different PPS */
    {
        int pps;
        for(pps = 1; pps < 10; pps++) {
            pimu_cache_clear(pimu);
            pimu_flow_pps_set(pimu, pps);
            verify_flow_pps__(pimu, pps, -1);
        }
        for(pps = 10; pps < 2000; pps+=100) {
            pimu_cache_clear(pimu);
            pimu_flow_pps_set(pimu, pps);
            verify_flow_pps__(pimu, pps, -1);
        }
    }

    pimu_cache_clear(pimu);
    pimu_flow_pps_set(pimu, 1000);
    pimu_global_pps_set(pimu, 800, 0);
    verify_total_pps__(pimu, 800, -1);

    init_packets__(0, 0x88DD, NULL);
    pimu_cache_clear(pimu);
    pimu_flow_pps_set(pimu, 1000);
    pimu_global_pps_set(pimu, 800, 0);
    pimu_prio_ether_type_add(pimu, 0x88DD);
    verify_total_pps__(pimu, 0, -1);
    pimu_prio_ether_type_remove(pimu, 0x88FF);
    verify_total_pps__(pimu, 0, -1);
    pimu_prio_ether_type_remove(pimu, 0x88DD);
    pimu_prio_ether_type_add(pimu, 0xFACE);
    pimu_prio_ether_type_add(pimu, 0xDEAD);
    init_packets__(1, 0xFACE, NULL);
    verify_total_pps__(pimu, 0, 0);
    init_packets__(2, 0xDEAD, NULL);
    verify_total_pps__(pimu, 0, 0);
    init_packets__(64, 0xF00D, NULL);
    verify_total_pps__(pimu, 800, 0);
    pimu_prio_ether_type_remove(pimu, 0xFACE);
    pimu_prio_ether_type_remove(pimu, 0xDEAD);

    {
        uint8_t da1[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };
        uint8_t da2[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x5 };

        pimu_prio_ether_da_add(pimu, da1);
        init_packets__(1, 1, da2);
        verify_total_pps__(pimu, 800, 0);
        init_packets__(1, 1, da1);
        verify_total_pps__(pimu, 0, 0);
        pimu_prio_ether_da_remove(pimu, da1);
        verify_total_pps__(pimu, 800, 0);
    }


    pimu_cache_clear(pimu);
    pimu_global_pps_set(pimu, 0, 0);
    pimu_group_pps_set(pimu, 1, 500, 0);
    pimu_group_pps_set(pimu, 2, 250, 0);
    verify_total_pps__(pimu, 500, 1);
    verify_total_pps__(pimu, 250, 2);
    pimu_global_pps_set(pimu, 600, 0);
    verify_total_pps__(pimu, 600, 3);
    verify_total_pps__(pimu, 600, -1);

    pimu_destroy(pimu);
    return 0;
}

