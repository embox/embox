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

#include <framework/mod/self.h>
#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_PROTO(_name, _type, _handle, _handle_error, \
		_init, _fini)                                           \
	extern const struct net_proto __net_proto_registry[];       \
	extern const struct mod_ops __net_proto_mod_ops;            \
	ARRAY_SPREAD_ADD_NAMED(__net_proto_registry,                \
			__net_proto_##_name, {                              \
			.type = _type,                                      \
			.init = _init,                                      \
			.fini = _fini,                                      \
			.handle = _handle,                                  \
			.handle_error = _handle_error                       \
		});                                                     \
	MOD_INFO_BIND(&__net_proto_mod_ops, &__net_proto_##_name)

#endif /* FRAMEWORK_NET_PROTO_SELF_IMPL_H_ */
