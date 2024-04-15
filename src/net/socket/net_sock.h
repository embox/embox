/**
 * @file
 * @brief Main Embox netsock include file for user net sockets.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#ifndef EMBOX_NET_SOCK_H_
#define EMBOX_NET_SOCK_H_

struct sock_proto_ops;

/**
 * Each netsock implements this interface.
 */
struct net_sock {
	int family;
	int type;
	int protocol;
	int is_default;
	const struct sock_proto_ops *ops;
};

extern const struct net_sock * net_sock_lookup(int family,
		int type, int protocol);

#include <lib/libds/array.h>

ARRAY_SPREAD_DECLARE(const struct net_sock, __net_sock_registry);

#define net_sock_foreach(net_sock_ptr) \
	array_spread_foreach_ptr(net_sock_ptr, __net_sock_registry)

#define EMBOX_NET_SOCK(_family, _type, _protocol, _is_default, _ops)   \
	static const struct sock_proto_ops _ops;                           \
	ARRAY_SPREAD_DECLARE(const struct net_sock,                        \
			__net_sock_registry);                                      \
	ARRAY_SPREAD_ADD_NAMED(__net_sock_registry,                        \
			__net_sock_##_family##_type##_protocol, {                  \
				.family = _family,                                     \
				.type = _type,                                         \
				.protocol = _protocol,                                 \
				.is_default = _is_default,                             \
				.ops = &_ops                                           \
			})

#endif /* EMBOX_NET_SOCK_H_ */
