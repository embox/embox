/**
 * @file
 * @brief Internal implementation of net sockets self definition macros.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_SOCK_SELF_IMPL_H_
#define FRAMEWORK_NET_SOCK_SELF_IMPL_H_

#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_SOCK(_name, _family, _type, _protocol, \
		_is_default, _ops)                                 \
	ARRAY_SPREAD_DECLARE(const struct net_sock,            \
			__net_sock_registry);                          \
	ARRAY_SPREAD_ADD_NAMED(__net_sock_registry,            \
			__net_sock_##_name, {                          \
				.family = _family,                         \
				.type = _type,                             \
				.protocol = _protocol,                     \
				.is_default = _is_default,                 \
				.ops = &_ops                               \
			})

#endif /* FRAMEWORK_NET_SOCK_SELF_IMPL_H_ */
