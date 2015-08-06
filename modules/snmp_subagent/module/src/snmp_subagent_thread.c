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

/** Thread control. */
typedef struct snmp_subagent_thread_ctrl_s {
    volatile int run;
    const char* name;
    int failed;
    int debugging;

} snmp_subagent_thread_ctrl_t;

static pthread_t subagent_thread_handle__;
static int       subagent_thread_running__ = 0;
static volatile snmp_subagent_thread_ctrl_t ctrl__;


/**
 * Signal handlers which terminal the worker thread.
 */
static RETSIGTYPE
sig_stop_handler__(int a) {
    ctrl__.run = 0;
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
        agent_check_and_process(1);
    }
    snmp_shutdown(ctrl->name);
    SOCK_CLEANUP;
    AIM_LOG_INFO("snmp suubagent thread finished (%s)", ctrl->name);

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
    pthread_join(subagent_thread_handle__, NULL);
    subagent_thread_running__ = 0;
    return 0;
}
