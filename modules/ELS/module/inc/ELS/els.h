/************************************************************
 * <bsn.cl fy=2013 v=epl-1.0>
 *
 *        Copyright 2013, 2014 Big Switch Networks, Inc.
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
 * </bsn.cl>
 ********************************************************//**
 *
 *  /module/inc/ELS/els.h
 *
 * @file
 * @brief Editline Server Module
 *
 *
 * @addtogroup els-els
 * @{
 *
 ***********************************************************/
#ifndef __ELS_H__
#define __ELS_H__

#include <ELS/els_config.h>
#include <editline/readline.h>
#include <histedit.h>

/**
 * This structure represents the EditLine Server object.
 * This structure is public to allow customization of the
 * underlying EditLine and History object if you are so inclined.
 *
 * Otherwise, it can be treated as an opaque type.
 */
typedef struct els_s {

    /** The editline object pointer. */
    EditLine* el;

    /** The history objects that will be associated with our editline session. */
    History* history;
    /** The histevent object that will be associated with out editline session.*/
    HistEvent ev;

#if ELS_CONFIG_INCLUDE_EVENTFD == 1
    int eventfd;
#endif

} els_t;


/**
 * You provide a function with this signature to return the prompt
 * that should be displayed to the user. See els_create().
 */
typedef int (*els_get_prompt_f)(char* prompt, int size, void* cookie);

/**
 * You can provide a function which handles completion events.
 * Any string returned will be appended to the input line.
 */
typedef char* (*els_complete_f)(void* cookie, const char* const line);

/**
 * @brief Create an editline server.
 * @param name The name of this server. Will be used to read
 * history and editline config files.
 * @param get_prompt A function which provides the current prompt.
 * @param cookie cookie for your get_prompt function.
 */
els_t* els_create(char* name, els_get_prompt_f get_prompt, void* cookie);

/**
 * @brief Enable tab-completion
 * @param els The editline server object.
 * @param cf The completion handler to register.
 * @param cookie User data for the completion handler.
 */
int els_complete_set(els_t* els, els_complete_f cf, void* cookie);


/**
 * @brief Start an editline server.
 * @param els The editline server object.
 *
 * @note This function initializes and starts the server thread.
 * You must call els_prompt() to indicate you are ready to prompt
 * and request user input.
 */
int els_start(els_t* els);

/**
 * @brief Stop an editline server.
 * @param els The editline server object.
 */
int els_stop(els_t* els);

/**
 * @brief Destroy an editline server.
 * @param els The editline server object.
 */
void els_destroy(els_t* els);

/**
 * @brief Get the current input line, if available.
 * @param els The editline server object.
 * @returns NULL if there is no input available, otherwise
 * a pointer to the input string.
 *
 * @note After you are done processing the input string, you
 * must call els_prompt() if you wish to continue gathering input.
 */
char* els_getline(els_t* els);

/**
 * @brief Wait for an input line to become available.
 * @param els The els object.
 */
void els_waitline(els_t* els);

/**
 * @brief Tell the editline server to prompt the user.
 * @param els The editline server object.
 * @note You must reprompt the user after you have finished
 * processing the result from a successful call to els_getline().
 */
int els_prompt(els_t* els);


#endif /* __ELS_H__ */
/*@}*/
