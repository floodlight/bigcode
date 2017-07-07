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
 ********************************************************//**
 *
 * cJSON Access Utilities
 *
 ***********************************************************/
#ifndef __CJSON_UTIL_H__
#define __CJSON_UTIL_H__

#include <stddef.h>
#include <stdarg.h>
#include <cjson/cJSON.h>


/**
 * @brief Load a json configuration file.
 * @param filename The filename.
 * @param result Receives the root node.
 * @returns AIM_ERROR_NOT_FOUND if the file is missing
 * @returns AIM_ERROR_INTERNAL if the file could not be read.
 * @reutrns AIM_ERROR_PARAM if the file could not be parsed.
 */

int cjson_util_parse_file(const char* filename, cJSON** result);

/**
 * @brief Lookup an object node.
 * @param root The root of the tree.
 * @param rv Receives the node if successful.
 * @param fmt Path format string.
 * @param ... Path format args.
 */
int cjson_util_lookup(cJSON* root, cJSON** rv, const char* fmt, ...);

/**
 * @brief Lookup an object node.
 * @param root The root of the tree.
 * @param rv Recevies the node if successful.
 * @param fmt Path format string.
 * @param vargs Format args.
 */
int cjson_util_vlookup(cJSON* root, cJSON** rv, const char* fmt, va_list vargs);

/**
 * @brief Lookup an integer value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fmt The path format string.
 * @param ... The path format args.
 */
int cjson_util_lookup_int(cJSON *root, int* result, const char* fmt, ...);

/**
 * @brief Lookup an integer value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fmt The path format string.
 * @param vargs The path format args.
 */
int cjson_util_vlookup_int(cJSON *root, int* result, const char* fmt,
                           va_list vargs);

/**
 * @brief Lookup an integer value with default.
 * @param root The root of the tree.
 * @param def The default value.
 * @param fmt The path format string.
 * @param ... The path format args;
 */
int cjson_util_lookup_int_default(cJSON* root, int def, const char* fmt, ...);

/**
 * @brief Lookup an integer value with default.
 * @param root The root of the tree.
 * @param def The default value.
 * @param fmt The path format string.
 * @param vargs The path format args;
 */
int cjson_util_vlookup_int_default(cJSON* root, int def, const char* fmt,
                                   va_list vargs);


/**
 * @brief Lookup a string value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fms The path format string.
 * @param ... The path format args.
 */
int cjson_util_lookup_string(cJSON* root, char** result, const char* fmt, ...);


/**
 * @brief Lookup a string value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fms The path format string.
 * @param vargs The path format args.
 */
int cjson_util_vlookup_string(cJSON* root, char** result, const char* fmt,
                              va_list vargs);

/**
 * @brief Lookup a string value.
 * @param root The root of the tree.
 * @param def The default value.
 * @param fms The path format string.
 * @param ... The path format args.
 */
char* cjson_util_lookup_string_default(cJSON* root, char* def, const char* fmt, ...);


/**
 * @brief Lookup a string value.
 * @param root The root of the tree.
 * @param def The default value.
 * @param fms The path format string.
 * @param vargs The path format args.
 */
char* cjson_util_vlookup_string_default(cJSON* root, char* def, const char* fmt,
                                        va_list vargs);


/**
 * @brief Lookup a double value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fmt The path format string.
 * @param ... The path format args.
 */
int cjson_util_lookup_double(cJSON *root, double* result, const char* fmt, ...);


/**
 * @brief Lookup a double value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fmt The path format string.
 * @param ... The path format args.
 */
int cjson_util_vlookup_double(cJSON *root, double* result, const char* fmt,
                              va_list vargs);

/**
 * @brief Lookup a double value.
 * @param root The root of the tree.
 * @param result Receives the result.
 * @param fmt The path format string.
 * @param ... The path format args.
 */
double cjson_util_lookup_double_default(cJSON *root, double def,
                                        const char* fmt, ...);


/**
 * @brief Lookup a double value.
 * @param root The root of the tree.
 * @param def The default.
 * @param fmt The path format string.
 * @param vargs The path format args.
 */
double cjson_util_vlookup_double_default(cJSON *root, double def, const char* fmt,
                                 va_list vargs);


/**
 * @brief Lookup a boolean value.
 * @param root The root of the tree.
 * @param result The result.
 * @param fmt The path format string.
 * @param ... The path format args.
 */
int cjson_util_lookup_bool(cJSON *root, int *result, const char *fmt, ...);


/**
 * @brief Lookup a boolean value.
 * @param root The root of the tree.
 * @param result The result.
 * @param fmt The path format string.
 * @param vargs The path format args.
 */
int cjson_util_vlookup_bool(cJSON *root, int *result, const char *fmt,
                            va_list vargs);

/**
 * @brief Lookup a boolean value.
 * @param root The root of the tree.
 * @param def The default.
 * @param fmt The path format string.
 * @param ... The path format args.
 */
int cjson_util_lookup_bool_default(cJSON *root, int def, const char *fmt, ...);


/**
 * @brief Lookup a boolean value.
 * @param root The root of the tree.
 * @param def The default.
 * @param fmt The path format string.
 * @param vargs The path format args.
 */
int cjson_util_vlookup_bool_default(cJSON *root, int def, const char *fmt,
                                    va_list vargs);

/**
 * @brief Lookup a value as a string.
 * @param root The root of the tree.
 * @param result The result.
 * @param fmt The path format string.
 * @param vargs The path format args.
 * @notes
 * Key values are converted to strings as follows:
 *     string -- the string is returned.
 *     bool   -- the string "true" or "false" is returned.
 *     number -- a best effort string representation of the integer or double.
 *
 * The result must be freed.
 */
int cjson_util_vlookup_svalue(cJSON *root, char** result, const char* fmt, va_list vargs);

/**
 * @brief Lookup a value as a string.
 * @param root The root of the tree.
 * @param result The result.
 * @param fmt The path format string.
 * @param ... The path format args.
 * @notes See cjson_util_vlookup_svalue()
 */
int cjson_util_lookup_svalue(cJSON *root, char** result, const char *fmt, ...);


#endif /* __CJSON_UTIL_H__ */
