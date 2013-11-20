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

#if VPI_CONFIG_INCLUDE_INTERFACE_QUEUE == 1

#define VPI_LOG_PREFIX1 ".queue"
#include "vpi_log.h"

#include "vpi_interface_queue.h"

#include <OS/os_sem.h>

static const char* queue_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: QUEUE \n"
    "----------------------------------------------------\n"
    "\n";



/**************************************************************************//**
 *
 * Packet container management.
 * These are stored in the packet queues.
 *
 *****************************************************************************/
typedef struct vpi_qpacket_s {
    char* data;
    int size;
} vpi_qpacket_t;

static vpi_qpacket_t*
qpacket_alloc__(unsigned char* data, int size)
{
    vpi_qpacket_t* qp = aim_zmalloc(sizeof(*qp));
    qp->data = aim_memdup(data, size);
    qp->size = size;
    return qp;
}
static void
qpacket_free__(vpi_qpacket_t* qp)
{
    if(qp) {
        if(qp->data) {
            aim_free(qp->data);
        }
        aim_free(qp);
    }
}

/**************************************************************************//**
 *
 * Packet queue management.
 * Each VPI instance has a read queue and a write queue.
 *
 *****************************************************************************/
typedef struct vpi_pq_s {
    /** The name of this queue */
    const char* name;

    /** Queue Management lock. */
    os_sem_t mlock;

    /**
     * Packets available semaphore.
     * Do not take while holding the management lock.
     */
    os_sem_t packets_available;

    /** The actual packet queue */
    biglist_t* list;

    /** Packet history queue (TBD) */
    biglist_t* history_list;

    /** Reference count. Queues are shared between
     * VPI endpoint instances.
     */
    int refcount;

} vpi_pq_t;


/** Global list of all active packet queues. */
static biglist_locked_t* pq_list__ = NULL;

/**
 * Find a packet queue by name.
 */
vpi_pq_t*
pq_find_locked__(const char* name)
{
    biglist_t* ble;
    vpi_pq_t* pq = NULL;

    BIGLIST_FOREACH_DATA(ble, pq_list__->list, vpi_pq_t*, pq) {
        if(!VPI_STRCMP(name, pq->name)) {
            break;
        }
    }

    return pq;
}

/**
 * Allocate a new packet queue.
 * pq_list__ must be locked prior to this call.
 */
vpi_pq_t*
pq_alloc_locked__(const char* name)
{
    vpi_pq_t* pq = aim_zmalloc(sizeof(*pq));

    pq->name = aim_strdup(name);
    pq->mlock = os_sem_create(1);
    pq->packets_available = os_sem_create(0);
    pq->refcount = 0;

    pq_list__->list = biglist_prepend(pq_list__->list, pq);

    return pq;
}

/**
 * Destroy a packet queue.
 */
void
pq_destroy__(vpi_pq_t* pq)
{
    pq->refcount--;
    if(pq->refcount <= 0) {
        biglist_locked_remove(pq_list__, pq);
        aim_free((char*)pq->name);
        os_sem_destroy(pq->mlock);
        os_sem_destroy(pq->packets_available);
        biglist_free_all(pq->list, (biglist_free_f)qpacket_free__);
        aim_free(pq);
    }
}

/**
 * Find or create the given packet queue.
 */
vpi_pq_t*
queue_find__(const char* name, int alloc)
{
    vpi_pq_t* pq;

    biglist_lock(pq_list__);

    pq = pq_find_locked__(name);
    if(pq == NULL && alloc) {
        pq = pq_alloc_locked__(name);
    }

    biglist_unlock(pq_list__);
    return pq;
}


/**************************************************************************//**
 *
 * VPI Instance implementation.
 *
 *
 *****************************************************************************/

typedef struct vpi_interface_queue_s {

/**************************************************************************//**
 *
 * This must always be first.
 * This is the public interface data.
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

    /** Our Read Queue */
    vpi_pq_t* rq;
    /** Our Write Queue */
    vpi_pq_t* wq;

} vpi_interface_queue_t;


#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_queue_t* _vi_name = (vpi_interface_queue_t*)(_vpi_name)


int
vpi_queue_interface_register(void)
{
    return vpi_interface_register("queue", vpi_queue_interface_create,
                                  NULL, queue_interface_docstring__);
}

void
pq_append__(vpi_pq_t* q, vpi_qpacket_t* qp)
{
    os_sem_take(q->mlock);
    q->list = biglist_append(q->list, qp);
    os_sem_give(q->mlock);
    os_sem_give(q->packets_available);
}

vpi_qpacket_t*
pq_unshift__(vpi_pq_t* q)
{
    vpi_qpacket_t* rv = NULL;
    os_sem_take(q->mlock);
    if(q->list) {
        rv = (vpi_qpacket_t*)(q->list->data);
        q->list = biglist_remove_link_free(q->list, q->list);
    }
    os_sem_give(q->mlock);
    return rv;
}

int
vpi_queue_interface_send(vpi_interface_t* _vi, unsigned char* data, int size)
{
    VICAST(vi, _vi);
    pq_append__(vi->wq, qpacket_alloc__(data, size));
    return size;
}

int
vpi_queue_interface_recv(vpi_interface_t* _vi, unsigned char* data, int size)
{
    VICAST(vi, _vi);
    vpi_qpacket_t* qpacket = NULL;
    int rv;

    do {
        /** A packet must be available. */
        os_sem_take(vi->rq->packets_available);

        qpacket = pq_unshift__(vi->rq);

        /**
         * We don't expect multiple queue readers, so the qp pointer
         * should never be NULL at this point.
         *
         * Treat this as a bug for now.
         */
        if(qpacket == NULL) {
            AIM_LOG_ERROR(vi, "packet semaphore was signalled but packet was missing.");
        }
    } while(qpacket == NULL);

    rv = aim_imin(size, qpacket->size);
    VPI_MEMCPY(data, qpacket->data, rv);
    qpacket_free__(qpacket);
    return rv;
}

int
vpi_queue_interface_recv_ready(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->rq->list != NULL;
}

int
vpi_queue_interface_create(vpi_interface_t** vi, char* args[], int flags,
                           const char* vpi_name_ptr)
{
    vpi_interface_queue_t* nvi = aim_zmalloc(sizeof(*nvi));
    char** arg = args;

    AIM_REFERENCE(flags);

    /**
     * Initialize global pq list if necessary.
     */
    if(pq_list__ == NULL) {
        pq_list__ = biglist_locked_create();
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

    /** Must have two queue arguments */
    if(*arg == NULL || *(arg+1) == NULL) {
        aim_free(nvi);
        return -1;
    }

    /*
     * The first arguement is the send queue.
     * The second argument is the recv queue;
     */
    nvi->wq = queue_find__(*arg, 1);
    nvi->wq->refcount++;
    arg++;

    nvi->rq = queue_find__(*arg, 1);
    nvi->rq->refcount++;
    arg++;

    /** Other arguments ignored */
    nvi->interface.send = vpi_queue_interface_send;
    nvi->interface.recv = vpi_queue_interface_recv;
    nvi->interface.recv_ready = vpi_queue_interface_recv_ready;
    nvi->interface.destroy = vpi_queue_interface_destroy;
    /* todo -- support eventfd */
    nvi->interface.descriptor = NULL;

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

int
vpi_queue_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);

    pq_destroy__(vi->wq);
    pq_destroy__(vi->rq);

    aim_free(vi);


    /** If there are no more packet queues, destroy our global list as well */
    if(pq_list__->list == NULL) {
        biglist_locked_free(pq_list__);
        pq_list__ = NULL;
    }

    return 0;
}

#endif /* INCLUDE_INTERFACE_QUEUE */
