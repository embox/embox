/**
 * @file
 * @brief The member of mod are structure that contain informations about
 *        the all allocators in this mode.
 *
 * @date 5.07.2011
 * @author Alexandr Kalmuk
 */

#include <util/array.h>
#include <util/macro.h>
#include __impl_x(framework/mod/types.h)

#ifndef FRAMEWORK_MOD_MEMBERS_H_
#define FRAMEWORK_MOD_MEMBERS_H_

#define __MOD_MEMBER_INFO(mod_nm) \
	MACRO_GUARD(mod_nm)

#define __MOD_MEMBER_INFO_DEF(mod_nm, _data, _ops) \
	static const struct mod_member_info __MOD_MEMBER_INFO(mod_nm)= { \
		.data = (void *) _data, \
		.ops = _ops, \
	}

#define MOD_MEMBER_BIND(ops, data) \
	__MOD_MEMBER_INFO_DEF(__EMBUILD_MOD__, data, ops); \
	extern const struct mod_member_info \
		*__MOD_MEMBER_ARRAY(__EMBUILD_MOD__, members)[]; \
	ARRAY_SPREAD_ADD(__MOD_MEMBER_ARRAY(__EMBUILD_MOD__, members), \
			&__MOD_MEMBER_INFO(__EMBUILD_MOD__))

#endif /* FRAMEWORK_MOD_MEMBERS_H_ */
