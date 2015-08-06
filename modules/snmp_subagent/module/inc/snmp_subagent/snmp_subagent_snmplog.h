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
#ifndef __SNMP_SUBAGENT_SNMPLOG_H__
#define __SNMP_SUBAGENT_SNMPLOG_H__

int snmp_subagent_snmplog_init(void);

int snmp_subagent_snmplog_callback(int major, int minor,
                                   void *serverarg, void *clientarg);

#endif /* __SNMP_SUBAGENT_SNMPLOG_H__ */
