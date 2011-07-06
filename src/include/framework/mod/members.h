/**
 * @file include/framework/mod/members.h
 * @brief The members of mod are structure that contain informations about
 * the all allocators in this mode.
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

#define __MOD_MEMBER_ARRAY(mod_nm, array_nm) __mod_##array_nm##__##mod_nm

#define __MOD_MEMBER_INFO_DEF(mod_nm, _mod_data, _mod_ops)        \
	 static	const struct __mod_info __MOD_MEMBER_INFO(mod_nm) = { \
				.data = (void *) _mod_data,                       \
				.ops = (struct mod_ops *) _mod_ops,               \
			}

#define MOD_MEMBERS_BIND(ops, data) \
         _MOD_MEMBER_INFO_DEF(__EMBUILD_MOD__, data, ops); \
          ARRAY_SPREAD_ADD(__MOD_MEMBER_ARRAY(__EMBUILD_MOD__, members), \
        		  &__MOD_MEMBER_INFO(__EMBUILD_MOD__))

#endif /* FRAMEWORK_MOD_SELF_H_ */
