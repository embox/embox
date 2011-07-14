/**
 * @file
 * @brief Internal implementation of net sockets self definition macros.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SOCK_SELF_IMPL_H_
#define FRAMEWORK_NET_SOCK_SELF_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <framework/mod/self.h>

#include "types.h"

#define __EMBOX_NET_SOCK(_sock, _net_proto_family)				   \
	extern const struct mod_ops __net_sock_mod_ops;         	   \
	extern const struct net_sock __net_sock_registry[];            \
	ARRAY_SPREAD_ADD_NAMED(__net_sock_registry, __net##_sock,   {  \
			.net_proto_family = _net_proto_family,				   \
			.netsock = &_sock,                     				   \
			.mod = &mod_self                   					   \
		});                                        				   \
	MOD_INFO_BIND(&__net_sock_mod_ops, &__net##_sock )

#endif /* FRAMEWORK_NET_SOCK_SELF_IMPL_H_ */
