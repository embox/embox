/**
 * @file
 * @brief Implements macros for binding #mod_member.
 *
 * @date Jul 05, 2011
 * @author Alexandr Kalmuk
 */

#ifndef FRAMEWORK_MOD_MEMBER_SELF_IMPL_H_
#define FRAMEWORK_MOD_MEMBER_SELF_IMPL_H_

#ifndef __EMBUILD_MOD__
# error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
#endif /* __EMBUILD_MOD__ */

#include <framework/mod/member/ops.h>

#include <util/array.h>

#include "../decls.h"

#define __MOD_MEMBER_BIND(ops, data) \
	__MOD_MEMBER_DEF(__EMBUILD_MOD__, ops, data)

#define __MOD_MEMBER_DEF(mod_nm, ops, data) \
	__MOD_MEMBER_DEF_NM(mod_nm, ops, data, \
			MACRO_GUARD(MACRO_CONCAT(__mod_member__, mod_nm)))

#define __MOD_MEMBER_DEF_NM(mod_nm, _ops, _data, member_nm) \
	static const struct mod_member member_nm = { \
		.mod = &__MOD(mod_nm),                   \
		.ops = _ops,                             \
		.data = (void *) _data,                  \
	};                                           \
	extern const struct mod_member *__MOD_MEMBERS(mod_nm)[]; \
	ARRAY_SPREAD_ADD(__MOD_MEMBERS(mod_nm), &member_nm)

#endif /* FRAMEWORK_MOD_MEMBER_SELF_IMPL_H_ */
