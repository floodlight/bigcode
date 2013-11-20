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
#include <uCli/ucli.h>
#include <VPI/vpi.h>

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <AIM/aim.h>
#include <AIM/aim_rl.h>
#include <PPE/ppe.h>

#include <OS/os_time.h>
#include <OS/os_sleep.h>

#if VPI_CONFIG_INCLUDE_UCLI == 0
#error Please recompile with VPI_CONFIG_INCLUDE_UCLI = 1
#else

typedef struct vt_ctrl_s {
    vpi_t vpi;
    uint8_t data[1024*20];
    int size;
    int bridges;
} vt_ctrl_t;

static vt_ctrl_t vt_ctrl__;

/**
 * Do the following at the beginning of each command handler:
 * 1. Cast and assign our controler structure.
 * 2. Clear and reset data buffer.
 */
#undef UCLI_COMMAND_INIT
#define UCLI_COMMAND_INIT                                \
    AIM_VAR_PCAST_SAFE(vt_ctrl_t*, vtc, uc, uc->cookie); \
    if(vtc) {                                            \
        memset(vtc->data, 0, sizeof(vtc->data));         \
        vtc->size = sizeof(vtc->data);                   \
    }


static int
vpi_recv__(ucli_context_t* uc, vt_ctrl_t* vtc)
{
    while(1) {
        vtc->size = vpi_recv(vtc->vpi, vtc->data, sizeof(vtc->data), 1);
        if(vtc->size == 0) {
            continue;
        }
        if(vtc->size < 0) {
            ucli_printf(uc, "vpi_recv(%{vpi}) returned %d\n",
                        vtc->vpi, vtc->size);
        }
        return vtc->size;
    }
}

ucli_status_t
vt_ucli_module__perf__(ucli_context_t* uc)
{
    uint64_t last;
    uint64_t now;
    double seconds;
    uint64_t deltat = 2000000;
    int count = 0;
    int direction;
    uint8_t data[128] = {0};
    int misscount = 0;
    int pid = 0;

    UCLI_COMMAND_INFO(uc,
                      "perf", 2,
                      "$summary#Receive performance test."
                      "$args#[send|recv] vpi");

    UCLI_ARGPARSE_OR_RETURN(uc, "{choice}{vpi}",
                            &direction, "direction", 2, "send", "recv",
                            &vtc->vpi);

    count = 0;
    last = os_time_monotonic();
    while(1) {

        if(direction == 0) { /* send */
            *(uint32_t*)(data) = pid++;
            vpi_send(vtc->vpi, data, sizeof(data));
            count++;
        }
        else { /* recv */
            if(vpi_recv__(uc, vtc) > 0) {
                uint32_t rpid = *(uint32_t*)(vtc->data);
                if(pid != rpid) {
                    misscount += (rpid - pid);
                    pid = rpid+1;
                }
                else {
                    pid++;
                }
                count++;
            }
        }

        now = os_time_monotonic();
        if((now - last) > deltat) {
            seconds = 1.0*(now-last)/1000000;
            ucli_printf(uc, "%d packets %s in %f seconds (%f pps) missed=%d\n",
                        count, direction ? "received" : "sent", seconds, count/seconds, misscount);
            count=0;
            last = now;
            misscount=0;
        }
    }
    return UCLI_STATUS_OK;
}


ucli_status_t
vt_ucli_module__dump__(ucli_context_t* uc)
{
    int count = 0;

    UCLI_COMMAND_INFO(uc,
                      "dump", 1,
                      "$summary#Dump all packets on the given VPI."
                      "$args#<vpi_spec>");

    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}", &vtc->vpi);

    while(1) {
        if(vpi_recv__(uc, vtc) > 0) {
            ucli_printf(uc, "[%.3d] recv(%{vpi}):\n%{data}\n",
                        count++, vtc->vpi, vtc->data, vtc->size);
        }
        else {
            break;
        }
    }
    return UCLI_STATUS_OK;
}

ucli_status_t
vt_ucli_module__ppedump__(ucli_context_t* uc)
{
    int count = 0;

    UCLI_COMMAND_INFO(uc,
                      "ppedump", 1,
                      "$summary#PPE dump all packets on the given VPI."
                      "$args#<vpi_spec>");

    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}", &vtc->vpi);

    while(1) {
        if(vpi_recv__(uc, vtc) > 0) {
            ppe_packet_t ppep;
            ppe_packet_init(&ppep, vtc->data, vtc->size);
            if(ppe_parse(&ppep) < 0) {
                ucli_printf(uc, "[%.3d] recv(%{vpi}):\n%{data}\n",
                            count, vtc->vpi, vtc->data, vtc->size);
            }
            else {
                ucli_printf(uc, "[%.3d] recv(%{vpi}):\n",
                            count, vtc->vpi);
                ppe_packet_dump(&ppep, &uc->pvs);
            }
            count++;
        }
        else {
            break;
        }
    }
    return UCLI_STATUS_OK;
}


ucli_status_t
vt_ucli_module__echo__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "echo", 2,
                      "$summary#Send a VPI echo request."
                      "$args#<vpi_spec> <data>");


    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}{idata}",
                            &vtc->vpi, vtc->data, &vtc->size);
    ucli_printf(uc, "sending echo request.\n");
    if(vpi_ioctl(vtc->vpi, VPI_PROTOCOL_OPCODE_ECHO, vtc->data, vtc->size) < 0) {
        return ucli_error(uc, "vpi_ioctl() failed.");
    }

    ucli_printf(uc, "waiting for response.\n");
    memset(vtc->data, 0, sizeof(vtc->data));
    if(vpi_recv__(uc, vtc) > 0) {
        ucli_printf(uc, "recv(%{vpi}):\n%{data}",
                    vtc->vpi, vtc->data, vtc->size);
        return UCLI_STATUS_OK;
    }
    else {
        return UCLI_STATUS_E_ERROR;
    }
}

ucli_status_t
vt_ucli_module__bridge__(ucli_context_t* uc)
{
    vpi_t v1, v2;
    vpi_bridge_t vbridge;

    UCLI_COMMAND_INFO(uc,
                      "bridge", 2,
                      "$summary#Forward all packets between two VPI interfaces."
                      "$args#<vpi_spec_1> <vpi_spec_2>");

    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}{vpi}", &v1, &v2);
    vbridge = vpi_bridge_create(v1, v2);
    if(vbridge == NULL) {
        return ucli_error(uc, "vpi_bridge_create(%{vpi},%{vpi}) failed.",
                          v1, v2);
    }
    if(vpi_bridge_start(vbridge) < 0) {
        return ucli_error(uc, "vpi_bridge_start() failed.");
    }
    vtc->bridges++;
    ucli_printf(uc, "bridging %s <-> %s\n", vpi_name_get(v1), vpi_name_get(v2));
    return UCLI_STATUS_OK;
}

ucli_status_t
vt_ucli_module__send__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "send", 2,
                      "$summary#Send a single packet on the given VPI."
                      "$args#<vpi_spec> <packet_data>");

    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}{idata}", &vtc->vpi, vtc->data, &vtc->size);
    if(vpi_send(vtc->vpi, vtc->data, vtc->size) < 0) {
        return ucli_error(uc, "vpi_send() failed.");
    }
    return UCLI_STATUS_OK;
}

ucli_status_t
vt_ucli_module__spam__(ucli_context_t* uc)
{
    aim_ratelimiter_t counter_rl;
    aim_ratelimiter_t send_rl;
    int count = 0;
    int last = 0;
    int pps = -1;

    UCLI_COMMAND_INFO(uc,
                      "spam", 3,
                      "$summary#Spam packet data on the given VPI."
                      "$args#<vpi_spec> <packet_data> <pps>");

    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}{idata}i",
                            &vtc->vpi, vtc->data, &vtc->size, &pps);

    aim_ratelimiter_init(&counter_rl, 1000000, 0, NULL);
    aim_ratelimiter_init(&send_rl, 1000000/pps, 0, NULL);

    for(;;) {
        uint64_t now = os_time_monotonic();
        if(aim_ratelimiter_limit(&counter_rl, now) == 0) {
            ucli_printf(uc, "Sent %d packets - %d pps\n", count,
                        (count - last));
            last = count;

        }
        if(aim_ratelimiter_limit(&send_rl, now) == 0) {
            if(vpi_send(vtc->vpi, vtc->data, vtc->size) < 0) {
                return ucli_error(uc, "vpi_send() failed.");
            }
            count++;
        }
        else {
            os_sleep_usecs(1);
        }
    }
    return UCLI_STATUS_OK;
}

ucli_status_t
vt_ucli_module__sendrecv__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "sendrecv", 2,
                      "$summary#Send a single packet on the given VPI and wait for a response."
                      "$args#<vpi_spec> <packet_data>");

    vtc->size = sizeof(vtc->data);
    UCLI_ARGPARSE_OR_RETURN(uc, "{vpi}{idata}", &vtc->vpi, vtc->data, &vtc->size);
    if(vpi_send(vtc->vpi, vtc->data, vtc->size) < 0) {
        return ucli_error(uc, "vpi_send() failed.");
    }
    if(vpi_recv__(uc, vtc) > 0) {
        ucli_printf(uc, "recv(%{vpi}):\n%{data}", vtc->vpi, vtc->data, vtc->size);
        return UCLI_STATUS_OK;
    }
    else {
        return UCLI_STATUS_E_ERROR;
    }
}


/* <auto.ucli.handlers.start> */
/******************************************************************************
 *
 * These handler table(s) were autogenerated from the symbols in this
 * source file.
 *
 *****************************************************************************/
static ucli_command_handler_f vt_ucli_module_handlers__[] =
{
    vt_ucli_module__perf__,
    vt_ucli_module__dump__,
    vt_ucli_module__ppedump__,
    vt_ucli_module__echo__,
    vt_ucli_module__bridge__,
    vt_ucli_module__send__,
    vt_ucli_module__spam__,
    vt_ucli_module__sendrecv__,
    NULL
};
/******************************************************************************/
/* <auto.ucli.handlers.end> */

ucli_module_t vt_module = {
    "vpitool",
    &vt_ctrl__,
    vt_ucli_module_handlers__,
    NULL,
    NULL
};

static void
__sighandler(int s)
{
    /*
     * VPI is registered with atexit().
     * Make sure it gets a chance to clean up properly on Ctrl-C
     */
    exit(0);
}

int aim_main(int argc, char* argv[])
{
    signal(SIGINT, __sighandler);

    if(argc == 1 ||
       !strcmp(argv[1], "help") ||
       !strcmp(argv[1], "-h") ||
       !strcmp(argv[1], "--help")) {
        const char* usage =
            "Usage:\n"
            "    vpitool -dump <vpiSpec>\n"
            "        Dump all packets on the given VPI\n"
            "\n"
            "    vpitool -bridge <vpiSec1> <vpiSpec2>\n"
            "        Forward all packets between two VPI interfaces.\n"
            "\n"
            "    vpitool -send <vpiSpec> <packetdata>\n"
            "        Send a single packet on the given VPI.\n"
            "\n"
            "    vpitool -echo <vpiSpec> <packetdata>\n"
            "        Send a VPI echo request.\n"
            "\n"
            "    vpitool -sendrecv <vpiSpec> <packetdata>\n"
            "        Send a single packet on the given VPI and wait for a response.\n"
            "\n"
            "    vpitool help\n"
            "        This help message.\n"
            "\n"
            "EXAMPLES:\n"
            "\n"
            "    # Dump all packets on veth0:\n"
            "    > vpitool -dump \"veth|veth0\"\n"
            "\n"
            "\n"
            "    # Bridge all packets between veth1 and veth2:\n"
            "    > vpitool -bridge \"veth|veth1\" \"veth|veth2\"\n"
            "\n"
            "    # Send a single packet to veth0\n"
            "    > vpitool -send \"veth|veth0\" {FFFFFFFFFFFF}{000000000001}{0800}{AABBCCDD}\n"
            "\n"
            "    # Send and receive a packet between two VPIs over UDP:\n"
            "    > vpitool -dump \"udp|bind|6000|6001\"\n"
            "    > vpitool -send \"udp|bind|6001|6000\" {FFFFFFFFFFFF}{000000000001}{0800}{AABBCCDD}\n"
            "\n"
            "    # A loopback server on port 8000:\n"
            "    > vpitool -bridge \"udp|recv:8000\" \"loop\"\n"
            "    # The corresponding client:\n"
            "    > vpitool -sendrecv \"*s|udp|send:8000|recv:5566\" {DEADBEEFCAFE} \n"
            "\n";

        printf("%s", usage);
        return 0;
    }

    {
        ucli_t* u;
        ucli_init();
        ucli_module_init(&vt_module);
        u = ucli_create(NULL, &vt_module, NULL);
        ucli_dispatch_argv(u, &aim_pvs_stdout, argv+1);
    }

    if(vt_ctrl__.bridges > 0) {
        for(;;) {
            sleep(1000);
        }
    }

    return 0;
}

#endif
