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

#include <VPI/vpi_config.h>
#include <VPI/vpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vpi_int.h>

#include <pthread.h>
#include "vpi_utests.h"

#define VPI_LOG_PREFIX1 ".utest"
#include "vpi_log.h"

vpi_testcase_t vpi_testcases[] =
    {
#if VPI_CONFIG_INCLUDE_INTERFACE_QUEUE == 1
        {
            "queue|implicit_normal",
            "queue|1|2",
            "queue|2|1",
        },
#endif
#if VPI_CONFIG_INCLUDE_INTERFACE_TCP == 1
        {
            "tcp|implicit_normal",
            "tcp|send:8001|recv:8002",
            "tcp|send:8002|recv:8001",
        },
        {
            "tcp|implicit_with_listener",
            "tcp|send:8001|recv:8002",
            "tcp|recv:8001",
            "tcp|send:8002",
        },
        {
            "tcp|localhost_normal",
            "tcp|recv:localhost:8001|send:localhost:8002",
            "tcp|recv:localhost:8002|send:localhost:8001",
        },
        {
            "tcp|localhost_with_listener",
            "tcp|recv:localhost:8001|send:localhost:8002",
            "tcp|recv:localhost:8002",
            "tcp|send:localhost:8001",
        },
#endif
#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1
        {
            "udp|implicit_normal",
            "udp|recv:6001|send:6002",
            "udp|recv:6002|send:6001"
        },
        {
            "udp|implicit_with_listener",
            "udp|recv:6001|send:6002",
            "udp|recv:6002",
            "udp|send:6001",
        },
        {
            "udp|localhost_normal",
            "udp|recv:localhost:10000|send:localhost:10001",
            "udp|recv:localhost:10001|send:localhost:10000",
        },
        {
            "udp|localhost_with_listener",
            "udp|recv:localhost:10000|send:localhost:10001",
            "udp|recv:localhost:10001",
            "udp|send:localhost:10000",
        },
        {
            "udp|loopback_normal",
            "udp|send:127.0.0.1:10010|recv:127.0.0.1:10011",
            "udp|send:127.0.0.1:10011|recv:127.0.0.1:10010"
        },
        {
            "udp|loopback_with_listener",
            "udp|send:127.0.0.1:10010|recv:127.0.0.1:10011",
            "udp|recv:127.0.0.1:10010",
            "udp|send:127.0.0.1:10011",
        },
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_VETH == 1
        {
            "VETH: vpi-veth0 and vpi-veth1",
            "veth|vpi-veth0",
            "veth|vpi-veth1",
        },
        {
            "VETH: vpi-veth2 and vpi-veth3",
            "veth|vpi-veth3",
            "veth|vpi-veth2",
        },
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_PCAP == 1
        {
            "PCAP: vpi-veth0 and vpi-veth1",
            "pcap|vpi-veth0",
            "pcap|vpi-veth1",
        },
        {
            "PCAP: vpi-veth2 and vpi-veth3",
            "pcap|vpi-veth3",
            "pcap|vpi-veth2",
        },
#endif
        {
            NULL
        }
    };


vpi_bridge_testcase_t vpi_bridge_testcases[] =
    {
#if VPI_CONFIG_INCLUDE_BRIDGING == 1

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1
        {
            "UDP bridge",
            "udp|recv:36500|send:36501",
            "udp|recv:36501|send:36500",
            "udp|recv:36511|send:36510",
            "udp|recv:36510|send:36511",
        },
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_VETH == 1
        {
            "VETH bridge",
            "veth|vpi-veth0",
            "veth|vpi-veth1",
            "veth|vpi-veth2",
            "veth|vpi-veth3"
        },
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1 && VPI_CONFIG_INCLUDE_INTERFACE_VETH == 1
        {
            "UDP->VETH bridge",
            "veth|vpi-veth0",
            "veth|vpi-veth1",
            "udp|recv:36600|send:36601",
            "udp|recv:36601|send:36600",
        },
        {
            "VETH->UDP bridge2",
            "veth|vpi-veth0",
            "veth|vpi-veth1",
            "udp|recv:36800|send:36700",
            "udp|recv:36700|send:36800",
        },
#endif

#if VPI_CONFIG_INCLUDE_INTERFACE_UDP == 1 && VPI_CONFIG_INCLUDE_INTERFACE_PCAP == 1
        {
            "UDP->PCAP bridge",
            "pcap|vpi-veth0",
            "pcap|vpi-veth1",
            "udp|recv:36600|send:36601",
            "udp|recv:36601|send:36600",
        },
        {
            "PCAP->UDP bridge2",
            "pcap|vpi-veth0",
            "pcap|vpi-veth1",
            "udp|recv:36800|send:36700",
            "udp|recv:36700|send:36800",
        },
#endif
#endif
        {
            NULL
        },
    };


#if VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK == 1

static int
loopback_test__(void)
{
    int rv;
    unsigned char data0[10];
    unsigned char data1[10];
    vpi_t v = NULL;

    if( (v = vpi_create("loop")) == NULL) {
        VPI_MERROR("loopback create failed.");
        goto loopback_test__error;
    }

    VPI_INFO(v, "Checking non-blocking receive.");
    if( (rv = vpi_recv(v, data0, sizeof(data0), 0)) != 0) {
        VPI_ERROR(v, "initial recv returned %d.", rv);
        goto loopback_test__error;
    }

    /* Single packet check */
    memset(data0, 0x55, sizeof(data0));
    if(vpi_send(v, data0, sizeof(data0)) < 0) {
        VPI_ERROR(v, "send failed.");
        goto loopback_test__error;
    }
    if( (rv = vpi_recv(v, data1, sizeof(data1), 1)) != sizeof(data1)) {
        VPI_ERROR(v, "recv failed.");
        goto loopback_test__error;
    }
    if(memcmp(data0, data1, sizeof(data1)) != 0) {
        VPI_ERROR(v, "recv data mismatch.");
        goto loopback_test__error;
    }

    if( (rv = vpi_recv(v, data1, sizeof(data1), 0)) != 0) {
        VPI_ERROR(v, "recv returned %d when it should have returned 0.", rv);
        goto loopback_test__error;
    }


    {
        int i;
        for(i = 0; i < 10; i++) {
            memset(data0, i, sizeof(data0));
            if(vpi_send(v, data0, sizeof(data0)) < 0) {
                VPI_ERROR(v, "send failed.");
                goto loopback_test__error;
            }
        }
        for(i = 0; i < 10; i++) {
            memset(data0, i, sizeof(data0));
            if( (rv = vpi_recv(v, data1, sizeof(data1), 0)) != sizeof(data1)) {
                VPI_ERROR(v, "Packet missing.");
                goto loopback_test__error;
            }
            if(memcmp(data0, data1, sizeof(data1)) != 0) {
                VPI_ERROR(v, "Recv packet data mismatch.");
                goto loopback_test__error;
            }
        }
        /* No more packets. */
        if( (rv = vpi_recv(v, data1, sizeof(data1), 0)) != 0) {
            VPI_ERROR(v, "Extra packets.");
            goto loopback_test__error;
        }
    }

    VPI_INFO(v, "PASS.");
    vpi_destroy(v);
    return 0;

 loopback_test__error:
    vpi_destroy(v);
    return -1;
}

#endif /* VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK */

void
makeveths__(void)
{
    system("ip link add vpi-veth0 type veth peer name vpi-veth1");
    system("ifconfig vpi-veth0 up");
    system("ifconfig vpi-veth1 up");
    system("ip link add vpi-veth2 type veth peer name vpi-veth3");
    system("ifconfig vpi-veth2 up");
    system("ifconfig vpi-veth3 up");
    system("ip link add vpi-veth4 type veth peer name vpi-veth5");
    system("ifconfig vpi-veth4 up");
    system("ifconfig vpi-veth5 up");
    system("ip link add vpi-veth6 type veth peer name vpi-veth7");
    system("ifconfig vpi-veth6 up");
    system("ifconfig vpi-veth7 up");
}

void
killveths__(void)
{
    system("ip link delete vpi-veth0 > /dev/null 2>&1");
    system("ip link delete vpi-veth2 > /dev/null 2>&1");
    system("ip link delete vpi-veth4 > /dev/null 2>&1");
    system("ip link delete vpi-veth6 > /dev/null 2>&1");
}



int
main(int argc, char* argv[])
{
    vpi_testcase_t* tc;

    int test_count = 0;
    int fail_count = 0;


    killveths__();
    makeveths__();
    atexit(killveths__);

    vpi_init();

/**************************************************************************//**
 *
 * Multithreaded/Blocking Tests
 *
 *
 *****************************************************************************/

    for(tc = vpi_testcases; tc->description; tc++) {
        if(argc == 1 || strstr(tc->description, argv[1])) {
            test_count++;
            if(vpi_blocking_testcase_run(tc) < 0) {
                fail_count++;
            }
        }
    }

#if VPI_CONFIG_INCLUDE_BRIDGING == 1
    {
        vpi_bridge_testcase_t* btc;
        for(btc = vpi_bridge_testcases; btc->description; btc++) {
            if(argc == 1 || strstr(btc->description, argv[1])) {
                test_count++;
                if(vpi_blocking_bridge_testcase_run(btc) < 0) {
                    fail_count++;
                }
            }
        }
    }
#endif

/**************************************************************************//**
 *
 * Singlethreaded/Select Tests
 *
 *
 *****************************************************************************/
    for(tc = vpi_testcases; tc->description; tc++) {
        if(argc == 1 || strstr(tc->description, argv[1])) {
            test_count++;
            if(vpi_non_blocking_testcase_run(tc) < 0) {
                fail_count++;
            }
        }
    }

#if VPI_CONFIG_INCLUDE_INTERFACE_LOOPBACK == 1
    {
        if(argc == 1 || strstr("loopback", argv[1])) {
            test_count++;
            if(loopback_test__() < 0) {
                fail_count++;
            }
        }
    }
#endif

    /* Loopback Test */
    printf("\n*** Results, VPI loopback: %d PASSED, %d FAILED, %d TOTAL\n\n",
           test_count-fail_count, fail_count, test_count);

    return 0;
}
