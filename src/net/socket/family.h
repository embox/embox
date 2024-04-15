/**
 * @file
 * @brief Main Embox netfamily include file for user netpacks.
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef EMBOX_NET_FAMILY_H_
#define EMBOX_NET_FAMILY_H_

#include <stddef.h>

/**
 * Prototypes
 */
struct sock_family_ops;
struct net_pack_out_ops;

struct net_family_type {
	int type;
	const struct sock_family_ops *ops;
};

/**
 * Each netfamily implements this interface.
 */
struct net_family {
	int family;
	const struct net_family_type *types;
	size_t types_sz;
	const struct net_pack_out_ops *const *out_ops;
};

#include <lib/libds/array.h>

ARRAY_SPREAD_DECLARE(const struct net_family, __net_family_registry);

#define net_family_foreach(net_family_ptr) \
	array_spread_foreach_ptr(net_family_ptr, __net_family_registry)

#define net_family_type_foreach(net_family_type_ptr, net_family_ptr)      \
	array_foreach_ptr(net_family_type_ptr, (net_family_ptr)->types,       \
			(net_family_ptr)->types_sz)

extern const struct net_family * net_family_lookup(int family);

extern const struct net_family_type * net_family_type_lookup(
		const struct net_family *nfamily, int type);

#define EMBOX_NET_FAMILY(_family, _types,_out_ops)                        \
	static const struct net_family_type _types[];                         \
	ARRAY_SPREAD_ADD_NAMED(__net_family_registry,                         \
			__net_family_##_family, {                                     \
				.family = _family,                                        \
				.types = &_types[0],                                      \
				.types_sz = ARRAY_SIZE(_types),                           \
				.out_ops = &_out_ops                                      \
			})

#endif /* EMBOX_FAMILY_H_ */
