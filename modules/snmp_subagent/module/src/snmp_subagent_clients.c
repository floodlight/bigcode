/************************************************************
 * <bsn.cl fy=2015 v=onl>
 * 
 *           Copyright 2015 Big Switch Networks, Inc.          
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
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#include <snmp_subagent/snmp_subagent_clients.h>

#include <AIM/aim_list.h>
#include <OS/os_sem.h>

#include "snmp_subagent_log.h"

/**
 * Client registration list.
 */
static struct list_head client_list__;

os_sem_t client_list_lock__;
#define CLIENT_LIST_LOCK()   do { os_sem_take(client_list_lock__); } while(0)
#define CLIENT_LIST_UNLOCK() do { os_sem_give(client_list_lock__); } while(0)


/** Client registration entry. */
typedef struct snmp_subagent_client_s {
    struct list_links links;

    char* name;
    snmp_subagent_clientf_t clientf;
    void* cookie;

} snmp_subagent_client_t;

/**
 * Find a registered client.
 */
static snmp_subagent_client_t*
snmp_subagent_client_find_locked__(const char* name)
{
    struct list_links *cur;

    LIST_FOREACH(&client_list__, cur) {
        snmp_subagent_client_t* c = container_of(cur, links, snmp_subagent_client_t);
        if(!strcmp(c->name, name)) {
            return c;
        }
    }
    return NULL;
}

/**
 * Register a client.
 */
static int
snmp_subagent_client_register_locked__(const char* name,
                                       snmp_subagent_clientf_t clientf,
                                       void* cookie)
{
    if(snmp_subagent_client_find_locked__(name) != NULL) {
        return -1;
    }

    snmp_subagent_client_t* c = aim_zmalloc(sizeof(*c));
    c->name = aim_strdup(name);
    c->clientf = clientf;
    c->cookie = cookie;
    list_push(&client_list__, &c->links);
    return 0;
}

/**
 * Unregister a client.
 */
static int
snmp_subagent_client_unregister_locked__(const char* name)
{
    snmp_subagent_client_t* c = snmp_subagent_client_find_locked__(name);
    if(c == NULL) {
        return -1;
    }
    list_remove(&c->links);
    aim_free(c->name);
    aim_free(c);
    return 0;
}

/**
 * Call all registered clients.
 */
static int
snmp_subagent_client_call_all_locked__(int enable)
{
    int rv;
    struct list_links* cur;

    LIST_FOREACH(&client_list__, cur) {
        snmp_subagent_client_t* c = container_of(cur, links, snmp_subagent_client_t);
        if( (rv = c->clientf(enable, c->cookie)) < 0) {
            AIM_LOG_ERROR("snmp_subagent client callback failed: client=%s enable=%d cookie=%p rv=%d",
                          c->name, enable, c->cookie, rv);
            return rv;
        }
    }
    return 0;
}

int
snmp_subagent_client_call_all(int enable)
{
    int rv;
    CLIENT_LIST_LOCK();
    rv = snmp_subagent_client_call_all_locked__(enable);
    CLIENT_LIST_UNLOCK();
    return rv;
}

int
snmp_subagent_client_register(const char* name,
                              snmp_subagent_clientf_t clientf,
                              void* cookie)
{
    int rv;
    CLIENT_LIST_LOCK();
    rv = snmp_subagent_client_register_locked__(name, clientf, cookie);
    CLIENT_LIST_UNLOCK();
    return rv;
}

int
snmp_subagent_client_unregister(const char* name)
{
    int rv;
    CLIENT_LIST_LOCK();
    rv = snmp_subagent_client_unregister_locked__(name);
    CLIENT_LIST_UNLOCK();
    return rv;
}

int
snmp_subagent_client_init(void)
{
    list_init(&client_list__);
    client_list_lock__ = os_sem_create(1);
    return 0;
}
