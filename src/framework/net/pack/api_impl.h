/**
 * @file
 * @brief Implementation of net framework getters and iterators.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PACK_API_IMPL_H_
#define FRAMEWORK_NET_PACK_API_IMPL_H_

#include <util/array.h>

#include "types.h"

ARRAY_SPREAD_DECLARE(const struct net_pack, __net_pack_registry);

#define __net_pack_foreach(net_pack_ptr) \
	array_spread_foreach_ptr(net_pack_ptr, __net_pack_registry)

#endif /* FRAMEWORK_NET_PACK_API_IMPL_H_ */
