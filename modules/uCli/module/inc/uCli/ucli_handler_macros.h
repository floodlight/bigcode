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

#ifndef __UCLI_HANDLERS_H__
#define __UCLI_HANDLERS_H__

#include <AIM/aim_log.h>

/**
 * Generates the "config" command for a module.
 * The "config" command displays the module's compile-time configuration
 * using "<mod>_config_show()".
 *
 */
#define UCLI_HANDLER_MACRO_MODULE_CONFIG(_modname)                    \
    UCLI_COMMAND_INFO(uc,                                             \
                      "config", 0,                                    \
                      "Show the " #_modname " build configuration."); \
    _modname##_config_show(&uc->pvs);                                 \
    return UCLI_STATUS_OK;


/**
 * Generates the "list" command for a module.
 * The "list" command displays a summary of all objects in the given
 * list by name.
 */
#define UCLI_HANDLER_MACRO_MODULE_LIST(_mod, _objtype, _objname, _objlist) \
    biglist_t* ble;                                                     \
    _objtype _obj;                                                      \
    UCLI_COMMAND_INFO(uc,                                               \
                      "list", 0,                                        \
                      "List all " #_objname " objects.");                \
    if(biglist_length(_objlist) == 0) {                                 \
        ucli_printf(uc, "None.\n");                                     \
    }                                                                   \
    else {                                                              \
        BIGLIST_FOREACH_DATA(ble, _objlist, _objtype, _obj) {           \
            ucli_printf(uc, "%s @ %p: %s\n", #_objname, _obj, _obj->name); \
        }                                                               \
    }                                                                   \
    return UCLI_STATUS_OK

/**
 * Generates the "show" command for a module.
 * The "show" command displays a single object from the module's
 * object list by name.
 */
#define UCLI_HANDLER_MACRO_MODULE_SHOW(_mod, _objtype, _objname, _objlist) \
    biglist_t* ble;                                                     \
    _objtype _obj;                                                      \
    char* name;                                                         \
    UCLI_COMMAND_INFO(uc, "show", 1, "Show an " #_objname " object.");   \
    UCLI_ARGPARSE_OR_RETURN(uc, "s", &name);                            \
    BIGLIST_FOREACH_DATA(ble, _objlist, _objtype, _obj) {               \
        if(!UCLI_STRCMP(name, _obj->name)) {                            \
            _objname##_show(_obj, &uc->pvs);                            \
            return UCLI_STATUS_OK;                                      \
        }                                                               \
    }                                                                   \
    return ucli_error(uc, "Not found.");


/**
 * @moveme
 */
ucli_node_t* ucli_module_log_node_create(const char* module_name);

#endif /* __UCLI_HANDLER_MACROS_H__ */


