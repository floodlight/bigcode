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
 * @brief Definitions for the PPE Unit Test module.
 *
 * @details
 * The PPE module provides a builtin Unit Test infrastructure
 * as a set of uCli commands.
 *
 * @addtogroup ppe-utm
 * @{
 *
 *
 ****************************************************************/

#ifndef __PPE_UTM_H__
#define __PPE_UTM_H__

#include <PPE/ppe_config.h>

#if PPE_CONFIG_INCLUDE_UTM == 1

#include <uCli/ucli.h>
#include <PPE/ppe.h>

/**************************************************************************//**
 *
 * This is the context structure for all PPE uCli test commands.
 *
 * This is publicly exported to allow modules which use PPE to
 * use these structures in their own CLI commands or to extend
 * the commands beyond those provided by this library.
 *
 *****************************************************************************/

typedef struct ppe_utm_ctrl_s {
    ppe_packet_t ppep;
} ppe_utm_ctrl_t;


/**
 * Get the PPE ucli test module.
 */
ucli_module_t*
ppe_utm_create(void);


#endif /* PPE_CONFIG_INCLUDE_UCLI */

#endif /* __PPE_UTM_H__ */
/*@}*/

