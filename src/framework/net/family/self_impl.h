/**
 * @file
 * @brief Internal implementation of net self definition macros.
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_SELF_IMPL_H_
#define FRAMEWORK_NET_FAMILY_SELF_IMPL_H_

#include <framework/mod/self.h>
#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_FAMILY(_name, _family, _create, _init,      \
		_fini) \
	extern const struct net_family __net_family_registry[];     \
	extern const struct mod_ops __net_family_mod_ops;           \
	ARRAY_SPREAD_ADD_NAMED(__net_family_registry,               \
			__net_family_##_name, {                             \
		.family = _family,                                      \
		.init = _init,                                          \
		.fini = _fini,                                          \
		.create = _create                                       \
	});                                                         \
	MOD_INFO_BIND(&__net_family_mod_ops, &__net_family_##_name)

#endif /* FRAMEWORK_NET_FAMILY_SELF_IMPL_H_ */
