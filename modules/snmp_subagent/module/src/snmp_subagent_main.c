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

#if SNMP_SUBAGENT_CONFIG_INCLUDE_MAIN == 1

#include <AIM/aim_daemon.h>
#include <AIM/aim_pvs_syslog.h>
#include <snmp_subagent/snmp_subagent.h>
#include <snmp_subagent/snmp_subagent_main.h>

#include "snmp_subagent_log.h"

#include <unistd.h>

int
snmp_subagent_main(int argc, char* argv[])
{
    /**
     * Fixme/Todo - argument parsing.
     */
#if AIM_CONFIG_INCLUDE_DAEMONIZE == 1
    if(getenv("DAEMON")) {
        /* Daemonize */
        aim_daemon_config_t config;
        memset(&config, 0, sizeof(config));
        config.stdlog = aim_fstrdup("/var/run/%s/%s/std.log", SNMP_SUBAGENT_CONFIG_AGENT_NAME);
        config.wd = "/";
        aim_daemonize(&config, NULL);
#if AIM_CONFIG_INCLUDE_PVS_SYSLOG == 1
        aim_log_pvs_set(AIM_LOG_STRUCT_POINTER, aim_pvs_syslog_get());
#endif
    }
#endif

    if(getenv("PID")) {
        FILE* fp;
        const char* pidfile = getenv("PID");
        if ((fp = fopen(pidfile, "w")) == NULL) {
            AIM_LOG_ERROR("Exit: can't open pid file %s", pidfile);
            exit(1);
        }
        fprintf(fp, "%d\n", (int) getpid());
        fclose(fp);
    }

    int debugging = 0;
    if(getenv("DEBUG")) {
        debugging = 1;
    }

    snmp_subagent_init();
    snmp_subagent_module_clients_register();
    snmp_subagent_start(SNMP_SUBAGENT_CONFIG_AGENT_NAME, 1, debugging);
    snmp_subagent_module_clients_unregister();
    return 0;
}

#endif /* SNMP_SUBAGENT_CONFIG_INCLUDE_MAIN */

