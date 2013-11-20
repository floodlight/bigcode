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

#if VPI_CONFIG_INCLUDE_INTERFACE_BPP == 1

#define VPI_LOG_PREFIX1 ".bpp"
#include "vpi_log.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "vpi_interface_bpp.h"
#include "vpi_interface_udp.h"
#include <stdio.h>

static const char* bpp_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: BPP \n"
    "----------------------------------------------------\n"
    "\n"
    "BigPatchPanel VPI endpoints.\n"
    "\n"
    "Create format:\n"
    "    # Connect to a BigPatchPanel device port:\n"
    "    'bpp|DEVICENAME:PORT'\n"
    "\n"
    "Requires $BIGLAB to point to the BigLab respository.\n"
    "\n"
    "\n"
    "\n";


int
vpi_bpp_interface_register(void)
{
    /*
     * We are just a preprocessor for UDP/TCP endpoints
     * used with the BigPatchPanel controller.
     */
    return vpi_interface_register("bpp", NULL,
                                vpi_bpp_interface_preprocess,
                                bpp_interface_docstring__);
}


char*
vpi_bpp_interface_preprocess(char* args[], const char* create_spec)
{
    char** arg = args;
    char track_script[256];
    char cmd[256];
    int port;
    char* type = "udp";

    AIM_REFERENCE(create_spec);


    /*
     * First arg is bpp
     */

    arg++;

    /*
     * The second argument may be an optional selection for
     * UDP or TCP. The default is udp.
     */
    if(!VPI_STRCMP("tcp", *arg) || !VPI_STRCMP("udp", *arg)) {
        type = *arg;
        arg++;
    }

    /*
     * Support special localhost processing.
     * The special "localhost:port" target points directly to
     * localhost on the given port number.
     *
     * This is mostly a testing convenience.
     *
     */
    if(VPI_SSCANF(*arg, "localhost:%d", &port) == 1) {
        VPI_SPRINTF(cmd, "*s|%s|send:localhost:%d|recv:0",
                    type, port);
    }
    else {
        /*
         * Determine the UDP spec we need to talk to the target.
         */
        char server[64] = {0};
        FILE* rv;

        if(getenv("TRACK_SCRIPT")) {
            VPI_STRCPY(track_script, getenv("TRACK_SCRIPT"));
        }
        else if(getenv("BIGLAB")) {
            VPI_SPRINTF(track_script, "%s/track", getenv("BIGLAB"));
        }
        else {
            VPI_STRCPY(track_script, "/usr/bin/track");
        }
        if(access(track_script, F_OK) != 0) {
            VPI_MERROR("Cannot access track script @ %s", track_script);
            return NULL;
        }

        VPI_SPRINTF(cmd, "%s vpi %s %s", track_script, type, *arg);

        rv = popen(cmd, "r");
        if(VPI_SSCANF(fgets(cmd, sizeof(cmd), rv), "%[^:]:%d", server, &port) == 2) {
            VPI_MINFO("track: server=%s port=%d", server, port);
        }
        else {
            VPI_MERROR("error reading output from track script.");
            return NULL;
        }

        VPI_SPRINTF(cmd, "*s|%s|send:%s:%d|recv:0",
                    type, server, port);
    }
    return aim_strdup(cmd);
}


#endif /* INCLUDE_INTERFACE_BPP */
