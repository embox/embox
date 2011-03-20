/**
 * @file
 * @brief Foreach iterations over mod collections and some inlined API methods.
 *
 * @date 12.06.2010
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

#define __MOD_FLAG_ENABLED       (1 << 0)

#define __mod_flag_tst(mod, mask)   ((mod)->private->flags &   (mask))

inline static bool mod_is_running(const struct mod *mod) {
	return mod != NULL && __mod_flag_tst(mod, __MOD_FLAG_ENABLED);
}

inline static void *mod_data(const struct mod *mod) {
	return (NULL != mod && NULL != mod->info) ? mod->info->data : NULL;
}

#endif /* FRAMEWORK_MOD_API_IMPL_H_ */
