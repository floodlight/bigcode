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

#ifndef __VPI_BPP_H__
#define __VPI_BPP_H__

#include "vpi_int.h"

#if VPI_CONFIG_INCLUDE_INTERFACE_BPP == 1

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP != 1
#error "The BPP interface requires the UDP interface."
#endif


/* Register the BPP interface with the VPI library */
int vpi_bpp_interface_register(void);

/* The BPP is just a preprocessor interface for UDP or TCP */
char* vpi_bpp_interface_preprocess(char* args[], const char* create_spec);

#endif

#endif /* __VPI_BPP_H__ */


