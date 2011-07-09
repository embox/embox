/**
 * @file
 * @brief Foreach iterations over mod collections and some inlined API methods.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_API_IMPL_H_
#define FRAMEWORK_MOD_API_IMPL_H_

#include <util/array.h>

#include "types.h"

extern const struct mod *__mod_registry[];

#define __mod_foreach(mod) \
	array_nullterm_foreach(mod, __mod_registry)

#define __mod_foreach_requires(dep, mod) \
	array_nullterm_foreach(dep, (mod)->requires)

#define __mod_foreach_provides(dep, mod) \
	array_nullterm_foreach(dep, (mod)->provides)

static inline void *mod_data(const struct mod *mod) {
	return (NULL != mod && NULL != mod->info) ? mod->info->data : NULL;
}

#endif /* FRAMEWORK_MOD_API_IMPL_H_ */
