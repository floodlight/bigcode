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
 **********************************************************/

#include <cjson_util/cjson_util_format.h>
#include <IOF/iof.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "cjson_util_log.h"
#include <AIM/aim_pvs_buffer.h>

static int format_yaml_type__(iof_t* iof, cJSON* root);

static void
format_yaml_string__(iof_t* iof, cJSON* item)
{
    iof_iprintf(iof, "%s%s%s",
                item->string ? item->string : "",
                item->string ? ": " : "",
                item->valuestring);
}

static void
format_yaml_number__(iof_t* iof, cJSON* item)
{
    double d = item->valuedouble;
    if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN) {
        iof_iprintf(iof, "%s%s%d",
                    item->string ? item->string : "",
                    item->string ? ": " : "",
                    item->valueint);
    }
    else {
        if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60) {
            iof_iprintf(iof, "%s%s%.0f",
                        item->string ? item->string : "",
                        item->string ? ": " : "",
                        d);
        }
        else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9) {
            iof_iprintf(iof, "%s%s%e",
                        item->string ? item->string : "",
                        item->string ? ": " : "",
                        d);
        } else {
            iof_iprintf(iof, "%s%s%f",
                        item->string ? item->string : "",
                        item->string ? ": " : "",
                        d);
        }
    }
}

static void
format_yaml_object__(iof_t* iof, cJSON* item)
{
    cJSON* p;

    if(item->string) {
        iof_push(iof, "%s", item->string);
    }

    for(p = item->child; p; p = p->next) {
        format_yaml_type__(iof, p);
    }

    if(item->string) {
        iof_pop(iof);
    }
}

static void
format_yaml_array__(iof_t* iof, cJSON* item)
{
    cJSON* p;

    if(item->string) {
        iof_push(iof, "%s", item->string);
    }

    for(p = item->child; p; p = p -> next) {
        iof_iprintfn(iof, "- ");
        format_yaml_type__(iof, p);
    }

    if(item->string) {
        iof_pop(iof);
    }
}

static void
format_yaml_immediate__(iof_t* iof, cJSON* item, const char* s)
{
    iof_iprintf(iof, "%s%s%s",
                item->string ? item->string : "",
                item->string ? ": " : "",
                s);
}

int
format_yaml_type__(iof_t* iof, cJSON* root)
{
    int rv = 0;

    if(root == NULL) {
        return 0;
    }

    switch((root->type) & 0xFF)
        {
        case cJSON_False:
            format_yaml_immediate__(iof, root, "False");
            break;
        case cJSON_True:
            format_yaml_immediate__(iof, root, "True");
            break;
        case cJSON_NULL:
            format_yaml_immediate__(iof, root, "None");
            break;
        case cJSON_String:
            format_yaml_string__(iof, root);
            break;
        case cJSON_Number:
            format_yaml_number__(iof, root);
            break;
        case cJSON_Object:
            format_yaml_object__(iof, root);
            break;
        case cJSON_Array:
            format_yaml_array__(iof, root);
            break;

        default:
            AIM_LOG_ERROR("type not handled. (%d)", root->type & 0xFF);
            rv = -1;
        }
    return rv;
}

int
cjson_util_yaml_pvs(aim_pvs_t* pvs, cJSON* root)
{
    iof_t iof;

    /** Initialize IOF for YAML formatting */
    iof_init(&iof, pvs);
    iof.push_string = ":";
    iof.pop_string = NULL;
    iof.indent_factor = 2;
    return format_yaml_type__(&iof, root);
}
char*
cjson_util_yaml_str(cJSON* root)
{
    char* rv;
    aim_pvs_t* pvs = aim_pvs_buffer_create();
    cjson_util_yaml_pvs(pvs,  root);
    rv = aim_pvs_buffer_get(pvs);
    aim_pvs_destroy(pvs);
    return rv;
}

int
cjson_util_json_pvs(aim_pvs_t* pvs, cJSON* root)
{
    char* str = cjson_util_json_str(root);
    aim_printf(pvs, "%s\n", str);
    aim_free(str);
    return 0;
}

char*
cjson_util_json_str(cJSON* root)
{
    return cJSON_Print(root);
}

cJSON*
cjson_util_flags(uint32_t flags, aim_map_si_t* map)
{
    aim_map_si_t* p;
    cJSON* rv = cJSON_CreateObject();
    for(p = map; p->s; p++) {
        cJSON_AddBoolToObject(rv, p->s, (flags & (p->i)));
    }
    return rv;
}

cJSON*
cjson_util_flag_array(uint32_t flags, aim_map_si_t* map)
{
    aim_map_si_t* p;
    cJSON* rv = cJSON_CreateArray();
    for(p = map; p->s; p++) {
        if(flags & (p->i)) {
            cJSON_AddItemToArray(rv, cJSON_CreateString(p->s));
        }
    }
    return rv;
}

int
cjson_util_array_to_flags(cJSON* cj, uint32_t* flagsp, aim_map_si_t* map)
{
    if(flagsp == NULL || cj == NULL) {
        return -1;
    }
    *flagsp = 0;

    int s = cJSON_GetArraySize(cj);
    if(s < 0) {
        return -1;
    }

    int i;
    for(i = 0; i < s; i++) {
        cJSON* item = cJSON_GetArrayItem(cj, i);
        if(item->type != cJSON_String) {
            return -1;
        }
        int v;
        if(aim_map_si_s(&v, item->valuestring, map, 0) < 0) {
            return -1;
        }
        *flagsp |= v;
    }

    return 0;
}

int
cjson_util_add_string_to_object(cJSON* item, const char* name,
                                const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = cjson_util_add_vstring_to_object(item, name, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
cjson_util_add_vstring_to_object(cJSON* item, const char* name,
                                 const char* fmt, va_list vargs)
{
    char* str = aim_vdfstrdup(fmt, vargs);
    cJSON_AddStringToObject(item, name, str);
    aim_free(str);
    return 0;
}
