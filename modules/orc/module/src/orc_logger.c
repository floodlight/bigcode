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

#include <assert.h>
#include "orc/orc_logger.h"

char * Debug_Names[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERR",
    "FATAL",
    "OUT_OF_RANGE"
};


enum Debug_Thresholds DEBUG_THRESHOLD = ORC_LOG_TRACE;


const char * orc_log_threshold() {
    return Debug_Names[DEBUG_THRESHOLD];
}

const char * orc_log_threshold_level(enum Debug_Thresholds level) {
    assert(level>=0);
    assert(level< ORC_LOG_MAX);
    return Debug_Names[level];
}
