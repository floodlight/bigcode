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
#define VPI_LOG_PREFIX2 ".nonblocking"
#include "vpi_log.h"

static int
vpi_non_blocking_testcase_run__(vpi_testcase_t* testcase,
                            vpi_t* client, vpi_t* server)
{
    int rv;

    unsigned char recv_data[2000];
    unsigned char send_data[200];
    int send_size = sizeof(send_data);
    int i;

    vpi_t vpi_client;
    vpi_t vpi_server;

    for(i = 0; i < send_size; i++) {
        send_data[i] = (char)i;
    }

    if( (vpi_client = vpi_create(testcase->spec_client)) == NULL) {
        VPI_MERROR("vpi_create(\"%s\") failed.", testcase->spec_client);
        return -1;
    }
    *client = vpi_client;

    if(testcase->spec_server) {
        if( (vpi_server = (vpi_create(testcase->spec_server))) == NULL) {
            VPI_MERROR("vpi_create(\"%s\") failed.", testcase->spec_server);
            return -1;
        }
    }
    else {
        vpi_server = vpi_client;
    }

    *server = vpi_server;

    if(testcase->send_listener) {
        vpi_ioctl(vpi_client, VPI_PROTOCOL_OPCODE_REGISTER_SEND_LISTENER,
                 (uint8_t*)testcase->send_listener,
                 strlen(testcase->send_listener)+1);
    }

    if( (rv = vpi_send(vpi_client, send_data, send_size)) < 0) {
        VPI_MERROR("send() failed");
        return -1;
    }

    i = 0;
    while(i < 10) {

        rv = vpi_recv(vpi_client, recv_data, sizeof(recv_data), 0);
        if(rv < 0) {
            VPI_MERROR("recv() failed %d", rv);
        }
        if(rv != 0) {
            /* Packet received */
            if(rv != send_size) {
                VPI_MERROR("receive size mismatch.");
                return -1;
            }
            if(vpi_send(vpi_client, recv_data, rv) < 0) {
                VPI_MERROR("send() failed.");
                return -1;
            }
            i++;
        }

        rv = vpi_recv(vpi_server, recv_data, sizeof(recv_data), 0);
        if(rv < 0) {
            VPI_MERROR("recv() failed %d", rv);
            return -1;
        }
        if(rv != 0) {
            /* Packet recieved */
            if(rv != send_size) {
                VPI_MERROR("receive size mismatch.");
                return -1;
            }
            if(vpi_send(vpi_server, recv_data, rv) < 0) {
                VPI_MERROR("send() failed.");
                return -1;
            }
        }
    }

    return 0;
}

int
vpi_non_blocking_testcase_run(vpi_testcase_t* testcase)
{
    int rv;
    vpi_t client = NULL, server = NULL;
    VPI_MINFO("\n\n--++-- Starting nonblocking testcase: '%s'", testcase->description);
    rv = vpi_non_blocking_testcase_run__(testcase, &client, &server);
    VPI_MINFO("--++-- Ending nonblocking testcase: '%s': %s\n", testcase->description,
         (rv < 0) ? "FAILED" : "PASSED");
    vpi_destroy(client);
    if(server != client) {
        vpi_destroy(server);
    }
    return rv;
}
