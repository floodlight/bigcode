/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
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
 ***************************************************************/

#ifndef __PPE_UTIL_H__
#define __PPE_UTIL_H__

#include <PPE/ppe_config.h>

uint32_t ppe_ip_header_checksum_update(ppe_packet_t* ppep);
uint32_t ppe_tcp_header_checksum_update(ppe_packet_t* ppep);
uint32_t ppe_udp_header_checksum_update(ppe_packet_t* ppep);


#endif /* __PPE_UTIL_H__ */
