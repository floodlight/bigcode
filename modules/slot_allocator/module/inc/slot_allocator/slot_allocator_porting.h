/**************************************************************************//**
 *
 * @file
 * @brief slot_allocator Porting Macros.
 *
 * @addtogroup slot_allocator-porting
 * @{
 *
 *****************************************************************************/
#ifndef __INDEX_ALLOCATOR_PORTING_H__
#define __INDEX_ALLOCATOR_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if INDEX_ALLOCATOR_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef INDEX_ALLOCATOR_MALLOC
    #if defined(GLOBAL_MALLOC)
        #define INDEX_ALLOCATOR_MALLOC GLOBAL_MALLOC
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_MALLOC malloc
    #else
        #error The macro INDEX_ALLOCATOR_MALLOC is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_FREE
    #if defined(GLOBAL_FREE)
        #define INDEX_ALLOCATOR_FREE GLOBAL_FREE
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_FREE free
    #else
        #error The macro INDEX_ALLOCATOR_FREE is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define INDEX_ALLOCATOR_MEMSET GLOBAL_MEMSET
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_MEMSET memset
    #else
        #error The macro INDEX_ALLOCATOR_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define INDEX_ALLOCATOR_MEMCPY GLOBAL_MEMCPY
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_MEMCPY memcpy
    #else
        #error The macro INDEX_ALLOCATOR_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define INDEX_ALLOCATOR_STRNCPY GLOBAL_STRNCPY
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_STRNCPY strncpy
    #else
        #error The macro INDEX_ALLOCATOR_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_VSNPRINTF
    #if defined(GLOBAL_VSNPRINTF)
        #define INDEX_ALLOCATOR_VSNPRINTF GLOBAL_VSNPRINTF
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_VSNPRINTF vsnprintf
    #else
        #error The macro INDEX_ALLOCATOR_VSNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define INDEX_ALLOCATOR_SNPRINTF GLOBAL_SNPRINTF
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_SNPRINTF snprintf
    #else
        #error The macro INDEX_ALLOCATOR_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef INDEX_ALLOCATOR_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define INDEX_ALLOCATOR_STRLEN GLOBAL_STRLEN
    #elif INDEX_ALLOCATOR_CONFIG_PORTING_STDLIB == 1
        #define INDEX_ALLOCATOR_STRLEN strlen
    #else
        #error The macro INDEX_ALLOCATOR_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __INDEX_ALLOCATOR_PORTING_H__ */
/* @} */
