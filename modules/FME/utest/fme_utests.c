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

#include <uCli/ucli.h>
ucli_block_t fme_utests[] =
{
  {
    "basic-expected",
    {
      "entry 0",
      "key   0x1 DEADBEEFCAFE0011 FFFFFFFFFFFF00FF",
      "expect 0",
      "match 0x1 DEADBEEFCAFE1111 0 0",
    },
  },
  {
    "basic-unexpected",
    {
      "entry 0",
      "key   0x1 DEADBEEFCAFE0011 FFFFFFFFFFFFFFFF",
      "match 0x1 DEADBEEFCAFE1111 0 0",
    },
  },
  {
    "keymask-unexpected",
    {
      "entry 0",
      "key   0x1111 DEADBEEF FFFFFFFF",
      "match 0x1110 DEADBEEF 0 0",
    },
  },
  {
    "keymask-superset",
    {
      "entry 0",
      "key 0xA0 DEADBEEFCAFECAFE FFFFFFFFFFFFFFFF",
      "expect 0",
      "match 0xFBF DEADBEEFCAFECAFE 0 0",
    },
  },
  {
    "counters",
    {
      "entry 0",
      "key 0xFF DEADBEEFBEEFCA5E FFFFFFFFFFFFFFFF",
      "expect 0",
      "match 0xFFFF DEADBEEFBEEFCA5E 456 0xFF",
    },
  },
  {
    "disabled",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "disable",
      "match 0x1 DEADBEEF 0 0",
    },
  },
  {
    "enable-disable",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "disable",
      "enable",
      "expect 0",
      "match 0x1 DEADBEEF 0 0",
    },
  },
  {
    "multiple-priority-0",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "entry 1",
      "key 0x1 DEADCAFE FFFFFFFF",
      "expect 0",
      "match 0x1 DEADBEEF 0 0",
    },
  },
  {
    "multiple-priority-1",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "entry 1",
      "key 0x1 DEADCAFE FFFFFFFF",
      "expect 1",
      "match 0x1 DEADCAFE 0 0",
    },
  },
  {
    "absolute-timeout-ignored",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "absolute-timeout 500",
      "expect 0",
      "match 0x1 DEADBEEF 0 0",
    },
  },
  {
    "absolute-timeout-matched",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "timestamp 300",
      "absolute-timeout 500",
      "expect 0",
      "match 0x1 DEADBEEF 0 400",
    },
  },
  {
    "absolute-timeout-not-matched",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "timestamp 499",
      "absolute-timeout 500",
      "match 0x1 DEADBEEF 0 501",
    },
  },
  {
    "relative-timeout-ignored",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "timestamp 1",
      "relative-timeout 2",
      "expect 0",
      "match 0x1 DEADBEEF 0 0",
    },
  },
  {
    "relative-timeout-matched",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "timestamp 500",
      "relative-timeout 50",
      "expect 0",
      "match 0x1 DEADBEEF 0 549",
    },
  },
  {
    "relative-timeout-not-matched",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "timestamp 500",
      "relative-timeout 50",
      "match 0x1 DEADBEEF 0 551",
    },
  },
  {
    "remove",
    {
      "entry 0",
      "key 0x1 DEADBEEF FFFFFFFF",
      "entry 10",
      "key 0xFF D0A0B0E0 F0F0F0F0",
      "entry 0",
      "remove",
      "expect 10",
      "match 0xFF DEADBEEF 0 0",
    },
  },
  {
    "perf-10000-10000-0",
    {
      "perf 10000 10000 0",
    },
  },
  {
    "perf-10000-10000-1",
    {
      "perf 10000 10000 1",
    },
  },
  { (void*)0 }
};
int fme_utests_count = sizeof(fme_utests)/sizeof(fme_utests[0]);

