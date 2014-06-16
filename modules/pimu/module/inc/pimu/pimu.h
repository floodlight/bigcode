/****************************************************************
 *
 *        Copyright 2013-2014, Big Switch Networks, Inc.
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
 * @brief Packet-In Management Unit
 *
 * @addtogroup pimu-pimu
 * @{
 *
 ***************************************************************/
#ifndef __PIMU_H__
#define __PIMU_H__

#include <pimu/pimu_config.h>

/** PIMU object pointer. */
typedef struct pimu_s pimu_t;

/**
 * @brief Create a PIMU.
 * @param block_size The block size for the PIMU flow cache.
 * @param entry_count The total number of entries in the PIMU cache.
 * @returns The PIMU object pointer;
 */
pimu_t* pimu_create(uint32_t block_size, uint32_t entry_count);

/**
 * @brief Destroy a PIMU.
 * @param pimu The PIMU object.
 */
void pimu_destroy(pimu_t* pimu);

/* <auto.start.enum(pimu_action).header> */
/** pimu_action */
typedef enum pimu_action_e {
    PIMU_ACTION_ERROR,
    PIMU_ACTION_FORWARD_NEW,
    PIMU_ACTION_FORWARD_EXISTING,
    PIMU_ACTION_FORWARD_PRIORITY,
    PIMU_ACTION_DROP,
    PIMU_ACTION_LAST = PIMU_ACTION_DROP,
    PIMU_ACTION_COUNT,
    PIMU_ACTION_INVALID = -1,
} pimu_action_t;

/** Strings macro. */
#define PIMU_ACTION_STRINGS \
{\
    "ERROR", \
    "FORWARD_NEW", \
    "FORWARD_EXISTING", \
    "FORWARD_PRIORITY", \
    "DROP", \
}
/** Enum names. */
const char* pimu_action_name(pimu_action_t e);

/** Enum values. */
int pimu_action_value(const char* str, pimu_action_t* e, int substr);

/** Enum descriptions. */
const char* pimu_action_desc(pimu_action_t e);

/** validator */
#define PIMU_ACTION_VALID(_e) \
    ( (0 <= (_e)) && ((_e) <= PIMU_ACTION_DROP))

/** pimu_action_map table. */
extern aim_map_si_t pimu_action_map[];
/** pimu_action_desc_map table. */
extern aim_map_si_t pimu_action_desc_map[];
/* <auto.end.enum(pimu_action).header> */

/* <auto.start.enum(pimu_keyf_ret).header> */
/** pimu_keyf_ret */
typedef enum pimu_keyf_ret_e {
    PIMU_KEYF_RET_ERROR,
    PIMU_KEYF_RET_CONTINUE,
    PIMU_KEYF_RET_DROP,
    PIMU_KEYF_RET_FORWARD,
    PIMU_KEYF_RET_LAST = PIMU_KEYF_RET_FORWARD,
    PIMU_KEYF_RET_COUNT,
    PIMU_KEYF_RET_INVALID = -1,
} pimu_keyf_ret_t;

/** Strings macro. */
#define PIMU_KEYF_RET_STRINGS \
{\
    "ERROR", \
    "CONTINUE", \
    "DROP", \
    "FORWARD", \
}
/** Enum names. */
const char* pimu_keyf_ret_name(pimu_keyf_ret_t e);

/** Enum values. */
int pimu_keyf_ret_value(const char* str, pimu_keyf_ret_t* e, int substr);

/** Enum descriptions. */
const char* pimu_keyf_ret_desc(pimu_keyf_ret_t e);

/** validator */
#define PIMU_KEYF_RET_VALID(_e) \
    ( (0 <= (_e)) && ((_e) <= PIMU_KEYF_RET_FORWARD))

/** pimu_keyf_ret_map table. */
extern aim_map_si_t pimu_keyf_ret_map[];
/** pimu_keyf_ret_desc_map table. */
extern aim_map_si_t pimu_keyf_ret_desc_map[];
/* <auto.end.enum(pimu_keyf_ret).header> */


/**
 * @brief Process a packet-in.
 * @param pimu The PIMU object.
 * @param gid The group id (optional)
 * @param pid The ingress port id.
 * @param data The packet data.
 * @param size The packet size.
 * @param now The current time.
 * @returns The action that should be take for this packet.
 */
pimu_action_t pimu_packet_in(pimu_t* pimu, int pid, int gid, uint8_t* data,
                             uint32_t size, uint64_t now);

/**
 * @brief Key constructor.
 * @param key Output array of PIMU_CONFIG_PACKET_KEY_SIZE bytes containing key;
     array is zeroed before key constructor is called.
 * @param pid The ingress port id.
 * @param data Pointer to bytes from which the key is constructed.
 * @param size Number of bytes pointed to by data parameter.
 * @returns PIMU_KEYF_RET_DROP to drop packet, 
 *    PIMU_KEYF_RET_FORWARD to forward packet without further processing, 
 *    PIMU_KEYF_RET_CONTINUE to continue processing with constructed key
 */
typedef pimu_keyf_ret_t (*pimu_key_f)(uint8_t* key, int pid, 
                                      uint8_t* data, int size);

/**
 * @brief Set the key constructor function for the given PIMU object.
 * @param pimu The PIMU object.
 * @param keyf Key construction function; see typedef above.
 */
int pimu_key_constructor_set(pimu_t* pimu, pimu_key_f keyf);

/**
 * @brief Set the default packet rate for each flow.
 * @param pimu The PIMU object.
 * @param pps Packets per second.
 * @param burst_size Allowable burst size.
 * @note All flows will be individually rate-limited to this value.
 * @note You must clear the flow cache if you change this
 * while under traffic. See pimu_cache_clear()
 */
int pimu_flow_pps_set(pimu_t* pimu, double pps, uint32_t burst_size);

/**
 * @brief Set the global packet rate for all flows.
 * @param pimu The pimu.
 * @param pps Packets per second.
 * @param burst_size Allowable burst size.
 */
int pimu_global_pps_set(pimu_t* pimu, double pps, uint32_t burst_size);

/**
 * @brief Set the group packet rate.
 * @param pimu The PIMU object.
 * @param group The group index.
 * @param pps The Packets per second.
 * @param burst_size Allowable burst size.
 * @note Use groups if you want to shape traffic based on
 * a meta-property, such as ingress-port number.
 */
int pimu_group_pps_set(pimu_t* pimu, int group, double pps,
                       uint32_t burst_size);

/**
 * @brief Clear the PIMU flow cache.
 * @param pimu The PIMU object.
 */
void pimu_cache_clear(pimu_t* pimu);

/**
 * @brief Prioritize packets with the given ethertype.
 * @param pimu The PIMU object.
 * @param ethertype The ethertype.
 * @note Packets matching the given ethertype will not be
 * ratelimited and will be returned immediately with action FORWARD_PRIORITY.
 */
int pimu_prio_ether_type_add(pimu_t* pimu, uint16_t ethertype);

/**
 * @brief Remove an ethertype prioritization.
 * @param pimu The PIMU object.
 * @param ethertype The ethertype.
 */
int pimu_prio_ether_type_remove(pimu_t* pimu, uint16_t ethertype);

/**
 * @brief Prioritize packets with the given destination address.
 * @param pimu The PIMU object.
 * @param da The destination mac address.
 * @note Packets matching the given destination mac address will not
 * be ratelimited and will be returned immediately with action FORWARD_PRIORITY.
 */
int pimu_prio_ether_da_add(pimu_t* pimu, uint8_t da[6]);

/**
 * @brief Remove an ether da prioritization.
 * @param pimu The PIMU object.
 * @param da The destination address.
 */
int pimu_prio_ether_da_remove(pimu_t* pimu, uint8_t da[6]);

#endif /* __PIMU_H__ */
/* @} */
