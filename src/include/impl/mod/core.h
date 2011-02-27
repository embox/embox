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

#include <util/array.h>

#include "types.h"

#define __mod_foreach_requires(dep, mod) \
		array_foreach(dep, (mod)->requires)

#define __mod_foreach_provides(dep, mod) \
		array_foreach(dep, (mod)->provides)
