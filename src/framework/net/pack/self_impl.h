/**
 * @file
 * @brief Internal implementation of net self definition macros.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PACK_SELF_IMPL_H_
#define FRAMEWORK_NET_PACK_SELF_IMPL_H_

#include <framework/mod/self.h>
#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_PACK(_name, _type, _rcv_pack, _init, _fini)  \
	ARRAY_SPREAD_DECLARE(const struct net_pack,                  \
			__net_pack_registry);                                \
	ARRAY_SPREAD_ADD_NAMED(__net_pack_registry,                  \
			__net_pack_##_name, {                                \
				.type = _type,                                   \
				.rcv_pack = _rcv_pack,                           \
				.init = _init,                                   \
				.fini = _fini                                    \
			});                                                  \
	extern const struct mod_ops __net_pack_mod_ops;              \
	MOD_INFO_BIND(&__net_pack_mod_ops, &__net_pack_##_name)

#endif /* FRAMEWORK_NET_PACK_SELF_IMPL_H_ */
