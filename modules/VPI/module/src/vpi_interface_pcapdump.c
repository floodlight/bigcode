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

#if VPI_CONFIG_INCLUDE_INTERFACE_PCAPDUMP == 1

#define VPI_LOG_PREFIX1 ".pcapdump"
#include "vpi_log.h"

#include "vpi_interface_pcapdump.h"
#include <pcap/pcap.h>

static const char* pcapdump_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: PCAPDUMP \n"
    "----------------------------------------------------\n"
    "\n"
    "Used as a Send/Recv listener to automatically dump a\n"
    "VPI packet stream to a PCAP file.\n"
    "\n"
    "Create format:\n"
    "    'pcapdump|FILENAME'\n"
    "\n"
    "     Example:\n"
    "     'pcap|foobar.pcap'\n"
    "\n"
    "\n";

typedef struct vpi_interface_pcapdump_s {

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
    pcap_dumper_t* pcap_dumper;
    int mpls;
    char ident[8];

} vpi_interface_pcapdump_t;


#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_pcapdump_t* _vi_name = (vpi_interface_pcapdump_t*)(_vpi_name)


int
vpi_pcapdump_interface_register(void)
{
    /* Register our module as handling type 'udp' */
    return vpi_interface_register("pcapdump", vpi_pcapdump_interface_create,
                                  NULL, pcapdump_interface_docstring__);
}

int
vpi_pcapdump_interface_create(vpi_interface_t** vi, char* args[], int flags,
                              const char* vpi_name_ptr)
{
    vpi_interface_pcapdump_t* nvi = aim_zmalloc(sizeof(*nvi));
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

    if((nvi->pcap = pcap_open_dead(DLT_EN10MB, 10000)) == NULL) {
        VPI_ERROR(nvi, "pcap_open_dead() failed: %s\n",
                  *arg, errbuf);
        aim_free(nvi);
        return -1;
    }

    /*
     * next arg - filename name.
     */
    if((nvi->pcap_dumper = pcap_dump_open(nvi->pcap, *arg)) == NULL) {
        VPI_ERROR(nvi, "pcap_dump_open(%s) failed\n", *arg);
        pcap_close(nvi->pcap);
        aim_free(nvi);
        return -1;
    }

    arg++;
    if(*arg) {
        /*
         * if the next arg is 'mpls', packets will be encapsulated
         * in an MPLS frame with the outer fields indicating useful
         * information, like port number, send or recieved, etc
         */
        if(!VPI_STRCMP(*arg, "mpls")) {
            nvi->mpls = 1;
        }
        arg++;
        if(*arg) {
            /*
             * Set the label field
             */
            aim_strlcpy(nvi->ident, *arg, sizeof(nvi->ident));
        }
    }

    nvi->interface.send = vpi_pcapdump_interface_send;
    nvi->interface.recv = NULL;
    nvi->interface.recv_ready = NULL;
    nvi->interface.destroy = vpi_pcapdump_interface_destroy;
    nvi->interface.descriptor = NULL;

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

int
vpi_pcapdump_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    struct pcap_pkthdr pkthdr;

    VPI_MEMSET(&pkthdr, 0, sizeof(pkthdr));
    gettimeofday(&pkthdr.ts, NULL);

    /**
     * We use the first bit of tv_usec to indicate whether the packet
     * was sent or received.
     *
     * if tv_usec is odd, the packet was sent.
     * if tv_usec is even, the packet was received.
     */
    if(_vi->flags & VPI_INTERFACE_FLAG_SEND_LISTENING) {
        pkthdr.ts.tv_usec |= 1;
    }
    else if(_vi->flags & VPI_INTERFACE_FLAG_RECV_LISTENING) {
        pkthdr.ts.tv_usec &= ~1;
    }

    if(vi->mpls) {
        /*
         * All packets are encapsulated with MPLS.
         * The outer EtherII and MPLS information will
         * be used to indicate meta data about our VPI.
         */
        int nlen = len+22;
        unsigned char* ndata = aim_zmalloc(nlen);
        VPI_MEMSET(ndata, 0, nlen);
        VPI_MEMCPY(ndata+22, data, len);

        /*
         * Outer destination address is the 5 character identifier.
         */
        VPI_MEMCPY(ndata+1, vi->ident, 5);
        ndata[6] = 0;
        /*
         * Outer source address specifies whether the packet
         * was sent or received
         */
        if(_vi->flags & VPI_INTERFACE_FLAG_RECV_LISTENING) {
            VPI_MEMCPY(ndata+8, "RECV", 4);
        }
        else {
            VPI_MEMCPY(ndata+8, "SENT", 4);
        }
        /* MPLS Ethertype */
        ndata[12] = 0x88; ndata[13] = 0x47;
        /* MPLS Label -- currently unused */
        ndata[14] = 1;
        ndata[15] = 0;
        /* stack bit -- only one label at the moment */
        ndata[16] = 1;
        ndata[17] = 0xFF; /* ttl */
        /* PW Control Word -- currently unused */
        ndata[18] = 0;
        ndata[19] = 0;
        ndata[20] = 0;
        ndata[21] = 0;

        pkthdr.caplen=nlen;
        pkthdr.len=nlen;
        pcap_dump((u_char*)vi->pcap_dumper, &pkthdr, ndata);
        aim_free(ndata);
    }
    else {
        pkthdr.caplen = len;
        pkthdr.len = len;
        pcap_dump((u_char*)vi->pcap_dumper, &pkthdr, data);
    }
    pcap_dump_flush(vi->pcap_dumper);
    return 0;
}

int
vpi_pcapdump_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    pcap_dump_close(vi->pcap_dumper);
    pcap_close(vi->pcap);
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_PCAPDUMP */
