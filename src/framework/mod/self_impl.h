/**
 * @file
 * @brief Implements macros for binding #mod_info.
 *
 * @date 10.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_SELF_IMPL_H_
#define FRAMEWORK_MOD_SELF_IMPL_H_

#ifndef __EMBUILD_MOD__
# error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
#endif /* __EMBUILD_MOD__ */

#include <framework/mod/ops.h>

#include "decls.h"

// well, this is rather bad idea
// TODO it would be better to use something like weakref or alias. -- Eldar
#define mod_self __MOD(__EMBUILD_MOD__)

#define __MOD_INFO_BIND(_mod_ops, _mod_data) \
	__MOD_INFO_DEF(__EMBUILD_MOD__, _mod_ops, _mod_data)

#define __MOD_INFO_DEF(mod_nm, _ops, _data) \
	const struct mod_info __MOD_INFO(mod_nm) = { \
		.data = (void *) _data,                  \
		.ops = _ops,                             \
	}

#endif /* FRAMEWORK_MOD_SELF_IMPL_H_ */
