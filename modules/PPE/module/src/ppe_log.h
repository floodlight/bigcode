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

#ifndef __PPE_LOG_H__
#define __PPE_LOG_H__

#include <PPE/ppe_config.h>

#define AIM_LOG_MODULE_NAME ppe
#include <AIM/aim_log.h>

extern aim_map_si_t ppe_log_flag_map[];

/* <auto.start.aim_custom_log_macro(ALL).header> */

/******************************************************************************
 *
 * Custom Module Log Macros
 *
 *****************************************************************************/

/** Log a module-level parse */
#define PPE_LOG_MOD_PARSE(...) \
    AIM_LOG_MOD_CUSTOM(PPE_LOG_FLAG_PARSE, "PARSE", __VA_ARGS__)
/** Log a module-level parse with ratelimiting */
#define PPE_LOG_MOD_RL_PARSE(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(PPE_LOG_FLAG_PARSE, "PARSE", _rl, _time, __VA_ARGS__)

/** Log a module-level format */
#define PPE_LOG_MOD_FORMAT(...) \
    AIM_LOG_MOD_CUSTOM(PPE_LOG_FLAG_FORMAT, "FORMAT", __VA_ARGS__)
/** Log a module-level format with ratelimiting */
#define PPE_LOG_MOD_RL_FORMAT(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(PPE_LOG_FLAG_FORMAT, "FORMAT", _rl, _time, __VA_ARGS__)

/******************************************************************************
 *
 * Custom Object Log Macros
 *
 *****************************************************************************/

/** Log an object-level parse */
#define PPE_LOG_OBJ_PARSE(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, PPE_LOG_FLAG_PARSE, "PARSE", __VA_ARGS__)
/** Log an object-level parse with ratelimiting */
#define PPE_LOG_OBJ_RL_PARSE(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, PPE_LOG_FLAG_PARSE, "PARSE", _rl, _time, __VA_ARGS__)

/** Log an object-level format */
#define PPE_LOG_OBJ_FORMAT(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, PPE_LOG_FLAG_FORMAT, "FORMAT", __VA_ARGS__)
/** Log an object-level format with ratelimiting */
#define PPE_LOG_OBJ_RL_FORMAT(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, PPE_LOG_FLAG_FORMAT, "FORMAT", _rl, _time, __VA_ARGS__)

/******************************************************************************
 *
 * Default Macro Mappings
 *
 *****************************************************************************/
#ifdef AIM_LOG_OBJ_DEFAULT

/** PARSE -> OBJ_PARSE */
#define PPE_LOG_PARSE PPE_LOG_OBJ_PARSE
/** RL_PARSE -> OBJ_RL_PARSE */
#define PPE_LOG_RL_PARSE PPE_LOG_RL_OBJ_PARSE


/** FORMAT -> OBJ_FORMAT */
#define PPE_LOG_FORMAT PPE_LOG_OBJ_FORMAT
/** RL_FORMAT -> OBJ_RL_FORMAT */
#define PPE_LOG_RL_FORMAT PPE_LOG_RL_OBJ_FORMAT


#else

/** PARSE -> MOD_PARSE */
#define PPE_LOG_PARSE PPE_LOG_MOD_PARSE
/** RL_PARSE -> MOD_RL_PARSE */
#define PPE_LOG_RL_PARSE PPE_LOG_MOD_RL_PARSE

/** FORMAT -> MOD_FORMAT */
#define PPE_LOG_FORMAT PPE_LOG_MOD_FORMAT
/** RL_FORMAT -> MOD_RL_FORMAT */
#define PPE_LOG_RL_FORMAT PPE_LOG_MOD_RL_FORMAT

#endif
/* <auto.end.aim_custom_log_macro(ALL).header> */

#endif /* __PPE_LOG_H__ */
