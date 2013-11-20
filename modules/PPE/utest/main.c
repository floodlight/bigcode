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

#if PPE_CONFIG_INCLUDE_UTM != 1
#error "The unit test depends upon PPE uCli support. Please compile with PPE_CONFIG_INCLUDE_UCLI=1"
#else


#include <PPE/ppe.h>
#include <PPE/uCli/ppe_utm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * From ppe_utests.c
 */
extern ucli_block_t ppe_utests[];
extern int ppe_utests_count;


int aim_main(int argc, char* argv[])
{
    ucli_t* uc = NULL;

    ucli_init();

    /**
     * The PPE unit test module.
     */
    uc = ucli_create("ppe", ppe_utm_create(), NULL);

    if(argc == 1 || !strcmp(argv[1], "tests")) {
        int rv;
        ucli_flags_set(uc, UCLI_F_DISPATCH_BLOCK_STATUS | UCLI_F_DISPATCH_ECHO_FAIL);
        rv = ucli_dispatch_blocks(uc, &aim_pvs_stdout,
                                  ppe_utests, (argv[1]) ? argv[2] : NULL);
        ucli_destroy(uc);
        ucli_denit();
        return rv;
    }
    else if (!strcmp(argv[1], "cli")) {
        return ucli_run(uc, "ppe");
    }
    else {
        fprintf(stderr, "unknown option %s\n", argv[1]);
        return 1;
    }

    return 0;
}

#endif /* PPE_CONFIG_INCLUDE_CLI */
