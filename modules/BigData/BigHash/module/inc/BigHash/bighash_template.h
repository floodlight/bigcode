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
/*
 * BigHash template
 *
 * This template can be used to create wrappers around the BigHash interfaces
 * specialized for particular object and key types. This is intended to cover
 * the simple majority of uses, where the key is embedded at a fixed offset
 * in the object and can be hashed and compared as an opaque byte array.
 *
 * The following macros must be defined before including this file:
 *   TEMPLATE_NAME - prefix for the created functions
 *   TEMPLATE_OBJ_TYPE - type (not a pointer) of the stored object
 *   TEMPLATE_KEY_FIELD - field name of the key
 *   TEMPLATE_ENTRY_FIELD - field name of the bighash_entry_t
 *
 * The above macros will be automatically undefined by this file.
 *
 * This file is intended to be included by a header that defines the parameter
 * macros. It should also use a guard to prevent multiple inclusion, because
 * this file deliberately does not.
 */

#include <BigHash/bighash.h>
#include <murmur/murmur.h>

#ifndef TEMPLATE_NAME
#error "Must define TEMPLATE_NAME"
#endif

#ifndef TEMPLATE_OBJ_TYPE
#error "Must define TEMPLATE_OBJ_TYPE"
#endif

#ifndef TEMPLATE_KEY_FIELD
#error "Must define TEMPLATE_KEY_FIELD"
#endif

#ifndef TEMPLATE_ENTRY_FIELD
#error "Must define TEMPLATE_ENTRY_FIELD"
#endif

/* Macro to create a function name */
#define BHT_NAME_PASTE(X,Y) X ## _ ## Y
#define BHT_NAME_EXPAND(X, Y) BHT_NAME_PASTE(X, Y)
#define BHT_NAME(X) BHT_NAME_EXPAND(TEMPLATE_NAME, X)

/* Derive the key type from the object type and field */
#define TEMPLATE_KEY_TYPE typeof(((TEMPLATE_OBJ_TYPE *)0)->TEMPLATE_KEY_FIELD)

/* Hash a key */
static inline uint32_t
BHT_NAME(hash)(const TEMPLATE_KEY_TYPE *key)
{
    return murmur_hash(key, sizeof(*key), 0);
}

/* Insert an object into the hashtable */
static inline void
BHT_NAME(insert)(bighash_table_t *table, TEMPLATE_OBJ_TYPE *obj)
{
    bighash_insert(table, &obj->TEMPLATE_ENTRY_FIELD,
                   BHT_NAME(hash)(&obj->TEMPLATE_KEY_FIELD));
}

/*
 * Given an entry, find the next object matching 'key'.
 * The given entry is included in this search.
 */
static inline TEMPLATE_OBJ_TYPE *
BHT_NAME(search)(bighash_entry_t *entry, const TEMPLATE_KEY_TYPE *key)
{
    while (entry != NULL) {
        TEMPLATE_OBJ_TYPE *obj = container_of(entry, TEMPLATE_ENTRY_FIELD, TEMPLATE_OBJ_TYPE);
        if (!memcmp(&obj->TEMPLATE_KEY_FIELD, key, sizeof(*key))) {
            return obj;
        }
        entry = bighash_next(entry);
    }
    return NULL;
}

/* Return the first object matching 'key', or NULL */
static inline TEMPLATE_OBJ_TYPE *
BHT_NAME(first)(bighash_table_t *table, const TEMPLATE_KEY_TYPE *key)
{
    return BHT_NAME(search)(bighash_first(table, BHT_NAME(hash)(key)), key);
}

/* Return the next object matching 'key', or NULL */
static inline TEMPLATE_OBJ_TYPE *
BHT_NAME(next)(TEMPLATE_OBJ_TYPE *prev)
{
    return BHT_NAME(search)(bighash_next(&prev->TEMPLATE_ENTRY_FIELD),
                            &prev->TEMPLATE_KEY_FIELD);
}

#undef BHT_NAME_PASTE
#undef BHT_NAME_EXPAND
#undef BHT_NAME

#undef TEMPLATE_KEY_TYPE

#undef TEMPLATE_NAME
#undef TEMPLATE_OBJ_TYPE
#undef TEMPLATE_KEY_FIELD
#undef TEMPLATE_ENTRY_FIELD
