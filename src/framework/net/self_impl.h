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
	extern const struct mod_ops __net_mod_ops;    \
	const struct net __net##_packet =   {         \
			.netpack = &_packet,                  \
			.mod = &mod_self                      \
		};                                        \
	MOD_INFO_BIND(&__net_mod_ops, &__net##_packet)

#endif /* FRAMEWORK_NET_SELF_IMPL_H_ */
