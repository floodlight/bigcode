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

#ifndef __VPI_UTESTS_H__
#define __VPI_UTESTS_H__

#include <VPI/vpi.h>

/******************************************************************************
 *
 * Test two VPI endpoints.
 *
 *
 *****************************************************************************/

typedef struct vpi_testcase_s {

    const char* description;
    const char* spec_client;
    const char* spec_server;
    const char* send_listener;
} vpi_testcase_t;

/******************************************************************************
 *
 * Test two VPI endpoints connected through a VPI bridge.
 *
 *
 *****************************************************************************/

typedef struct vpi_bridge_testcase_s {
    const char* description;
    const char* e0;
    const char* b0;
    const char* b1;
    const char* e1;

} vpi_bridge_testcase_t;


extern vpi_testcase_t vpi_testcases[];
extern vpi_bridge_testcase_t vpi_bridge_testcases[];

extern int vpi_blocking_testcase_run(vpi_testcase_t* testcase);
extern int vpi_blocking_bridge_testcase_run(vpi_bridge_testcase_t* testcase);
extern int vpi_non_blocking_testcase_run(vpi_testcase_t* testcase);
extern int vpi_blocking_bridge_testcase_run(vpi_bridge_testcase_t* testcase);

#endif /* __VPI_UTESTS_H__ */
