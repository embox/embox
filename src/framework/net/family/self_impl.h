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

#define __EMBOX_NET_FAMILY(_name, _family, _types, _init, _fini) \
	ARRAY_SPREAD_DECLARE(const struct net_family,                \
			__net_family_registry);                              \
	ARRAY_SPREAD_ADD_NAMED(__net_family_registry,                \
			__net_family_##_name, {                              \
				.family = _family,                               \
				.types = &_types[0],                             \
				.types_sz = ARRAY_SIZE(_types),                  \
				.init = _init,                                   \
				.fini = _fini,                                   \
			});                                                  \
	extern const struct mod_ops __net_family_mod_ops;            \
	MOD_INFO_BIND(&__net_family_mod_ops, &__net_family_##_name)

#endif /* FRAMEWORK_NET_FAMILY_SELF_IMPL_H_ */
