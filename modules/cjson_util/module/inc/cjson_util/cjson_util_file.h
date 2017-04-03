/************************************************************
 * <bsn.cl fy=2016 v=epl>
 *
 *           Copyright 2016 Big Switch Networks, Inc.
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
 * cJSON File Management
 *
 ***********************************************************/
#ifndef __CJSON_UTIL_FILE_H__
#define __CJSON_UTIL_FILE_H__

#include <cjson_util/cjson_util_config.h>

#include <stddef.h>
#include <stdarg.h>
#include <cjson/cJSON.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Control Structure for JSON file management.
 */
typedef struct cjson_util_file_s {
    /** public */
    cJSON* root;
    char* filename;

    /** private */
    uint64_t mtime;
    char* defaultj;
} cjson_util_file_t;

/**
 * @brief Load a JSON file handle.
 * @param filename The filename
 * @param jfs [out] Receives the handle.
 * @param def Default JSON value if the file does not exist.
 */
int cjson_util_file_open(const char* filename, cjson_util_file_t* jfs, const char* defaultj);

/**
 * @brief Close a JSON file handle.
 * @param jfs The handle to close.
 */
void cjson_util_file_close(cjson_util_file_t* jfs);

/**
 * @brief Reload operation on a JSON file.
 * @param jfs The handle
 * @param force Force reload.
 * @returns 1 if reloaded, 0 if not reloaded.
 * @note The file will be reloaded if it has changed since the last time
 * it was loaded, or if the reload is forced.
 */
int cjson_util_file_reload(cjson_util_file_t* jfs, int force);

#endif /* __CJSON_UTIL_FILE_H__ */
