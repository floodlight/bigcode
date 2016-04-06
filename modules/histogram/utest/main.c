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

static void
check(struct histogram *hist, uint32_t k, uint32_t v)
{
    uint32_t i = histogram_bucket(k);
    AIM_ASSERT(i < HISTOGRAM_BUCKETS);
    uint32_t v2 = hist->counts[i];
    if (v != v2) {
        AIM_DIE("Expected %u at key %u (index %u), found %u", v, k, i, v2);
    }
}

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

void
test_basic(void)
{
    struct histogram *hist = histogram_create("basic");

    check(hist, 0, 0);

    histogram_inc(hist, 0);
    check(hist, 0, 1);

    histogram_inc(hist, 0);
    check(hist, 0, 2);

    /* 1 is a different bucket */
    histogram_inc(hist, 1);
    check(hist, 1, 1);

    /* 32 and 33 are the same bucket */
    histogram_inc(hist, 32);
    histogram_inc(hist, 33);
    check(hist, 32, 2);
    check(hist, 33, 2);

    histogram_inc(hist, UINT32_MAX);
    check(hist, UINT32_MAX, 1);

    histogram_destroy(hist);
}

void
test_all(void)
{
    struct histogram *hist = histogram_create("all");
    uint32_t k;

    for (k = 0; k < UINT32_MAX; k++) {
        check(hist, k, 0);
    }
    check(hist, k, 0);

    for (k = 0; k < UINT32_MAX; k++) {
        histogram_inc(hist, k);
    }
    histogram_inc(hist, k);

    int i;
    for (i = 4; i < 32; i++) {
        uint32_t c = (1u << i) / 16;
        for (k = 1<<i; k < (2<<i) - 1; k++) {
            check(hist, k, c);
        }
        check(hist, k, c);
    }

    histogram_destroy(hist);
}

void
test_list(void)
{
    struct histogram *hist1 = histogram_create("hist1");
    struct histogram *hist2 = histogram_create("hist2");
    struct histogram *hist3 = aim_zmalloc(sizeof(*hist3));
    histogram_register(hist3, "hist3");

    AIM_ASSERT(!strcmp(hist1->name, "hist1"));
    AIM_ASSERT(!strcmp(hist2->name, "hist2"));
    AIM_ASSERT(!strcmp(hist3->name, "hist3"));

    struct list_head *head = histogram_list();

    struct list_links *cur = list_first(head);
    AIM_ASSERT(container_of(cur, links, struct histogram) == hist1);

    cur = cur->next;
    AIM_ASSERT(container_of(cur, links, struct histogram) == hist2);

    cur = cur->next;
    AIM_ASSERT(container_of(cur, links, struct histogram) == hist3);

    AIM_ASSERT(cur->next == &head->links);

    histogram_destroy(hist1);
    histogram_destroy(hist2);
    histogram_unregister(hist3);
    aim_free(hist3);
}

void
test_find(void)
{
    AIM_ASSERT(histogram_find("hist1") == NULL);
    AIM_ASSERT(histogram_find("hist2") == NULL);

    struct histogram *hist1 = histogram_create("hist1");
    AIM_ASSERT(histogram_find("hist1") == hist1);
    AIM_ASSERT(histogram_find("hist2") == NULL);

    struct histogram *hist2 = histogram_create("hist2");
    AIM_ASSERT(histogram_find("hist1") == hist1);
    AIM_ASSERT(histogram_find("hist2") == hist2);

    histogram_destroy(hist1);
    AIM_ASSERT(histogram_find("hist1") == NULL);
    AIM_ASSERT(histogram_find("hist2") == hist2);

    histogram_destroy(hist2);
    AIM_ASSERT(histogram_find("hist1") == NULL);
    AIM_ASSERT(histogram_find("hist2") == NULL);
}

int aim_main(int argc, char* argv[])
{
    test_bucket();
    test_basic();
    test_all();
    test_list();
    test_find();
    return 0;
}
