/**
 * @file
 * @brief Implementation of net framework getters and iterators.
 *
 * @date 12..05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_API_IMPL_H_
#define FRAMEWORK_NET_FAMILY_API_IMPL_H_

#include <util/array.h>

#include "types.h"

extern const struct net_family __net_family_registry[];

#define __net_family_foreach(net_family_ptr)                 \
	array_foreach_ptr(net_family_ptr, __net_family_registry, \
			ARRAY_SPREAD_SIZE(__net_family_registry))

#endif /* FRAMEWORK_NET_FAMILY_API_IMPL_H_ */
