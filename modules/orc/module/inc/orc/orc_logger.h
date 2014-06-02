/************************************************************
 * <bsn.cl fy=2013 v=epl>
 * 
 *        Copyright 2013, 2014 Big Switch Networks, Inc.       
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

/*********
 * Collection of logging utilities
 * TODO Come back and make this more useful
 * TODO Give option to write to syslog
 */

#ifndef ORC_LOG_H
#define ORC_LOG_H

#include <stdlib.h>  /* for exit() */
#include <string.h>

enum Debug_Thresholds {
        ORC_LOG_TRACE,
        ORC_LOG_DEBUG,
        ORC_LOG_INFO,
        ORC_LOG_WARN,
        ORC_LOG_ERR,
        ORC_LOG_FATAL,
        ORC_LOG_MAX
};

extern enum Debug_Thresholds DEBUG_THRESHOLD;


#include <errno.h>
#include <stdio.h>

#define __SFILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define _orc_log(level, p, x...) do { \
                if (DEBUG_THRESHOLD <= level) { \
                        fprintf(stderr, p": %20s:%-4u:: ", __SFILE__, __LINE__); \
                        fprintf(stderr, x); \
                } \
        } while(0);

#define orc_trace(x...) _orc_log(ORC_LOG_TRACE, "TRC",x)
#define orc_debug(x...) _orc_log(ORC_LOG_DEBUG, "DBG",x)
#define orc_info(x...) _orc_log(ORC_LOG_INFO, "LOG",x)
#define orc_log(x...) _orc_log(ORC_LOG_INFO, "LOG",x)
#define orc_warn(x...) _orc_log(ORC_LOG_INFO, "WRN",x)
#define orc_err(x...) _orc_log(ORC_LOG_ERR, "ERR",x)

#define orc_fatal(x...) do { \
        fprintf(stderr, "FATAL_ERR: %s:%u ::", __FILE__, __LINE__); \
        fprintf(stderr, x); \
        exit(-1); \
        } while(0);


#define orc_abort_errno(x, y...) do { \
        orc_err(y); \
        perror(x); \
        abort(); \
        } while(0);


/** return a string value of the current debug level */
const char * orc_log_threshold();
const char * orc_log_threshold_level();


#endif
