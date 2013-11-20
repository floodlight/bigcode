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

#if VPI_CONFIG_INCLUDE_INTERFACE_DUMP == 1

#define VPI_LOG_PREFIX1 ".dump"
#include "vpi_log.h"

#include "vpi_interface_dump.h"

static const char* dump_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: DUMP \n"
    "----------------------------------------------------\n"
    "\n";

typedef struct vpi_interface_dump_s {

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
    FILE* fps[16];
    int fpcount;

} vpi_interface_dump_t;


#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_dump_t* _vi_name = (vpi_interface_dump_t*)(_vpi_name)


int
vpi_dump_interface_register(void)
{
    return vpi_interface_register("dump", vpi_dump_interface_create,
                                  NULL, dump_interface_docstring__);
}

int
vpi_dump_interface_create(vpi_interface_t** vi, char* args[], int flags,
                              const char* vpi_name_ptr)
{
    vpi_interface_dump_t* nvi = aim_zmalloc(sizeof(*nvi));
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

    if(*arg) {
        int i = 0;
        /*
         * Filename arguments.
         */
        while(*arg) {
            if(!VPI_STRCMP(*arg, "stdout") || !VPI_STRCMP(*arg, "-")) {
                nvi->fps[i] = stdout;
            }
            else if(!VPI_STRCMP(*arg, "stderr")) {
                nvi->fps[i] = stderr;
            }
            else {
                nvi->fps[i] = fopen(*arg, "w");
            }
            arg++;
            i++;
        }
        nvi->fpcount = i;
    }
    else {
        /*
         * Default to stdout.
         */
        nvi->fpcount = 1;
        nvi->fps[0] = stdout;
    }

    nvi->interface.send = vpi_dump_interface_send;
    nvi->interface.recv = NULL;
    nvi->interface.recv_ready = NULL;
    nvi->interface.destroy = vpi_dump_interface_destroy;
    nvi->interface.descriptor = NULL;

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

static int
dump__(const char* name, FILE* fp, unsigned char* data, int len)
{
    int i;
    fprintf(fp, "{VPI::%s:%.4d bytes:", name, len);
    for(i = 0; i < len; i++) {
        fprintf(fp, "%.2x", data[i]);
    }
    fprintf(fp, "}\n");
    fflush(fp);
    return 0;
}

int
vpi_dump_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    int i;

    for(i = 0; i < vi->fpcount; i++) {
        dump__(vpi_name_get(vi->interface.parent), vi->fps[i], data, len);
    }
    return len;
}

int
vpi_dump_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    int i;
    for(i = 0; i < vi->fpcount; i++) {
        if( (vi->fps[i] != stdout) && (vi->fps[i] != stderr)) {
            fclose(vi->fps[i]);
        }
    }
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_DUMP */
