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
#include <cjson_util/cjson_util_config.h>
#include <cjson_util/cjson_util.h>
#include <AIM/aim_map.h>
#include <AIM/aim_error.h>
#include <AIM/aim_memory.h>
#include "cjson_util_log.h"
#include <limits.h>
#include <math.h>
#include <float.h>
#include <errno.h>
/*
 * Given the start of a string containing newlines and another pointer
 * inside that string, return the line and column number of that position.
 *
 * Line and column numbering starts at 1. Tab width is assumed to be 8.
 */
static void
find_line_number__(const char *start, const char *pos, int *line, int *col)
{
    const char *cur = start;
    const int tabwidth = 8;
    *line = 1;
    *col = 1;

    while (cur < pos) {
        if (*cur == '\n') {
            (*line)++;
            *col = 1;
        } else if (*cur == '\t') {
            (*col) += tabwidth - ((*col - 1) % tabwidth);
        } else {
            (*col)++;
        }
        cur++;
    }
}

int
cjson_util_parse_file(const char* filename, cJSON** result)
{
    FILE *f;
    long len;
    char *data;
    cJSON *root;

    f = fopen(filename, "r");
    if (f == NULL) {
        return AIM_ERROR_NOT_FOUND;
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = aim_zmalloc(len + 1);
    if (fread(data, 1, len, f) == 0) {
        aim_free(data);
        if (feof(f)) {
            AIM_LOG_ERROR("%s: end of file", filename);
        }
        if (ferror(f)) {
            AIM_LOG_ERROR("%s: %{errno}", filename, errno);
        }
        fclose(f);
        AIM_LOG_ERROR("failed to read %s", filename);
        return AIM_ERROR_INTERNAL;
    }
    data[len] = 0;
    fclose(f);

    root = cJSON_Parse(data);
    if (!root) {
        int line, col;
        char *next_newline;
        const char *errptr = cJSON_GetErrorPtr();
        find_line_number__(data, errptr, &line, &col);

        /* Terminate the snippet before the next newline */
        next_newline = strchr(errptr, '\n');
        if (next_newline != NULL) {
            *next_newline = '\0';
        }

        AIM_LOG_ERROR("%s: Error at line %d col %d: [%.8s]\n", filename,
                      line, col, errptr);
        aim_free(data);
        return AIM_ERROR_PARAM;
    }

    aim_free(data);
    if(result) {
        *result = root;
    }
    else {
        cJSON_Delete(root);
    }
    return 0;
}

static int
cjson_util_lookup_path__(cJSON *root, cJSON** result, char *path)
{
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

    if(node->type == cJSON_Number) {
        if (node->valueint != node->valuedouble) {
            return AIM_ERROR_PARAM;
        }
        *result = node->valueint;
        rv = 0;
    }
    else if(node->type == cJSON_String) {
        /*
         * Special case -- allow hexadecimal-as-a-string when expecting
         * an integer.
         * The json format does not support hexadecimal number formats,
         * which sometimes makes things tedious.
         */
        rv = AIM_ERROR_PARAM;
        if(node->valuestring && node->valuestring[0] == '0') {
            if(node->valuestring[1] == 'x' || node->valuestring[1] == 'X') {
                if(CJSON_UTIL_SSCANF(node->valuestring+2, "%x", result) == 1) {
                    rv = 0;
                }
            }
        }
    }
    return rv;
}

int
cjson_util_vlookup_int_default(cJSON *root, int def, const char* fmt,
                               va_list vargs)
{
    int rv;
    if(cjson_util_vlookup_int(root, &rv, fmt, vargs) < 0) {
        rv = def;
    }
    return rv;
}

int
cjson_util_lookup_int_default(cJSON *root, int def, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_int_default(root, def, fmt, vargs);
    va_end(vargs);
    return rv;
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

char*
cjson_util_vlookup_string_default(cJSON* root, char* def, const char* fmt,
                                  va_list vargs)
{
    char* rv;
    if(cjson_util_vlookup_string(root, &rv, fmt, vargs) < 0) {
        rv = def;
    }
    return rv;
}

char*
cjson_util_lookup_string_default(cJSON* root, char* def, const char* fmt, ...)
{
    char* rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_string_default(root, def, fmt, vargs);
    va_end(vargs);
    return rv;
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

double
cjson_util_vlookup_double_default(cJSON* root, double def, const char* fmt,
                                  va_list vargs)
{
    double rv;
    if(cjson_util_vlookup_double(root, &rv, fmt, vargs) < 0) {
        rv = def;
    }
    return rv;
}

double
cjson_util_lookup_double_default(cJSON* root, double def, const char* fmt, ...)
{
    double rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_double_default(root, def, fmt, vargs);
    va_end(vargs);
    return rv;
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
cjson_util_vlookup_bool_default(cJSON* root, int def, const char* fmt,
                                va_list vargs)
{
    int rv;
    if(cjson_util_vlookup_bool(root, &rv, fmt, vargs) < 0) {
        rv = def;
    }
    return rv;
}

int
cjson_util_lookup_bool_default(cJSON* root, int def, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_bool_default(root, def, fmt, vargs);
    va_end(vargs);
    return rv;
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

int
cjson_util_vlookup_svalue(cJSON *root, char** result, const char* fmt, va_list vargs)
{
    cJSON* node;
    int rv;

    rv = cjson_util_vlookup(root, &node, fmt, vargs);

    if(rv < 0) {
        return rv;
    }
    switch(node->type)
        {
        case cJSON_String:
            *result = aim_strdup(node->valuestring);
            break;
        case cJSON_True:
            *result = aim_strdup("true");
            break;
        case cJSON_False:
            *result = aim_strdup("false");
            break;
        case cJSON_Number:
            {
                /** Same handling as https://github.com/floodlight/bigcode/blob/master/modules/cjson/module/src/cJSON.c#L119 */
                double d = node->valuedouble;
                if (fabs(((double)node->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN) {
                    *result = aim_fstrdup("%d", node->valueint);
                }
                else {
                    if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60) {
                        *result = aim_fstrdup("%.0f", d);
                    }
                    else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9) {
                        *result = aim_fstrdup("%e", d);
                    } else {
                        *result = aim_fstrdup("%f", d);
                    }
                }
                break;
            }
        default:
            return AIM_ERROR_PARAM;
        }

    return 0;
}

int
cjson_util_lookup_svalue(cJSON *root, char** result, const char *fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_vlookup_svalue(root, result, fmt, vargs);
    va_end(vargs);
    return rv;
}
