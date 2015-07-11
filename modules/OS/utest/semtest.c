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

#include <OS/os_config.h>
#include <OS/os.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>

typedef struct sem_test_s {
    pthread_t thread;

    char name[64];
    os_sem_t take;
    os_sem_t give;
    uint64_t timeout;
    int timeout_required;
    int* acquired;

} sem_test_t;

static void*
sem_test_thread__(void* p)
{
    int rv;
    sem_test_t* t = (sem_test_t*)p;

    uint64_t t0 = os_time_monotonic();
    rv = os_sem_take_timeout(t->take, t->timeout);
    uint64_t t1 = os_time_monotonic();

    uint64_t duration = t1 - t0;
    int64_t delta = t->timeout - duration;

    printf("%s: rv=% d timeout: %.8" PRId64 " duration: %.8" PRId64 " delta: % .8" PRId64 "\n", t->name, rv, t->timeout, duration, delta);

    if(rv == -1) {
        if(delta > 0) {
            AIM_DIE("%s: premature timeout.", t->name);
        }
        /* Arbitrary, within 30ms */
        if(delta < -30000 && t->timeout) {
            AIM_DIE("%s: timeout blown.", t->name);
        }
    }
    else if(rv == 0) {
        if(delta < 0 && t->timeout) {
            AIM_DIE("%s: waited longer than timeout.", t->name);
        }
        if(t->timeout_required) {
            AIM_DIE("%s: timeout was required.", t->name);
        }
        (*t->acquired)++;
    }

    if(t->give) {
        os_sem_give(t->give);
    }
    return NULL;
}

void
sem_test_multiple(uint32_t flags, int giving_threads, int taking_threads)
{
    int i;

    int thread_count = giving_threads + taking_threads;
    sem_test_t* tests = aim_zmalloc(sizeof(*tests)*thread_count);

    os_sem_t take = os_sem_create_flags(0, flags);
    os_sem_t give = os_sem_create_flags(0, flags);
    int acquired = 0;
    sem_test_t* t = tests;

    /*
     * These threads wait on a semaphore timeout
     * that will always expire. Timeout in [10000,100000].
     *
     * When the first timeout expires they give a different sem.
     */
    for(i = 0; i < giving_threads; i++, t++) {
        t->take = take;
        t->give = give;
        sprintf(t->name, "(1:%.4d)", i);
        t->timeout = ( (random() % 10) + 1 ) * 100000;
        t->timeout_required = 1;
        t->acquired = &acquired;
        pthread_create(&t->thread, NULL, sem_test_thread__, t);
    }

    /*
     * These threads wait on the semaphore given at the end
     * of the waiting period for the first set of threads.
     *
     * Some will timeout, some will acquire.
     *
     * Their timeouts are all in [2000000, 3000000].
     */
    for(i = 0; i < taking_threads; i++, t++) {
        t->take = give;
        t->give = NULL;
        sprintf(t->name, "(2:%.4d)", i);
        t->timeout = 2000000 + (( (random() % 10) + 1 ) * 100000);
        t->give = 0;
        t->timeout_required = 0;
        t->acquired = &acquired;
        pthread_create(&t->thread, NULL, sem_test_thread__, t);
    }

    for(i = 0; i < thread_count; i++) {
        void* rv;
        pthread_join(tests[i].thread, &rv);
    }

    /*
     * The acquisition count should always be equal to the number
     * of giving threads when (giving_threads <= taking_threads).
     */
    printf("Acquisition count: %d\n", acquired);
    AIM_TRUE_OR_DIE(acquired == giving_threads);

    aim_free(give);
    aim_free(take);
    aim_free(tests);
}
