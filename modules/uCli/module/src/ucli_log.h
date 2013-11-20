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

#ifndef __UCLI_LOG_H__
#define __UCLI_LOG_H__


#include <uCli/ucli_config.h>
#define AIM_LOG_MODULE_NAME ucli
#include <AIM/aim_log.h>

#define UCLI_MERROR AIM_LOG_MOD_ERROR
#define UCLI_MINTERNAL AIM_LOG_MOD_INTERNAL
#define UCLI_MWARN AIM_LOG_MOD_WARN

#define UCLI_ERROR AIM_LOG_ERROR

#endif /* __UCLI_LOG_H__ */
