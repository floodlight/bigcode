/**************************************************************************//**
 *
 * @file
 * @brief ORC uCli Definitions. 
 *
 *
 * @addtogroup ivs-ucli
 * @{
 *
 *
 *****************************************************************************/
#ifndef __ORC_UCLI_H__
#define __ORC_UCLI_H__

#include <orc/options.h>
#include <uCli/ucli.h>

/**
 * @brief Get the configuration ucli node. 
 */
ucli_node_t* orc_ucli_node_create(orc_options_t* ivs); 

#endif /* __ORC_UCLI_H__ */
/*@}*/
