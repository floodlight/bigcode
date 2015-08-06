/************************************************************
 * <bsn.cl fy=2015 v=epl>
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
#include <snmp_subagent/snmp_subagent.h>
#include <snmp_subagent/snmp_subagent_snmplog.h>
#include <snmp_subagent/snmp_subagent_clients.h>
#include "snmp_subagent_int.h"
#include <OS/os_sem.h>
#include <AIM/aim_list.h>


#include "snmp_subagent_example.h"

static int
snmp_callback_start__(int major, int minor, void *serverarg, void *clientarg)
{
    return SNMP_ERR_NOERROR;
}

static int
snmp_callback_stop__(int major, int minor, void *serverarg, void *clientarg)
{
    return SNMP_ERR_NOERROR;
}

int
snmp_subagent_init(void)
{
    static int initialized__ = 0;

    if(initialized__) {
        return 0;
    }

    snmp_subagent_snmplog_init();
    snmp_subagent_client_init();

    snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_LOGGING,
                           snmp_subagent_snmplog_callback, NULL);
    snmp_enable_calllog();

    snmp_register_callback(SNMP_CALLBACK_APPLICATION,
                           SNMPD_CALLBACK_INDEX_START,
                           snmp_callback_start__, NULL);

    snmp_register_callback(SNMP_CALLBACK_APPLICATION,
                           SNMPD_CALLBACK_INDEX_STOP,
                           snmp_callback_stop__, NULL);

    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                           NETSNMP_DS_AGENT_ROLE, 1);

    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                           NETSNMP_DS_AGENT_NO_CONNECTION_WARNINGS, 1);

    initialized__ = 1;


#if SNMP_SUBAGENT_CONFIG_INCLUDE_EXAMPLE_CLIENT == 1
    snmp_subagent_client_register("snmp_subagent_example_client",
                                  snmp_subagent_example_client, NULL);
#endif

    return 0;
}



#define MODULE_FUNCTION(_module, _type) _module##_snmp_subagent_##_type
#define DEPENDMODULE_SNMP_SUBAGENT_TYPE(_module, _type)                 \
    do {                                                                \
        extern int MODULE_FUNCTION(_module, _type)(void);               \
        int rv = MODULE_FUNCTION(_module, _type)();                     \
        if(rv < 0) {                                                    \
            AIM_LOG_ERROR("snmp subagent %s for module %s failed", #_type, #_module); \
        }                                                               \
        else {                                                          \
            AIM_LOG_MSG("snmp subagent %s for module %s succeeded.",#_type,  #_module); \
        }                                                               \
    } while(0);

int
snmp_subagent_module_clients_register(void)
{
#define DEPENDMODULE_SNMP_SUBAGENT(_module) DEPENDMODULE_SNMP_SUBAGENT_TYPE(_module, register)
#include <dependmodules.x>
    return 0;
#undef DEPENDMODULE_SNMP_SUBAGENT
}

int
snmp_subagent_module_clients_unregister(void)
{
#define DEPENDMODULE_SNMP_SUBAGENT(_module) DEPENDMODULE_SNMP_SUBAGENT_TYPE(_module, unregister)
#include <dependmodules.x>
#undef DEPENDMODULE_SNMP_SUBAGENT
    return 0;
}

