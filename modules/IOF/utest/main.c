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

#include <AIM/aim.h>
#include <IOF/iof.h>
#include <stdio.h>
#include <stdarg.h>


#define IOF_IPRINTF()                                                   \
    iof_iprintf(&iof, "iprintf(%s:%d) ", __FUNCTION__, __LINE__);

#define IOF_IPRINTFN() \
    iof_iprintfn(&iof, "iprintfn(%s:%d) ", __FUNCTION__, __LINE__);
#define IOF_UPRINTF()                                                   \
    iof_uprintf(&iof, "uprintf(%s:%d)\n", __FUNCTION__, __LINE__);

#define IOF_PUSH()                                              \
    iof_push(&iof, "push(%s:%d)", __FUNCTION__, __LINE__);

#define IOF_POP()                               \
    iof_pop(&iof)

static int
inherit__(aim_pvs_t* pvs, int level)
{
    iof_t iof;

    if(level-- == 0) {
        return 0;
    }

    iof_init(&iof, pvs);
    IOF_IPRINTF();
    IOF_PUSH();
    IOF_IPRINTFN();
    IOF_UPRINTF();
    IOF_PUSH();
    IOF_IPRINTF();
    inherit__(&iof.inherit, level);
    IOF_POP();
    IOF_POP();
    return 0;
}


int main(int argc, char* argv[])
{
    iof_t iof;

    AIM_REFERENCE(argc);
    AIM_REFERENCE(argv);

    iof_init(&iof, &aim_pvs_stdout);
    inherit__(&iof.inherit, 3);
    inherit__(&iof.inherit, 4);
    inherit__(&iof.inherit, 2);

    iof_init(&iof, &aim_pvs_stdout);
    iof.indent_string="-";
    iof.indent_terminator = "> ";
    iof.push_string = " [";
    iof.pop_string = "]";
    inherit__(&iof.inherit, 2);
    return 0;
}
