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

#ifndef __UCLI_INT_H__
#define __UCLI_INT_H__


#include <uCli/ucli_config.h>
#include <uCli/ucli_porting.h>
#include <uCli/ucli_common.h>
#include <uCli/ucli_module.h>
#include <uCli/ucli_node.h>
#include <uCli/ucli.h>
#include <uCli/ucli_errors.h>
#include <IOF/iof.h>
#include "ucli_util.h"

#define UCLI_LOG_OBJ_DEFAULT

#include "ucli_log.h"


/**
 * Tokenizer
 */
typedef struct ucli_tokens_s {
    /** total token count */
    int count;
    /** tokens */
    const char** tokens;
    /** Internal */
    char* _string;
} ucli_tokens_t;


ucli_tokens_t* ucli_tokens(const char* string, const char* delim);

void ucli_tokens_free(ucli_tokens_t* tokens);

void ucli_biglist_free_element(void* data);

#endif /* __UCLI_INT_H__ */
