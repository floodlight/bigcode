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

#include <PPE/ppe_config.h>
#include <PPE/ppe.h>
#include "ppe_int.h"
#include "ppe_log.h"
#include <AIM/aim_datatypes.h>

AIM_LOG_STRUCT_DEFINE(
                      PPE_CONFIG_LOG_OPTIONS_DEFAULT,
                      PPE_CONFIG_LOG_BITS_DEFAULT,
                      ppe_log_flag_map,     /* Custom Log Map */
                      PPE_CONFIG_LOG_CUSTOM_BITS_DEFAULT
                      );




