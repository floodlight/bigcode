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

#ifndef DEBUG_COUNTER_H
#define DEBUG_COUNTER_H

#include <AIM/aim.h>
#include <AIM/aim_list.h>

/**
 * Debug counter
 *
 * This struct should be allocated by the caller and registered with
 * debug_counter_register. The 'value' field should be treated as private
 * to allow future optimizations.
 */
typedef struct debug_counter_s {
    uint64_t value;
    uint64_t counter_id;
    list_links_t links;
    char name[32];
    char description[256];
} debug_counter_t;


/**
 * Register a debug counter
 *
 * The 'name' and 'description' fields of the counter should be initialized by
 * the caller. The remaining fields do not need to be initialized.
 */
void debug_counter_register(debug_counter_t *counter);

/**
 * Unregister a debug counter
 */
void debug_counter_unregister(debug_counter_t *counter);

/**
 * Return a list of registered debug counters
 *
 * The list is through the 'links' field of debug_counter_t.
 *
 * Iterating over the list may not be done concurrently with calls to
 * debug_counter_register or debug_counter_unregister.
 */
list_head_t *debug_counter_list(void);


/* Trivial inline functions */

static inline uint64_t
debug_counter_get(debug_counter_t *counter)
{
    return counter->value;
}

static inline void
debug_counter_inc(debug_counter_t *counter)
{
    counter->value++;
}

static inline void
debug_counter_add(debug_counter_t *counter, uint64_t v)
{
    counter->value += v;
}

static inline void
debug_counter_reset(debug_counter_t *counter)
{
    counter->value = 0;
}

#endif
