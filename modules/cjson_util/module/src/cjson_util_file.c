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
 ************************************************************
 *
 * cJSON Access Utilities
 *
 ***********************************************************/
#include <cjson_util/cjson_util_file.h>

#include <AIM/aim_map.h>
#include <AIM/aim_error.h>
#include <AIM/aim_memory.h>
#include "cjson_util_log.h"
#include <cjson_util/cjson_util.h>

static uint64_t
mtime__(const char* fname)
{
    struct stat st;
    if(stat(fname, &st) == 0) {
        return (uint64_t) st.st_mtim.tv_sec * 1000*1000*1000
            + st.st_mtim.tv_nsec;
    }
    else {
        return 0;
    }
}

void
cjson_util_file_close(cjson_util_file_t* jfs)
{
    if(jfs) {
        if(jfs->filename) {
            aim_free(jfs->filename);
        }
        if(jfs->root) {
            cJSON_Delete(jfs->root);
        }
        if(jfs->defaultj) {
            aim_free(jfs->defaultj);
        }
        memset(jfs, 0, sizeof(*jfs));
    }
}

static int reload__(cjson_util_file_t* jfs)
{
    int rv = -1;

    if(jfs->root) {
        cJSON_Delete(jfs->root);
        jfs->root = NULL;
    }

    if(jfs->filename) {
        rv = cjson_util_parse_file(jfs->filename, &jfs->root);

        if(rv < 0 || jfs->root == NULL) {
            /** Missing/Error */
            switch(rv)
                {
                case AIM_ERROR_NOT_FOUND:
                    {
                        if(jfs->defaultj) {
                            /* Load default value */
                            jfs->root = cJSON_Parse(jfs->defaultj);
                            if(jfs->root == NULL) {
                                AIM_LOG_ERROR("Default JSON data for '%s' does not parse.", jfs->filename);
                            }
                            else {
                                jfs->mtime = 0;
                                rv = 0;
                            }
                        }
                        else {
                            AIM_LOG_ERROR("Required JSON file '%s' does not exist.", jfs->filename);
                        }
                        break;
                    }
                case AIM_ERROR_INTERNAL:
                    {
                        AIM_LOG_ERROR("'%s' could not be read.", jfs->filename);
                        break;
                    }
                case AIM_ERROR_PARAM:
                    {
                        AIM_LOG_ERROR("'%s' could not be parsed.", jfs->filename);
                        break;
                    }
                default:
                    {
                        AIM_LOG_ERROR("error %d file loading '%s'", jfs->filename);
                        break;
                    }
                }
        }
        else {
            jfs->mtime = mtime__(jfs->filename);
        }
    }
    return rv;
}

int
cjson_util_file_open(const char* fname, cjson_util_file_t* jfs,
                     const char* defaultj)
{
    cjson_util_file_close(jfs);
    jfs->filename = aim_strdup(fname);
    if(defaultj) {
        jfs->defaultj = aim_strdup(defaultj);
    }
    return reload__(jfs);
}

int
cjson_util_file_reload(cjson_util_file_t* jfs, int force)
{
    uint64_t mtime = mtime__(jfs->filename);
    if( (mtime != jfs->mtime) || force ) {
        int rv = reload__(jfs);
        return (rv < 0) ? rv : 1;
    }
    return 0;
}
