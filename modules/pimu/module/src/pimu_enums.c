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
/************************************************************//**
 *
 *
 *
 ***************************************************************/
#include <pimu/pimu_config.h>
#include <pimu/pimu.h>

/* <auto.start.enum(ALL).source> */
aim_map_si_t pimu_action_map[] =
{
    { "ERROR", PIMU_ACTION_ERROR },
    { "FORWARD_NEW", PIMU_ACTION_FORWARD_NEW },
    { "FORWARD_EXISTING", PIMU_ACTION_FORWARD_EXISTING },
    { "FORWARD_PRIORITY", PIMU_ACTION_FORWARD_PRIORITY },
    { "DROP", PIMU_ACTION_DROP },
    { NULL, 0 }
};

aim_map_si_t pimu_action_desc_map[] =
{
    { "None", PIMU_ACTION_ERROR },
    { "None", PIMU_ACTION_FORWARD_NEW },
    { "None", PIMU_ACTION_FORWARD_EXISTING },
    { "None", PIMU_ACTION_FORWARD_PRIORITY },
    { "None", PIMU_ACTION_DROP },
    { NULL, 0 }
};

const char*
pimu_action_name(pimu_action_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, pimu_action_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'pimu_action'";
    }
}

int
pimu_action_value(const char* str, pimu_action_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, pimu_action_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
pimu_action_desc(pimu_action_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, pimu_action_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'pimu_action'";
    }
}

/* <auto.end.enum(ALL).source> */

