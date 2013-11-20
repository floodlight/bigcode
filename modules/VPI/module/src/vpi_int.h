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

#ifndef __VPI_INT_H__
#define __VPI_INT_H__

#include <VPI/vpi_config.h>
#include <VPI/vpi.h>
#include <BigList/biglist.h>
#include <BigList/biglist_locked.h>
#include <time.h>

#include <VPI/vpi_interface.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>


struct vpi_s {

    char* log_string;

    /* Specification used to create this VPI */
    char create_spec[VPI_CONFIG_CREATE_SPEC_MAX];

    /* The type of this VPI */
    char type[VPI_CONFIG_CREATE_SPEC_MAX];

    /* The name of this particular instance */
    char name[VPI_CONFIG_CREATE_SPEC_MAX];

    vpi_interface_t* interface;
    volatile void* cookie;

    biglist_locked_t* recv_listeners;
    biglist_locked_t* send_listeners;
    biglist_locked_t* sendrecv_listeners;

    int ref_count;

    uint32_t flags;
};

#include <pthread.h>

#define VPI_ARG_GET(_biglist) ( (char*)(_biglist->data))

extern biglist_locked_t* vpi_instances__;





#endif /* __VPI_INT_H__ */
