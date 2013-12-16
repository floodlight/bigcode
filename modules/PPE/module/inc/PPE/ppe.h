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
 * @brief Packet Parsing Engine Interface.
 *
 * @addtogroup ppe-ppe
 * @{
 *
 ****************************************************************/

#ifndef __PPE_H__
#define __PPE_H__

#include <PPE/ppe_config.h>
#include <PPE/ppe_types.h>


/**
 * @brief Initialize a PPE Packet structure with the given data.
 * This does nothing but initialize the structure.
 *
 * @param ppep The PPE packet structure to be initialized.
 * @param data The pointer to your packet data. The data is not copied.
 * @param size The size, in bytes, of your packet data.
 *
 * @returns 0 if successful, negative on error.
 */
int ppe_packet_init(ppe_packet_t* ppep, uint8_t* data, int size);

/**
 * @brief Deinitialize a PPE packet structure.
 * @param ppep The PPE packet structure.
 */
void ppe_packet_denit(ppe_packet_t* ppep);

/**
 * @brief Parse the packet. This initializes the field and header
 * information about the packet. This is required before calling
 * any of the field or header related functions.
 *
 * @param ppep The PPE packet structure.
 *
 * @returns 0 if successful, negative on error.
 *
 */
int ppe_parse(ppe_packet_t* ppep);


/**
 * @brief Duplicate a packet.
 * @param dst The destination packet.
 * @param src The source packet.
 */
int ppe_packet_dup(ppe_packet_t* dst, ppe_packet_t* src);

/**
 * @brief Get the value of the given field.
 *
 * @param ppep The PPE packet structure.
 * @param field The PPE field identifier.
 * @param [out] rv The pointer to the location in which to store the value.
 *
 * @returns 0 if successful, negative on error.
 *
 * @note It is an error to call this function with a field that is wider
 * than 32 bits.
 */
int ppe_field_get(ppe_packet_t* ppep, ppe_field_t field,
                  uint32_t* rv);

/**
 * @brief Set the value of the given field. This modifies the packet data
 * directly.
 *
 * @param ppep The PPE packet structure.
 * @param field the PPE field identifier.
 * @param sv The new value for the field.
 *
 * @note It is an error to call this fucntion witha  field that is wider
 * than 32 bits.
 */
int ppe_field_set(ppe_packet_t* ppep, ppe_field_t field,
                  uint32_t sv);

/**
 * @brief Copy a field value between headers
 * @param ppep The PPE packet structure.
 * @param dst_field The destination field.
 * @param src_field The source field.
 *
 * @returns 0 if successfull, negative otherwise.
 */
int ppe_field_copy(ppe_packet_t* ppep, ppe_field_t dst_field,
                   ppe_field_t src_field);

/**
 * @brief Copy a wide field value between headers
 * @param ppep The PPE packet structure.
 * @param dst_field The destination field.
 * @param src_field The source field.
 */
int ppe_wide_field_copy(ppe_packet_t* ppep, ppe_field_t dst_field,
                        ppe_field_t src_field);

/**
 * @brief Set the value of the given field in the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param field The PPE field identifier.
 * @param sv The value for the field.
 *
 */
int ppe_field_set_header(uint8_t* header_start, ppe_field_t field,
                         uint32_t sv);

/**
 * @brief Set the value of the given field info in the given header buffer.
 * @param header_start Pointer to the start of the buffer.
 * @param fi The PPE Field info structure.
 * @param sv The value for the field.
 */
int ppe_field_info_set_header(uint8_t* header_start,
                              const ppe_field_info_t* fi,
                              uint32_t sv);

/**
 * @brief Get the value of the given field from the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param field The PPE field identifier.
 * @param [out] rv The pointer to the location in which to store the value.
 */
int ppe_field_get_header(uint8_t* header_start, ppe_field_t field,
                         uint32_t* rv);

/**
 * @brief Get the value of the given field from the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param fi The PPE field info structure.
 * @param [out] rv The pointer to the location in which to store the value.
 */
int ppe_field_info_get_header(uint8_t* header_start,
                              ppe_field_info_t* fi, uint32_t* rv);

/**
 * @brief Get the value of the given wide field.
 * @param ppep The PPE packet structure.
 * @param field The PPE field identifier.
 * @param [out] rv The buffer which receives the value of the field.
 *
 * @note It is an error to call this function with a field <= 32 bits.
 * @note You must pass a buffer of sufficient size for the given field.
 */
int ppe_wide_field_get(ppe_packet_t* ppep, ppe_field_t field,
                              uint8_t* rv);

/**
 * @brief Get the value of the given wide field from the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param field The PPE field identifier.
 * @param [out] rv The buffer which receives the value of the field.
 */
int ppe_wide_field_get_header(uint8_t* header_start, ppe_field_t field,
                              uint8_t* rv);

/**
 * @brief Get the value of the given wide field from the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param fi The field info structure.
 * @param [out] rv The buffer which receives the value of the field.
 */
int ppe_wide_field_info_get_header(uint8_t* header_start,
                                   ppe_field_info_t* fi, uint8_t* rv);
/**
 * @brief Set the value of the given wide field.
 * @param ppep The PPE packet structure.
 * @param field The PPE field identifier.
 * @param [in] sv The data used to set the field.
 *
 * @note It is an error to call this function with a field <= 32 bits.
 * @note You must pass a buffer of sufficient size for the given field.
 * @note The field will be set to zero if sv is NULL.
 */
int ppe_wide_field_set(ppe_packet_t* ppep, ppe_field_t field,
                       uint8_t* sv);

/**
 * @brief Set the value of the given wide field in the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param field The PPE field identifier.
 * @param [in] sv The data used to set the field.
 */
int ppe_wide_field_set_header(uint8_t* header_start, ppe_field_t field,
                              uint8_t* sv);

/**
 * @brief Set the value of the given wide field in the given header buffer.
 * @param header_start Pointer to the start of the header buffer.
 * @param fi The field information structure.
 * @param [in] sv The data used to set the field.
 */
int ppe_wide_field_info_set_header(uint8_t* header_start,
                                   const ppe_field_info_t* fi, uint8_t* sv);

/**
 * @brief Update dynamic packet fields.
 * @param ppep The PPE packet structure.
 * @returns 0 on success, -1 on error
 */
int ppe_packet_update(ppe_packet_t* ppep);

/**
 * @brief Get the current packet format.
 * @param ppep The PPE packet structure.
 * @param [out] rv The PPE header identifier.
 * @returns 0 if succesfull, negative on error.
 *
 * @note The header value will be PPE_HEADER_8021Q or PPE_HEADER_ETHERII.
 * These are insufficient categories.
 */
int ppe_packet_format_get(ppe_packet_t* ppep, ppe_header_t* rv);


/**
 * @brief Set the current packet format.
 * @param ppep The PPE packet structure.
 * @param format The new format, either PPE_HEADER_ETHERII or PPE_HEADER_8021Q
 *
 * @returns 1 if successful and the buffer was reallocated.
 * @returns 0 if successful with no reallocation.
 * @returns -1 on error.
 *
 * @note These are insufficient categories.
 */
int ppe_packet_format_set(ppe_packet_t* ppep, ppe_header_t format);

/**
 * @brief Determine whether the given header exists in the packet.
 * @param ppep The PPE packet structure.
 * @param header The PPE header identifer.
 * @returns 1 if the header exists, 0 if it doesn't, and < 0 on error
 *
 */
int ppe_header_exists(ppe_packet_t* ppep, ppe_header_t header);

/**
 * @brief Determine whether the given field exists in the packet.
 * @param ppep The PPE packet structure.
 * @param field The PPE field identifier
 * @returns 1 if the field exists, 0 if it doesn't, and < 0 on error
 */
int ppe_field_exists(ppe_packet_t* ppep, ppe_field_t field);

/**
 * @brief Retrieve the pointer to the given header in the packet.
 * @param ppep The PPE packet structure.
 * @param header the PPE header identifier.
 * @returns NULL if the header does not exist, otherwise a pointer into the
 * packet data buffer.
 *
 */
uint8_t* ppe_header_get(ppe_packet_t* ppep, ppe_header_t header);

/**
 * @brief Retrieve the pointer to the given field in the packet.
 * @param ppep The PPE packet structure.
 * @param field The PPE field identifier.
 * @returns NULL if the field does not exist, otherwise a pointer to the
 * packet data buffer;
 *
 */
uint8_t* ppe_fieldp_get(ppe_packet_t* ppep, ppe_field_t field);


/**
 * @brief Treat the given pointer as the start of the field's parent
 * header and retrieve the pointer to the given field.
 * @param start The base address of the field's parent header.
 * @param field The PPE Field identifier.
 * @returns The address of the first byte (in network order) containing
 * data for the field.
 */
uint8_t* ppe_fieldp_get_header(uint8_t* start, ppe_field_t field);

/**
 * @brief Get the field information pointer for the given field.
 * @param field The PPE field identifier.
 * @returns A pointer to the field information structure.
 */
const ppe_field_info_t* ppe_field_info_get(ppe_field_t field);

/**
 * @brief Set the header point in a given packet.
 * @param ppep The PPE packet structure.
 * @param header The PPE header identifier.
 * @param start The new header start address.
 * @returns 0 on success, negative on error.
 *
 */
int ppe_header_set(ppe_packet_t* ppep, ppe_header_t header,
                   uint8_t* start);

/**
 * @brief Dump the contents of the given packet header.
 * @param ppep The PPE packet structure.
 * @param header The PPE header identifier.
 * @param pvs The output stream.
 */
int ppe_header_dump(ppe_packet_t* ppep, ppe_header_t header, aim_pvs_t* pvs);

/**
 * @brief Dump the contents of the given packet field.
 * @param ppep The PPE packet structure.
 * @param f The PPE Field identifier.
 * @param pvs The output stream.
 */
int ppe_field_dump(ppe_packet_t* ppep, ppe_field_t f, aim_pvs_t* pvs);

/**
 * @brief Treat the given pointer as the base of the given header and
 * dump the header's contents.
 * @param start The base address for the header.
 * @param header The PPE header identifer.
 * @param pvs The output stream.
 */
int ppe_header_dump_header(uint8_t* start, ppe_header_t header, aim_pvs_t* pvs);

/**
 * @brief Treat the given pointer as the base of the parent header and
 * dump the field's contents.
 * @param start The base of the field's parent header.
 * @param field The PPE Field identifier.
 * @param pvs The output stream.
 */
int ppe_field_dump_header(uint8_t* start, ppe_field_t field, aim_pvs_t* pvs);

/**
 * @brief Initialize a dynamic field key.
 * @param dfk The DFK to initialize.
 * @param fields The fields in this key.
 * @param fcount The number of fields in this key.
 */
int ppe_dfk_init(ppe_dfk_t* dfk, ppe_field_t* fields, int fcount);

/**
 * @brief Destroy a dynamic field key.
 * @param dfk The DFK to destroy.
 */
int ppe_dfk_destroy(ppe_dfk_t* dfk);

/**
 * @brief Display the contents of a field key.
 * @param dfk The dfk
 * @param pvs The output pvs.
 */
int ppe_dfk_show(ppe_dfk_t* dfk, aim_pvs_t* pvs);


/**
 * @brief Build a dfk from the given packet.
 * @param src The PPE packet structure.
 * @param dst The PPE dynamic key structure.
 */
int ppe_packet_dfk(ppe_packet_t* src, ppe_dfk_t* dst);

/**
 * @brief Set the value of a field in a field key.
 * @param dfk Destination key.
 * @param field The field to set.
 * @param value The data.
 */
int ppe_dfk_field_set(ppe_dfk_t* dfk, ppe_field_t field, uint32_t value);

/**
 * @brief Get the value of a field in a field key.
 * @param dfk Source key.
 * @param field The field to get.
 * @param [out] value Receives the field's value.
 */
int ppe_dfk_field_get(ppe_dfk_t* dfk, ppe_field_t field, uint32_t* value);

/**
 * @brief Set the value of a wide field in a field key.
 * @param dfk Destination key.
 * @param field Field to set.
 * @param data Field data.
 */
int ppe_dfk_wide_field_set(ppe_dfk_t* dfk, ppe_field_t field, uint8_t* data);

/**
 * @brief Get the value of a wide field in a field key.
 * @param dfk Source key.
 * @param field Field to get.
 * @param data Receives the field data.
 */
int ppe_dfk_wide_field_get(ppe_dfk_t* dfk, ppe_field_t field, uint8_t* data);

/**
 * @brief Dump all fields in the packet.
 * @param ppep The PPE packet structure.
 * @param pvs The output pvs.
 */
int ppe_packet_dump(ppe_packet_t* ppep, aim_pvs_t* pvs);

#endif /* __PPE_H__ */
/*@}*/
