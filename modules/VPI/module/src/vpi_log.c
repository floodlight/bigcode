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

#include <VPI/vpi_config.h>
#include "vpi_log.h"
#include <VPI/vpi.h>

extern aim_map_si_t vpi_log_flag_map[];

AIM_LOG_STRUCT_DEFINE(
                      VPI_CONFIG_LOG_OPTIONS_DEFAULT,
                      VPI_CONFIG_LOG_BITS_DEFAULT,
                      vpi_log_flag_map,         /* Custom Log Map */
                      VPI_CONFIG_LOG_CUSTOM_BITS_DEFAULT
                      );


