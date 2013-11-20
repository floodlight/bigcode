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
#include <FME/fme.h>
#include "fme_int.h"
#include <stdlib.h>
#include <IOF/iof.h>
#include "fme_log.h"

static int fme_key_dump_default__(fme_key_t* key, aim_pvs_t* ap);

int
fme_create(fme_t** rv, const char* name, int max_entries)
{
    fme_t* fme = aim_zmalloc(sizeof(*fme));
    fme->max_entries = max_entries;
    fme->num_entries = 0;
    fme->entries = aim_zmalloc(sizeof(fme_entry_t*)*max_entries);
    fme->log_string = aim_strdup(name ? name : "None");
    *rv = fme;
    return 0;
}

void
fme_destroy_all(fme_t* fme)
{
    int i;
    for(i = 0; i < fme->num_entries; i++) {
        fme_entry_destroy(fme->entries[i]);
    }
    if(fme->log_string) {
        aim_free((void*)fme->log_string);
    }
    fme_destroy(fme);
}

void
fme_destroy(fme_t* fme)
{
    aim_free(fme->entries);
    aim_free(fme);
}


int
fme_entry_create(fme_entry_t** rv)
{
    fme_entry_t* fe;
    AIM_LOG_FENTER();
    fe = aim_zmalloc(sizeof(*fe));
    fe->enabled = 1;
    fe->prio = -1;
    *rv = fe;
    AIM_LOG_FEXIT();
    return 0;
}

void
fme_entry_destroy(fme_entry_t* entry)
{
    aim_free(entry);
}


int
fme_entry_key_set(fme_entry_t* entry, fme_key_t* key)
{
    FME_MEMCPY(&entry->key, key, sizeof(*key));
    if(entry->key.dumper == NULL) {
        entry->key.dumper = fme_key_dump_default__;
    }
    return 0;
}

int
fme_entry_key_get(fme_entry_t* entry, fme_key_t* key)
{
    FME_MEMCPY(key, &entry->key, sizeof(*key));
    return 0;
}

int
fme_entry_timeout_status(fme_entry_t* entry, fme_timeval_t now)
{
    if( (entry->timestamp != 0) && /* Timestamp must be valid */
        (now != 0) ) {             /* Current time must be valid */

        /* Absolute timeout check */
        if( (entry->absolute_timeout != 0) && /* Absolute timeout enabled */
            (now >= entry->absolute_timeout) ) { /* Entry too old */
            /* This entry has timed out */
            return 1;
        }

        /* Relative timeout check */
        if( (entry->relative_timeout != 0) && /* Relative timeout enabled */
            ( (now-entry->timestamp) > entry->relative_timeout) ) {
            /* This entry has timed out */
            return 1;
        }
    }
    return 0;
}

int
fme_entry_timeouts(fme_t* fme, fme_timeval_t now, biglist_t** list)
{
    biglist_t* rv = NULL;
    int i;
    int count = 0;

    if(list == NULL || now == 0) {
        return 0;
    }
    for(i = 0; fme->num_entries; i++) {
        if(fme_entry_timeout_status(fme->entries[i], now)) {
            rv = biglist_prepend(rv, fme->entries[i]);
            count++;
        }
    }
    *list = rv;
    return count;
}

static void
fme_update_indexes__(fme_t* fme)
{
    int i;
    for(i = 0; i < fme->num_entries; i++) {
        fme->entries[i]->index = i;
    }
}

int
fme_add_entry(fme_t* fme, fme_entry_t* entry)
{
    int i;

    if(fme->num_entries == fme->max_entries) {
        return -1;
    }

    if(fme->num_entries == 0) {
        fme->entries[fme->num_entries++] = entry;
        return 0;
    }

    if(fme->entries[fme->num_entries-1]->prio > entry->prio) {
        fme->entries[fme->num_entries] = entry;
        entry->index = fme->num_entries++;
    }
    else {
        /* todo -- binary search for index insertion if this is kept at all */
        i = 0;
        while(fme->entries[i]->prio > entry->prio) {
            i++;
        }
        FME_MEMMOVE(fme->entries+i+1, fme->entries+i, (fme->num_entries-i)*sizeof(fme_entry_t*));
        fme->entries[i] = entry;
        entry->index = i;
        ++fme->num_entries;
        fme_update_indexes__(fme);
    }
    return 0;
}

int
fme_remove_entry(fme_t* fme, fme_entry_t* entry)
{
    if(fme->entries[entry->index] == entry) {
        int msize = fme->num_entries - entry->index - 1;
        --fme->num_entries;
        if(msize) {
            FME_MEMMOVE(fme->entries+(entry->index),
                        fme->entries+(entry->index+1),
                        msize*sizeof(fme_entry_t*));
            fme_update_indexes__(fme);
        }
    }
    else {
        AIM_LOG_WARN("entry not found");
    }
    return 0;
}

static int
fme_key_match__(fme_key_t* value, fme_key_t* matchkey)
{
    int i;
    uint32_t* vp;
    uint32_t* mvp;
    uint32_t* mmp;

    if(value->size != matchkey->size) {
        /** This is an error at this point */
        return -1;
    }

    /** The incoming value must have a keymask that is a superset
     * of the match key.
     */
    if( (value->keymask & matchkey->keymask) != matchkey->keymask) {
        /* No match possible */
        return 0;
    }

    /* value word */
    vp = (uint32_t*) value->values;
    /* key value words */
    mvp = (uint32_t*) matchkey->values;
    /* key value masks */
    mmp = (uint32_t*) matchkey->masks;

    for(i = 0; i < value->size/4; i++) {
        if( (vp[i] & mmp[i]) != mvp[i] ) {
            /* No match */
            return 0;
        }
    }
    return 1;
}

static
int fme_entry_match__(fme_entry_t* entry, fme_key_t* key, fme_timeval_t now,
                      int size)
{
    if(entry->enabled == 0) {
        /* entry is disabled */
        return 0;
    }
    if(fme_entry_timeout_status(entry, now)) {
        /* entry has timed-out and should be disabled */
        entry->enabled = 0;
        return 0;
    }
    if(fme_key_match__(key, &entry->key) == 0) {
        /* no match */
        return 0;
    }

    /* entry has matched */
    ++entry->counters.matches;
    entry->counters.bytes += size;
    entry->timestamp = now;
    return 1;
}

int
fme_match(fme_t* fme, fme_key_t* key, fme_timeval_t now, int size,
          fme_entry_t** matched)
{
    int i;
    int rv = 0;
    iof_t iof;

    if(AIM_LOG_ENABLED(VERBOSE)) {
        iof_init(&iof, &aim_pvs_stdout);
        iof_push(&iof, "fme_match ");
        iof_push(&iof, "incoming key: ");
        fme_key_dump(key, &iof.inherit);
        iof_pop(&iof);
        iof_iprintf(&iof, "\n");
        fme_dump(fme, &iof.inherit);
        iof_iprintf(&iof, "\n");
    }

    /*
     * Find the first match, in priority order
     */
    for(i = 0; i < fme->num_entries; i++) {
        fme_entry_t* fe = fme->entries[i];
        if(fe == NULL) {
            AIM_LOG_INTERNAL("entry is null");
            continue;
        }
        if(fme_entry_match__(fe, key, now, size)) {
            *matched = fe;
            AIM_LOG_VERBOSE("matched index %d", fe->index);
            rv = 1;
            break;
        }
    }

    if(AIM_LOG_ENABLED(VERBOSE)) {
        iof_pop(&iof);
    }

    if(rv == 0) {
        /* no match */
        AIM_LOG_VERBOSE("no match.");
    }

    return rv;
}

int
fme_matches(fme_t* fme, fme_key_t* key, fme_timeval_t now, int size,
            biglist_t** matches)
{
    int i;
    int count = 0;
    biglist_t* rv = NULL;

    /*
     * Find all matches
     */
    for(i = 0; i < fme->num_entries; i++) {
        fme_entry_t* fe = fme->entries[i];
        if(fme->entries[i] == NULL) {
            AIM_LOG_INTERNAL("entry is null");
            continue;
        }
        if(fme_entry_match__(fe, key, now, size)) {
            rv = biglist_prepend(rv, fe);
            count++;
        }
    }
    if(count > 1) {
        rv = biglist_reverse(rv);
    }
    *matches = rv;
    return count;
}


void
fme_dump(fme_t* fme, aim_pvs_t* ap)
{
    int i;
    iof_t iof;
    iof_init(&iof, ap);

    iof_push(&iof, "fme @ %p", fme);
    if(fme) {
        iof_iprintf(&iof, "name=%s num_entries = %d, max_entries = %d",
                    fme->log_string, fme->num_entries, fme->max_entries);
        for(i = 0; i < fme->num_entries; i++) {
            fme_entry_dump(fme->entries[i], &iof.inherit);
        }
    }
    iof_pop(&iof);
}

static int
fme_key_dump_default__(fme_key_t* key, aim_pvs_t* ap)
{
    int i;
    iof_t iof;

    iof_init(&iof, ap);
    iof_push(&iof, "fme_key @ %p", key);
    iof_iprintf(&iof, "keymask = 0x%.8x", key->keymask);
    iof_iprintf(&iof, "size = %d", key->size);

    iof_iprintfn(&iof, "value = ");
    if(key->size) {
        for(i = 0; i < key->size; i++) {
            iof_uprintf(&iof, "%.2x", key->values[i]);
        }
    }
    else {
        iof_uprintf(&iof, "None");
    }
    iof_uprintf(&iof, "\n");

    iof_iprintfn(&iof, "mask  = ");
    if(key->size) {
        for(i = 0; i < key->size; i++) {
            iof_uprintf(&iof, "%.2x", key->masks[i]);
        }
    }
    else {
        iof_uprintf(&iof, "None");
    }
    iof_iprintf(&iof, "\n");
    iof_pop(&iof);
    return 0;
}

void
fme_entry_dump(fme_entry_t* entry, aim_pvs_t* ap)
{
    iof_t iof;
    iof_init(&iof, ap);

    iof_push(&iof, "entry @ %p", entry);
    if(entry) {
        iof_iprintf(&iof,
                    "prio=%d, enabled=%d, index=%d",
                    entry->prio, entry->enabled, entry->index);
        fme_key_dump(&entry->key, &iof.inherit);
        if(entry->cdumper) {
            entry->cdumper(entry->cookie, &iof.inherit);
        }
    }
    iof_pop(&iof);
}

void
fme_key_dump(fme_key_t* key, aim_pvs_t* ap)
{
    if(key->dumper == NULL) {
        key->dumper = fme_key_dump_default__;
    }
    key->dumper(key, ap);
}
