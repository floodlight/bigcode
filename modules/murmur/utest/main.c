/****************************************************************
 *
 *        Copyright 2014, Big Switch Networks, Inc.
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

#include <murmur/murmur.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <AIM/aim.h>

/*
 * SMHasher verification test for MurmurHash3, ported from C++
 *
 * https://code.google.com/p/smhasher/source/browse/trunk/KeysetTest.cpp
 *
 * SMHasher is in the public domain.
 */
static void
test_murmur(void)
{
    uint8_t key[256];
    uint32_t hashes[256];

    // Hash keys of the form {0}, {0,1}, {0,1,2}... up to N=255,using 256-N as
    // the seed

    int i;
    for (i = 0; i < 256; i++)
    {
        key[i] = (uint8_t)i;
        hashes[i] = murmur_hash(key, i, 256 - i);
    }

    // Then hash the result array

    uint32_t final = murmur_hash(hashes, sizeof(hashes), 0);

    AIM_TRUE_OR_DIE(final == 0xB0F57EE3);
}

int aim_main(int argc, char* argv[])
{
    test_murmur();
    return 0;
}
