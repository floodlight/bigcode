/************************************************************
 * <bsn.cl fy=2017 v=epl>
 *
 *           Copyright 2017 Big Switch Networks, Inc.
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
 * </bsn.cl>
 ********************************************************//**
 *
 * cJSON Formatting Utilities
 *
 ***********************************************************/
#ifndef __CJSON_UTIL_FORMAT_H__
#define __CJSON_UTIL_FORMAT_H__

#include <cjson_util/cjson_util_config.h>
#include <AIM/aim_pvs.h>
#include <cjson/cJSON.h>

/**
 * @brief Output JSON to YAML
 * @param pvs The output pvs.
 * @param root The CJSON root.
 */
int cjson_util_yaml_pvs(aim_pvs_t* pvs, cJSON* root);

/**
 * @brief Convert to YAML
 * @param root The CJSON root.
 */
char* cjson_util_yaml_str(cJSON* root);

/**
 * @brief Output JSON
 * @param pvs The output pvs.
 * @param root The CJSON root.
 */
int cjson_util_json_pvs(aim_pvs_t* pvs, cJSON* root);

/**
 * @brief Convert to JSON string.
 * @param The CJSON root.
 */
char* cjson_util_json_str(cJSON* root);

/**
 * @brief Create a flagmap JSON object.
 * @param flags The flags
 * @param map The simap
 */

cJSON* cjson_util_flags(uint32_t flags, aim_map_si_t* map);

/**
 * @brief Create a string array containing true flags.
 */
cJSON* cjson_util_flag_array(uint32_t flags, aim_map_si_t* map);

/**
 * @brief Convert a string array to flags.
 */
int cjson_util_array_to_flags(cJSON* cj, uint32_t* flagsp, aim_map_si_t* map);

/**
 * @brief Add a string with formatting to an object.
 * @param item The object.
 * @param name The name.
 * @param fmt
 * @param ... The format arguments.
 */
int cjson_util_add_string_to_object(cJSON* item, const char* name, const char* fmt, ...);
int cjson_util_add_vstring_to_object(cJSON* item, const char* name, const char* fmt, va_list vargs);

#endif /* __CJSON_UTIL_FORMAT_H__ */
