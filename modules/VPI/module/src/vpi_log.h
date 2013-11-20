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

#ifndef __VPI_LOG_H__
#define __VPI_LOG_H__

#define AIM_LOG_MODULE_NAME vpi
#define AIM_LOG_OBJ_DEFAULT
#include <AIM/aim_log.h>


#define VPI_MINFO AIM_LOG_MOD_INFO
#define VPI_MERROR AIM_LOG_MOD_ERROR
#define VPI_MTRACE AIM_LOG_MOD_TRACE
#define VPI_MWARN AIM_LOG_MOD_WARN
#define VPI_MINTERNAL AIM_LOG_MOD_INTERNAL

#define VPI_INFO AIM_LOG_INFO
#define VPI_ERROR AIM_LOG_ERROR
#define VPI_WARN AIM_LOG_WARN
#define VPI_TRACE AIM_LOG_OBJ_TRACE

#define VPI_FENTER AIM_LOG_FENTER
#define VPI_FEXIT AIM_LOG_FEXIT

/* <auto.start.aim_custom_log_macro(ALL).header> */

/******************************************************************************
 *
 * Custom Module Log Macros
 *
 *****************************************************************************/

/** Log a module-level create */
#define VPI_LOG_MOD_CREATE(...) \
    AIM_LOG_MOD_CUSTOM(VPI_LOG_FLAG_CREATE, "CREATE", __VA_ARGS__)
/** Log a module-level create with ratelimiting */
#define VPI_LOG_MOD_RL_CREATE(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(VPI_LOG_FLAG_CREATE, "CREATE", _rl, _time, __VA_ARGS__)

/** Log a module-level send */
#define VPI_LOG_MOD_SEND(...) \
    AIM_LOG_MOD_CUSTOM(VPI_LOG_FLAG_SEND, "SEND", __VA_ARGS__)
/** Log a module-level send with ratelimiting */
#define VPI_LOG_MOD_RL_SEND(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(VPI_LOG_FLAG_SEND, "SEND", _rl, _time, __VA_ARGS__)

/** Log a module-level recv */
#define VPI_LOG_MOD_RECV(...) \
    AIM_LOG_MOD_CUSTOM(VPI_LOG_FLAG_RECV, "RECV", __VA_ARGS__)
/** Log a module-level recv with ratelimiting */
#define VPI_LOG_MOD_RL_RECV(_rl, _time, ...)           \
    AIM_LOG_MOD_RL_CUSTOM(VPI_LOG_FLAG_RECV, "RECV", _rl, _time, __VA_ARGS__)

/******************************************************************************
 *
 * Custom Object Log Macros
 *
 *****************************************************************************/

/** Log an object-level create */
#define VPI_LOG_OBJ_CREATE(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, VPI_LOG_FLAG_CREATE, "CREATE", __VA_ARGS__)
/** Log an object-level create with ratelimiting */
#define VPI_LOG_OBJ_RL_CREATE(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, VPI_LOG_FLAG_CREATE, "CREATE", _rl, _time, __VA_ARGS__)

/** Log an object-level send */
#define VPI_LOG_OBJ_SEND(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, VPI_LOG_FLAG_SEND, "SEND", __VA_ARGS__)
/** Log an object-level send with ratelimiting */
#define VPI_LOG_OBJ_RL_SEND(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, VPI_LOG_FLAG_SEND, "SEND", _rl, _time, __VA_ARGS__)

/** Log an object-level recv */
#define VPI_LOG_OBJ_RECV(_obj, ...) \
    AIM_LOG_OBJ_CUSTOM(_obj, VPI_LOG_FLAG_RECV, "RECV", __VA_ARGS__)
/** Log an object-level recv with ratelimiting */
#define VPI_LOG_OBJ_RL_RECV(_obj, _rl, _time, ...) \
    AIM_LOG_OBJ_RL_CUSTOM(_obj, VPI_LOG_FLAG_RECV, "RECV", _rl, _time, __VA_ARGS__)

/******************************************************************************
 *
 * Default Macro Mappings
 *
 *****************************************************************************/
#ifdef AIM_LOG_OBJ_DEFAULT

/** CREATE -> OBJ_CREATE */
#define VPI_LOG_CREATE VPI_LOG_OBJ_CREATE
/** RL_CREATE -> OBJ_RL_CREATE */
#define VPI_LOG_RL_CREATE VPI_LOG_RL_OBJ_CREATE


/** SEND -> OBJ_SEND */
#define VPI_LOG_SEND VPI_LOG_OBJ_SEND
/** RL_SEND -> OBJ_RL_SEND */
#define VPI_LOG_RL_SEND VPI_LOG_RL_OBJ_SEND


/** RECV -> OBJ_RECV */
#define VPI_LOG_RECV VPI_LOG_OBJ_RECV
/** RL_RECV -> OBJ_RL_RECV */
#define VPI_LOG_RL_RECV VPI_LOG_RL_OBJ_RECV


#else

/** CREATE -> MOD_CREATE */
#define VPI_LOG_CREATE VPI_LOG_MOD_CREATE
/** RL_CREATE -> MOD_RL_CREATE */
#define VPI_LOG_RL_CREATE VPI_LOG_MOD_RL_CREATE

/** SEND -> MOD_SEND */
#define VPI_LOG_SEND VPI_LOG_MOD_SEND
/** RL_SEND -> MOD_RL_SEND */
#define VPI_LOG_RL_SEND VPI_LOG_MOD_RL_SEND

/** RECV -> MOD_RECV */
#define VPI_LOG_RECV VPI_LOG_MOD_RECV
/** RL_RECV -> MOD_RL_RECV */
#define VPI_LOG_RL_RECV VPI_LOG_MOD_RL_RECV

#endif
/* <auto.end.aim_custom_log_macro(ALL).header> */

#endif /* __VPI_LOG_H__ */
