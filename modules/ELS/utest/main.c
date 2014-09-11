/************************************************************
 * <bsn.cl fy=2013 v=epl-1.0>
 *
 *        Copyright 2013, 2014 Big Switch Networks, Inc.
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
 *  ELS Unit Testing
 *
 ***********************************************************/

#include <ELS/els_config.h>
#include <ELS/els.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

static int
prompt__(char* p, int size, void* cookie)
{
    static int count = 0;
    assert(cookie == (void*) 0xDEAD);
    sprintf(p, "%.2d> ", count++);
    (void)size;
    return 0;
}

static char*
complete_handler__(void* cookie, const char* const line)
{
    AIM_REFERENCE(cookie);
    if(strstr(line, "<tab>") == NULL) {
        return aim_strdup("<tab> ");
    }
    return NULL;
}


int
main(int argc, char* argv[])
{
    int i;
    els_t* els = els_create("foo", prompt__, (void*)0xDEAD);

    AIM_REFERENCE(argc);
    AIM_REFERENCE(argv);


    if(argc == 1) {
        printf("Starting...\n");
        els_start(els);
        els_prompt(els);
        els_getline(els);
        printf("Stopping...\n");
        els_stop(els);
        printf("done\n");
    }
    else {
        for(;;) {
            printf("Starting...\n");
            els_start(els);
            els_complete_set(els, complete_handler__, NULL);
            for(i = 0; i < 5; i++) {
                char* line;
                els_prompt(els);
                while((line = els_getline(els)) == NULL) {
                    sleep(1);
                }
                printf("input: %s", line);
            }
            printf("Stopping...");
            els_stop(els);
            printf("Done.\n");
        }
    }
    els_destroy(els);
    return 0;
}
