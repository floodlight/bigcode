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
#include <snmp_subagent/snmp_subagent_clients.h>
#include "snmp_subagent_int.h"

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/eventfd.h>


/** Thread control. */
typedef struct snmp_subagent_thread_ctrl_s {
    volatile int run;
    const char* name;
    int failed;
    int debugging;
    int eventfd;  /* eventfd for aborting check_and_process */
} snmp_subagent_thread_ctrl_t;

static pthread_t subagent_thread_handle__;
static int       subagent_thread_running__ = 0;
static volatile snmp_subagent_thread_ctrl_t ctrl__;


/**
 * Send notification to stop worker thread.
 */
static void
notify_stop(void)
{
    uint64_t one = 1;
    if (write(ctrl__.eventfd, &one, sizeof(one)) < 0) {
        /* silence warn_unused_result */
    }
}


/**
 * Signal handlers which terminal the worker thread.
 */
static RETSIGTYPE
sig_stop_handler__(int a) {
    ctrl__.run = 0;
    notify_stop();
}


/**
 * net-snmp main loop,
 * modified from net-snmp-5.4.3/agent/snmp_agent.c:agent_check_and_process().
 * fdset is extended with eventfd to signal that check_and_process loop should
 * be aborted early.
 */
static int
check_and_process(int block, int evtfd)
{
    int             numfds;
    fd_set          fdset;
    struct timeval  timeout = { LONG_MAX, 0 }, *tvp = &timeout;
    int             count;
    int             fakeblock = 0;

    numfds = 0;
    FD_ZERO(&fdset);
    snmp_select_info(&numfds, &fdset, tvp, &fakeblock);
    if (block != 0 && fakeblock != 0) {
        /*
         * There are no alarms registered, and the caller asked for blocking, so
         * let select() block forever.
         */

        tvp = NULL;
    } else if (block != 0 && fakeblock == 0) {
        /*
         * The caller asked for blocking, but there is an alarm due sooner than
         * LONG_MAX seconds from now, so use the modified timeout returned by
         * snmp_select_info as the timeout for select().
         */

    } else if (block == 0) {
        /*
         * The caller does not want us to block at all.
         */

        tvp->tv_sec = 0;
        tvp->tv_usec = 0;
    }

    /* add evtfd to fdset, update numfds */
    FD_SET(evtfd, &fdset);
    if (evtfd + 1 > numfds) {
        numfds = evtfd + 1;
    }

    AIM_LOG_INFO("before select, evtfd %d", evtfd);
    count = select(numfds, &fdset, 0, 0, tvp);
    AIM_LOG_INFO("after select");

    if (count > 0) {
        /* check evtfd, abort if necessary */
        if (FD_ISSET(evtfd, &fdset)) {
            return count;
        }

        /*
         * packets found, process them
         */
        snmp_read(&fdset);
    } else
        switch (count) {
        case 0:
            snmp_timeout();
            break;
        case -1:
            if (errno != EINTR) {
                snmp_log_perror("select");
            }
            return -1;
        default:
            snmp_log(LOG_ERR, "select returned %d\n", count);
            return -1;
        }                       /* endif -- count>0 */

    /*
     * Run requested alarms.
     */
    run_alarms();

    netsnmp_check_outstanding_agent_requests();

    return count;
}


/**
 * This thread performs SNMP agentx dispatching.
 */
static void*
snmp_subagent_worker__(void* p)
{
    volatile snmp_subagent_thread_ctrl_t* ctrl = (volatile snmp_subagent_thread_ctrl_t*) p;
    char buffer[128];

    AIM_LOG_INFO("snmp subagent thread starting (%s)", ctrl->name);
    aim_snprintf(buffer, sizeof(buffer), "snmp.subagent.%s", ctrl->name);
    os_thread_name_set(buffer);

    if(ctrl->debugging) {
        snmp_set_do_debugging(1);
        debug_register_tokens(DEBUG_ALWAYS_TOKEN);
        AIM_LOG_INFO("DEBUG_ALL Set do_debugging=%d\n", snmp_get_do_debugging());
    }

    /**
     * NetSNMP AgentX initialization.
     */
    SOCK_STARTUP;
    init_agent(ctrl->name);

    /**
     * Client initialization.
     */
    snmp_subagent_client_call_all(1);

    signal(SIGTERM, sig_stop_handler__);
    signal(SIGINT,  sig_stop_handler__);

    /**
     * Agent process.
     */
    init_snmp(ctrl->name);
    AIM_LOG_INFO("snmp subagent thread processing (%s)", ctrl->name);
    while(ctrl->run) {
        check_and_process(1, ctrl->eventfd);
    }
    snmp_shutdown(ctrl->name);
    SOCK_CLEANUP;
    AIM_LOG_INFO("snmp subagent thread finished (%s)", ctrl->name);

    return (void*)ctrl;
}


/**
 * Start the subagent worker.
 */
int
snmp_subagent_start(const char* name, int join, int debugging)
{
    if(subagent_thread_running__) {
        return -1;
    }

    memset((void*)&ctrl__, 0, sizeof(ctrl__));
    ctrl__.run = 1;
    ctrl__.name = aim_strdup(name);
    ctrl__.failed = 0;
    ctrl__.debugging = debugging;
    ctrl__.eventfd = eventfd(0, 0);

    AIM_TRUE_OR_DIE(ctrl__.eventfd >= 0);

    if(pthread_create(&subagent_thread_handle__, NULL,
                      snmp_subagent_worker__, (void*)&ctrl__) < 0) {
        AIM_LOG_ERROR("snmp subagent thread creation failed.");
        return -1;
    }
    subagent_thread_running__ = 1;

    if(join) {
        pthread_join(subagent_thread_handle__, NULL);
        subagent_thread_running__ = 0;
    }
    return 0;
}

int
snmp_subagent_stop(void)
{
    if(!subagent_thread_running__) {
        return -1;
    }
    ctrl__.run = 0;
    notify_stop();
    pthread_join(subagent_thread_handle__, NULL);
    subagent_thread_running__ = 0;
    return 0;
}
