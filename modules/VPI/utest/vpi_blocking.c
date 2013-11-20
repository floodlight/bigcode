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

#include "vpi_utests.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define VPI_LOG_PREFIX1 ".utest"
#define VPI_LOG_PREFIX2 ".blocking"
#include "vpi_log.h"

/******************************************************************************
 *
 * Server thread for processing one end of the testcase
 *
 *
 *****************************************************************************/
typedef struct server_thread_ctrl_s {

    const char* name;
    vpi_t vpi;

    volatile char* expected_packet;
    int expected_size;

    volatile int loop;

} server_thread_ctrl_t;

/*
 * The blocking testcase perform the following operations:
 *
 * Server Side: recv, increment, send, recv, increment, send
 * Client Side: send, recv, send, recv
 *
 */

static void*
server_thread__(void* arg)
{
    unsigned char data[2000];
    int rv;
    server_thread_ctrl_t* ctrl = (server_thread_ctrl_t*)(arg);
    int count = 0;

    while(ctrl->loop && count++ < 2) {
        /* Recv Data */
        if((rv = vpi_recv(ctrl->vpi, data, sizeof(data), 1)) < 0) {
            VPI_MERROR("vpi_recv() returned %d", rv);
            return NULL;
        }

        if(rv == 0) {
            count--;
            continue;
        }

        if(rv != ctrl->expected_size) {
            VPI_MERROR( "vpi_recv() returned %d bytes, but expected %d bytes.", rv, ctrl->expected_size);
            return NULL;
        }
        if(memcmp(data, (void*)ctrl->expected_packet, ctrl->expected_size) != 0) {
            VPI_MERROR( "packet data mismatch.");
            return NULL;
        }

        /* Increment first data byte and resend */
        data[0]++;
        if((rv = vpi_send(ctrl->vpi, data, rv)) < 0) {
            VPI_MERROR( "vpi_send() returned %d", rv);
            return NULL;
        }
    }
    return (void*)1;
}

static int
vpi_blocking_testcase_run__(vpi_testcase_t* testcase, vpi_t* client, vpi_t* server)
{
    int rv;
    void* thread_rv = NULL;

    unsigned char recv_data[2000];
    unsigned char send_data[200];

    pthread_t pth;
    server_thread_ctrl_t sctrl;
    vpi_t vpi_client;
    int i;
    int size = sizeof(send_data);

    for(i = 0; i < size; i++) {
        send_data[i] = (char)i;
    }

    if( (vpi_client = vpi_create(testcase->spec_client)) == NULL) {
        VPI_MERROR("vpi_create(\"%s\") failed.", testcase->spec_client);
        return -1;
    }
    *client = vpi_client;

    memset(&sctrl, 0, sizeof(sctrl));

    sctrl.loop = 1;
    if(testcase->spec_server) {
        if( (sctrl.vpi = vpi_create(testcase->spec_server)) == NULL) {
            VPI_MERROR("vpi_create(\"%s\") failed.", testcase->spec_server);
            return -1;
        }
    }
    else {
        sctrl.vpi = vpi_client;
    }
    *server = sctrl.vpi;

    sctrl.expected_size = size;
    sctrl.expected_packet = (char*)send_data;

    /* Start server thread */
    if(pthread_create(&pth, NULL, server_thread__, &sctrl) < 0) {
        VPI_MERROR("could not create server thread: %s", strerror(errno));
        return -1;
    }

    if(testcase->send_listener) {
        vpi_ioctl(vpi_client, VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER,
                 (uint8_t*)testcase->send_listener,
                 strlen(testcase->send_listener)+1);
    }

    send_data[0] = 0x42;
    if((rv=vpi_send(vpi_client, send_data, size)) < 0) {
        VPI_MERROR("send() returned %d: %s", rv, strerror(errno));
        return -1;
    }

    do {
        if((rv = vpi_recv(vpi_client, recv_data, sizeof(recv_data),1)) < 0) {
            VPI_MERROR("recv() returned %d: %s", rv, strerror(errno));
            return -1;
        }
    } while(rv == 0);

    if(rv != size) {
        VPI_MERROR("received %d bytes but expected %d\n",
                 rv, size);
        return -1;
    }
    if(recv_data[0] != 0x43) {
        VPI_MERROR("received packet is not incremented.");
        return -1;
    }
    if(memcmp(recv_data+1, send_data+1, sizeof(send_data)-1) != 0) {
        VPI_MERROR("data mistmach.");
        return -1;
    }

    send_data[0] = 0x65;
    if((rv=vpi_send(vpi_client, send_data, sizeof(send_data))) < 0) {
        VPI_MERROR("send() returned %d: %s", rv, strerror(errno));
        return -1;
    }

    if((rv = vpi_recv(vpi_client, recv_data, sizeof(recv_data),1)) < 0) {
        VPI_MERROR("recv() returned %d: %s", rv, strerror(errno));
        return -1;
    }
    if(rv != sizeof(send_data)) {
        VPI_MERROR("received %d bytes but expected %d\n", rv, size);
        return -1;
    }
    if(recv_data[0] != 0x66) {
        VPI_MERROR("received packet is not incremented.");
        return -1;
    }
    if(memcmp(recv_data+1, send_data+1, sizeof(send_data)-1) != 0) {
        VPI_MERROR("data mistmach.");
        return -1;
    }

    VPI_MINFO("joining server thread.");
    pthread_join(pth, &thread_rv);

    if(thread_rv == NULL) {
        VPI_MINFO("server thread reported failure.");
        return -1;
    }
    else {
        VPI_MINFO("server thread reported success.");
        return 0;
    }
}

int
vpi_blocking_testcase_run(vpi_testcase_t* testcase)
{
    int rv;
    vpi_t client = NULL, server = NULL;

    VPI_MINFO("\n\n--++-- Starting blocking testcase: '%s'", testcase->description);
    rv = vpi_blocking_testcase_run__(testcase, &client, &server);
    VPI_MINFO("--++-- Ending blocking testcase: '%s': %s\n", testcase->description,
              (rv < 0) ? "FAILED" : "PASSED");
    vpi_destroy(client);
    if(client != server) {
        vpi_destroy(server);
    }
    return rv;
}

#if VPI_CONFIG_INCLUDE_BRIDGING == 1

static int
vpi_blocking_bridge_testcase_run__(vpi_bridge_testcase_t* testcase)
{
    vpi_t bridge0 = vpi_create(testcase->b0);
    vpi_t bridge1 = vpi_create(testcase->b1);
    vpi_bridge_t b = vpi_bridge_create(bridge0, bridge1);
    vpi_testcase_t t;
    int rv;

    memset(&t, 0, sizeof(t));

    t.description = testcase->description;
    t.spec_client = testcase->e0;
    t.spec_server = testcase->e1;
    vpi_bridge_start(b);
    rv = vpi_blocking_testcase_run(&t);
    vpi_bridge_stop(b);
    vpi_bridge_destroy(b);
    return rv;
}

int
vpi_blocking_bridge_testcase_run(vpi_bridge_testcase_t* testcase)
{
    return vpi_blocking_bridge_testcase_run__(testcase);
}

#endif
