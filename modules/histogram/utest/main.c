/****************************************************************
 *
 *        Copyright 2016, Big Switch Networks, Inc.
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

#include <histogram/histogram.h>
#include <AIM/aim.h>

void
test_bucket(void)
{
    struct {
        uint32_t k;
        uint32_t expect;
    } tests[] = {
        { 0, 0 },
        { 1, 1 },
        { 2, 2 },
        { 14, 14 },
        { 15, 15 },
        { 16, 16 },
        { 17, 17 },
        { 30, 30 },
        { 31, 31 },
        { 32, 32 },
        { 33, 32 },
        { 34, 33 },
        { 62, 47 },
        { 63, 47 },
        { 64, 48 },
        { 65, 48 },
        { 66, 48 },
        { 67, 48 },
        { 68, 49 },
        { UINT32_MAX, 463 },
    };

    int i;
    for (i = 0; i < AIM_ARRAYSIZE(tests); i++) {
        uint32_t actual = histogram_bucket(tests[i].k);
        if (tests[i].expect != actual) {
            AIM_DIE("histogram_bucket test failed: k=%u expect=%u actual=%u",
                    tests[i].k, tests[i].expect, actual);
        }
    }
}

int aim_main(int argc, char* argv[])
{
    test_bucket();
    return 0;
}
