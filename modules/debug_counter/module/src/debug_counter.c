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

#include <debug_counter/debug_counter.h>

static uint64_t debug_counter_next_id = 0;
static LIST_DEFINE(debug_counters);
static debug_counter_t register_counter;
static debug_counter_t unregister_counter;

void
debug_counter_register(debug_counter_t *counter, const char *name, const char *description)
{
    counter->value = 0;
    counter->counter_id = debug_counter_next_id++;
    list_push(&debug_counters, &counter->links);
    AIM_ASSERT(strlen(name) > 0 && strlen(name) < 64);
    counter->name = name;
    AIM_ASSERT(strlen(description) > 0 && strlen(description) < 256);
    counter->description = description;
    debug_counter_inc(&register_counter);
}

void
debug_counter_unregister(debug_counter_t *counter)
{
    debug_counter_inc(&unregister_counter);
    list_remove(&counter->links);
    memset(counter, 0, sizeof(*counter));
}

list_head_t *
debug_counter_list(void)
{
    return &debug_counters;
}

void
debug_counter_module_init(void)
{
    debug_counter_register(&register_counter, "debug_counter.register",
	                   "Number of calls to debug_counter_register");
    debug_counter_register(&unregister_counter, "debug_counter.unregister",
	                   "Number of calls to debug_counter_unregister");
}
