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
 * @brief Sleeping Services
 *
 ***************************************************************/
#ifndef __OS_SLEEP_H__
#define __OS_SLEEP_H__

/**
 * @brief Sleep for the given number of microseconds.
 * @param usecs The time to sleep.
 * @returns 0 if the sleep was succesful.
 * @returns -1 on interruption or error.
 */
int os_sleep_usecs(uint64_t usecs);


#endif /* __OS_SLEEP_H__ */
