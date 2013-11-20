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

#include "vpi_int.h"

#if VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK == 1

#define VPI_LOG_PREFIX1 ".loopback"
#include "vpi_log.h"

#include "vpi_interface_loopback.h"

#include <BigList/biglist.h>

#include <unistd.h>
#include <semaphore.h>


static const char* loopback_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: Loopback \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI Loopback endpoint. \n"
    "Packets sent() to the interface will be returned\n"
    "as recv().\n"
    "\n"
    "Create format (no arguments):\n"
    "    'loop'"
    "\n"
    "\n";


typedef struct vpi_loopback_packet_s {
    char* data;
    int size;
} vpi_loopback_packet_t;

vpi_loopback_packet_t*
loopback_packet__(unsigned char* data, int len)
{
    vpi_loopback_packet_t* rv = aim_zmalloc(sizeof(*rv));
    if(rv) {
        rv->data = aim_zmalloc(len);
        if(rv->data == NULL) {
            aim_free(rv);
            return NULL;
        }
        VPI_MEMCPY(rv->data, data, len);
        rv->size = len;
    }
    return rv;
}
void
loopback_packet__Free(vpi_loopback_packet_t* vp)
{
    if(vp) {
        if(vp->data) {
            aim_free(vp->data);
        }
        aim_free(vp);
    }
}

typedef struct vpi_interface_loopback_s {

/**************************************************************************//**
 *
 * This must always be first.
 * This is the public inteface data.
 *
 *****************************************************************************/
    vpi_interface_t interface;


/**************************************************************************//**
 *
 * Private implementation data.
 *
 *
 *****************************************************************************/
    const char* log_string;

    biglist_t* packet_list;

    sem_t lock;

} vpi_interface_loopback_t;

#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_loopback_t* _vi_name = (vpi_interface_loopback_t*)(_vpi_name)


int
vpi_loopback_interface_register(void)
{
    return vpi_interface_register("loop", vpi_loopback_interface_create, NULL,
                                loopback_interface_docstring__);
}

/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/

int
vpi_loopback_interface_create(vpi_interface_t** vi, char* args[], int flags,
                       const char* vpi_name_ptr)
{
    vpi_interface_loopback_t* nvi = aim_zmalloc(sizeof(*nvi));
    char** arg = args;

    AIM_REFERENCE(flags);


    if(nvi == NULL) {
        VPI_MERROR("interface allocation failed for %s.",
                   vpi_name_ptr);
        return -1;
    }

    /*
     * Point our log_string to our name so we can use it immediately
     * in log messages.
     */
    nvi->log_string = vpi_name_ptr;


    /*
     * The first argument is the type -- skip for now
     */
    arg++;

    nvi->interface.send = vpi_loopback_interface_send;
    nvi->interface.recv = vpi_loopback_interface_recv;
    nvi->interface.recv_ready = vpi_loopback_interface_recv_ready;
    nvi->interface.destroy = vpi_loopback_interface_destroy;

    sem_init(&nvi->lock, 0, 1);

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

int
vpi_loopback_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    vpi_loopback_packet_t* rv = loopback_packet__(data, len);
    if(rv) {
        VPI_INFO(vi, "send: data=%p size=%d", data, len);
        sem_wait(&vi->lock);
        vi->packet_list = biglist_append((biglist_t*)vi->packet_list, rv);
        sem_post(&vi->lock);
    }
    return (rv) ? 0 : -1;
}

int
vpi_loopback_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    vpi_loopback_packet_t* rv;

    VICAST(vi, _vi);
    while(vi->packet_list == NULL) {
        AIM_USLEEP(250000);
    }
    sem_wait(&vi->lock);
    rv = (vpi_loopback_packet_t*)vi->packet_list->data;
    vi->packet_list = biglist_remove((biglist_t*)vi->packet_list, rv);
    sem_post(&vi->lock);

    if(len > rv->size) {
        len = rv->size;
    }
    VPI_MEMCPY(data, rv->data, len);
    VPI_INFO(vi, "recv: data=%p size=%d", data, len);
    loopback_packet__Free(rv);
    return len;
}

int
vpi_loopback_interface_recv_ready(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->packet_list != NULL;
}

int
vpi_loopback_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    biglist_t* ble;
    for(ble = vi->packet_list; ble; ble = biglist_next(ble)) {
        loopback_packet__Free((vpi_loopback_packet_t*)ble->data);
    }
    biglist_free(vi->packet_list);
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_LOOPBACK */
