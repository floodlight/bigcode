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

#if OS_CONFIG_INCLUDE_OSX == 1

#include <mach/mach_time.h>

uint64_t
os_time_monotonic(void)
{
    return os_time_realtime();
}

uint64_t
os_time_realtime(void)
{
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t now = mach_absolute_time();
    now *= info.numer;
    now /= info.denom;
    return now/(1000);
}

uint64_t
os_time_thread(void)
{
    return os_time_realtime();
}

#else
int __not_empty__;
#endif

