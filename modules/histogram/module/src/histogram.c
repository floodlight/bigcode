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

#define AIM_LOG_MODULE_NAME histogram
#include <AIM/aim_log.h>

AIM_LOG_STRUCT_DEFINE(AIM_LOG_OPTIONS_DEFAULT, AIM_LOG_BITS_DEFAULT, NULL, 0);

void
__histogram_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
}

struct histogram *
histogram_create(const char *name)
{
    struct histogram *hist = aim_zmalloc(sizeof(*hist));
    return hist;
}

void
histogram_destroy(struct histogram *hist)
{
    aim_free(hist);
}
