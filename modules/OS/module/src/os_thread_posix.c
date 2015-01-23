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
#include <OS/os_thread.h>

#if OS_CONFIG_INCLUDE_POSIX == 1

/** Not posix or portable. */

#define _GNU_SOURCE
#include <pthread.h>

int pthread_setname_np(pthread_t thread, const char *name);
int pthread_getname_np(pthread_t thread,
                       char *name, size_t len);
void
os_thread_name_set(const char* name)
{
    pthread_setname_np(pthread_self(), name);
}

char*
os_thread_name_get(char* name, int max)
{
    if(name) {
        name[0] = 0;
        pthread_getname_np(pthread_self(), name, max);
    }
    return name;
}

#endif

