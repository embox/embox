/**
 * @file
 * @brief Implementation of net protos framework iterators.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PROTO_API_IMPL_H_
#define FRAMEWORK_NET_PROTO_API_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <framework/mod/api.h>

#include "types.h"

extern const struct net_proto __net_proto_registry[];

#define __net_proto_foreach(net_proto_ptr) \
	array_foreach_ptr(net_proto_ptr, __net_proto_registry, \
			ARRAY_SPREAD_SIZE(__net_proto_registry))

#endif /* FRAMEWORK_NET_PROTO_API_IMPL_H_ */
