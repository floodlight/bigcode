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

#include <PPE/ppe_config.h>
#include <AIM/aim_datatypes.h>
#include "ppe_log.h"
#include <PPE/ppe_types.h>


/*
 * The {ppe_field_info} type will convert the argument
 * directly to a ppe_field_info_t pointer
 */
static int
ppe_aim_fs__ppe_field_info(aim_datatype_context_t* dtc,
                            const char* arg, aim_va_list_t* vargs)
{
    const ppe_field_info_t** fip = va_arg(vargs->val,
                                          const ppe_field_info_t**);
    ppe_field_t field;

    AIM_REFERENCE(dtc);

    if(ppe_field_value(arg, &field, 0) == 0) {
        /* Found -- return corresponding pointer */
        *fip = ppe_field_info_table+field;
        return AIM_DATATYPE_OK;
    }
    else {
        return AIM_DATATYPE_ERROR;
    }
}


static int
datatypes_init__(void)
{
#define PPE_ENUMERATION_ENTRY(_enum_name, _desc)                   \
    AIM_DATATYPE_MAP_REGISTER(_enum_name, _enum_name##_map, _desc, \
                              AIM_LOG_INTERNAL);
#include <PPE/ppe.x>


    aim_datatype_register(0, "ppe_field_info", "PPE field identifier",
                          ppe_aim_fs__ppe_field_info, NULL, NULL);

    return 0;
}

void
__ppe_module_init__(void)
{
    AIM_LOG_STRUCT_REGISTER();
    datatypes_init__();
}
