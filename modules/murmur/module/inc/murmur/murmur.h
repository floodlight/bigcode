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
 ****************************************************************/

/*
 * Port of MurmurHash3 (public domain) to C.
 * http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp
 *
 * This port supports incremental hashing. Example:
 *
 *     uint32_t hash = 42; // seed
 *     for (i = 0; i < num_words; i++) {
 *         hash = murmur_round(hash, data[i]);
 *     }
 *     hash = murmur_finish(hash, num_words*sizeof(uint32_t));
 *
 * Incremental hashing will not produce the same result as murmur_hash when
 * the length is not a multiple of 4 bytes.
 */

#ifndef MURMUR_MURMUR_H
#define MURMUR_MURMUR_H

#include <stdint.h>

#define MURMUR_C1 0xcc9e2d51
#define MURMUR_C2 0x1b873593

static inline uint32_t
murmur_rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

static inline uint32_t
murmur_fmix(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static inline uint32_t
murmur_round(uint32_t state, uint32_t data)
{
    data *= MURMUR_C1;
    data = murmur_rotl32(data, 15);
    data *= MURMUR_C2;

    state ^= data;
    state = murmur_rotl32(state, 13);
    state = state * 5 + 0xe6546b64;

    return state;
}

static inline uint32_t
murmur_finish(uint32_t state, int len)
{
    state ^= len;
    state = murmur_fmix(state);
    return state;
}

static inline uint32_t
murmur_hash(const void *key, int len, uint32_t seed)
{
    const uint8_t *data = key;
    const int nblocks = len / 4;

    uint32_t h1 = seed;

    /* Workaround for strict aliasing */
#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) uint32_aliased_t;
#else
    typedef uint32_t uint32_aliased_t;
#endif

    const uint32_aliased_t *blocks = (const uint32_aliased_t *)(data + nblocks*4);

    const uint8_t* tail;
    uint32_t k1;

    int i;
    for(i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];
        h1 = murmur_round(h1, k1);
    }

    tail = (const uint8_t*)(data + nblocks*4);
    k1 = 0;

    switch(len & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= MURMUR_C1; k1 = murmur_rotl32(k1,15); k1 *= MURMUR_C2; h1 ^= k1;
    };

    return murmur_finish(h1, len);
}

#endif
