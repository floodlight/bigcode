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
#include <OS/os_sem.h>

#if OS_CONFIG_INCLUDE_POSIX == 1


#include <semaphore.h>
#include <errno.h>
#include <time.h>

struct os_sem_s {
    sem_t sem;
};

os_sem_t
os_sem_create(int count)
{
    os_sem_t s = aim_zmalloc(sizeof(*s));
    sem_init(&s->sem, 0, count);
    return s;
}

#define VALIDATE(_sem) AIM_TRUE_OR_DIE(_sem != NULL, "null semaphore passed to  %s", __FUNCTION__)

void
os_sem_destroy(os_sem_t sem)
{
    VALIDATE(sem);
    sem_destroy(&sem->sem);
    aim_free(sem);
}

int
os_sem_take(os_sem_t sem)
{
    VALIDATE(sem);

    for(;;) {
        if(sem_wait(&sem->sem) == 0) {
            return 0;
        }
        switch(errno)
            {
            case EINTR:
                break;
            case EINVAL:
                AIM_DIE("Invalid or corrupted semaphore in os_sem_take().");
                break;
            default:
                AIM_DIE("Unhandled error condition in os_sem_take() for errno=%{errno}", errno);
                break;
            }
    }
}

void
os_sem_give(os_sem_t sem)
{
    VALIDATE(sem);
    sem_post(&sem->sem);
}


/**
 * Initialize a timespec structure for the given timeout interval.
 */
static void
timespec_init_timeout__(struct timespec* ts, uint64_t us)
{
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += (us / 1000000);
    us %= 1000000;
    ts->tv_nsec += (us*1000);
    ts->tv_sec += (ts->tv_nsec / 1000000000);
    ts->tv_nsec %= 1000000000;
}

int
os_sem_take_timeout(os_sem_t sem, uint64_t usecs)
{
    VALIDATE(sem);

    if(usecs == 0) {
        /** Normal wait */
        return os_sem_take(sem);
    }
    else {

        struct timespec ts;
        timespec_init_timeout__(&ts, usecs);

        for(;;) {
            if(sem_timedwait(&sem->sem, &ts) == 0) {
                return 0;
            }
            switch(errno)
                {
                case EINTR:
                    break;
                case ETIMEDOUT:
                    return -1;

                case EINVAL:
                    AIM_DIE("Invalid or corrupted semaphore or the timespec was invalid in os_sem_take_timeout().");
                    break;
                default:
                    AIM_DIE("Unhandled error condition in os_sem_take() for errno=%{errno}", errno);
                    break;
                }
        }
    }
}

#else
int not_empty;
#endif

