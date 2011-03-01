/**
 * @file
 * @brief TODO
 *
 * @date 10.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_SELF_H_
# error "Do not include this file directly, use <mod/self.h> instead!"
#endif /* MOD_SELF_H_ */

#ifndef __EMBUILD_MOD__
# error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
#endif /* __EMBUILD_MOD__ */

#include "decls.h"
#include "info.h"

// XXX to be dropped soon. -- Eldar
#define __MOD_INFO_DEF(s_mod, _mod_data, _mod_ops) \
		const struct __mod_info __MOD_INFO(s_mod) = { \
				.data = (void *) _mod_data, \
				.ops = (struct mod_ops *) _mod_ops, \
			}

/* Here goes public macros API implementation. */

#define __MOD_SELF_BIND(_mod_data, _mod_ops) \
		__MOD_INFO_DEF(__EMBUILD_MOD__, _mod_data, _mod_ops)

// well, this is rather bad idea
// TODO it would be better to use something like weakref or alias. -- Eldar
#define mod_self __MOD(__EMBUILD_MOD__)
