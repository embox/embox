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
#include "info.h"

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

