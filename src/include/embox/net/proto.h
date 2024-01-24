/**
 * @file
 * @brief Main Embox netproto include file for user netprotos.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef EMBOX_NET_PROTO_H_
#define EMBOX_NET_PROTO_H_

struct sk_buff;

/**
 * Each netproto implements this interface.
 */
typedef struct net_proto {
	unsigned short pack;
	unsigned char type;
	int (*handle)(struct sk_buff *skb);
	void (*handle_error)(const struct sk_buff *skb, int error_info);
} net_proto_t;


extern const struct net_proto * net_proto_lookup(
		unsigned short pack, unsigned char type);
static inline void net_proto_handle_error_none(
		const struct sk_buff *skb, int error_info) { }

#include <lib/libds/array.h>

ARRAY_SPREAD_DECLARE(const struct net_proto, __net_proto_registry);

#define net_proto_foreach(net_proto_ptr) \
	array_spread_foreach_ptr(net_proto_ptr, __net_proto_registry)

#define EMBOX_NET_PROTO(_pack, _type, _handle, _handle_error)                \
	static int _handle(struct sk_buff *skb);                                 \
	static void _handle_error(const struct sk_buff *skb, int error_info);    \
	ARRAY_SPREAD_DECLARE(const struct net_proto, __net_proto_registry);      \
	ARRAY_SPREAD_ADD_NAMED(__net_proto_registry,                             \
			__net_proto_##_pack##_type, {                                    \
				.pack = _pack,                                               \
				.type = _type,                                               \
				.handle = _handle,                                           \
				.handle_error = _handle_error                                \
			})

#endif /* EMBOX_NET_PROTO_H_ */
