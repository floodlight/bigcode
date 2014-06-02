/************************************************************
 * <bsn.cl fy=2013 v=epl>
 * 
 *        Copyright 2013, 2014 Big Switch Networks, Inc.       
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

#ifndef UTILS_H
#define UTILS_H

#include <features.h>

#define ETH_FORMAT "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x"
#define ETH_ADDR_PRINT(x) x[0],x[1],x[2],x[3],x[4],x[5]

#define IPV4_FORMAT "%u.%u.%u.%u"
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    #define IPV4_ADDR_PRINT(ip)  (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) & 0xff), ((ip >> 24) & 0xff)
# else
    #define IPV4_ADDR_PRINT(ip)  ((ip >> 24) & 0xff), ((ip >> 16) & 0xff), ((ip >> 8) & 0xff),  (ip & 0xff)
#endif


typedef unsigned int u32;
typedef unsigned char u8;


#endif
