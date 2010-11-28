/**
 * @file
 * @brief Implements structure and macroses for MODS interface
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_CORE_H_
# error "Do not include this file directly, use <mod/core.h> instead!"
#endif /* MOD_CORE_H_ */

#include "types.h"

struct mod;

struct mod_iterator {
	struct mod **p_mod;
};
