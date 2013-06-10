/**
 * @file
 * @brief Internal implementation of net sockets self definition macros.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SOCK_SELF_IMPL_H_
#define FRAMEWORK_NET_SOCK_SELF_IMPL_H_

#include <framework/mod/member/self.h>
#include <framework/mod/self.h>
#include <util/array.h>
#include <stddef.h>

#include "types.h"

#define __EMBOX_NET_SOCK(_name, _family, _type, _protocol, \
		_is_default, _ops, _init, _fini)                   \
	extern const struct net_sock __net_sock_registry[];    \
	extern const struct mod_member_ops                     \
			__net_sock_mod_member_ops;                     \
	ARRAY_SPREAD_ADD_NAMED(__net_sock_registry,            \
			__net_sock_##_name, {                          \
				.family = _family,                         \
				.type = _type,                             \
				.protocol = _protocol,                     \
				.is_default = _is_default,                 \
				.ops = &_ops,                              \
				.init = _init,                             \
				.fini = _fini,                             \
				.mod = &mod_self                           \
			});                                            \
	MOD_MEMBER_BIND(&__net_sock_mod_member_ops,            \
			&__net_sock_##_name)

#endif /* FRAMEWORK_NET_SOCK_SELF_IMPL_H_ */
