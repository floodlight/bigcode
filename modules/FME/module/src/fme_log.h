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

#ifndef __FME_LOG_H__
#define __FME_LOG_H__

#include <FME/fme_config.h>

#define AIM_LOG_MODULE_NAME fme
#include <AIM/aim_log.h>


/**
 * Custom FME log types.
 */

/* <auto.start.enum(fme_log_flag).header> */
/** fme_log_flag */
typedef enum fme_log_flag_e {
    FME_LOG_FLAG_MATCH,
    FME_LOG_FLAG_TIMEOUT,
} fme_log_flag_t;

/** Enum names. */
const char* fme_log_flag_name(fme_log_flag_t e);

/** Enum values. */
int fme_log_flag_value(const char* str, fme_log_flag_t* e, int substr);

/** Enum descriptions. */
const char* fme_log_flag_desc(fme_log_flag_t e);

/** Enum validator. */
int fme_log_flag_valid(fme_log_flag_t e);

/** validator */
#define FME_LOG_FLAG_VALID(_e) \
    (fme_log_flag_valid((_e)))

/** fme_log_flag_map table. */
extern aim_map_si_t fme_log_flag_map[];
/** fme_log_flag_desc_map table. */
extern aim_map_si_t fme_log_flag_desc_map[];
/* <auto.end.enum(fme_log_flag).header> */

/* <auto.start.aim_custom_log_macro(ALL).header> */

/******************************************************************************
 *
 * Custom Module Log Macros
 *
 *****************************************************************************/

/** Log a module-level match */
#define FME_LOG_MOD_MATCH(...) \
    AIM_LOG_MOD_CUSTOM(FME_LOG_FLAG_MATCH, "MATCH", __VA_ARGS__)
/** Log a module-level match with ratelimiting */
#define FME_LOG_MOD_RL_MATCH(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(FME_LOG_FLAG_MATCH, "MATCH", _rl, _time, __VA_ARGS__)

/** Log a module-level timeout */
#define FME_LOG_MOD_TIMEOUT(...) \
    AIM_LOG_MOD_CUSTOM(FME_LOG_FLAG_TIMEOUT, "TIMEOUT", __VA_ARGS__)
/** Log a module-level timeout with ratelimiting */
#define FME_LOG_MOD_RL_TIMEOUT(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(FME_LOG_FLAG_TIMEOUT, "TIMEOUT", _rl, _time, __VA_ARGS__)

/******************************************************************************
 *
 * Custom Object Log Macros
 *
 *****************************************************************************/

/** Log an object-level match */
#define FME_LOG_OBJ_MATCH(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, FME_LOG_FLAG_MATCH, "MATCH", __VA_ARGS__)
/** Log an object-level match with ratelimiting */
#define FME_LOG_OBJ_RL_MATCH(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, FME_LOG_FLAG_MATCH, "MATCH", _rl, _time, __VA_ARGS__)

/** Log an object-level timeout */
#define FME_LOG_OBJ_TIMEOUT(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, FME_LOG_FLAG_TIMEOUT, "TIMEOUT", __VA_ARGS__)
/** Log an object-level timeout with ratelimiting */
#define FME_LOG_OBJ_RL_TIMEOUT(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, FME_LOG_FLAG_TIMEOUT, "TIMEOUT", _rl, _time, __VA_ARGS__)

/******************************************************************************
 *
 * Default Macro Mappings
 *
 *****************************************************************************/
#ifdef AIM_LOG_OBJ_DEFAULT

/** MATCH -> OBJ_MATCH */
#define FME_LOG_MATCH FME_LOG_OBJ_MATCH
/** RL_MATCH -> OBJ_RL_MATCH */
#define FME_LOG_RL_MATCH FME_LOG_RL_OBJ_MATCH


/** TIMEOUT -> OBJ_TIMEOUT */
#define FME_LOG_TIMEOUT FME_LOG_OBJ_TIMEOUT
/** RL_TIMEOUT -> OBJ_RL_TIMEOUT */
#define FME_LOG_RL_TIMEOUT FME_LOG_RL_OBJ_TIMEOUT


#else

/** MATCH -> MOD_MATCH */
#define FME_LOG_MATCH FME_LOG_MOD_MATCH
/** RL_MATCH -> MOD_RL_MATCH */
#define FME_LOG_RL_MATCH FME_LOG_MOD_RL_MATCH

/** TIMEOUT -> MOD_TIMEOUT */
#define FME_LOG_TIMEOUT FME_LOG_MOD_TIMEOUT
/** RL_TIMEOUT -> MOD_RL_TIMEOUT */
#define FME_LOG_RL_TIMEOUT FME_LOG_MOD_RL_TIMEOUT

#endif
/* <auto.end.aim_custom_log_macro(ALL).header> */

#endif /* __FME_LOG_H__ */
