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
 * @brief BigRing Public Interface
 *
 * @addtogroup bigring-bigring
 * @{
 *
 ***************************************************************/
#ifndef __BIGRING_H__
#define __BIGRING_H__

#include <BigRing/bigring_config.h>


/**
 * BigRing object.
 */
typedef struct bigring_s bigring_t;

/**
 * Entry deallocator.
 */
typedef void (*bigring_free_entry_f)(void* entry);

/**
 * @brief Builtin deallocator -- calls AIM_FREE() on the entry.
 * @param entry The entry
 * @note Pass this as the deallocator if your objects should
 * just be freed using AIM_FREE().
 */
void bigring_aim_free_entry(void* entry);

/**
 * @brief Create a bigring buffer
 * @param size The maximum size of the buffer.
 * @param free_entry The entry deallocator.
 * @note If free_entry is NULL, the entries will not be freed.
 * @note pass bigring_aim_free_entry() if you want normal deallocation.
 * using AIM_FREE()
 */
bigring_t* bigring_create(int size, bigring_free_entry_f free_entry);

/**
 * @brief Destroy a bigring buffer.
 * @param br The bigring object.
 * @note All existing elements will be freed.
 */
void bigring_destroy(bigring_t* br);

/**
 * @brief Lock a bigring
 * @param br The bigring object
 * @note This is a noop unless BIGRING_CONFIG_INCLUDE_LOCKING is set.
 */
void bigring_lock(bigring_t* br);

/**
 * @brief Unlock a bigring.
 * @param br The bigring object.
 * @note This is a noop unless BIGRING_CONFIG_INCLUDE_LOCKING is set.
 */
void bigring_unlock(bigring_t* br);

/**
 * @brief Get the maximum size of the ring buffer.
 * @param br The bigring object.
 */
int bigring_size(bigring_t* br);

/**
 * @brief Get the number of entries in the buffer
 * @param br The BigRing object.
 * @returns The number of entries in the buffer.
 */
int bigring_count(bigring_t* br);

/**
 * @brief Get the number of entries in the buffer.
 * @param br The BigRing object.
 * @returns The number of entries in the buffer.
 * @note Call this if you have already locked the buffer
 * (for example, during an iterate operation).
 */
int bigring_count_locked(bigring_t* br);

/**
 * @brief Add an entry to the ring buffer.
 * @param br The bigring object.
 * @param entry The entry to add.
 */
void bigring_push(bigring_t* br, void* entry);

/**
 * @brief Remove the next element from the ring buffer.
 * @param br The bigring object.
 * @returns The entry if it exists, NULL otherwise.
 */
void* bigring_shift(bigring_t* br);

/**
 * @brief Begin iteration over all elements.
 * @param br The bigring object.
 * @param iter Iterator cookie.
 * @note You must lock and unlock the ring before and after.
 */
void bigring_iter_start(bigring_t* br, int* iter);

/**
 * @brief Get the next element in the iteration.
 * @param br The bigring object.
 * @param iter Iterator cookie.
 * @returns The next element, or NULL if empty.
 */
void* bigring_iter_next(bigring_t* br, int* iter);


#endif /* __BIGRING_H__ */
/* @} */

