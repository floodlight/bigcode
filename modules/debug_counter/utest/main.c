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

int aim_main(int argc, char* argv[])
{
    /* Test counting */
    {
        debug_counter_t counter;

        debug_counter_register(&counter, "counter", "long description of counter");
        AIM_ASSERT(debug_counter_get(&counter) == 0);
        debug_counter_inc(&counter);
        AIM_ASSERT(debug_counter_get(&counter) == 1);
        debug_counter_add(&counter, 5);
        AIM_ASSERT(debug_counter_get(&counter) == 6);
        debug_counter_reset(&counter);
        AIM_ASSERT(debug_counter_get(&counter) == 0);
        debug_counter_unregister(&counter);
    }

    /* Test registration/unregistration */
    {
        list_head_t *counters;
        list_links_t *cur;
        debug_counter_t counter1;
        debug_counter_t counter2;

        counters = debug_counter_list();
        AIM_ASSERT(list_empty(counters));

        debug_counter_register(&counter1, "counter 1", "long description of counter 1");
        debug_counter_register(&counter2, "counter 2", "long description of counter 2");

        AIM_ASSERT(counter1.counter_id != counter2.counter_id);

        debug_counter_add(&counter1, 3);
        debug_counter_add(&counter2, 5);

        counters = debug_counter_list();
        AIM_ASSERT(list_length(counters) == 2);

        LIST_FOREACH(counters, cur) {
            debug_counter_t *counter = container_of(cur, links, debug_counter_t);
            if (counter->counter_id == counter1.counter_id) {
                AIM_ASSERT(debug_counter_get(counter) == 3);
            } else if (counter->counter_id == counter2.counter_id) {
                AIM_ASSERT(debug_counter_get(counter) == 5);
            } else {
                AIM_ASSERT(0);
            }
        }

        debug_counter_unregister(&counter1);
        debug_counter_unregister(&counter2);

        counters = debug_counter_list();
        AIM_ASSERT(list_empty(counters));
    }

    return 0;
}
