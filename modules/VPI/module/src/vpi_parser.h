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
/******************************************************************************
 *
 * String and create spec argument parsing utilities.
 *
 *
 *****************************************************************************/
#ifndef __VPI_PARSER_H__
#define __VPI_PARSER_H__

#include "vpi_int.h"

/******************************************************************************
 *
 * Tokenize a string and return the results in a list.
 *
 *
 *****************************************************************************/
biglist_t* vpi_parse_list(const char* string, char* delim, int* len);

/******************************************************************************
 *
 * Tokenize a string and return the results in NULL-terminated
 * argument array.
 *
 * You must free the pointers and array when finished.
 *
 *****************************************************************************/
char** vpi_parse_array(const char* string, char* delim, int* len);
void vpi_parse_arrayFree(char** a);


/******************************************************************************
 *
 * Tokenize an argument of the form:
 *
 * [send|recv]:host:port.
 *
 * Utility function provided for IP-based VPI endpoints.
 *
 *****************************************************************************/
typedef struct vpi_ip_endpoint_s {

    char* direction;
    char* host;
    int port;

} vpi_ip_endpoint_t;

vpi_ip_endpoint_t* vpi_parse_ip_endpoint(const char* string);
void vpi_free_ip_endpoint(vpi_ip_endpoint_t** p);


/******************************************************************************
 *
 * Parse and return:
 *      1. At most one recv endpoint argument.
 *      2. At most one send endpoint argument.
 *
 * Any remaining arguments are an error.
 *
 * Endpoints should be freed by the caller.
 *
 *
 *****************************************************************************/

int
vpi_parse_send_recv_ip_endpoints(char** args, vpi_ip_endpoint_t** recv_endpoint,
                            vpi_ip_endpoint_t** send_endpoint);




#endif /* __VPI_PARSER_H__ */
