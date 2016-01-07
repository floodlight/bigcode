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

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


static void
_subagent_help(const char* name, const char* desc, FILE* fp)
{
    fprintf(fp, "NAME\n");
    fprintf(fp, "        %s - %s\n\n", name, desc);
    fprintf(fp, "SYNOPSIS\n\n");
    fprintf(fp, "        %s [-dr|d] [-pid file] [-h | --help]\n", name);
    fprintf(fp, "OPTIONS\n");
    fprintf(fp, "        -d            Daemonize.\n");
    fprintf(fp, "        -dr           Daemonize with automatic restart.\n");
    fprintf(fp, "        -pid <file>   Write PID to the given filename.\n");
    fprintf(fp, "\n");
    fprintf(fp, "        -h            This help message.\n");
    fprintf(fp, "        --help\n");
    fprintf(fp, "\n");
    fprintf(fp, "        -x             Enable debugging.\n");
    fprintf(fp, "\n");
}


int
snmp_subagent_main(int argc, char* argv[])
{
    char** arg;
    char* pidfile = NULL;
    int daemonize = 0;
    int restart = 0;
    int debugging = 0;

    aim_pvs_t* aim_pvs_syslog = NULL;

    for(arg = argv+1; *arg; arg++) {
        if(!strcmp(*arg, "-dr")) {
            daemonize = 1;
            restart = 1;
        }
        else if(!strcmp(*arg, "-d")) {
            daemonize = 1;
            restart = 0;
        }
        else if(!strcmp(*arg, "-pid")) {
            arg++;
            pidfile = *arg;
            if(!pidfile) {
                fprintf(stderr, "-pid requires an argument.\n");
                exit(1);
            }
        }
        else if(!strcmp(*arg, "-h") || !strcmp(*arg, "--help")) {
            _subagent_help(SNMP_SUBAGENT_CONFIG_AGENT_NAME, SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION, stdout);
            exit(0);
        }
        else if (!strcmp(*arg, "-x")) {
            debugging = 1;
        }
        else {
            _subagent_help(SNMP_SUBAGENT_CONFIG_AGENT_NAME, SNMP_SUBAGENT_CONFIG_AGENT_DESCRIPTION, stderr);
            exit(1);
        }
    }

    aim_pvs_syslog = aim_pvs_syslog_open(SNMP_SUBAGENT_CONFIG_AGENT_NAME, LOG_PID, LOG_DAEMON);

    if(daemonize) {
        aim_daemon_restart_config_t rconfig;
        aim_daemon_config_t config;

        memset(&config, 0, sizeof(config));
        aim_daemon_restart_config_init(&rconfig, 1, 1);
        AIM_BITMAP_CLR(&rconfig.signal_restarts, SIGTERM);
        AIM_BITMAP_CLR(&rconfig.exit_restarts, 0);
        rconfig.maximum_restarts=50;
        rconfig.pvs = aim_pvs_syslog_get();
        config.wd = "/";

        aim_daemonize(&config, (restart) ? &rconfig : NULL);
    }

    if(pidfile) {
        FILE* fp = fopen(pidfile, "w");
        if(fp == NULL) {
            int e = errno;
            aim_printf(aim_pvs_syslog, "fatal: open(%s): %s\n",
                       pidfile, strerror(e));
            aim_printf(&aim_pvs_stderr, "fatal: open(%s): %s\n",
                       pidfile, strerror(e));

            /* Don't attempt restart */
            raise(SIGTERM);
        }
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    }

    snmp_subagent_init();
    snmp_subagent_module_clients_register();
    snmp_subagent_start(SNMP_SUBAGENT_CONFIG_AGENT_NAME, 1, debugging);
    snmp_subagent_module_clients_unregister();
    return 0;
}

#endif /* SNMP_SUBAGENT_CONFIG_INCLUDE_MAIN */

