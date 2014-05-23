/************************************************************
 * <bsn.cl fy=2014 v=epl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
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
#include <cjson_util/cjson_util.h>
#include <AIM/aim_map.h>
#include <AIM/aim_error.h>
#include <AIM/aim_memory.h>

static int
cjson_util_lookup_path__(cJSON *root, cJSON** result, const char *path_)
{
    /* strtok_r mutates its input, need to copy */
    char *path = strdup(path_);
    char *saveptr = NULL;
    char *token;
    cJSON *node = root;
    int err = 0;

    while ((err >= 0) &&
           (token = strtok_r(node == root ? path : NULL, ".", &saveptr)) != NULL) {
        if (node->type != cJSON_Object) {
            err = AIM_ERROR_PARAM;
        } else if ((node = cJSON_GetObjectItem(node, token)) == NULL) {
            err = AIM_ERROR_NOT_FOUND;
        }
    }

    free(path);
    *result = err == 0 ? node : NULL;
    return err;
}

static int
vsnprintf_size__(const char* fmt, va_list vargs)
{
    int rv;
    va_list vacpy;
    va_copy(vacpy, vargs);
    rv = vsnprintf(NULL, 0, fmt, vacpy);
    va_end(vacpy);
    return rv;
}

int
cjson_util_vlookup(cJSON* root, cJSON** result, const char* fmt, va_list vargs)
{
    int rv = AIM_ERROR_NOT_FOUND;

    if(root) {
        int size = vsnprintf_size__(fmt, vargs);
        char* path = aim_zmalloc(size+1);
        vsnprintf(path, size+1, fmt, vargs);
        rv = cjson_util_lookup_path__(root, result, path);
        aim_free(path);
    }

    return rv;
}

int
cjson_util_lookup(cJSON* root, cJSON** result, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup(root, result, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
cjson_util_vlookup_int(cJSON *root, int* result, const char* fmt,
                           va_list vargs)
{
    cJSON *node;
    int rv;

    rv = cjson_util_vlookup(root, &node, fmt, vargs);
    if(rv < 0) {
        return rv;
    }

    if (node->type != cJSON_Number) {
        return AIM_ERROR_PARAM;
    }

    if (node->valueint != node->valuedouble) {
        return AIM_ERROR_PARAM;
    }

    *result = node->valueint;

    return 0;
}


int
cjson_util_lookup_int(cJSON *root, int* result, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_int(root, result, fmt, vargs);
    va_end(vargs);
    return rv;

}

int
cjson_util_vlookup_string(cJSON* root, char** result, const char* fmt,
                          va_list vargs)
{
    cJSON *node;
    int rv;

    rv = cjson_util_vlookup(root, &node, fmt, vargs);
    if(rv < 0) {
        return rv;
    }

    if(node->type != cJSON_String) {
        return AIM_ERROR_PARAM;
    }

    *result = node->valuestring;
    return 0;
}


int
cjson_util_lookup_string(cJSON* root, char** result, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_string(root, result, fmt, vargs);
    va_end(vargs);
    return rv;
}


int
cjson_util_vlookup_double(cJSON *root, double* result, const char* fmt,
                          va_list vargs)
{
    cJSON *node;
    int rv;

    rv = cjson_util_vlookup(root, &node, fmt, vargs);
    if(rv < 0) {
        return rv;
    }

    if (node->type != cJSON_Number) {
        return AIM_ERROR_PARAM;
    }

    *result = node->valuedouble;

    return 0;
}



int
cjson_util_lookup_double(cJSON *root, double* result, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_double(root, result, fmt, vargs);
    va_end(vargs);
    return rv;
}


int
cjson_util_vlookup_bool(cJSON *root, int* result, const char* fmt,
                        va_list vargs)
{
    cJSON *node;
    int rv;

    rv = cjson_util_vlookup(root, &node, fmt, vargs);
    if(rv < 0) {
        return rv;
    }

    if(node->type == cJSON_True) {
        *result = 1;
    }
    else if(node->type == cJSON_False) {
        *result = 0;
    }
    else {
        return AIM_ERROR_PARAM;
    }

    return 0;
}


int
cjson_util_lookup_bool(cJSON *root, int* result, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_bool(root, result, fmt, vargs);
    va_end(vargs);
    return rv;

}

