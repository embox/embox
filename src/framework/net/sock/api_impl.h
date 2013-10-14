/**
 * @file
 * @brief Implementation of net sockets framework iterators.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SOCK_API_IMPL_H_
#define FRAMEWORK_NET_SOCK_API_IMPL_H_

#include <util/array.h>

#include "types.h"

extern volatile const struct net_sock __net_sock_registry[];

#define __net_sock_foreach(net_sock_ptr)                 \
	array_foreach_ptr(net_sock_ptr, __net_sock_registry, \
			ARRAY_SPREAD_SIZE(__net_sock_registry))

#endif /* FRAMEWORK_NET_SOCK_API_IMPL_H_ */
