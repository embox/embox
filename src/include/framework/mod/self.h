/**
 * @file
 * @brief Self mod bindings for #mod_info.
 * @details
 *   Self mod is the one that is associated with the current compilation unit.
 *   Bindings associate application-specific operations and data with the self
 *   mod. Such information is then used to properly initialize and finalize
 *   the @link #mod_info module @endlink.
 *
 * @note
 *   Do not include from any source which is not a part of some mod.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 *          - Initial implementation for mod_info
 */

#ifndef FRAMEWORK_MOD_SELF_H_
#define FRAMEWORK_MOD_SELF_H_

#include <framework/mod/api.h>
#include <framework/mod/ops.h>
#include <framework/mod/options.h>

#include <util/array.h>

#include "decls.h"

#ifndef __EMBUILD_MOD__
# error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
#endif /* __EMBUILD_MOD__ */

// well, this is rather bad idea
// TODO it would be better to use something like weakref or alias. -- Eldar
#define mod_self __MOD(__EMBUILD_MOD__)

/** The #mod structure corresponding to the self mod. */
extern struct mod mod_self __attribute__((weak));

/**
 * Binds the specified mod data and operations to the self mod.
 *
 * @param mod_ops
 *   Pointer to the #mod_ops structure (if any).
 * @param mod_data
 *   Pointer to the module specific data (if any).
 */
#define MOD_INFO_BIND(mod_ops, mod_data) \
	__MOD_INFO_DEF(__EMBUILD_MOD__, mod_ops, mod_data)

#ifndef __cplusplus
#define __MOD_INFO_DEF(mod_nm, _ops, _data) \
	const struct mod_info __MOD_INFO(mod_nm) = { \
		.mod = &__MOD(mod_nm),                   \
		.ops = _ops,                             \
		.data = (void *) _data,                  \
	}
#else
#define __MOD_INFO_DEF(mod_nm, _ops, _data) \
	const struct mod_info __MOD_INFO(mod_nm) = { \
		&__MOD(mod_nm),                   \
		_ops,                             \
		(void *) _data,                   \
	}
#endif

/**
 * Binds the specified data and operations to the mod member.
 *
 * @param member_ops
 *   Pointer to the #mod_member_ops structure (if any).
 * @param member_data
 *   Pointer to the member specific data (if any).
 */
#define MOD_MEMBER_BIND(member_ops, member_data) \
	__MOD_MEMBER_DEF(__EMBUILD_MOD__, member_ops, member_data)

#define __MOD_MEMBER_DEF(mod_nm, ops, data) \
	__MOD_MEMBER_DEF_NM(mod_nm, ops, data, \
			MACRO_GUARD(MACRO_CONCAT(__mod_member__, mod_nm)))

#define __MOD_MEMBER_DEF_NM(mod_nm, _ops, _data, member_nm) \
	static const struct mod_member member_nm = {             \
		.ops = _ops,                                     \
		.data = (void *) _data,                          \
	};                                                       \
	extern const struct mod_member *__MOD_MEMBERS(mod_nm)[]; \
	ARRAY_SPREAD_ADD(__MOD_MEMBERS(mod_nm), &member_nm)

#endif /* FRAMEWORK_MOD_SELF_H_ */
