/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_FRAMEWORK_H_
# error "Do not include this file directly, use <mod/framework.h> instead!"
#endif /* MOD_FRAMEWORK_H_ */

#include "types.h"

struct mod;

struct mod_iterator {
	struct mod **p_mod;
};
