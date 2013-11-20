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

#include <uCli/ucli_config.h>
#include <uCli/ucli.h>
#include "ucli_int.h"


static int
ucli_hex_val__(char c)
{
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static char* builtin__ =
    "000000000001.000000000002.88DD.00112233445566778899AABBCCDDEEFF"
    "000000000000000000000000000000000000000000000000000000000000000"
    "00000000000000000000000000000000000000000000000000000DEADBEEFCAFE";

uint8_t*
ucli_util_data_from_string(const char* string, int* size)
{
    unsigned char* bytes;
    unsigned char byte;
    int i;
    int nibble_count;
    int string_len;

    if(string == NULL || size == NULL || string[0] == 0) {
        return NULL;
    }

    if(!UCLI_STRCMP(string, "NULL")) {
        *size = 0;
        return NULL;
    }

    if(!UCLI_STRCMP(string, "builtin")) {
        string = builtin__;
    }

    string_len = UCLI_STRLEN(string);

    if( (bytes = aim_zmalloc((string_len+1) / 2)) == NULL) {
        return NULL;
    }

    byte = 0;
    nibble_count = 0;

    for(i = 0; i < string_len; i++) {
        int hv = ucli_hex_val__(string[i]);
        if(hv >= 0) {
            byte |= hv;
            if( (nibble_count % 2) == 0) {
                byte <<= 4;
            }
            else {
                bytes[nibble_count/2] = byte;
                byte = 0;
            }
            nibble_count++;
        }
    }
    if(nibble_count & 1) {
        /* Last nibble will be padded as zero */
        bytes[nibble_count++/2] = byte;
    }

    *size = nibble_count / 2;
    return bytes;
}

char*
ucli_util_data_to_string(ucli_context_t* uc, uint8_t* data, int size,
                         int columns)
{
    int c;
    int i;

    /**
     * We will need at least 3*size for all bytes.
     * Each column requires 8 extra characters, including newline.
     *
     */
    int len = (size*3) + (columns*8) + 1;
    char* s = aim_zmalloc(len+1);
    char* sp = s;

    AIM_REFERENCE(uc);

    for(i = 0; i < size; i++) {
        if(i % columns == 0) {
            if(i != 0) {
                c = aim_snprintf(sp, len, "\n");
                sp += c, len -= c;
            }
            if(size > columns) {
                c = aim_snprintf(sp, len, "  %.4x: ", i);
                sp += c, len -= c;
            }
        }
        c = aim_snprintf(sp, len, "%.2x ", data[i]);
        sp += c, len -= c;
    }
    if((size > columns) && (size % columns != 0)) {
        c = aim_snprintf(sp, len, "\n");
        sp += c, len -= c;
    }
    return s;
}


ucli_tokens_t*
ucli_tokens(const char* string, const char* delim)
{
    const char** tokens;
    char* tok;
    int count;
    char* saveptr;
    char* s;
    ucli_tokens_t* rv;

    /*
     * Determine how many tokens we'll have
     */
    s = aim_strdup(string);

    count = 0;
    tok = aim_strtok_r(s, delim, &saveptr);
    while(tok) {
        count++;
        tok = aim_strtok_r(NULL, delim, &saveptr);
    }

    tokens = aim_zmalloc((sizeof(char*)*(count+2)));
    /* reset string */
    UCLI_STRCPY(s, string);
    tok = aim_strtok_r(s, delim, &saveptr);
    count = 0;
    while(tok) {
        tokens[count++] = tok;
        tok = aim_strtok_r(NULL, delim, &saveptr);
    }

    rv = aim_zmalloc(sizeof(*rv));
    rv->count = count;
    rv->tokens = tokens;
    rv->_string = s;

    return rv;
}

void
ucli_tokens_free(ucli_tokens_t* tokens)
{
    aim_free(tokens->_string);

    /*
     * The tokens array contains pointers into tokens->_string, so
     * we shouldn't free them, just the array containing them.
     */
    aim_free(tokens->tokens);

    aim_free(tokens);
}

void
ucli_biglist_free_element(void* data)
{
    aim_free(data);
}
