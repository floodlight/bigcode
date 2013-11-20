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
/****************************************************************
 *
 * @file
 * @brief FME Unit Test Module
 *
 * @addtogroup FME
 * @{
 *
 *
 ***************************************************************/
#ifndef __FME_UTM_H__
#define __FME_UTM_H__

#include <FME/fme_config.h>

#if FME_CONFIG_INCLUDE_UTM == 1

#include <uCli/ucli.h>
#include <FME/fme.h>

/**************************************************************************//**
 *
 * This is the context structure for all FME uCli test commands.
 *
 * This is publicly exported to allow modules which use FME to
 * use these structures in their own CLI commands or to extend
 * the commands beyond those provided by this library.
 *
 *****************************************************************************/

#ifndef FME_CONFIG_UTM_ENTRIES
#define FME_CONFIG_UTM_ENTRIES 16
#endif

typedef struct fme_utm_ctrl_s {
    fme_t* fme;
    fme_entry_t* entries[FME_CONFIG_UTM_ENTRIES];
    int expects[FME_CONFIG_UTM_ENTRIES];
    int eid;
} fme_utm_ctrl_t;


/**
 * Get the FME ucli test module.
 */
ucli_module_t*
fme_utm_create(void);


#endif /* FME_CONFIG_INCLUDE_UTM */

#endif /* __FME_UTM_H__ */
/*@}*/
