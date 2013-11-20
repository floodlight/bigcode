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
#include <OS/os_time.h>

#if OS_CONFIG_INCLUDE_POSIX == 1

#include <time.h>
/** TODO -- check clock_* availability macros first */


uint64_t
os_time_monotonic(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return ((uint64_t)tp.tv_sec * 1000*1000) + (tp.tv_nsec / 1000);
}

uint64_t
os_time_realtime(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return ((uint64_t)tp.tv_sec * 1000*1000) + (tp.tv_nsec / (1000));
}

uint64_t
os_time_thread(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID,  &tp);
    return ((uint64_t)tp.tv_sec * 1000*1000) + (tp.tv_nsec / (1000));
}

#else
int __not_empty__;
#endif

