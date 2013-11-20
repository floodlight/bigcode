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
#include <FME/uCli/fme_utm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uCli/ucli.h>

#if FME_CONFIG_INCLUDE_UTM != 1
#error "The unit test depends upon FME UTM support. Please compile with FME_CONFIG_INCLUDE_UTM=1"
#else


extern ucli_block_t fme_utests[];
extern int fme_utests_count;
extern ucli_module_t* fme_ucli_module_create(void);

int aim_main(int argc, char* argv[])
{
    ucli_t* uc = NULL;
    int rv;

    ucli_init();
    uc = ucli_create("fme", fme_utm_create(), NULL);

    if(argc == 1 || !strcmp(argv[1], "tests")) {
        ucli_flags_set(uc, UCLI_F_DISPATCH_BLOCK_STATUS | UCLI_F_DISPATCH_ECHO_FAIL);
        rv = ucli_dispatch_blocks(uc, &aim_pvs_stdout,
                                  fme_utests, (argv[1]) ? argv[2] : NULL);
    }
    else if (!strcmp(argv[1], "cli")) {
        rv = ucli_run(uc, "fme");
    }
    else {
        rv = ucli_dispatch_argv(uc, &aim_pvs_stdout, argv+1);
    }
    ucli_destroy(uc);
    ucli_denit();
    return rv;
}

#endif /* FME_CONFIG_INCLUDE_UCLI */
