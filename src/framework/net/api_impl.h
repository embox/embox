/**
 * @file
 * @brief Implementation of net framework iterators.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_API_IMPL_H_
#define FRAMEWORK_NET_API_IMPL_H_

#include <stddef.h>

#include <util/array.h>

#include "types.h"

extern const struct net __net_registry[];

#define __net_foreach(net_ptr) \
	array_foreach_ptr(net_ptr, __net_registry, \
			ARRAY_SPREAD_SIZE(__net_registry))

#endif /* FRAMEWORK_NET_API_IMPL_H_ */
