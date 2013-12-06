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

#include <PPE/ppe_config.h>

#if PPE_CONFIG_INCLUDE_UTM == 1

#include <PPE/uCli/ppe_utm.h>
#include <uCli/ucli_argparse.h>
#include "ppe_util.h"


/**
 * Cast and assign our control pointer in every command handler
 */
#undef UCLI_COMMAND_INIT
#define UCLI_COMMAND_INIT \
    AIM_VAR_PCAST_SAFE(ppe_utm_ctrl_t*, ppec, uc, uc->cookie); \
    AIM_REFERENCE(ppec)


static ucli_status_t
ppe_ucli_utm__config__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "config", 0,
                      "Show the PPE build configuration.");

    ppe_config_show(&uc->pvs);
    return 0;
}

static inline int
field_required__(ucli_context_t* uc, ppe_field_t f)
{
    AIM_VAR_PCAST_SAFE(ppe_utm_ctrl_t*, ppec, uc, uc->cookie);

    int e = ppe_field_exists(&ppec->ppep, f);
    if(e == 1) {
        return 0;
    }
    else if(e == 0) {
        return ucli_error(uc, "field %{ppe_field} does not exist in the packet.",
                          f);
    }
    else {
        return ucli_e_internal(uc, "ppe_field_exists() failed internally.");
    }
}


/**
 * Report an error and return if the given field does not exists
 */
#define PPE_FIELD_EXISTS_OR_RETURN(_uc, _f)             \
    do {                                                \
        int rv = field_required__(_uc, _f);             \
        if(rv < 0) {                                    \
            return rv;                                  \
        }                                               \
    } while(0)

/**
 * Report an error if field get fails.
 */
#define PPE_FIELD_GET_OR_RETURN(_uc, _ppep, _f, _rv)                    \
    do {                                                                \
        int rv = ppe_field_get(_ppep, _f, _rv);                         \
        if(rv < 0) {                                                    \
            return ucli_e_internal(_uc, "ppe_field_get(%{ppe_field})", _f); \
        }                                                               \
    } while(0)

/**
 * Report an error if field set fails.
 */
#define PPE_FIELD_SET_OR_RETURN(_uc, _ppep, _f, _sv)                    \
    do {                                                                \
        int rv = ppe_field_set(_ppep, _f, _sv);                         \
        if(rv < 0) {                                                    \
            return ucli_e_internal(_uc, "ppe_field_set(%{ppe_field}", _f); \
        }                                                               \
    } while(0)

/**
 * Report an error if a wide_field get fails
 */
#define PPE_WIDE_FIELD_GET_OR_RETURN(_uc, _ppep, _f, _rv)               \
    do {                                                                \
        int rv = ppe_wide_field_get(_ppep, _f, _rv);                    \
        if(rv < 0) {                                                    \
            return ucli_e_internal(_uc, "ppe_wide_field_get(%{ppe_field}", _f); \
        }                                                               \
    } while(0)

/**
 * Report an error if a wide_field set fails
 */
#define PPE_WIDE_FIELD_SET_OR_RETURN(_uc, _ppep, _f, _sv)               \
    do {                                                                \
        int rv = ppe_wide_field_set(_ppep, _f, _sv);                    \
        if(rv < 0) {                                                    \
            return ucli_e_internal(_uc, "ppe_wide_field_set(%{ppe_field})", _f); \
        }                                                               \
    } while(0)


/**
 * Report an error if a field is greater than 32 bits
 */
#define PPE_FIELD32_OR_RETURN(_uc, _size_bits)                          \
    do {                                                                \
        if(_size_bits > 32) {                                           \
            return ucli_error(_uc, "only fields that are 32 bits or less can be used with this command."); \
        }                                                               \
    } while(0)


static ucli_status_t
ppe_ucli_utm__update__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "update", -1,
                      "Update all dynamic/checksum fields in the packet.");

    ppe_packet_update(&ppec->ppep);
    return 0;
}

static ucli_status_t
ppe_ucli_utm__chm__(ucli_context_t* uc)
{
    ppe_header_t header;
    int bool;

    UCLI_COMMAND_INFO(uc,
                      "chm", 2,
                      "Check the header mask.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_header}{bool}", &header, &bool);
    if(bool && !(ppec->ppep.header_mask & (1 << header))) {
        return ucli_error(uc, "header bit %{ppe_header} is 0.", header);
    }
    if(!bool && (ppec->ppep.header_mask & (1 << header))) {
        return ucli_error(uc, "header bit %{ppe_header} is 1.", header);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__setheader__(ucli_context_t* uc)
{
    uint8_t* data;
    int size;
    ppe_header_t header;

    UCLI_COMMAND_INFO(uc,
                      "setheader", 2,
                      "Set header data.");


    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_header}{data}", &header, &data, &size);
    if(ppe_header_get(&ppec->ppep, header)) {
        aim_free(ppe_header_get(&ppec->ppep, header));
    }
    if(ppe_header_set(&ppec->ppep, header, data) < 0) {
        return ucli_e_internal(uc, "ppe_set_header()");
    }
    return UCLI_STATUS_OK;
}


static ucli_status_t
ppe_ucli_utm__format__(ucli_context_t* uc)
{
    ppe_header_t header;

    UCLI_COMMAND_INFO(uc,
                      "format", 1,
                      "Change the format of the current packet.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_header}", &header);
    if(header == PPE_HEADER_8021Q || header == PPE_HEADER_ETHERII) {
        int rv;
        rv = ppe_packet_format_set(&ppec->ppep, header);
        if(rv < 0) {
            return ucli_error(uc, "packet format conversion failed.");
        }
        if(rv == 1) {
            aim_free(ppec->ppep._data);
        }
    }
    else {
        return ucli_error(uc, "%{ppe_header} is not a valid conversion.",
                          header);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__fdump__(ucli_context_t* uc)
{
    ppe_field_t field;
    ppe_header_t format;


    UCLI_COMMAND_INFO(uc,
                      "fdump", 0,
                      "Dump all packet fields.");

    ppe_packet_format_get(&ppec->ppep, &format);
    ucli_printf(uc, "format=%{ppe_header}, size=%d\n",
                format, ppec->ppep.size);

    for(field = 0; field < PPE_FIELD_COUNT; field++) {
        const ppe_field_info_t* fi = ppe_field_info_get(field);

        if(fi->size_bits == 0) {
            continue;
        }
        if(ppe_field_exists(&ppec->ppep, field) == 0) {
            continue;
        }

        if(fi->size_bits <= 32) {
            uint32_t data;
            ppe_field_get(&ppec->ppep, field, &data);
            ucli_printf(uc, "%{ppe_field} = 0x%x (%d)\n",
                        field, data, data);
        }
        else {
            uint8_t* p = ppe_fieldp_get(&ppec->ppep, field);
            ucli_printf(uc, "%{ppe_field} = %{data}\n", field,
                        p, fi->size_bits/8);
        }
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__set__(ucli_context_t* uc)
{
    int data;
    ppe_field_info_t* fi;

    UCLI_COMMAND_INFO(uc,
                      "set", 2,
                      "Set a packet field.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_field_info}i", &fi, &data);
    PPE_FIELD32_OR_RETURN(uc, fi->size_bits);
    PPE_FIELD_EXISTS_OR_RETURN(uc, fi->field);
    PPE_FIELD_SET_OR_RETURN(uc, &ppec->ppep, fi->field, data);

    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__get__(ucli_context_t* uc)
{
    ppe_field_info_t* fi;

    UCLI_COMMAND_INFO(uc,
                      "get", 1,
                      "Get a packet field.");


    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_field_info}", &fi);

    PPE_FIELD_EXISTS_OR_RETURN(uc, fi->field);

    if(fi->size_bits <= 32) {
        uint32_t value;
        PPE_FIELD_GET_OR_RETURN(uc, &ppec->ppep, fi->field, &value);
        ucli_printf(uc, "%{ppe_field} = 0x%x (%d)\n",
                    fi->field, value, value);
        return UCLI_STATUS_OK;
    }
    else {
        int rv;
        int size = fi->size_bits/8;
        uint8_t* data = aim_zmalloc(size);
        rv = ppe_wide_field_get(&ppec->ppep, fi->field, data);
        if(rv < 0) {
            ucli_e_internal(uc, "ppe_wide_field_get(%{ppe_field})", fi->field);
        }
        else {
            ucli_printf(uc, "%{ppe_field} = %{data}\n", data, size);
        }
        aim_free(data);
        return rv;
    }
}

static ucli_status_t
ppe_ucli_utm__dump__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "dump", 0,
                      "Dump raw packet contents.");

    ucli_printf(uc, "%{data}", ppec->ppep.data, ppec->ppep.size);
    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__data__(ucli_context_t* uc)
{
    uint8_t* data;
    int size;

    UCLI_COMMAND_INFO(uc,
                      "data", 1,
                      "Assign packet data.");


    UCLI_ARGPARSE_OR_RETURN(uc, "{data}", &data, &size);

    if(ppec->ppep.data) {
        aim_free(ppec->ppep.data);
    }

    if(ppe_packet_init(&ppec->ppep, data, size) < 0) {
        return ucli_e_internal(uc, "ppe_packet_init()");
    }

    if(ppe_parse(&ppec->ppep) < 0) {
        return ucli_e_internal(uc, "ppe_parse()");
    }

    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__missing__(ucli_context_t* uc)
{
    ppe_field_t f;
    int rv;

    UCLI_COMMAND_INFO(uc,
                      "missing", 1,
                      "Check that a field is missing in the packet.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_field}", &f);
    rv = ppe_field_exists(&ppec->ppep, f);
    if(rv == 0) {
        return UCLI_STATUS_OK;
    }
    else if(rv == 1) {
        return ucli_error(uc, "field %{ppe_field} exists in the packet.",
                          f);
    }
    else {
        return ucli_e_internal(uc, "ppe_field_exists()");
    }
}

static ucli_status_t
ppe_ucli_utm__checkf__(ucli_context_t* uc)
{
    ppe_header_t cheader;
    ppe_header_t pheader;

    UCLI_COMMAND_INFO(uc,
                      "checkf", 1,
                      "Check the packet format.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_header}", &cheader);
    ppe_packet_format_get(&ppec->ppep, &pheader);
    if(pheader != cheader) {
        return ucli_error(uc, "packet format is currently %{ppe_header}.",
                          pheader);
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__check__(ucli_context_t* uc)
{
    ppe_field_info_t* fi;
    uint32_t cvalue;
    uint32_t pvalue;

    aim_datatype_map_t* operation;
    aim_datatype_map_t operation_map[] = {
        { "==", 'e' }, { "!=", 'n' }, { NULL }
    };

    UCLI_COMMAND_INFO(uc,
                      "check", 3,
                      "Check packet field values and status.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_field_info}{map}i", &fi,
                            &operation, operation_map, "operation",
                            &cvalue);

    PPE_FIELD_EXISTS_OR_RETURN(uc, fi->field);
    PPE_FIELD32_OR_RETURN(uc, fi->size_bits);
    PPE_FIELD_GET_OR_RETURN(uc, &ppec->ppep, fi->field, &pvalue);

    switch(operation->i)
        {
        case 'e':
            {
                if(pvalue != cvalue) {
                    return ucli_error(uc,
                                      "field %{ppe_field} is 0x%x (%d) (should be 0x%x (%d)",
                                      fi->field, pvalue, pvalue, cvalue, cvalue);
                }
                return UCLI_STATUS_OK;
                break;
            }
        case 'n':
            {
                if(pvalue == cvalue) {
                    return ucli_error(uc,
                                      "field %{ppe_field} is 0x%x (%d)",
                                      fi->field, pvalue, pvalue);
                }
                return UCLI_STATUS_OK;
                break;
            }
        default:
            return ucli_e_internal(uc, "unknown operation.");
        }
}

static ucli_status_t
ppe_ucli_utm__checkw__(ucli_context_t* uc)
{
    ppe_field_info_t* fi;
    uint8_t* cvalue;
    uint8_t pvalue[128];
    unsigned int csize;

    aim_datatype_map_t* operation;
    aim_datatype_map_t operation_map[] = {
        { "==", 'e' }, { "!=", 'n' }, { NULL }
    };

    UCLI_COMMAND_INFO(uc,
                      "checkw", 3,
                      "Check wide packet field values and status.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{ppe_field_info}{map}{data}", &fi,
                            &operation, operation_map, "operation",
                            &cvalue, &csize);

    PPE_FIELD_EXISTS_OR_RETURN(uc, fi->field);

    if(fi->size_bits/8 != csize) {
        return ucli_error(uc,
                          "field %{ppe_field} is %d bytes wide.", fi->field,
                          fi->size_bits/8);
    }

    PPE_WIDE_FIELD_GET_OR_RETURN(uc, &ppec->ppep, fi->field, pvalue);

    switch(operation->i)
        {
        case 'e':
            {
                if(PPE_MEMCMP(pvalue, cvalue, csize)) {
                    return ucli_error(uc,
                                      "field %{ppe_field} is %{data} (should be %{data}",
                                      fi->field, pvalue, csize, cvalue, csize);
                }
                return UCLI_STATUS_OK;
                break;
            }
        case 'n':
            {
                if(!PPE_MEMCMP(pvalue, cvalue, csize)) {
                    return ucli_error(uc,
                                      "field %{ppe_field} is %{data}",
                                      fi->field, pvalue, csize);
                }
                return UCLI_STATUS_OK;
                break;
            }
        default:
            return ucli_e_internal(uc, "unknown operation.");
        }
}

static ucli_status_t
ppe_ucli_utm__listf__(ucli_context_t* uc)
{
    ppe_field_info_t* fi;
    UCLI_COMMAND_INFO(uc,
                      "listf", 0,
                      "List known packet fields.");

    for(fi = ppe_field_info_table; fi->field != -1; fi++) {
        if(fi->size_bits != 0) {
            ucli_printf(uc, "%{ppe_field} sizebits=%d offset=%d shiftbits=%d\n",
                        fi->field, fi->size_bits, fi->offset_bytes, fi->shift_bits);
        }
    }
    return UCLI_STATUS_OK;
}

static ucli_status_t
ppe_ucli_utm__dfk__(ucli_context_t* uc)
{
    ppe_dfk_t dfk;
    ppe_field_t fields[4];
    uint8_t* verify_data;
    unsigned int verify_data_size;
    int rv = UCLI_STATUS_OK;
    unsigned int i;

    UCLI_COMMAND_INFO(uc,
                      "dfk", AIM_ARRAYSIZE(fields)+1,
                      "Generate and verify a dynamic field key.");
    UCLI_ARGPARSE_OR_RETURN(uc,
                            "{ppe_field}{ppe_field}{ppe_field}{ppe_field}{data}",
                            fields+0, fields+1, fields+2, fields+3,
                            &verify_data, &verify_data_size);

    ppe_dfk_init(&dfk, fields, AIM_ARRAYSIZE(fields));
    i = ppe_packet_dfk(&ppec->ppep, &dfk);

    if(i != verify_data_size) {
        rv = ucli_error(uc, "dfk size is %d, verify data size is %d",
                        i, verify_data_size);
        goto dfk_error;
    }

    for(i = 0; i < AIM_ARRAYSIZE(fields); i++) {
        const ppe_field_info_t* fi = ppe_field_info_get(fields[i]);
        int exists = ppe_field_exists(&ppec->ppep, fi->field);
        if(exists && ( (dfk.mask & (1<<i)) == 0)) {
            /* Should be in the field key but isn't.*/
            rv = ucli_error(uc, "%{ppe_field} exists in packet but not in field key.",
                            fi->field);
            goto dfk_error;
        }
        if(!(exists) && (dfk.mask & (1<<i))) {
            /* Should not be in the field key but is. */
            rv = ucli_error(uc, "%{ppe_field} is in the key but not the packet.",
                            fi->field);
            goto dfk_error;
        }

    }
    for(i = 0; i < verify_data_size; i++) {
        if(verify_data[i] != dfk.data[i]) {
            rv = ucli_error(uc, "key data mismatch at byte %d.\nkey=%{data}, verify=%{data}",
                            i, dfk.data, verify_data_size, verify_data, verify_data_size);
            goto dfk_error;
        }
    }
    for(i = 0; i < AIM_ARRAYSIZE(fields); i++) {
        if(dfk.mask & (1<<i)) {
            const ppe_field_info_t* fi = ppe_field_info_get(fields[i]);
            if(fi->size_bits <= 32) {
                uint32_t pdata;
                uint32_t kdata;
                ppe_field_get(&ppec->ppep, fi->field, &pdata);
                ppe_dfk_field_get(&dfk, fi->field, &kdata);
                if(pdata != kdata) {
                    rv = ucli_error(uc, "field_get mismatch: p=0x%x, k=0x%x");
                    goto dfk_error;
                }
            }
            else {
                unsigned int i;
                uint8_t pdata[128];
                uint8_t kdata[128];

                ppe_wide_field_get(&ppec->ppep, fi->field, pdata);
                ppe_dfk_wide_field_get(&dfk, fi->field, kdata);
                for(i = 0; i < fi->size_bits/8; i++) {
                    if(pdata[i] != kdata[i]) {
                        rv = ucli_error(uc, "wide_field_get mismatch @ %d: p=0x%x k=0x%x",
                                        i, pdata[i], kdata[i]);
                        goto dfk_error;
                    }
                }
            }
        }
    }

    aim_free(verify_data);
    ppe_dfk_destroy(&dfk);
    return UCLI_STATUS_OK;

 dfk_error:
    ucli_printf(uc, "key: ");
    ppe_dfk_show(&dfk, &uc->pvs);
    ppe_dfk_destroy(&dfk);
    aim_free(verify_data);
    ucli_printf(uc, "\n");
    return rv;
}

static ucli_status_t
ppe_ucli_utm__rwall__(ucli_context_t* uc)
{
    ppe_packet_t ppep;
    ppe_header_t header;
    ppe_field_t f;
    int rv = UCLI_STATUS_OK;

    UCLI_COMMAND_INFO(uc,
                      "rwall", 0,
                      "Read and write all packet fields in all headers.");

    ppe_packet_init(&ppep, NULL, 0);

    /**
     * Allocate and assign a header pointer for every header type.
     * All bits will be initialized to 1.
     */
    for(header = 0; header < PPE_HEADER_COUNT; header++) {
        uint8_t* hp = aim_zmalloc(1000);
        PPE_MEMSET(hp, 0xFF, 1000);
        ppe_header_set(&ppep, header, hp);
    }

    /**
     * Check that every field reads back as all 1's, with the correct width
     */
    for(f = 0; f < PPE_FIELD_COUNT; f++) {
        const ppe_field_info_t* fi = ppe_field_info_get(f);
        if(fi->size_bits == 0) {
            continue;
        }
        if(fi->size_bits <= 32) {
            uint32_t v;
            ppe_field_get(&ppep, f, &v);
            if(fi->size_bits == 32) {
                if(v != 0xFFFFFFFF)  {
                    rv = ucli_error(uc, "first read: field %{ppe_field} is 0x%x, should be 0x%x",
                                    f, v, -1);
                }
            }
            else {
                if(v != ( (1U << fi->size_bits) - 1)) {
                    rv = ucli_error(uc, "first read: field %{ppe_field} is 0x%x, should be 0x%x (%d bits)",
                                    f, v, (1<<fi->size_bits) - 1, fi->size_bits);
                }
            }
            /** clear field and re-read */
            ppe_field_set(&ppep, f, 0);
            ppe_field_get(&ppep, f, &v);
            if(v != 0) {
                rv = ucli_error(uc, "second read: field %{ppe_field} is 0x%x when it should be 0.",
                                f, v);
            }
        }
        else {
            uint8_t vb[1024];
            int bytes = ppe_wide_field_get(&ppep, f, vb);
            int i;
            for(i = 0; i < bytes; i++) {
                if(vb[i] != 0xFF) {
                    rv = ucli_error(uc, "first read: field %{ppe_field}[%d] is 0x%.2x, should be 0x%.2x",
                                    f, i, vb[i], 0xFF);
                }
            }
            PPE_MEMSET(vb, 0, sizeof(vb));
            /** clear field and re-read */
            ppe_wide_field_set(&ppep, f, NULL);
            PPE_MEMSET(vb, 0xFF, sizeof(vb));
            ppe_wide_field_get(&ppep, f, vb);
            for(i = 0; i < bytes; i++) {
                if(vb[i] != 0) {
                    rv = ucli_error(uc, "second read: field %{ppe_field}[%d] is 0x%.2x, should be 0.",
                                    f, i, vb[i]);
                }
            }
        }

        /** continue reading other fields, making sure the field we just cleared
         * does not change the value of fields we have not yet visited. */

    }

    for(header = 0; header < PPE_HEADER_COUNT; header++) {
        aim_free(ppe_header_get(&ppep, header));
    }
    return rv;
}

/* <auto.ucli.handlers.start> */
/******************************************************************************
 *
 * These handler table(s) were autogenerated from the symbols in this
 * source file.
 *
 *****************************************************************************/
static ucli_command_handler_f ppe_ucli_utm_handlers__[] =
{
    ppe_ucli_utm__config__,
    ppe_ucli_utm__update__,
    ppe_ucli_utm__chm__,
    ppe_ucli_utm__setheader__,
    ppe_ucli_utm__format__,
    ppe_ucli_utm__fdump__,
    ppe_ucli_utm__set__,
    ppe_ucli_utm__get__,
    ppe_ucli_utm__dump__,
    ppe_ucli_utm__data__,
    ppe_ucli_utm__missing__,
    ppe_ucli_utm__checkf__,
    ppe_ucli_utm__check__,
    ppe_ucli_utm__checkw__,
    ppe_ucli_utm__listf__,
    ppe_ucli_utm__dfk__,
    ppe_ucli_utm__rwall__,
    NULL
};
/******************************************************************************/
/* <auto.ucli.handlers.end> */


static void
ppe_utm_module_destroy__(ucli_module_t* module)
{
    ppe_utm_ctrl_t* ppec = (ppe_utm_ctrl_t*)(module->cookie);
    if(ppec->ppep.data) {
        aim_free(ppec->ppep.data);
    }
    aim_free(ppec);
}

static ucli_module_t
ppe_utm_module__ =
    {
        "ppe_utm",
        NULL,
        ppe_ucli_utm_handlers__,
        NULL,
        ppe_utm_module_destroy__
    };


ucli_module_t*
ppe_utm_create(void)
{
    ppe_utm_ctrl_t* ppec = aim_zmalloc(sizeof(*ppec));
    ppe_utm_module__.cookie = ppec;
    ucli_module_init(&ppe_utm_module__);
    return &ppe_utm_module__;
}


#endif /* PPE_CONFIG_INCLUDE_UTM */
