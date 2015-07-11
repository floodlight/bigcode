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

int main(int argc, char* argv[])
{

    os_config_show(&aim_pvs_stdout);

    printf("os_sem_create=%p\n", os_sem_create);
    printf("os_time_monotonic=%"PRIu64"\n", os_time_monotonic());
    printf("os_time_monotonic=%"PRIu64"\n", os_time_monotonic());
    printf("os_time_monotonic=%"PRIu64"\n", os_time_monotonic());
    printf("os_time_realtime=%"PRIu64"\n", os_time_realtime());
    printf("os_time_thread=%"PRIu64"\n", os_time_thread());

    {
        os_sem_t sem = os_sem_create(1);
        printf("sem(regular)=%p\n", sem);
        printf("take\n");
        os_sem_take(sem);
        printf("give\n");
        os_sem_give(sem);
        printf("take\n");
        os_sem_take(sem);
        os_sem_destroy(sem);
    }

    {
        os_sem_t sem = os_sem_create_flags(1, OS_SEM_CREATE_F_TRUE_RELATIVE_TIMEOUTS);
        printf("sem(relative timeout)=%p\n", sem);
        printf("take\n");
        os_sem_take(sem);
        printf("give\n");
        os_sem_give(sem);
        printf("take\n");
        os_sem_take(sem);
        os_sem_destroy(sem);
    }

    {
        int i;
        printf("Should print every second (sleep)...");
        for(i = 0; i <= 5; i++) {
            aim_printf(&aim_pvs_stdout, "%d ", i);
            if(i & 1) {
                os_sleep_usecs(999999);
            }
            else {
                os_sleep_usecs(1000001);
            }
        }
        printf("\n");
    }
    {
        int i;
        os_sem_t s = os_sem_create(1);
        printf("Should print every second (sem)...");
        for(i = 0; i <= 5; i++) {
            aim_printf(&aim_pvs_stdout, "%d ", i);
            AIM_TRUE_OR_DIE(os_sem_take_timeout(s, 0) == 0);
            AIM_TRUE_OR_DIE(os_sem_take_timeout(s, 1000000) == -1);
            os_sem_give(s);
            AIM_TRUE_OR_DIE(os_sem_take_timeout(s, 1000000) == 0);
            os_sem_give(s);
        }
        printf("\n");
    }
    {
        int i;
        os_sem_t s = os_sem_create_flags(1, OS_SEM_CREATE_F_TRUE_RELATIVE_TIMEOUTS);
        printf("Should print every second (relative-timeout)...");
        for(i = 0; i <= 5; i++) {
            aim_printf(&aim_pvs_stdout, "%d ", i);
            AIM_TRUE_OR_DIE(os_sem_take_timeout(s, 0) == 0);
            AIM_TRUE_OR_DIE(os_sem_take_timeout(s, 1000000) == -1);
            os_sem_give(s);
            AIM_TRUE_OR_DIE(os_sem_take_timeout(s, 1000000) == 0);
            os_sem_give(s);
        }
        printf("\n");
    }
    {
        const char* name = "thready";
        char n[32];
        os_thread_name_set(name);
        os_thread_name_get(n, sizeof(n));
        AIM_TRUE_OR_DIE(!strcmp(name, n));
    }
    {
        /* From semtest.c */
        extern void sem_test_multiple(uint32_t flags, int givers, int takers);
        printf("Semaphore timeout test (normal)...\n");
        sem_test_multiple(0, 512, 1024);
        printf("Semaphore timeout test (relative)...\n");
        sem_test_multiple(OS_SEM_CREATE_F_TRUE_RELATIVE_TIMEOUTS, 512, 1024);
    }
    return 0;
}
