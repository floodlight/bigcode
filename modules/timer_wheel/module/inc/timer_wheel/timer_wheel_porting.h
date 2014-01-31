/**************************************************************************//**
 *
 * @file
 * @brief timer_wheel Porting Macros.
 *
 * @addtogroup timer_wheel-porting
 * @{
 *
 *****************************************************************************/
#ifndef __TIMER_WHEEL_PORTING_H__
#define __TIMER_WHEEL_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if TIMER_WHEEL_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef TIMER_WHEEL_MALLOC
    #if defined(GLOBAL_MALLOC)
        #define TIMER_WHEEL_MALLOC GLOBAL_MALLOC
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_MALLOC malloc
    #else
        #error The macro TIMER_WHEEL_MALLOC is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_FREE
    #if defined(GLOBAL_FREE)
        #define TIMER_WHEEL_FREE GLOBAL_FREE
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_FREE free
    #else
        #error The macro TIMER_WHEEL_FREE is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define TIMER_WHEEL_MEMSET GLOBAL_MEMSET
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_MEMSET memset
    #else
        #error The macro TIMER_WHEEL_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define TIMER_WHEEL_MEMCPY GLOBAL_MEMCPY
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_MEMCPY memcpy
    #else
        #error The macro TIMER_WHEEL_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define TIMER_WHEEL_STRNCPY GLOBAL_STRNCPY
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_STRNCPY strncpy
    #else
        #error The macro TIMER_WHEEL_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_VSNPRINTF
    #if defined(GLOBAL_VSNPRINTF)
        #define TIMER_WHEEL_VSNPRINTF GLOBAL_VSNPRINTF
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_VSNPRINTF vsnprintf
    #else
        #error The macro TIMER_WHEEL_VSNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define TIMER_WHEEL_SNPRINTF GLOBAL_SNPRINTF
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_SNPRINTF snprintf
    #else
        #error The macro TIMER_WHEEL_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef TIMER_WHEEL_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define TIMER_WHEEL_STRLEN GLOBAL_STRLEN
    #elif TIMER_WHEEL_CONFIG_PORTING_STDLIB == 1
        #define TIMER_WHEEL_STRLEN strlen
    #else
        #error The macro TIMER_WHEEL_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __TIMER_WHEEL_PORTING_H__ */
/* @} */
