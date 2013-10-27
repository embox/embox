/**
 * @file
 * @brief Internal implementation of net proto self definition macros.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PROTO_SELF_IMPL_H_
#define FRAMEWORK_NET_PROTO_SELF_IMPL_H_

#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_PROTO(_name, _pack, _type, _handle, \
		_handle_error)                                  \
	ARRAY_SPREAD_DECLARE(const struct net_proto,        \
			__net_proto_registry);                      \
	ARRAY_SPREAD_ADD_NAMED(__net_proto_registry,        \
			__net_proto_##_name, {                      \
				.pack = _pack,                          \
				.type = _type,                          \
				.handle = _handle,                      \
				.handle_error = _handle_error           \
			})

#endif /* FRAMEWORK_NET_PROTO_SELF_IMPL_H_ */
