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

// well, this is rather bad idea
// TODO it would be better to use something like weakref or alias. -- Eldar
#define mod_self __MOD(__EMBUILD_MOD__)

#define __MOD_SELF_BIND(_mod_data, _mod_ops) \
	__MOD_INFO_DEF(__EMBUILD_MOD__, _mod_data, _mod_ops)

#define __MOD_INFO_DEF(mod_nm, _data, _ops) \
	const struct mod_info __MOD_INFO(mod_nm) = { \
		.data = (void *) _data,                  \
		.ops = _ops,                             \
	}

#define __MOD_MEMBER_BIND(ops, data) \
	__MOD_MEMBER_DEF(__EMBUILD_MOD__, ops, data)

#define __MOD_MEMBER(mod_nm) \
	MACRO_GUARD(MACRO_CONCAT(__mod_member__, mod_nm))

#define __MOD_MEMBER_DEF(mod_nm, _ops, _data) \
	static const struct mod_member __MOD_MEMBER(mod_nm) = {  \
		.data = (void *) _data,                              \
		.ops = _ops,                                         \
	};                                                       \
	extern const struct mod_member *__MOD_MEMBERS(mod_nm)[]; \
	ARRAY_SPREAD_ADD(__MOD_MEMBERS(mod_nm), &__MOD_MEMBER(mod_nm))

#endif /* FRAMEWORK_MOD_SELF_IMPL_H_ */
