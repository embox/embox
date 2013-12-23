/**
 * @file
 * @brief Internal implementation of net self definition macros.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PACK_SELF_IMPL_H_
#define FRAMEWORK_NET_PACK_SELF_IMPL_H_

#include <util/array.h>

#include "types.h"

#define __EMBOX_NET_PACK(_name, _type, _rcv_pack) \
	ARRAY_SPREAD_DECLARE(const struct net_pack,   \
			__net_pack_registry);                 \
	ARRAY_SPREAD_ADD_NAMED(__net_pack_registry,   \
			__net_pack_##_name, {                 \
				.type = _type,                    \
				.rcv_pack = _rcv_pack             \
			})

#endif /* FRAMEWORK_NET_PACK_SELF_IMPL_H_ */
