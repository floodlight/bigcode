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
 * @brief Virtual Port Interface.
 *
 * @addtogroup vpi-vpi
 * @{
 *
 ***************************************************************/

#ifndef __VPI_H__
#define __VPI_H__

#include <VPI/vpi_config.h>
#include <VPI/vpi_protocol.h>

/**
 * @brief Initialize the VPI module.
 * @note This must be called before any other VPI calls.
 */
int vpi_init(void);

/**
 * @brief Close the VPI module.
 * @note Deallocates all outstanding VPI instances.
 */

void vpi_close(void);


/**
 * This is the opaque handle to a VPI object.
 */
typedef struct vpi_s* vpi_t;


/**
 * @brief Create a VPI based on the given create specification.
 * @param spec The VPI create spec.
 * @returns The new VPI instance.
 */
vpi_t vpi_create(const char* spec);


/**
 * @brief Set the name of a VPI instance.
 * @param vpi The VPI object.
 * @param name The new name.
 * @note Optional.  A VPI object's name defaults to its create spec.
 */
int vpi_name_set(vpi_t vpi, const char* name);

/**
 * @brief Get the name of a VPI instance.
 * @param vpi The VPI object.
 * @returns The name of the given VPI.
 * @note A VPI object's name defaults to its create spec.
 */
const char* vpi_name_get(vpi_t vpi);


/**
 * @brief Increase the reference count of a VPI instance.
 * @param vpi The VPI object.
 */
int vpi_ref(vpi_t vpi);

/**
 * @brief Descrease the reference ount of a VPI instance.
 * @param vpi The VPI object.
 * @note If the reference count becomes zero, the
 * @note VPI will be destroyed.
 * @note This function is equivalent to vpi_destroy().
 */
int vpi_unref(vpi_t vpi);

/**
 * @brief Send a packet on a VPI
 * @param vpi The VPI object.
 * @param data The packet data.
 * @param size The size of the packet data.
 */
int vpi_send(vpi_t vpi, uint8_t* data, uint32_t size);


/**************************************************************************//**
 *
 *
 *
 *
 *
 *****************************************************************************/
/**
 * @brief Recv a packet on a VPI.
 * @param vpi The VPI object.
 * @param data The buffer to receive the data.
 * @param size The maximum buffer size.
 * @param block Whether to block on recv.
 *
 * @note If block is true, will block indefinitely until
 * @note data is received.
 * @note If block is false, will return immediately if
 * @note no data is ready.
 *
 * @returns The number of bytes received.
 */
int vpi_recv(vpi_t vpi, uint8_t* data, uint32_t size, int block);


/**
 * @brief Drain any packets currently queued on the VPI.
 * @param vpi The VPI object.
 *
 * @note This calls vpi_recv() until no more data is available.
 */
int vpi_drain(vpi_t vpi);



/* <auto.start.enum(ALL).header> */
/** vpi_protocol_opcode */
typedef enum vpi_protocol_opcode_e {
    VPI_PROTOCOL_OPCODE_PACKET,
    VPI_PROTOCOL_OPCODE_HELLO,
    VPI_PROTOCOL_OPCODE_REGISTER_RECV_LISTENER,
    VPI_PROTOCOL_OPCODE_UNREGISTER_RECV_LISTENER,
    VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER,
    VPI_PROTOCOL_OPCODE_UNREGISTER_SEND_LISTENER,
    VPI_PROTOCOL_OPCODE_ECHO,
    VPI_PROTOCOL_OPCODE_REGISTER_SENDRECV_LISTENER,
    VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER,
    VPI_PROTOCOL_OPCODE_LAST = VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER,
    VPI_PROTOCOL_OPCODE_COUNT,
    VPI_PROTOCOL_OPCODE_INVALID = -1,
} vpi_protocol_opcode_t;

/** Strings macro. */
#define VPI_PROTOCOL_OPCODE_STRINGS \
{\
    "packet", \
    "hello", \
    "register_recv_listener", \
    "unregister_recv_listener", \
    "register_send_listener", \
    "unregister_send_listener", \
    "echo", \
    "register_sendrecv_listener", \
    "unregister_sendrecv_listener", \
}
/** Enum names. */
const char* vpi_protocol_opcode_name(vpi_protocol_opcode_t e);

/** Enum values. */
int vpi_protocol_opcode_value(const char* str, vpi_protocol_opcode_t* e, int substr);

/** Enum descriptions. */
const char* vpi_protocol_opcode_desc(vpi_protocol_opcode_t e);

/** validator */
#define VPI_PROTOCOL_OPCODE_VALID(_e) \
    ( (0 <= (_e)) && ((_e) <= VPI_PROTOCOL_OPCODE_UNREGISTER_SENDRECV_LISTENER))

/** vpi_protocol_opcode_map table. */
extern aim_map_si_t vpi_protocol_opcode_map[];
/** vpi_protocol_opcode_desc_map table. */
extern aim_map_si_t vpi_protocol_opcode_desc_map[];

/** vpi_log_flag */
typedef enum vpi_log_flag_e {
    VPI_LOG_FLAG_CREATE,
    VPI_LOG_FLAG_SEND,
    VPI_LOG_FLAG_RECV,
} vpi_log_flag_t;

/** Enum names. */
const char* vpi_log_flag_name(vpi_log_flag_t e);

/** Enum values. */
int vpi_log_flag_value(const char* str, vpi_log_flag_t* e, int substr);

/** Enum descriptions. */
const char* vpi_log_flag_desc(vpi_log_flag_t e);

/** Enum validator. */
int vpi_log_flag_valid(vpi_log_flag_t e);

/** validator */
#define VPI_LOG_FLAG_VALID(_e) \
    (vpi_log_flag_valid((_e)))

/** vpi_log_flag_map table. */
extern aim_map_si_t vpi_log_flag_map[];
/** vpi_log_flag_desc_map table. */
extern aim_map_si_t vpi_log_flag_desc_map[];
/* <auto.end.enum(ALL).header> */


/**
 * @brief Send a VPI inband ioctl message (on interfaces that support it).
 * @param vpi The VPI object.
 * @param cmd The VPI Protocol opcode for the ioctl.
 * @param data The data for the opcode.
 * @param size The size of the opcode data.
 */
int vpi_ioctl(vpi_t vpi, vpi_protocol_opcode_t cmd,
              uint8_t* data, int size);


/**
 * @brief Get the file descriptor associated with this VPI.
 * @param vpi The VPI object.
 *
 * @returns A valid file descriptor if the interface supports it.
 * @returns 0 if the interface does have a file descriptor.
 */
int vpi_descriptor_get(vpi_t vpi);


/**
 * @brief Destroy a VPI instance.
 * @param vpi The VPI object to destroy.
 */
int vpi_destroy(vpi_t vpi);

/**
 * @brief Return the create specification for a given VPI.
 * @param vpi The VPI object.
 * @returns The create spec that created this VPI object.
 */
const char* vpi_get_create_spec(vpi_t vpi);

/**
 * @brief Get the recv half of the create specification for this VPI.
 * @param vpi The VPI object.
 * @returns The recv: portion of the create spec.
 */
const char* vpi_get_recv_spec(vpi_t vpi);

/**
 * @brief Get the send half of the create specification for this VPI.
 * @param vpi The VPI object.
 * @returns The send: portion of the create spec.
 */
const char* vpi_get_send_spec(vpi_t vpi);

/**
 * @brief Return the VPI specification necessary to communicate and
 * connect to the given VPI.
 * @param vpi The VPI object.
 *
 * @details
 * This function returns the specification you would have to use if
 * you wanted to create a VPI endpoint that was *connected* to
 * the given VPI.
 *
 * Many interfaces have dynamic properties that are known only at
 * create time. This function tells you how to create a new VPI that
 * can talk to the given VPI.
 */
const char* vpi_get_send_to_spec(vpi_t vpi);

/**
 * @brief Return the interface type of the given VPI.
 * @param vpi The VPI object.
 * @returns The interface type.
 */
const char* vpi_type_get(vpi_t vpi);

/******************************************************************************
 *
 * VPI Endpoints can have listeners for debug, monitoring, or
 * dynamic attach and detach purposes.
 *
 * You can attach a VPI as a listener for another VPI's sends and receives.
 *
 *
 *****************************************************************************/

/**
 * @brief Add a VPI as a listener for the given VPI's receives.
 * @param vpi The VPI on which to register as a listener.
 * @param listener The VPI that will listen to the first VPI's recieves.
 */
int vpi_add_recv_listener(vpi_t vpi, vpi_t listener);

/**
 * @brief Create a VPI and add it as a listener for the given VPI's receives.
 * @param vpi The VPI on which to register as a listener.
 * @param create_spec The create spec for the new listener VPI.
 */
int vpi_add_recv_listener_spec(vpi_t vpi, const char* create_spec);

/**
 * @brief Remove a VPI as a listener for the given  VPI's receives.
 * @param vpi The VPI on which to unregister the listener.
 * @param listener The VPI to unregister.
 */
int vpi_remove_recv_listener(vpi_t vpi, vpi_t listener);

/**
 * @brief Remove the VPI that has the give create_spec as a listener for
 * the given VPI's receives.
 * @param vpi The VPI on which to unregister the listener.
 * @param create_spec The specification for the VPI to unregister.
 */
int vpi_remove_recv_listener_spec(vpi_t vpi, const char* create_spec);

/**
 * @brief Get the current count of all recv listeners on the given VPI.
 * @param vpi The VPI object.
 * @returns The total number of VPI instances that are currently registered
 * as receive listeners on the given VPI.
 */
int vpi_recv_listener_count(vpi_t vpi);

/**
 * @brief Drop all receive listeners.
 * @param vpi The VPI object.
 */
int vpi_recv_listeners_drop(vpi_t vpi);


/**
 * @brief Add a VPI as a listener for the given VPI's sends.
 * @param vpi The VPI on which to register as a listener.
 * @param listener The VPI that will listen to the first VPI's sends.
 */
int vpi_add_send_listener(vpi_t vpi, vpi_t listener);

/**
 * @brief Create a VPI and add it as a listener for the given VPI's sends.
 * @param vpi The VPI on which to register as a listener.
 * @param create_spec The create spec for the new listener VPI.
 */
int vpi_add_send_listener_spec(vpi_t vpi, const char* create_spec);

/**
 * @brief Remove a VPI as a listener for the given  VPI's sends.
 * @param vpi The VPI on which to unregister the listener.
 * @param listener The VPI to unregister.
 */
int vpi_remove_send_listener(vpi_t vpi, vpi_t listener);

/**
 * @brief Remove the VPI that has the give create_spec as a listener for
 * the given VPI's sends.
 * @param vpi The VPI on which to unregister the listener.
 * @param create_spec The specification for the VPI to unregister.
 */
int vpi_remove_send_listener_spec(vpi_t vpi, const char* create_spec);

/**
 * @brief Get the current count of all send listeners on the given VPI.
 * @param vpi The VPI object.
 * @returns The total number of VPI instances that are currently registered
 * as send listeners on the given VPI.
 */
int vpi_send_listener_count(vpi_t vpi);

/**
 * @brief Drop all send listeners.
 * @param vpi The VPI object.
 */
int vpi_send_listeners_drop(vpi_t vpi);

/**
 * @brief Add a VPI as a listener for the given VPI's sends and receives.
 * @param vpi The VPI on which to register as a listener.
 * @param listener The VPI that will listen to the first VPI's
 * sends and receives.
 */
int vpi_add_sendrecv_listener(vpi_t vpi, vpi_t listener);

/**
 * @brief Create a VPI and add it as a listener for the given VPI's sends
 * and receives.
 * @param vpi The VPI on which to register as a listener.
 * @param create_spec The create spec for the new listener VPI.
 */
int vpi_add_sendrecv_listener_spec(vpi_t vpi, const char* create_spec);


/**
 * @brief Remove a VPI as a listener for the given  VPI's sends and receives.
 * @param vpi The VPI on which to unregister the listener.
 * @param listener The VPI to unregister.
 */
int vpi_remove_sendrecv_listener(vpi_t vpi, vpi_t listener);

/**
 * @brief Remove the VPI that has the give create_spec as a listener for
 * the given VPI's sends and receives.
 * @param vpi The VPI on which to unregister the listener.
 * @param create_spec The specification for the VPI to unregister.
 */
int vpi_remove_sendrecv_listener_spec(vpi_t vpi, const char* create_spec);

/**
 * @brief Get the current count of all sendrecv listeners on the given VPI.
 * @param vpi The VPI object.
 * @returns The total number of VPI instances that are currently registered
 * as send and receive listeners on the given VPI.
 */
int vpi_sendrecv_listener_count(vpi_t vpi);

/**
 * @brief Drop all sendreceive listeners.
 * @param vpi The VPI object.
 */
int vpi_sendrecv_listeners_drop(vpi_t vpi);

/**
 * @brief Show a VPI.
 * @param vpi The VPI object.
 * @param pvs The output pvs.
 */
void vpi_show(vpi_t vpi, aim_pvs_t* pvs);

#if VPI_CONFIG_INCLUDE_BRIDGING == 1

/** Handle to a VPI bridge instance. */
typedef struct vpi_bridge_s* vpi_bridge_t;

/**
 * @brief Create a VPI bridge between the two given VPI instances.
 * @param v1 One side of the bridge.
 * @param v2 The other side of the bridge.
 */
vpi_bridge_t vpi_bridge_create(vpi_t v1, vpi_t v2);

/**
 * @brief Start bridging packets.
 * @param br The bridge.
 */
int vpi_bridge_start(vpi_bridge_t br);

/**
 * @brief Stop bridging packets.
 * @param br The bridge.
 */
int vpi_bridge_stop(vpi_bridge_t br);

/**
 * @brief Destroy a VPI bridge.
 * @param br The bridge to destroy.
 */
int vpi_bridge_destroy(vpi_bridge_t br);

#endif

#endif /* __VPI_H__ */
/*@}*/

