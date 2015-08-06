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
 * NetSNMP Subagent Framework.
 *
 ***********************************************************/
#include <snmp_subagent/snmp_subagent_config.h>
#include <snmp_subagent/snmp_subagent_clients.h>

/**
 * Initialize as a NetSNMP subagent.
 */
int snmp_subagent_init(void);

/**
 * Start the subagent.
 */
int snmp_subagent_start(const char* name, int join, int debugging);

/**
 * Stop the subagent.
 */
int snmp_subagent_stop(void);


/**
 * Register and unregister all module clients.
 */
int snmp_subagent_module_clients_register(void);
int snmp_subagent_module_clients_unregister(void);

