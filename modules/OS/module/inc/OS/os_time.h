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
 * @brief Timing Services
 *
 ***************************************************************/
#ifndef __OS_TIME_H__
#define __OS_TIME_H__

#include <OS/os_config.h>

/**
 * @brief The fundamental time unit is a 64 bit integer in microseconds.
 */
typedef uint64_t os_time_t;

/**
 * @brief Monotonic timer value.
 */

uint64_t os_time_monotonic(void);

/**
 * @brief Current time in ms.
 */
uint64_t os_time_realtime(void);

/**
 * @brief Thread-time, if available.
 */
uint64_t os_time_thread(void);


#endif /* __OS_TIME_H__ */
