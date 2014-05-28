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
 *
 *
 ***********************************************************/

#include <cjson_util/cjson_util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

#define TRY(_expr)                                   \
    do {                                             \
        int _rv = _expr;                             \
        if(_rv < 0) {                                \
            AIM_DIE("%s: failed %d\n", #_expr, _rv); \
        }                                            \
        else {                                       \
            printf("%s: ok\n", #_expr);              \
        }                                            \
    } while(0)

int aim_main(int argc, char* argv[])
{
    int iv;
    double dv;
    char* sv;
    int bv;

    const char* json =
        "{"
        "  \"data\": {"
        "    \"int\": {"
        "      \"i1\": 1, "
        "      \"bad\": \"notanint\", "
        "      \"i2\": 2, "
        "      \"more\": { "
        "        \"i3\": 3, "
        "        \"i4\": 4, "
        "        \"hex\": \"0xDEADf00d\","
        "        \"hexbad\": \"DEADBEEF\""
        "        }"
        "     }, "
        " \"doubles\": {"
        "    \"pi\": 3.14159"
        "   }, "
        " \"strings\": {"
        "    \"alpha\": {"
        "      \"bravo\": {"
        "        \"charlie\": {"
        "          \"delta\": \"foxtrot\" "
        "          }"
        "         }"
        "        }"
        "       }, "
        " \"bools\": {"
        "    \"bad\": 1, "
        "    \"t\": true, "
        "    \"f\": false "
        "      }"
        "    }"
        "}";

    cJSON* root = cJSON_Parse(json);
    int jlen = strlen(json)+1 ;
    if(root == NULL) {
        printf("The Unit test JSON input data cannot be parsed:\n");
        printf("%s\n", json);
        exit(1);
    }
    printf("Test Data:\n");
    char* jstr = cJSON_Print(root);
    printf("%s\n", jstr);
    free(jstr);
    cJSON_Delete(root);
    root = NULL;

    /* Data is validated, write it to a temp file and reload it */
    FILE* fp;
    char fname[] = "/tmp/cjson_util_utest.XXXXXX";

    AIM_ASSERT(cjson_util_parse_file(fname, &root) == AIM_ERROR_NOT_FOUND);

    AIM_ASSERT(fp = fopen(mktemp(fname), "w"));
    AIM_ASSERT(fwrite(json, 1, jlen-10, fp) == jlen-10);
    fclose(fp);

    AIM_ASSERT(cjson_util_parse_file(fname, &root) == AIM_ERROR_PARAM);

    AIM_ASSERT(fp = fopen(fname, "w"));
    AIM_ASSERT(fwrite(json, 1, jlen, fp) == jlen);
    fclose(fp);

    TRY(cjson_util_parse_file(fname, &root));

    TRY(cjson_util_lookup_int(root, &iv, "data.int.i1"));
    AIM_ASSERT(iv == 1);
    TRY(cjson_util_lookup_int(root, &iv, "data.%s.i%c", "int", '1'));
    AIM_ASSERT(iv == 1);
    TRY(cjson_util_lookup_int(root, &iv, "%s.int.i2", "data"));
    AIM_ASSERT(iv == 2);
    AIM_ASSERT(cjson_util_lookup_int(root, &iv, "data.int.bad") == AIM_ERROR_PARAM);
    TRY(cjson_util_lookup_int(root, &iv, "data.int.more.i%d", 4));
    AIM_ASSERT(iv == 4);
    TRY(cjson_util_lookup_bool(root, &bv, "data.bools.%c", 't'));
    AIM_ASSERT(bv == 1);
    TRY(cjson_util_lookup_bool(root, &bv, "data.bools.%c", 'f'));
    AIM_ASSERT(bv == 0);
    AIM_ASSERT(cjson_util_lookup_bool(root, &iv, "data.int.bad") == AIM_ERROR_PARAM);
    TRY(cjson_util_lookup_double(root, &dv, "data.doubles.pi"));
    AIM_ASSERT(dv >= 3.141 && dv <= 3.15);
    TRY(cjson_util_lookup_string(root, &sv, "data.strings.alpha.bravo.charlie.delta"));
    AIM_ASSERT(!strcmp(sv, "foxtrot"));
    TRY(cjson_util_lookup_string(root, &sv, "data.int.more.hex"));
    AIM_ASSERT(!strcmp(sv, "0xDEADf00d"));
    TRY(cjson_util_lookup_int(root, &iv, "data.int.more.hex"));
    AIM_ASSERT(iv == 0xdeadf00d);
    TRY(cjson_util_lookup_string(root, &sv, "data.int.more.hexbad"));
    AIM_ASSERT(cjson_util_lookup_int(root, &iv, "data.int.more.hexbad") == AIM_ERROR_PARAM);
    cJSON_Delete(root);
    return 0;
}

