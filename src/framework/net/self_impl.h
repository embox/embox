/**
 * @file
 * @brief Internal implementation of net self definition macros.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SELF_IMPL_H_
#define FRAMEWORK_NET_SELF_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <framework/mod/self.h>

#include "types.h"

#define __EMBOX_NET(_packet) \
	extern const struct net __net_registry[];          \
	ARRAY_SPREAD_ADD_NAMED(__net_registry, __net,   {  \
			.packet = _packet,                     \
			.mod = &mod_self                   \
		});                                        \
	MOD_SELF_BIND(__net, NULL)

#endif
