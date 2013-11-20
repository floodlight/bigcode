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

#include "pimu_log.h"

#include <nwac/nwac.h>
#include <AIM/aim_rl.h>

#include <inttypes.h>

typedef struct pimu_entry_s {
    /** Cache entry header */
    nwac_entry_header_t hdr;
    /** Packet data key */
    uint8_t key[PIMU_CONFIG_PACKET_KEY_SIZE];
    /** Ratelimiter */
    aim_ratelimiter_t rl;
    /** Ingress port for this entry */
    int pid;
} pimu_entry_t;

typedef struct pimu_group_s {
    /** PPS for this group */
    uint32_t pps;
    /** Ratelimiter for this group */
    aim_ratelimiter_t rl;
} pimu_group_t;


typedef struct pimu_prio_ether_types_s {
    uint16_t values[PIMU_CONFIG_MAX_ETHER_TYPES];
    int count;
} pimu_prio_ether_types_t;

typedef struct pimu_prio_ether_das_s {
    uint8_t values[PIMU_CONFIG_MAX_ETHER_DAS][6];
    int count;
} pimu_prio_ether_das_t;

struct pimu_s {
    /** Flow cache */
    nwac_t* nwac;
    /** Flow Packets/sec */
    uint32_t flow_pps;

    /** Global Group -- applicable to all packets */
    pimu_group_t global;

    /** User-defined groups */
    pimu_group_t groups[PIMU_CONFIG_GROUP_COUNT];

    /** Priority Classes */
    struct {
        struct {
            pimu_prio_ether_types_t types;
            pimu_prio_ether_das_t   das;
        } ether;
    } prio;

}; /* pimu_t */


#define GROUP_VALID(_gid) ( ((_gid) >= 0) && ((_gid) < PIMU_CONFIG_GROUP_COUNT))

pimu_t*
pimu_create(uint32_t block_size, uint32_t entry_count)
{
    pimu_t* pimu = aim_zmalloc(sizeof(*pimu));
    pimu->nwac = nwac_create(block_size, PIMU_CONFIG_PACKET_KEY_SIZE,
                             sizeof(pimu_entry_t), entry_count);
    return pimu;
}

void
pimu_destroy(pimu_t* pimu)
{
    nwac_destroy(pimu->nwac);
    aim_free(pimu);
}

int
pimu_flow_pps_set(pimu_t* pimu, uint32_t pps)
{
    pimu->flow_pps = pps;
    return 0;
}

static void
group_init__(pimu_group_t* group, uint32_t pps, uint32_t burst_size)
{
    group->pps = pps;
    if(pps) {
        aim_ratelimiter_init(&group->rl, 1000000/pps, burst_size, NULL);
    }
}

int
pimu_global_pps_set(pimu_t* pimu, uint32_t pps, uint32_t burst_size)
{
    group_init__(&pimu->global, pps, burst_size);
    return 0;
}

int
pimu_group_pps_set(pimu_t* pimu, int gid, uint32_t pps, uint32_t burst_size)
{
    if(GROUP_VALID(gid)) {
        group_init__(pimu->groups + gid, pps, burst_size);
        return 0;
    }
    return -1;
}


static pimu_action_t
pimu_flow_action__(pimu_t* pimu, int pid,
                   uint8_t* data, uint32_t size, uint64_t now)
{
    pimu_entry_t* pe;
    uint8_t key[PIMU_CONFIG_PACKET_KEY_SIZE] = { 0 };

    if(pimu->flow_pps == 0) {
        return PIMU_ACTION_FORWARD_NEW;
    }

    if(size < PIMU_CONFIG_PACKET_KEY_SIZE) {
        PIMU_MEMCPY(key, data, size);
        data = key;
    }
    pe = (pimu_entry_t*)nwac_search(pimu->nwac, data, now);
    if(pe) {
        if(pe->hdr.valid && pe->pid == pid) {
            /** Entry exists and the ingress port hasn't changed */
            if(aim_ratelimiter_limit(&pe->rl, now) == 0) {
                /* Allowed */
                return PIMU_ACTION_FORWARD_EXISTING;
            }
            else {
                /* Rate limit exceeded */
                return PIMU_ACTION_DROP;
            }
        }
        else {
            /** New entry or ingress port change */
            pe->hdr.valid = 1;
            PIMU_MEMCPY(pe->key, data, PIMU_CONFIG_PACKET_KEY_SIZE);
            aim_ratelimiter_init(&pe->rl, 1000000/pimu->flow_pps, 0, NULL);
            aim_ratelimiter_limit(&pe->rl, now);
            pe->pid = pid;
            return PIMU_ACTION_FORWARD_NEW;
        }
    }
    else {
        /* This should only happen if 'now' is zero */
        AIM_LOG_WARN("search returned NULL (now=%" PRId64 ")", now);
        return PIMU_ACTION_FORWARD_NEW;
    }
}

static inline uint16_t
data16__(uint8_t* start, int offset)
{
    uint16_t rv = start[offset];
    rv <<= 8;
    rv += start[offset+1];
    return rv;
}

static inline int
pimu_priority_check_ether_type__(pimu_t* pimu, uint8_t* data, uint32_t size)
{
    int i;
    uint16_t x;

    x = data16__(data, 12);

    if(x < 0x600) {
        /* 802.3, assumed LLC */
        /* TODO -- check for SNAP */
        return 0;
    }
    if(x == 0x8100) {
        /* Assume single tagged */
        x = data16__(data, 16);
    }

    for(i = 0; i < pimu->prio.ether.types.count; i++) {
        if(x == pimu->prio.ether.types.values[i]) {
            return 1;
        }
    }
    return 0;
}

static inline int
pimu_priority_check_ether_da__(pimu_t* pimu, uint8_t* data, uint32_t size)
{
    int i;
    for(i = 0; i < pimu->prio.ether.das.count; i++) {
        if(PIMU_MEMCMP(data, pimu->prio.ether.das.values[i], 6) == 0) {
            return 1;
        }
    }
    return 0;
}

static inline int
pimu_priority_check__(pimu_t* pimu, uint8_t* data, uint32_t size)
{
    if(pimu->prio.ether.types.count) {
        if(pimu_priority_check_ether_type__(pimu, data, size)) {
            return 1;
        }
    }
    if(pimu->prio.ether.das.count) {
        if(pimu_priority_check_ether_da__(pimu, data, size)) {
            return 1;
        }
    }
    return 0;
}

pimu_action_t
pimu_packet_in(pimu_t* pimu, int pid,
               int gid, uint8_t* data, uint32_t size, uint64_t now)
{
    if(pimu_priority_check__(pimu, data, size)) {
        return PIMU_ACTION_FORWARD_PRIORITY;
    }
    if(pimu->global.pps) {
        /* Global rate limiting is enabled. Check this first */
        if(aim_ratelimiter_limit(&pimu->global.rl, now) == -1) {
            /* Denied */
            return PIMU_ACTION_DROP;
        }
    }
    if(GROUP_VALID(gid) && pimu->groups[gid].pps) {
        /* Group rate limiting is enabled and a group was specified */
        if(aim_ratelimiter_limit(&pimu->groups[gid].rl, now) == -1) {
            /* Denied */
            return PIMU_ACTION_DROP;
        }
    }
    /* Return flow action */
    return pimu_flow_action__(pimu, pid, data, size, now);
}

void
pimu_cache_clear(pimu_t* pimu)
{
    nwac_clear(pimu->nwac);
}


int
pimu_prio_ether_type_add(pimu_t* pimu, uint16_t ethertype)
{
    int i;
    int size = AIM_ARRAYSIZE(pimu->prio.ether.types.values);
    pimu_prio_ether_types_t* pet = &pimu->prio.ether.types;

    /* Already prioritized? */
    for(i = 0; i < pet->count; i++) {
        if(pet->values[i] == ethertype) {
            return 0;
        }
    }

    if(pet->count == size) {
        AIM_LOG_ERROR("Ether type priority table is full (%d entries)",
                      size);
        return -1;
    }

    pet->values[pet->count++] = ethertype;

    return 0;
}

int
pimu_prio_ether_type_remove(pimu_t* pimu, uint16_t ethertype)
{
    int i;
    pimu_prio_ether_types_t* pet = &pimu->prio.ether.types;

    for(i = 0; i < pet->count; i++) {
        if(pet->values[i] == ethertype) {
            if(pet->count != (i+1)) {
                /* Swap with last entry */
                pet->values[i] =
                    pet->values[pet->count-1];
            }
            pet->count--;
            return 0;
        }
    }
    return -1;
}

int
pimu_prio_ether_da_add(pimu_t* pimu, uint8_t da[6])
{
    int i;
    int size = AIM_ARRAYSIZE(pimu->prio.ether.das.values);
    pimu_prio_ether_das_t* pdas = &pimu->prio.ether.das;

    /* Already prioritized? */
    for(i = 0; i < pdas->count; i++) {
        if(PIMU_MEMCMP(pdas->values[i], da, 6) == 0) {
            return 0;
        }
    }

    if(pdas->count == size) {
        AIM_LOG_ERROR("Ether DA priority table is full (%d entries)",
                      size);
        return -1;
    }

    PIMU_MEMCPY(pdas->values[pdas->count++],
                da, 6);
    return 0;
}

int
pimu_prio_ether_da_remove(pimu_t* pimu, uint8_t da[6])
{
    int i;
    pimu_prio_ether_das_t* pdas = &pimu->prio.ether.das;

    for(i = 0; i < pdas->count; i++) {
        if(PIMU_MEMCMP(pdas->values[i], da, 6) == 0) {
            if(pdas->count != (i+1)) {
                /* Swap with last entry */
                PIMU_MEMCPY(pdas->values[i], pdas->values[pdas->count-1],
                            6);
            }
            pdas->count--;
            return 0;
        }
    }
    return 0;
}
