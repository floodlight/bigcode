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

#include <sys/eventfd.h>
#include <unistd.h>
#include <poll.h>


struct os_sem_s {
    sem_t sem;
    int efd;
};

os_sem_t
os_sem_create(int count, ...)
{
    va_list vargs;
    va_start(vargs, count);
    uint32_t flags = va_arg(vargs, uint32_t);
    va_end(vargs);

    os_sem_t s = aim_zmalloc(sizeof(*s));

    if(flags & OS_SEM_CREATE_F_TRUE_RELATIVE_TIMEOUTS) {
        s->efd = eventfd(count, EFD_SEMAPHORE);
        if(s->efd == -1) {
            AIM_DIE("eventfd() failed - %s", strerror(errno));
        }
    }
    else {
        sem_init(&s->sem, 0, count);
    }
    return s;
}



#define VALIDATE(_sem) AIM_TRUE_OR_DIE(_sem != NULL, "null semaphore passed to  %s", __FUNCTION__)

void
os_sem_destroy(os_sem_t sem)
{
    VALIDATE(sem);

    if(sem->efd) {
        close(sem->efd);
    } else {
        sem_destroy(&sem->sem);
    }

    aim_free(sem);
}

int
os_sem_take(os_sem_t sem)
{
    VALIDATE(sem);

    for(;;) {

        if(sem->efd) {
            eventfd_t value;
            int rv = eventfd_read(sem->efd, &value);
            if(rv == 0 && value == 1) {
                return 0;
            }
        }
        else {
            if(sem_wait(&sem->sem) == 0) {
                return 0;
            }
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

    if(sem->efd) {
        eventfd_write(sem->efd, 1);
    }
    else {
        sem_post(&sem->sem);
    }
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

static int
os_sem_take_timeout_efd__(os_sem_t sem, uint64_t usecs)
{
    AIM_TRUE_OR_DIE(sem->efd > 0);


    if(usecs == 0) {
        /** Normal wait. */
        return os_sem_take(sem);
    }
    else {

        /** poll() with timeout (in ms) */
        struct pollfd fds;

        fds.fd = sem->efd;
        fds.events = POLLIN;
        fds.revents = 0;

        int to = usecs / 1000;
        int rv;

        switch( (rv = poll(&fds, 1, to)) )
            {
            case 0:
                {
                    /* Timed out */
                    return -1;
                }
                break;
            case 1:
                {
                    if(fds.revents & POLLIN) {
                        /* Descriptor Ready */
                        return os_sem_take(sem);
                    }
                    AIM_DIE("Unexpected revents from poll(): 0x%x", fds.revents);
                }
                break;

            default:
                {
                    AIM_DIE("Unexpected return value from poll() - %d", rv);
                }
                break;
            }
    }
}


static int
os_sem_take_timeout_sem__(os_sem_t sem, uint64_t usecs)
{
    AIM_TRUE_OR_DIE(sem->efd == 0);

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

int
os_sem_take_timeout(os_sem_t sem, uint64_t usecs)
{
    VALIDATE(sem);
    if(sem->efd > 0) {
        return os_sem_take_timeout_efd__(sem, usecs);
    }
    else {
        return os_sem_take_timeout_sem__(sem, usecs);
    }
}

#else
int not_empty;
#endif

