/**
 * @file
 * @brief Implementation of net protos framework iterators.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PROTO_API_IMPL_H_
#define FRAMEWORK_NET_PROTO_API_IMPL_H_

#include <util/array.h>

#include "types.h"

ARRAY_SPREAD_DECLARE(const struct net_proto, __net_proto_registry);

#define __net_proto_foreach(net_proto_ptr) \
	array_spread_foreach_ptr(net_proto_ptr, __net_proto_registry)

static inline void net_proto_handle_error_none(
		const struct sk_buff *skb, int error_info) { }

#endif /* FRAMEWORK_NET_PROTO_API_IMPL_H_ */
