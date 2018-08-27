/****************************************************************
 *
 *        Copyright 2018, Big Switch Networks, Inc.
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

#if VPI_CONFIG_INCLUDE_INTERFACE_MMAP == 1

#define VPI_LOG_PREFIX1 ".mmap"
#include "vpi_log.h"

#include "vpi_interface_mmap.h"
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

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

static const char* mmap_interface_docstring__ =
    "----------------------------------------------------\n"
    "VPI Interface: MMAP \n"
    "----------------------------------------------------\n"
    "\n"
    "VPI endpoints using MMAP interfaces.\n"
    "\n"
    "Create format:\n"
    "    'mmap|INTERFACE'\n"
    "\n"
    "     Example:\n"
    "     'mmap|eth0'\n"
    "\n"
    "\n";

#define BLOCK_SIZE      (1 << 22) /* 4 MB */
#define FRAME_SIZE      (1 << 11) /* 2 KB */
#define NUM_BLOCKS      64
#define NUM_FRAMES      ((BLOCK_SIZE*NUM_BLOCKS)/FRAME_SIZE)

typedef struct frame_control_s {
    void* base;
} frame_control_t;

typedef struct ring_control_s {
    void* buf;
    frame_control_t frames[NUM_FRAMES];
    int current_frame;
} ring_control_t;

typedef struct vpi_interface_mmap_s {

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

    /* Our raw socket descriptor */
    int fd;

    /* Interface index of our assigned network interface */
    int ifindex;

    /* Interface name */
    char interface_name[IFNAMSIZ];

    /* Ring control */
    ring_control_t rx_ring;

} vpi_interface_mmap_t;

#define VICAST(_vi_name, _vpi_name) \
    vpi_interface_mmap_t* _vi_name = (vpi_interface_mmap_t*)(_vpi_name)


int
vpi_mmap_interface_register(void)
{
    /* Register our module as handling type 'udp' */
    return vpi_interface_register("mmap", vpi_mmap_interface_create, NULL,
                                mmap_interface_docstring__);
}


/**************************************************************************//**
 *
 * Send packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/
static int
sendto__(vpi_interface_mmap_t* vi, char* data, int len)
{
    struct sockaddr_ll sockaddr;

    VPI_MEMSET(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sll_family  = AF_PACKET;
    sockaddr.sll_ifindex = vi->ifindex;

    if (sendto(vi->fd, data, len, 0, (struct sockaddr *)&sockaddr,
               sizeof(sockaddr)) < 0) {
        VPI_ERROR(vi, "sendto() failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

/**************************************************************************//**
 *
 * Receive packet data on our socket. Assumes no fragmentation.
 *
 *
 *****************************************************************************/

static int
read__(vpi_interface_mmap_t* vi, char* data, int len)
{
    ring_control_t* rc;
    frame_control_t* fc;
    struct tpacket2_hdr* hdr;
    int size;
    bool tagged = false;
    void* copy_start;

    rc = &vi->rx_ring;
    fc = &rc->frames[rc->current_frame];
    hdr = (struct tpacket2_hdr*)fc->base;

    /* check min buffer size */
    if(len < 64) {
        return -1;
    }

    /* not ready */
    if(!(hdr->tp_status & TP_STATUS_USER)) {
        return 0;
    }

    size = hdr->tp_snaplen;
    /* check vlan tag */
    if((hdr->tp_status & TP_STATUS_VLAN_VALID) &&
       (hdr->tp_status & TP_STATUS_VLAN_TPID_VALID)) {
        size += 4;
        tagged = true;
    }

    size = (len < size) ? len : size;
    copy_start = (void*)hdr + hdr->tp_mac;
    if(tagged) {
        /* convert to network order */
        uint16_t tpid = htons(hdr->tp_vlan_tpid);
        uint16_t vlan = htons(hdr->tp_vlan_tci);

        /* L2 src and dst */
        VPI_MEMCPY(data, copy_start, 12);
        /* TPID */
        VPI_MEMCPY(data + 12, &tpid, 2);
        /* VLAN */
        VPI_MEMCPY(data + 14, &vlan, 2);
        /* Rest of packet */
        VPI_MEMCPY(data + 16, copy_start + 12, size - 16);
    } else {
        VPI_MEMCPY(data, copy_start, size);
    }
    VPI_INFO(vi, "packet received: %d bytes read", size);

    /* advance and flush */
    rc->current_frame = (rc->current_frame + 1)%NUM_FRAMES;
    hdr->tp_status = TP_STATUS_KERNEL;

    return size;
}

/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/

int
vpi_mmap_interface_create(vpi_interface_t** vi, char* args[], int flags,
                       const char* vpi_name_ptr)
{
    struct sockaddr_ll sockaddr;
    struct packet_mreq sockparams;
    struct tpacket_req treq;
    struct ifreq ifreq;
    vpi_interface_mmap_t* nvi = aim_zmalloc(sizeof(*nvi));
    char** arg = args;
    int version;
    int i;
    frame_control_t* fc;

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

    aim_strlcpy(nvi->interface_name, *arg, sizeof(nvi->interface_name));

    /* Create RAW socket */
    if((nvi->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        VPI_ERROR(nvi, "socket() failed: %s\n", strerror(errno));
        aim_free(nvi);
        return -1;
    }

    /* Set version */
    version = TPACKET_V2;
    if(setsockopt(nvi->fd, SOL_PACKET, PACKET_VERSION,
                  &version, sizeof(version)) < 0) {
        VPI_ERROR(nvi, "setsockopt() version failed: %s\n", strerror(errno));
        aim_free(nvi);
        return -1;
    }

    /*
     * Get the interface index for the requested interface, as specified
     * in the current argument.
     */
    VPI_MEMSET(&ifreq, 0, sizeof(ifreq));
    aim_strlcpy(ifreq.ifr_name, nvi->interface_name, IFNAMSIZ);
    if(ioctl(nvi->fd, SIOCGIFINDEX, &ifreq) < 0) {
        VPI_ERROR(nvi, "ioctl() failed: %s", strerror(errno));
        close(nvi->fd);
        aim_free(nvi);
        return -1;
    }
    nvi->ifindex = ifreq.ifr_ifindex;
    VPI_INFO(nvi, "ifndex is %d", nvi->ifindex);

    /* Set promisc */
    VPI_MEMSET(&sockparams, 0, sizeof(sockparams));
    sockparams.mr_type = PACKET_MR_PROMISC;
    sockparams.mr_ifindex = nvi->ifindex;
    if(setsockopt(nvi->fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
                  (void *)&sockparams, sizeof(sockparams)) < 0) {
        VPI_ERROR(nvi, "setsockopt() promisc failed. %s\n", strerror(errno));
        close(nvi->fd);
        aim_free(nvi);
        return -1;
    }

    /* Set up rx_ring buffer */
    VPI_MEMSET(&treq, 0, sizeof(treq));
    treq.tp_block_size = BLOCK_SIZE;
    treq.tp_frame_size = FRAME_SIZE;
    treq.tp_block_nr = NUM_BLOCKS;
    treq.tp_frame_nr = NUM_FRAMES;;
    if(setsockopt(nvi->fd, SOL_PACKET, PACKET_RX_RING,
                  (void*)&treq , sizeof(treq)) < 0) {
        VPI_ERROR(nvi, "setsockopt() rx_ring failed. %s\n", strerror(errno));
        close(nvi->fd);
        aim_free(nvi);
        return -1;
    }

    /* If num blocks change, bail! */
    if(treq.tp_block_nr != NUM_BLOCKS) {
        AIM_DIE("Unhandled: RX_RING block number changed!\n");
    }

    /* Set up rx_ring */
    nvi->rx_ring.buf = mmap(NULL, BLOCK_SIZE*NUM_BLOCKS,
                            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
                            nvi->fd, 0);
    if(nvi->rx_ring.buf == MAP_FAILED) {
        VPI_ERROR(nvi, "mmap() failed.\n");
        close(nvi->fd);
        aim_free(nvi);
        return -1;
    }

    /* Set up rx_ring and frame controls */
    nvi->rx_ring.current_frame = 0;
    for(i = 0; i < NUM_FRAMES; i++) {
        fc = &nvi->rx_ring.frames[i];
        fc->base = nvi->rx_ring.buf + (i*FRAME_SIZE);
    }

    /* Bind */
    VPI_MEMSET(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sll_family = AF_PACKET;
    sockaddr.sll_protocol = htons(ETH_P_ALL);
    sockaddr.sll_ifindex = nvi->ifindex;
    if(bind(nvi->fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        VPI_ERROR(nvi, "bind() failed.\n");
        close(nvi->fd);
        aim_free(nvi);
        return -1;
    }

    nvi->interface.send = vpi_mmap_interface_send;
    nvi->interface.recv = vpi_mmap_interface_recv;
    nvi->interface.recv_ready = vpi_mmap_interface_recv_ready;
    nvi->interface.destroy = vpi_mmap_interface_destroy;
    nvi->interface.descriptor = vpi_mmap_interface_descriptor;

    *vi = (vpi_interface_t*)nvi;
    return 0;
}

int
vpi_mmap_interface_send(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return sendto__(vi, (char*)data, len);
}

int
vpi_mmap_interface_recv(vpi_interface_t* _vi, unsigned char* data, int len)
{
    VICAST(vi, _vi);
    return read__(vi, (char*)data, len);
}

int
vpi_mmap_interface_recv_ready(vpi_interface_t* _vi)
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
vpi_mmap_interface_descriptor(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    return vi->fd;
}

int
vpi_mmap_interface_destroy(vpi_interface_t* _vi)
{
    VICAST(vi, _vi);
    close(vi->fd);
    aim_free(vi);
    return 0;
}

#endif /* INCLUDE_INTERFACE_MMAP */
