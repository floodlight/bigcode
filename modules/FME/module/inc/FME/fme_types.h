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
/************************************************************//**
 *
 * @file
 * @brief FME Datatype Definitions.
 *
 *
 * @addtogroup fme-fme
 * @{
 *
 ***************************************************************/
#ifndef __FME_TYPES_H__
#define __FME_TYPES_H__

#include <FME/fme_config.h>
#include <FME/fme.h>
#include <inttypes.h>
#include <limits.h>

#include <PPE/ppe_types.h>
#include <BigList/biglist.h>

/*
 * Fixme -- assumes availability of uint64_t
 */

/** Timer value. */
typedef uint64_t fme_timeval_t;

/** Counter value. */
typedef uint64_t fme_counter_t;

/**
 * Statistics are kepts for all packet matches.
 */
typedef struct fme_counters_s {
    /** The number of types this entry has matched */
    fme_counter_t matches;
    /** The total number of bytes matched with this entry */
    fme_counter_t bytes;
} fme_counters_t;


struct fme_key_s;

/** Key dumper signature. */
typedef int (*fme_key_dump_f)(struct fme_key_s* key, aim_pvs_t* pvs);

/** Client entry dumper signature. */
typedef int (*fme_entry_cookie_dump_f)(void* cookie, aim_pvs_t* pvs);

/**
 * This structure represents the matching criteria for an FME entry.
 */
typedef struct fme_key_s {

    /** Mask of key bits for this entry. */
    uint32_t keymask;

    /** The length of this key (in bytes) */
    int size;
    /** The value bytes for this key */
    uint8_t values[FME_CONFIG_KEY_SIZE_WORDS*4];

    /** The mask bytes for this key */
    uint8_t masks[FME_CONFIG_KEY_SIZE_WORDS*4];

    /** The function to use for dumping this key data */
    fme_key_dump_f dumper;

} fme_key_t;


/**
 * This is the fundamental match entry for a packet flow.
 *
 */
typedef struct fme_entry_s {

    /** Priority of this entry in the parent FME. */
    int prio;

    /** Match key for this entry. */
    fme_key_t key;

    /** Match counters. */
    fme_counters_t counters;

    /** Absolute timeout value */
    fme_timeval_t absolute_timeout;
    /** Relative timeout value */
    fme_timeval_t relative_timeout;

    /** The last time this entry matched. */
    fme_timeval_t timestamp;

    /** Enable or disable state of this entry */
    int enabled;

    /** Current index in the parent FME's entry table. */
    int index;

    /** Client's assigned cookie for this entry. */
    void* cookie;

    /** Client entry dumper */
    fme_entry_cookie_dump_f cdumper;

} fme_entry_t;

/**
 * This is the collection of fme_entry_t structures,
 * kept in priority order, against which you can attempt
 * matches.
 *
 * @todo: convert to inclusive list processing for the entry table.
 */
typedef struct fme_s {
    /** string used for object log messages. */
    const char* log_string;

    /** Maximum allowable entries in this FME. */
    int max_entries;

    /** The current number of entries. */
    int num_entries;

    /** The entry table. */
    fme_entry_t** entries;

} fme_t;


#endif /* __FME_TYPES_H__ */
/*@}*/
