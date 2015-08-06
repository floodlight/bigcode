/************************************************************
 * <bsn.cl fy=2015 v=onl>
 * 
 *           Copyright 2015 Big Switch Networks, Inc.          
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
#include <snmp_subagent/snmp_subagent_config.h>
#include <snmp_subagent/snmp_subagent_snmplog.h>

#include <AIM/aim_rl.h>

#include "snmp_subagent_int.h"

/**
 * SNMP Log messages map to AIM log levels.
 */
static aim_ratelimiter_t limiter__;

int
snmp_subagent_snmplog_init(void)
{
    aim_ratelimiter_init(&limiter__, 1000*1000, 5, NULL);
    return 0;
}

int
snmp_subagent_snmplog_callback(int major, int minor,
                               void *serverarg, void *clientarg)
{
    /*
     * FIXME: This implements the same mapping from syslog level as
     * aim_log_syslog_level_map(), should be merged
     */
    struct snmp_log_message *slm = (struct snmp_log_message *) serverarg;

    if (slm->priority >= LOG_DEBUG)
        AIM_LOG_RL_TRACE(&limiter__,    0, "%s", slm->msg);
    else if (slm->priority >= LOG_INFO)
        AIM_LOG_RL_VERBOSE(&limiter__,  0, "%s", slm->msg);
    else if (slm->priority >= LOG_NOTICE)
        AIM_LOG_RL_INTERNAL(&limiter__, 0, "notice: %s", slm->msg);
    else if (slm->priority >= LOG_WARNING)
        AIM_LOG_RL_INTERNAL(&limiter__, 0, "warn: %s", slm->msg);
    else if (slm->priority >= LOG_ALERT)
        AIM_LOG_RL_INTERNAL(&limiter__, 0, "alert: %s", slm->msg);
    else
        AIM_LOG_RL_INFO(&limiter__,     0, "other: %s", slm->msg);

    return SNMP_ERR_NOERROR;
}



