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

#include <uCli/ucli_config.h>
#include <uCli/ucli.h>
#include <uCli/ucli_common.h>

int
ucli_pvs_vprintf(aim_pvs_t* pvs, const char* fmt, va_list vargs)
{
    return ucli_vprintf((ucli_context_t*)pvs->object.cookie, fmt, vargs);
}

int
ucli_printf(ucli_context_t* uc, const char* fmt, ...)
{
    int rv;
    va_list vargs;
    va_start(vargs, fmt);
    rv = ucli_vprintf(uc, fmt, vargs);
    va_end(vargs);
    return rv;
}

int
ucli_vprintf(ucli_context_t* uc, const char* fmt, va_list _vargs)
{
    return aim_vprintf(uc->epvs, fmt, _vargs);
}
