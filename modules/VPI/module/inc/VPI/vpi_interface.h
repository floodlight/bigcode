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
 * @brief Definition of the VPI Interface driver.
 *
 * @details
 * This file defines the interface you must implement
 * to plug your communication channel into the VPI infrastructure.
 *
 * @addtogroup vpi-interface
 * @{
 *
 ***************************************************************/
#ifndef __VPI_INTERFACE_H__
#define __VPI_INTERFACE_H__

#include <BigList/biglist.h>

/** vpi interface */
typedef struct vpi_interface_s vpi_interface_t;


/**
 * This structure represents the VPI interface driver.
 * At a minimum you must define and implement the definitions
 * in this structure.
 */
struct vpi_interface_s {

    /** Parent VPI instance for this interface */
    vpi_t parent;

    /** The interface supports the VPI protocol */
#define VPI_INTERFACE_FLAG_PROTOCOL                0x1
    /** The packet is a result of a RECV listen */
#define VPI_INTERFACE_FLAG_RECV_LISTENING          0x2
    /** The packet is a result of a SEND listen */
#define VPI_INTERFACE_FLAG_SEND_LISTENING          0x4
    /** Interface flags */
    uint32_t flags;


    /** The receive specification for this interface, if applicable */
    char recv_spec[VPI_CONFIG_CREATE_SPEC_MAX];

    /** The send specification for this interface, if applicable */
    char send_spec[VPI_CONFIG_CREATE_SPEC_MAX];

    /**
     * The VPI specification required to send to this endpoint.
     * Typically the reverse of the recv_spec.
     */
    char send_to_spec[VPI_CONFIG_CREATE_SPEC_MAX];

    /**
     * Send a packet on this interface.
     */
    int (*send)(vpi_interface_t* vi, unsigned char* data, int len);

    /**
     * Recv a packet on this interface (blocking)
     */
    int (*recv)(vpi_interface_t* vi, unsigned char* data, int len);

    /**
     * Returns whether Recv is ready or not (would not block)
     */
    int (*recv_ready)(vpi_interface_t* vi);


    /**
     * Returns the descriptor for this channel, if applicable.
     * Used for external calls to select().
     *
     * If the VPI channel does not have a descriptor then
     * returning < 0 for an error and == 0 for no-descriptor
     * is acceptable.
     */
    int (*descriptor)(vpi_interface_t* vi);


    /** Destroy this interface. */
    int (*destroy)(vpi_interface_t* vi);

    /** Filter create specs for this interface dynamically [Optional] */
    char* (*filter_create_spec)(vpi_interface_t* vi, const char* src_spec);

    /** Cookie [Optional] */
    void* cookie;

};


/**
 * The function signature for interface creators.
 */
typedef int (*vpi_interface_create_f)(vpi_interface_t** vi,
                                      char* args[],
                                      int flags,
                                      const char* create_spec);

/**
 * The create_spec preprocessor for interface creators.
 */
typedef char* (*vpi_interface_preprocess_f)(char* args[],
                                            const char* create_spec);

/**
 * @brief Register your interface with the VPI core.
 * @param type The name of your VPI interface type.
 * @param creator The creator function for your type.
 * @param preprocessor The preprocess for your type.
 * @param doc_string The help string for using your interface.
 *
 * @note (creator || preprocessor) != False
 */
int
vpi_interface_register(const char* type,
                       vpi_interface_create_f creator,
                       vpi_interface_preprocess_f preprocessor,
                       const char* doc_string);

/**
 * @brief Unregister an interface.
 * @param type The type to unregister.
 */
int
vpi_interface_unregister(const char* type);


#endif /* __VPI_INTERFACE_H__ */
/*@}*/
