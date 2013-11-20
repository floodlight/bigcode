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
/************************************************************//**
 *
 * vpi_parser.c
 *
 * Generic parsing utilities
 *
 *
 ***************************************************************/
#include "vpi_int.h"
#include "vpi_parser.h"

#include <BigList/biglist.h>

/**************************************************************************//**
 *
 * Parse a string based on the specified delimiter.
 *
 *
 *****************************************************************************/
biglist_t*
vpi_parse_list(const char* string, char* delim, int* len)
{
    biglist_t* bl = NULL;

    /* Internal token buffer */
    char* _tokens = aim_strdup(string);
    char* arg;
    char* save_ptr;
    int count = 0;

    arg = aim_strtok_r(_tokens, delim, &save_ptr);
    while(arg) {
        bl = biglist_append(bl, aim_strdup(arg));
        arg = aim_strtok_r(NULL, delim, &save_ptr);
        count++;
    }

    aim_free(_tokens);
    if(len) {
        *len = count;
    }
    return bl;
}

char**
vpi_parse_array(const char* string, char* delim, int* len)
{
    char** rv;
    biglist_t* bl;

    bl = vpi_parse_list(string, delim, len);
    /* We want the argument array to be null terminated */
    bl = biglist_append(bl, NULL);
    rv = (char**)biglist_to_data_array(bl, NULL);
    biglist_free(bl);
    return rv;
}

void
vpi_parse_arrayFree(char** a)
{
    char** s;
    if(a) {
        for(s = a; *s; s++) {
            aim_free(*s);
        }
        aim_free(a);
    }
}


vpi_ip_endpoint_t*
vpi_parse_ip_endpoint(const char* string)
{
    vpi_ip_endpoint_t* ne;
    char** tokens;
    int len = 0;
    char** arg;

    if( (tokens = vpi_parse_array(string, ":", &len)) == NULL) {
        return NULL;
    }

    if(len != 2 && len != 3) {
        /* Need between 2 and 3 tokens only */
        vpi_parse_arrayFree(tokens);
        return NULL;
    }

    arg = tokens;

    /*
     * First token must be 'send', or 's',  or 'recv', or 'r'
     */
    if(VPI_STRCMP(*arg, "send") &&
       VPI_STRCMP(*arg, "recv") &&
       VPI_STRCMP(*arg, "r") &&
       VPI_STRCMP(*arg, "s")) {
        /* None of them */
        vpi_parse_arrayFree(tokens);
        return NULL;
    }

    ne = aim_zmalloc(sizeof(*ne));
    ne->direction = aim_strdup(*arg);
    arg++;

    if(len == 2) {
        /* The argument must be a port number. */
        ne->port = VPI_ATOI(*arg);
        if(ne->port < 0) {
            /* Invalid */
            goto vpi_parse_ip_endpoint_Error;
        }
        else {
            /* localhost is implied if not specified */
            ne->host = aim_strdup("0.0.0.0");
        }
    }
    else {
        ne->host = aim_strdup(*arg);
        arg++;
        ne->port = VPI_ATOI(*arg);
        if(ne->port <= 0) {
            /* Invalid */
            goto vpi_parse_ip_endpoint_Error;
        }
    }

    /* Success - 'ne' is now initialized */
    vpi_parse_arrayFree(tokens);
    return ne;

 vpi_parse_ip_endpoint_Error:
    vpi_parse_arrayFree(tokens);
    vpi_free_ip_endpoint(&ne);
    return NULL;
}

int
vpi_parse_send_recv_ip_endpoints(char** args, vpi_ip_endpoint_t** recv_endpoint,
                            vpi_ip_endpoint_t** send_endpoint)
{
    int i;
    vpi_ip_endpoint_t* ep;

    if(recv_endpoint == NULL || send_endpoint == NULL) {
        return -1;
    }

    *recv_endpoint = NULL;
    *send_endpoint = NULL;

    for(i = 0; args[i]; i++) {
        ep = vpi_parse_ip_endpoint(args[i]);
        if(ep == NULL) {
            goto vpi_parse_send_recv_ip_endpoints_Error;
        }
        if(ep->direction[0] == 'r') {
            /* Recv endpoint specified */
            if(*recv_endpoint) {
                /* already specified */
                goto vpi_parse_send_recv_ip_endpoints_Error;
            }
            else {
                *recv_endpoint = ep;
            }
        }
        else {
            /* Send endpoint specified */
            if(*send_endpoint) {
                /* already specified */
                goto vpi_parse_send_recv_ip_endpoints_Error;
            }
            else {
                *send_endpoint = ep;
            }
        }
    }

    /* Success */
    return 0;


 vpi_parse_send_recv_ip_endpoints_Error:

    if(ep) {
        vpi_free_ip_endpoint(&ep);
    }

    if(*recv_endpoint) {
        vpi_free_ip_endpoint(recv_endpoint);
    }

    if(*send_endpoint) {
        vpi_free_ip_endpoint(send_endpoint);
    }

    return -1;
}

void
vpi_free_ip_endpoint(vpi_ip_endpoint_t** p)
{
    if(p && *p) {
        if((*p)->direction) {
            aim_free((*p)->direction);
        }
        if((*p)->host) {
            aim_free((*p)->host);
        }
        aim_free(*p);
        *p = NULL;
    }
}
