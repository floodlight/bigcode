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
/************************************************************//**
 *
 * @file
 * @brief Semaphore Abstractions
 *
 ***************************************************************/
#ifndef __OS_SEMAPHORE_H__
#define __OS_SEMAPHORE_H__

#include <OS/os_config.h>

/**
 * Semaphore handle.
 */
typedef struct os_sem_s* os_sem_t;

/**
 * @brief Create a semaphore.
 * @param count Initial count.
 */
os_sem_t os_sem_create(int count);

/**
 * Specify this flag in os_sem_create_flags() if you plan to
 * used timeouts with your semaphore. This option
 * will implement true relative timeouts that are immune
 * to clock adjustments (but will not perform as well).
 */
#define OS_SEM_CREATE_F_TRUE_RELATIVE_TIMEOUTS 0x1

/**
 * @brief Create a semaphore.
 * @param count Initial count.
 * @param ... Optional uint32_t creation flags.
 */
os_sem_t os_sem_create_flags(int count, uint32_t flags);


/**
 * @brief Destroy a semaphore.
 * @param sem The semaphore.
 */
void os_sem_destroy(os_sem_t sem);

/**
 * @brief Take a semaphore
 * @param sem The semaphore.
 */
int os_sem_take(os_sem_t sem);

/**
 * @brief Give a semaphore.
 * @param sem The semaphore.
 */
void os_sem_give(os_sem_t sem);

/**
 * @brief Take a semaphore (with timeout).
 * @param sem The semaphore.
 * @param to usecs timeout in usecs
 * @returns 0 on success. -1 on timeout.
 */
int os_sem_take_timeout(os_sem_t sem, uint64_t usecs);


#endif /* __OS_SEMAPHORE_H__ */
