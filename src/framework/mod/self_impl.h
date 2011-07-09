/**
 * @file
 * @brief Implements macro for binding #mod_info to the self #mod.
 *
 * @date 10.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_SELF_IMPL_H_
#define FRAMEWORK_MOD_SELF_IMPL_H_

#ifndef __EMBUILD_MOD__
# error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
#endif /* __EMBUILD_MOD__ */

#include "decls.h"
#include "types.h"

// XXX to be dropped soon. -- Eldar
#define __MOD_INFO_DEF(mod_nm, _data, _ops) \
	const struct mod_info __MOD_INFO(mod_nm) = { \
		.data = (void *) _data, \
		.ops = _ops, \
	}

/* Here goes public macros API implementation. */

#define __MOD_SELF_BIND(_mod_data, _mod_ops) \
	__MOD_INFO_DEF(__EMBUILD_MOD__, _mod_data, _mod_ops)

// well, this is rather bad idea
// TODO it would be better to use something like weakref or alias. -- Eldar
#define mod_self __MOD(__EMBUILD_MOD__)

#endif /* FRAMEWORK_MOD_SELF_IMPL_H_ */
