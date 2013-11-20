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


#if VPI_CONFIG_INCLUDE_BRIDGING == 1


#define VPI_LOG_PREFIX1 ".bridge"
#include "vpi_log.h"

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

struct vpi_bridge_s {

    vpi_t v1;
    vpi_t v2;

    pthread_t thread;
    volatile int running;
    volatile int terminate;
};

static int
bridge_interface__(vpi_t from, vpi_t to, unsigned char* pkt, int size)
{
    int rv = 0;
    size = vpi_recv(from, pkt, size, 0);
    if(size > 0) {
        VPI_MINFO("Recv packet from '%s' sending to '%s' (%d bytes)",
                 vpi_name_get(from), vpi_name_get(to), size);
        if((rv = vpi_send(to, pkt, size)) < 0) {
            VPI_MERROR("Send failed %d", rv);
            return rv;
        }
    }
    return rv;
}

static void*
bridge_thread__(void* data)
{
    vpi_bridge_t bridge = (vpi_bridge_t)(data);
    unsigned char* pkt = aim_zmalloc(VPI_CONFIG_MAX_PACKET);

    if(pkt == NULL) {
        VPI_MERROR("packet allocation for bridge failed.");
        bridge->running = 0;
        return NULL;
    }

    bridge->running = 1;
    while(!bridge->terminate) {
        bridge_interface__(bridge->v1, bridge->v2, pkt, VPI_CONFIG_MAX_PACKET);
        bridge_interface__(bridge->v2, bridge->v1, pkt, VPI_CONFIG_MAX_PACKET);
        AIM_USLEEP(1);
    }
    bridge->running = 0;
    aim_free(pkt);
    return NULL;
}

vpi_bridge_t
vpi_bridge_create(vpi_t v1, vpi_t v2)
{
    vpi_bridge_t b;

    if(v1 == NULL || v2 == NULL) {
        return NULL;
    }

    if((b = aim_zmalloc(sizeof(*b))) == NULL) {
        return NULL;
    }

    b->v1 = v1;
    b->v2 = v2;
    b->terminate = 0;

    if(pthread_create(&b->thread, NULL, bridge_thread__,
                      b) < 0) {
        VPI_MERROR("bridge thread creation failed.");
        return NULL;
    }
    return b;
}

int
vpi_bridge_start(vpi_bridge_t v)
{
    AIM_REFERENCE(v);
    return 0;
}
int
vpi_bridge_stop(vpi_bridge_t v)
{
    if(v->running) {
        v->terminate = 1;
        VPI_MTRACE("joining bridge thread...");
        pthread_join(v->thread, NULL);
        VPI_MTRACE("bridge thread has terminated.");
    }
    return 0;
}
int
vpi_bridge_destroy(vpi_bridge_t v)
{
    vpi_bridge_stop(v);
    aim_free(v);
    return 0;
}





#else /* VPI_CONFIG_INCLUDE_BRIDGING */
int vpi_bridge_c_not_empty = 1;
#endif
