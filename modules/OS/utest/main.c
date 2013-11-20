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

#include <OS/os_sem.h>
#include <OS/os_time.h>
#include <inttypes.h>

int main(int argc, char* argv[])
{
    printf("OS Utest Is Empty\n");
    os_config_show(&aim_pvs_stdout);

    printf("os_sem_create=%p\n", os_sem_create);
    printf("os_time_monotonic=%"PRIu64"\n", os_time_monotonic());
    printf("os_time_monotonic=%"PRIu64"\n", os_time_monotonic());
    printf("os_time_monotonic=%"PRIu64"\n", os_time_monotonic());
    printf("os_time_realtime=%"PRIu64"\n", os_time_realtime());
    printf("os_time_thread=%"PRIu64"\n", os_time_thread());

    {
        os_sem_t sem = os_sem_create(1);
        printf("sem=%p\n", sem);
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
        printf("Should print every second...");
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

    return 0;
}
