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

void
os_sem_destroy(os_sem_t sem)
{
    if(sem) {
        sem_destroy(&sem->sem);
        aim_free(sem);
    }
}

int
os_sem_take(os_sem_t sem)
{
    if(sem) {
        sem_wait(&sem->sem);
    }
    return 0;
}

void
os_sem_give(os_sem_t sem)
{
    if(sem) {
        sem_post(&sem->sem);
    }
}

#else
int not_empty;
#endif

