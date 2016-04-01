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

#if VPI_CONFIG_INCLUDE_INTERFACE_PCAP == 1

#define VPI_LOG_PREFIX1 ".pcap"
#include "vpi_log.h"

#include "vpi_interface_pcap.h"
#include <pcap/pcap.h>

static const char* pcap_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: PCAP \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI endpoints using PCAP interfaces.\n"
    "\n"
    "Create format:\n"
    "    'pcap|INTERFACE'\n"
    "\n"
    "     Example:\n"
    "     'pcap|eth0'\n"
    "\n"
    "\n";

typedef struct vpi_interface_pcap_s {

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

    /* Our pcap object */
    pcap_t* pcap;

    /* Descriptor */
    int fd;

} vpi_interface_pcap_t;


#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_pcap_t* _vi_name = (vpi_interface_pcap_t*)(_vpi_name)


int
vpi_pcap_interface_register(void)
{
    /* Register our module as handling type 'udp' */
    return vpi_interface_register("pcap", vpi_pcap_interface_create, NULL,
                                pcap_interface_docstring__);
}


/**************************************************************************//**
 *
 * Send packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/
static int
sendto__(vpi_interface_pcap_t* vi, char* data, int len)
{
    return pcap_sendpacket(vi->pcap, (unsigned char*)data, len);
}

/**************************************************************************//**
 *
 * Receive packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/
static int
read__(vpi_interface_pcap_t* vi, char* data, unsigned int len)
{
    int rv;
    struct pcap_pkthdr *pkt_header;
    const uint8_t *pkt_data;

    if (pcap_next_ex(vi->pcap, &pkt_header, &pkt_data) != 1) {
        VPI_ERROR(vi, "pcap_next_ex() failed: %s", pcap_geterr(vi->pcap));
        return -1;
    }

    if (pkt_header->caplen != pkt_header->len) {
        VPI_ERROR(vi, "pcap truncated packet: len=%d caplen=%d",
                  pkt_header->caplen, pkt_header->len);
        return -1;
    }

    rv = (len < pkt_header->len) ? len : pkt_header->len;
    VPI_MEMCPY(data, pkt_data, rv);
    return rv;
}

/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/

int
vpi_pcap_interface_create(vpi_interface_t** vi, char* args[], int flags,
                       const char* vpi_name_ptr)
{
    vpi_interface_pcap_t* nvi = aim_zmalloc(sizeof(*nvi));
    char** arg = args;
    char errbuf[PCAP_ERRBUF_SIZE];

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

    /*
     * next arg - interface name.
     */
    if((nvi->pcap = pcap_create(*arg, errbuf)) == NULL) {
        VPI_ERROR(nvi, "pcap_create(%s) failed: %s\n",
                  *arg, errbuf);
        aim_free(nvi);
        return -1;
    }

    if(pcap_set_promisc(nvi->pcap, 1) != 0) {
        VPI_WARN(nvi, "pcap_set_promisc() failed.");
    }

    if (pcap_activate(nvi->pcap) != 0) {
        VPI_ERROR(nvi, "pcap_activate() failed: %s", pcap_geterr(nvi->pcap));
        pcap_close(nvi->pcap);
        aim_free(nvi);
        return -1;
     }

    nvi->fd = pcap_get_selectable_fd(nvi->pcap);
    if(nvi->fd < 0) {
        VPI_WARN(nvi, "pcap_get_selectable_fd() returned %d", nvi->fd);
    }

    nvi->interface.send = vpi_pcap_interface_send;
    nvi->interface.recv = vpi_pcap_interface_recv;
    nvi->interface.recv_ready = vpi_pcap_interface_recv_ready;
    nvi->interface.destroy = vpi_pcap_interface_destroy;
    nvi->interface.descriptor = vpi_pcap_interface_descriptor;

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

int
vpi_pcap_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return sendto__(vi, (char*)data, len);
}

int
vpi_pcap_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return read__(vi, (char*)data, len);
}

int
vpi_pcap_interface_recv_ready(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(vi->fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return select(vi->fd+1, &rfds, NULL, NULL, &tv);
}

int
vpi_pcap_interface_descriptor(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->fd;
}

int
vpi_pcap_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    pcap_close(vi->pcap);
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_PCAP */
