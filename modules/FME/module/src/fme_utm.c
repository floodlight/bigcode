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

#include <FME/fme_config.h>

#if FME_CONFIG_INCLUDE_UTM == 1

#include <FME/uCli/fme_utm.h>
#include <uCli/ucli.h>
#include <uCli/ucli_argparse.h>
#include <OS/os_time.h>

/**
 * Cast and assign our control pointer in every command handler
 */
#undef UCLI_COMMAND_INIT
#define UCLI_COMMAND_INIT \
    AIM_VAR_PCAST_SAFE(fme_utm_ctrl_t*, fmec, uc, uc->cookie); \
    AIM_REFERENCE(fmec)


static ucli_status_t
fme_ucli_utm__config__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "config", 0,
                      "Show the FME build configuration.");

    fme_config_show(&uc->pvs);
    return 0;
}

static ucli_status_t
fme_ucli_utm__entry__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "entry", 1,
                      "Select an entry.");

    fmec->eid = 0;
    UCLI_ARGPARSE_OR_RETURN(uc, "{rint}",
                            &fmec->eid, 0, FME_CONFIG_UTM_ENTRIES, "entry id");


    if(fmec->entries[fmec->eid] == NULL) {
        fme_entry_create(fmec->entries+fmec->eid);
        fmec->entries[fmec->eid]->prio = fmec->eid;
        fmec->entries[fmec->eid]->timestamp = 1;
        fme_add_entry(fmec->fme, fmec->entries[fmec->eid]);
    }

    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__expect__(ucli_context_t* uc)
{
    int eid;
    UCLI_COMMAND_INFO(uc,
                      "expect", 1,
                      "Expect a match on the entry.");

    UCLI_ARGPARSE_OR_RETURN(uc, "{rint}",
                            &eid, 0, FME_CONFIG_UTM_ENTRIES, "entry id");
    fmec->expects[eid] = 1;
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__dump__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "dump", 0,
                      "Dump the FME.");

    fme_dump(fmec->fme, &uc->pvs);
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__edump__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "edump", 0,
                      "Dump the current FME entry.");

    fme_entry_dump(fmec->entries[fmec->eid], &uc->pvs);
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__remove__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "remove", 0,
                      "Remove the current entry.");

    fme_remove_entry(fmec->fme, fmec->entries[fmec->eid]);
    fme_entry_destroy(fmec->entries[fmec->eid]);
    fmec->entries[fmec->eid] = NULL;
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__enable__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "enable", 0,
                      "Enable the current entry.");

    fmec->entries[fmec->eid]->enabled = 1;
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__disable__(ucli_context_t* uc)
{
    UCLI_COMMAND_INFO(uc,
                      "disable", 0,
                      "Disable the current entry.");

    fmec->entries[fmec->eid]->enabled = 0;
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__key__(ucli_context_t* uc)
{
    uint32_t keymask;
    uint8_t* value = NULL;
    int value_size = 0;
    uint8_t* mask = NULL;
    int mask_size = 0;
    int rv = UCLI_STATUS_OK;
    fme_key_t key;

    UCLI_COMMAND_INFO(uc,
                      "key",  3,
                      "Set the match key for the current entry.");


    UCLI_ARGPARSE_OR_RETURN(uc, "i{data}{data}", &keymask, &value, &value_size,
                            &mask, &mask_size);

    if(value_size != mask_size) {
        rv = ucli_error(uc, "value size (%d) and mask size (%d) must be equal.",
                        value_size, mask_size);
        goto __key__error;
    }
    if(value_size % 4 != 0) {
        rv = ucli_error(uc, "values and masks must be 32bit aligned (size=%d)",
                        value_size);
        goto __key__error;
    }


    FME_MEMSET(&key, 0, sizeof(key));
    key.keymask = keymask;
    key.size = value_size;
    FME_MEMCPY(key.values, value, value_size);
    FME_MEMCPY(key.masks, mask, mask_size);
    aim_free(value);
    aim_free(mask);
    fme_entry_key_set(fmec->entries[fmec->eid], &key);

    return rv;

 __key__error:

    aim_free(value);
    aim_free(mask);

    return rv;
}

static ucli_status_t
fme_ucli_utm__match__(ucli_context_t* uc)
{
    int rv;
    int i;

    uint32_t keymask;
    uint8_t* value;
    int value_size;
    int _now;
    fme_timeval_t now;
    unsigned int size;
    fme_entry_t* fe = NULL;
    fme_key_t key;

    UCLI_COMMAND_INFO(uc,
                      "match", 4,
                      "Run the key and data.");

    UCLI_ARGPARSE_OR_RETURN(uc, "i{data}ii", &keymask, &value, &value_size,
                            &size, &_now);
    now = _now;
    FME_MEMSET(&key, 0, sizeof(key));
    key.keymask = keymask;
    FME_MEMCPY(key.values, value, value_size);
    key.size = value_size;
    FME_MEMSET(key.masks, 0, sizeof(key.masks));
    aim_free(value);

    rv = fme_match(fmec->fme, &key, now, size, &fe);

    if(rv == 1) {
        if(fmec->expects[fe->prio] == 0) {
            /* Unexpected match */
            ucli_error(uc, "unexpected match: ");
            fme_entry_dump(fe, &uc->pvs);
            goto __match__error;
        }

        /* match is expected -- check counter values */
        if(fe->counters.matches != 1) {
            ucli_error(uc, "match counter was not incremented.");
            goto __match__error;
        }
        if(fe->counters.bytes != size) {
            ucli_error(uc, "byte counter is incorrect.");
            goto __match__error;
        }
        if(fe->timestamp != now) {
            ucli_error(uc, "timestamp was not updated.");
            goto __match__error;
        }

        /* all good */
    }
    else if(rv == 0) {
        /* Make sure we have to entries expected to match */
        for(i = 0; i < FME_CONFIG_UTM_ENTRIES; i++) {
            if(fmec->expects[i]) {
                ucli_error(uc, "expected match was not received: ");
                fme_entry_dump(fmec->entries[i], &uc->pvs);
                goto __match__error;
            }
        }
        /* all good */
    }
    else {
        ucli_e_internal(uc, "fme_match() returned %d", rv);
        goto __match__error;
    }

    /* all good - reset */
    fme_destroy_all(fmec->fme);
    FME_MEMSET(fmec, 0, sizeof(*fmec));
    fme_create(&fmec->fme, "fme_utm", FME_CONFIG_UTM_ENTRIES);
    return UCLI_STATUS_OK;

 __match__error:

    ucli_error(uc, "fme: ");
    fme_dump(fmec->fme, &uc->pvs);
    return UCLI_STATUS_E_ERROR;
}

static ucli_status_t
fme_ucli_utm__absolutetimeout(ucli_context_t* uc)
{
    int to;

    UCLI_COMMAND_INFO(uc,
                      "absolute-timeout", 1,
                      "Set the absolute timeout on the current entry.");


    UCLI_ARGPARSE_OR_RETURN(uc, "i", &to);
    fmec->entries[fmec->eid]->absolute_timeout=to;
    return UCLI_STATUS_OK;
}
static ucli_status_t
fme_ucli_utm__relativetimeout(ucli_context_t* uc)
{
    int to;
    UCLI_COMMAND_INFO(uc,
                      "relative-timeout", 1,
                      "Set the relative timeout on the current entry.");
    UCLI_ARGPARSE_OR_RETURN(uc, "i", &to);
    fmec->entries[fmec->eid]->relative_timeout=to;
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__timestamp(ucli_context_t* uc)
{
    int to;
    UCLI_COMMAND_INFO(uc,
                      "timestamp", 1,
                      "Set the timestamp on the current entry.");

    UCLI_ARGPARSE_OR_RETURN(uc, "i", &to);
    fmec->entries[fmec->eid]->timestamp=to;
    return UCLI_STATUS_OK;
}

static ucli_status_t
fme_ucli_utm__perf__(ucli_context_t* uc)
{
    int set_keymask;
    int table_size;
    int iterations;
    int i;
    fme_key_t mkey;
    fme_entry_t* match;
    fme_t* fme;
    double seconds;
    int keysize;
    uint64_t start, end;

    UCLI_COMMAND_INFO(uc,
                      "perf", 3,
                      "$summary#Run performance matching tests."
                      "$args#<table_size> <iterations> <use_keymask>");


    if(aim_valgrind_status() == 1) {
        ucli_printf(uc, "Skipping performance test while running under valgrind.\n");
        return UCLI_STATUS_OK;
    }

    UCLI_ARGPARSE_OR_RETURN(uc, "iib", &table_size, &iterations, &set_keymask);

    fme_create(&fme, "fme_utm", table_size);

    keysize = sizeof(mkey.values);

    for(i = 0; i < table_size; i++) {
        fme_entry_t* entry;
        fme_key_t key;
        uint8_t byte = (i == 0 || i & 0xFF) ? i : 0x1;

        FME_MEMSET(&key, 0, sizeof(key));
        FME_MEMSET(&key.values, byte, sizeof(key.values));
        FME_MEMSET(&key.masks, byte, sizeof(key.masks));
        key.size = keysize;

        if(set_keymask) {
            key.keymask = i;
        }

        fme_entry_create(&entry);
        fme_entry_key_set(entry, &key);
        entry->prio=i;
        fme_add_entry(fme, entry);
    }

    FME_MEMSET(&mkey, 0, sizeof(mkey));
    mkey.size = keysize;


    start = os_time_thread();
    for(i = 0; i < iterations; i++) {
        int rv = fme_match(fme, &mkey, 0, 0, &match);
        /* The lowest priority entry will always match */
        if(rv != 1 || match->index != table_size-1){
            return ucli_printf(uc, "i=%d error: rv=%d, index=%d\n", i, rv,
                               (match) ? match->index : -1);
        }
    }
    end = os_time_thread();
    seconds = (end - start) / (1000.0*1000);
    ucli_printf(uc, "%d matches in %f seconds (%f matches/sec)\n", iterations, seconds,
                iterations/seconds);

    fme_destroy_all(fme);
    return UCLI_STATUS_OK;
}

/* <auto.ucli.handlers.start> */
/******************************************************************************
 *
 * These handler table(s) were autogenerated from the symbols in this
 * source file.
 *
 *****************************************************************************/
static ucli_command_handler_f fme_ucli_utm_handlers__[] =
{
    fme_ucli_utm__config__,
    fme_ucli_utm__entry__,
    fme_ucli_utm__expect__,
    fme_ucli_utm__dump__,
    fme_ucli_utm__edump__,
    fme_ucli_utm__remove__,
    fme_ucli_utm__enable__,
    fme_ucli_utm__disable__,
    fme_ucli_utm__key__,
    fme_ucli_utm__match__,
    fme_ucli_utm__absolutetimeout,
    fme_ucli_utm__relativetimeout,
    fme_ucli_utm__timestamp,
    fme_ucli_utm__perf__,
    NULL
};
/******************************************************************************/
/* <auto.ucli.handlers.end> */

static void
fme_utm_module_destroy__(ucli_module_t* module)
{
    fme_utm_ctrl_t* fmec = (fme_utm_ctrl_t*)(module->cookie);
    fme_destroy_all(fmec->fme);
    aim_free(fmec);
}

static ucli_module_t
fme_utm_module__ =
    {
        "fme_utm",
        NULL,
        fme_ucli_utm_handlers__,
        NULL,
        fme_utm_module_destroy__,
    };


ucli_module_t*
fme_utm_create(void)
{
    fme_utm_ctrl_t* fmec = aim_zmalloc(sizeof(*fmec));

    fme_create(&fmec->fme, "fme_utm", FME_CONFIG_UTM_ENTRIES);
    fme_utm_module__.cookie = fmec;
    ucli_module_init(&fme_utm_module__);
    return &fme_utm_module__;
}

#endif /* FME_CONFIG_INCLUDE_UTM */



