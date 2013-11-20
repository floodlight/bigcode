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

#include <OS/os_config.h>
#include <OS/os.h>
#include "os_int.h"
#include "os_log.h"

AIM_LOG_STRUCT_DEFINE(
                      OS_CONFIG_LOG_OPTIONS_DEFAULT,
                      OS_CONFIG_LOG_BITS_DEFAULT,
                      NULL, /* custom map */
                      OS_CONFIG_LOG_CUSTOM_BITS_DEFAULT
                      );
