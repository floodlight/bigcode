/**************************************************************************//**
 *
 * @file
 * @brief debug_counter Porting Macros.
 *
 * @addtogroup debug_counter-porting
 * @{
 *
 *****************************************************************************/
#ifndef __DEBUG_COUNTER_PORTING_H__
#define __DEBUG_COUNTER_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if DEBUG_COUNTER_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef DEBUG_COUNTER_MALLOC
    #if defined(GLOBAL_MALLOC)
        #define DEBUG_COUNTER_MALLOC GLOBAL_MALLOC
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_MALLOC malloc
    #else
        #error The macro DEBUG_COUNTER_MALLOC is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_FREE
    #if defined(GLOBAL_FREE)
        #define DEBUG_COUNTER_FREE GLOBAL_FREE
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_FREE free
    #else
        #error The macro DEBUG_COUNTER_FREE is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define DEBUG_COUNTER_MEMSET GLOBAL_MEMSET
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_MEMSET memset
    #else
        #error The macro DEBUG_COUNTER_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define DEBUG_COUNTER_MEMCPY GLOBAL_MEMCPY
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_MEMCPY memcpy
    #else
        #error The macro DEBUG_COUNTER_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define DEBUG_COUNTER_STRNCPY GLOBAL_STRNCPY
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_STRNCPY strncpy
    #else
        #error The macro DEBUG_COUNTER_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_VSNPRINTF
    #if defined(GLOBAL_VSNPRINTF)
        #define DEBUG_COUNTER_VSNPRINTF GLOBAL_VSNPRINTF
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_VSNPRINTF vsnprintf
    #else
        #error The macro DEBUG_COUNTER_VSNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define DEBUG_COUNTER_SNPRINTF GLOBAL_SNPRINTF
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_SNPRINTF snprintf
    #else
        #error The macro DEBUG_COUNTER_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef DEBUG_COUNTER_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define DEBUG_COUNTER_STRLEN GLOBAL_STRLEN
    #elif DEBUG_COUNTER_CONFIG_PORTING_STDLIB == 1
        #define DEBUG_COUNTER_STRLEN strlen
    #else
        #error The macro DEBUG_COUNTER_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __DEBUG_COUNTER_PORTING_H__ */
/* @} */
