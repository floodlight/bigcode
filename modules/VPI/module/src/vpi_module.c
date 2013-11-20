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

#include <VPI/vpi_config.h>
#include <VPI/vpi.h>
#include "vpi_log.h"
#include <AIM/aim_datatypes.h>

/**************************************************************************//**
 *
 * Generic datatype registration and basic support routines.
 *
 *
 *****************************************************************************/

/*
 * The {vpi} type will convert the VPI specification argument
 * directly to a vpi_t pointer
 */
static int
vpi_aim_fs__vpi(aim_datatype_context_t* dtc,
                 const char* arg, aim_va_list_t* vargs)
{
    vpi_t* vpip = va_arg(vargs->val, vpi_t*);
    *vpip = vpi_create(arg);
    if(*vpip == NULL) {
        aim_printf(dtc->epvs, "failed to create vpi: '%s'", arg);
        return AIM_DATATYPE_ERROR;
    }
    return AIM_DATATYPE_OK;
}
static int
vpi_aim_ts__vpi(aim_datatype_context_t* dtc,
                 aim_va_list_t* vargs,
                 const char** rv)
{
    vpi_t vpi = va_arg(vargs->val, vpi_t);
    *rv = aim_strdup(vpi_name_get(vpi));
    AIM_REFERENCE(dtc);
    return AIM_DATATYPE_OK;
}


int
vpi_datatypes_init(void)
{
    /** Register datatype handlers for all of our enumeration types */
#define VPI_ENUMERATION_ENTRY(_enum_name, _desc) \
    AIM_DATATYPE_MAP_REGISTER(_enum_name, _enum_name##_map, _desc, VPI_MINTERNAL);
#include <VPI/vpi.x>

    aim_datatype_register(0, "vpi", NULL,
                          vpi_aim_fs__vpi, vpi_aim_ts__vpi, NULL);
    return 0;
}

void
__vpi_module_init__(void)
{
    vpi_init();
    vpi_datatypes_init();
}



