/**
 * @file
 * @brief Internal implementation of net self definition macros.
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_SELF_IMPL_H_
#define FRAMEWORK_NET_FAMILY_SELF_IMPL_H_

#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_FAMILY(_name, _family, _types, \
		_out_ops)                                  \
	ARRAY_SPREAD_DECLARE(const struct net_family,  \
			__net_family_registry);                \
	ARRAY_SPREAD_ADD_NAMED(__net_family_registry,  \
			__net_family_##_name, {                \
				.family = _family,                 \
				.types = &_types[0],               \
				.types_sz = ARRAY_SIZE(_types),    \
				.out_ops = &_out_ops               \
			})

#endif /* FRAMEWORK_NET_FAMILY_SELF_IMPL_H_ */
