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
#ifndef __SNMP_SUBAGENT_CLIENTS_H__
#define __SNMP_SUBAGENT_CLIENTS_H__

#include <snmp_subagent/snmp_subagent_config.h>


/**
 * Your custom agent initialization handler.
 * Called When the AgentX thread is initializing.
 *
 * Use this to register and unregister your handlers.
 */
typedef int (*snmp_subagent_clientf_t)(int enable, void* cookie);


/**
 * Initialize clients management.
 */
int snmp_subagent_client_init(void);

/**
 * Register a custom client.
 */
int snmp_subagent_client_register(const char* name,
                                  snmp_subagent_clientf_t client,
                                  void* cookie);

/**
 * Unregister a custom client.
 */
int snmp_subagent_client_unregister(const char* name);

/**
 * Call all clients.
 */
int snmp_subagent_client_call_all(int enable);



#endif /* __SNMP_SUBAGENT_CLIENTS_H__ */

