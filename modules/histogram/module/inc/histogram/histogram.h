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

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdint.h>

#define HISTOGRAM_SHIFT 4

/*
 * Map 32-bit key to bucket index
 *
 * Each power of two is divided into 16 buckets. We first calculate
 * the log base 2 and multiply by 16 to get the first bucket for that
 * power. We then add the next 4 bits after the MSB to get the final bucket.
 */
static inline uint32_t
histogram_bucket(uint32_t k)
{
    if ((k >> HISTOGRAM_SHIFT) == 0) {
        return k;
    }

    uint32_t log2 = 31 - __builtin_clz(k >> HISTOGRAM_SHIFT);
    return (log2 << HISTOGRAM_SHIFT) + (k >> log2);
}

#endif
