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
 * @brief Forwarding Match Engine.
 *
 * @addtogroup fme-fme
 * @{
 *
 *
 ***************************************************************/
#ifndef __FME_H__
#define __FME_H__

#include <AIM/aim.h>
#include <FME/fme_config.h>
#include <FME/fme_types.h>
#include <BigList/biglist.h>

/**
 * @brief Create an FME.
 * @param name The name for this FME (optional)
 * @param [out] rv Receives the new FME object.
 * @param max_entries The maximum number of entries allowed in this FME.
 */
int fme_create(fme_t** rv, const char* name, int max_entries);


/**
 * @brief Destroy an FME group.
 * @param fme The FME to destroy.
 * @note This does not destroy the entries contained within this FME.
 */
void fme_destroy(fme_t* fme);

/**
 * @brief Destroy an FME group and all the entries it contains.
 * @param fme The FME to destroy.
 */
void fme_destroy_all(fme_t* fme);

/**
 * @brief Create an FME entry.
 * @param [out] rv The fme_entry_t pointer to receive the new structure.
 * @returns 0 on success, negative on error
 */
int fme_entry_create(fme_entry_t** rv);

/**
 * @brief Destroy an FME entry.
 * @param entry The FME entry pointer.
 */
void fme_entry_destroy(fme_entry_t* entry);

/**
 * @brief Set the FME entry key.
 * @param entry The FME entry pointer.
 * @param key The new key data.
 *
 */
int fme_entry_key_set(fme_entry_t* entry, fme_key_t* key);

/**
 * @brief Get the FME entry key.
 * @param entry The FME entry pointer.
 * @param key The key data pointer.
 */
int fme_entry_key_get(fme_entry_t* entry, fme_key_t* key);

/**
 * @brief determine whether an FME entry has timed out.
 * @param entry The FME entry.
 * @param now The current time.
 *
 * @returns 1 if the entry has timed out, 0 otherwise.
 *
 */
int fme_entry_timeout_status(fme_entry_t* entry, fme_timeval_t now);


/**
 * @brief Add an FME entry to the given FME.
 * @param fme The FME
 * @param entry The FME entry
 */
int fme_add_entry(fme_t* fme, fme_entry_t* entry);

/**
 * @brief Remove an FME entry fromt he given FME.
 * @param fme The FME.
 * @param entry The FME entry.
 */
int fme_remove_entry(fme_t* fme, fme_entry_t* entry);

/**
 * @brief Get all entries in the FME that have timed out.
 * @param fme The FME.
 * @param now The current time.
 * @param [out] list Receives the list of timed-out entries.
 */
int fme_entry_timeouts_get(fme_t* fme, fme_timeval_t now, biglist_t** list);




/**
 * @brief Get the highest priority match.
 * @param fme The FME.
 * @param key The incoming match key.
 * @param now The current time (for entry timeout processing).
 * @param size The size of the incoming data (for entry counter updates).
 * @param [out] matched Receives the FME entry that matches the key.
 *
 * @returns 1 if a match is found.
 *          0 if a match is not found.
 *          < 0 on error conditions.
 */
int fme_match(fme_t* fme, fme_key_t* key, fme_timeval_t now, int size,
              fme_entry_t** matched);

/**
 * @brief Get all matching entries, in priority order.
 * @param fme The FME.
 * @param key The incoming match key.
 * @param now The current time (for entry timeout processing).
 * @param size The size of the incoming data (for entry counter updates).
 * @param [out] rv Receives a biglist_t* with all matched entries.
 *
 * @returns The number of matches, or negative on error conditions.
 */
int fme_matches(fme_t* fme, fme_key_t* key,  fme_timeval_t now, int size,
                biglist_t** rv);


/**
 * @brief Dump an FME object.
 * @param fme The FME object to dump.
 * @param pvs The target output stream.
 */
void fme_dump(fme_t* fme, aim_pvs_t* pvs);

/**
 * @brief Dump an FME Entry object.
 * @param entry The FME Entry.
 * @param pvs The target output stream.
 */
void fme_entry_dump(fme_entry_t* entry, aim_pvs_t* pvs);

/**
 * @brief Dump an FME Key object.
 * @param key The FME Key.
 * @param pvs The target output stream.
 */
void fme_key_dump(fme_key_t* key, aim_pvs_t* pvs);




#endif /* __FME_H__ */
/*@}*/
