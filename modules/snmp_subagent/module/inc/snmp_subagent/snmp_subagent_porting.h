/**************************************************************************//**
 *
 * @file
 * @brief snmp_subagent Porting Macros.
 *
 * @addtogroup snmp_subagent-porting
 * @{
 *
 *****************************************************************************/
#ifndef __SNMP_SUBAGENT_PORTING_H__
#define __SNMP_SUBAGENT_PORTING_H__


/* <auto.start.portingmacro(ALL).define> */
#if SNMP_SUBAGENT_CONFIG_PORTING_INCLUDE_STDLIB_HEADERS == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <memory.h>
#endif

#ifndef SNMP_SUBAGENT_MALLOC
    #if defined(GLOBAL_MALLOC)
        #define SNMP_SUBAGENT_MALLOC GLOBAL_MALLOC
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_MALLOC malloc
    #else
        #error The macro SNMP_SUBAGENT_MALLOC is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_FREE
    #if defined(GLOBAL_FREE)
        #define SNMP_SUBAGENT_FREE GLOBAL_FREE
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_FREE free
    #else
        #error The macro SNMP_SUBAGENT_FREE is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_MEMSET
    #if defined(GLOBAL_MEMSET)
        #define SNMP_SUBAGENT_MEMSET GLOBAL_MEMSET
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_MEMSET memset
    #else
        #error The macro SNMP_SUBAGENT_MEMSET is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_MEMCPY
    #if defined(GLOBAL_MEMCPY)
        #define SNMP_SUBAGENT_MEMCPY GLOBAL_MEMCPY
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_MEMCPY memcpy
    #else
        #error The macro SNMP_SUBAGENT_MEMCPY is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_STRNCPY
    #if defined(GLOBAL_STRNCPY)
        #define SNMP_SUBAGENT_STRNCPY GLOBAL_STRNCPY
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_STRNCPY strncpy
    #else
        #error The macro SNMP_SUBAGENT_STRNCPY is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_VSNPRINTF
    #if defined(GLOBAL_VSNPRINTF)
        #define SNMP_SUBAGENT_VSNPRINTF GLOBAL_VSNPRINTF
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_VSNPRINTF vsnprintf
    #else
        #error The macro SNMP_SUBAGENT_VSNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_SNPRINTF
    #if defined(GLOBAL_SNPRINTF)
        #define SNMP_SUBAGENT_SNPRINTF GLOBAL_SNPRINTF
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_SNPRINTF snprintf
    #else
        #error The macro SNMP_SUBAGENT_SNPRINTF is required but cannot be defined.
    #endif
#endif

#ifndef SNMP_SUBAGENT_STRLEN
    #if defined(GLOBAL_STRLEN)
        #define SNMP_SUBAGENT_STRLEN GLOBAL_STRLEN
    #elif SNMP_SUBAGENT_CONFIG_PORTING_STDLIB == 1
        #define SNMP_SUBAGENT_STRLEN strlen
    #else
        #error The macro SNMP_SUBAGENT_STRLEN is required but cannot be defined.
    #endif
#endif

/* <auto.end.portingmacro(ALL).define> */


#endif /* __SNMP_SUBAGENT_PORTING_H__ */
/* @} */
