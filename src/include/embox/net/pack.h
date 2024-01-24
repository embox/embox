/**
 * @file
 * @brief Main Embox netpack include file for user netpacks.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef EMBOX_NET_PACK_H_
#define EMBOX_NET_PACK_H_

#include <stddef.h>
/**
 * Prototypes
 */
struct net_device;
struct sk_buff;
struct sock;
struct sockaddr;

/**
 * Netpack outgoing options
 */
struct net_pack_out_ops {
	int (*make_pack)(const struct sock *sk,
			const struct sockaddr *to,
			size_t *data_size,
			struct sk_buff **out_skb);
	int (*snd_pack)(struct sk_buff *skb);
};

/**
 * Each netpack implements this interface.
 */
struct net_pack {
	unsigned short type;  /* type of packet */
	/* packet handler */
	int (*rcv_pack)(struct sk_buff *skb,struct net_device *dev);
};

extern const struct net_pack * net_pack_lookup(unsigned short type);

#include <lib/libds/array.h>

ARRAY_SPREAD_DECLARE(const struct net_pack, __net_pack_registry);

#define net_pack_foreach(net_pack_ptr) \
	array_spread_foreach_ptr(net_pack_ptr, __net_pack_registry)

#define EMBOX_NET_PACK(_type, _rcv_pack)                                 \
	static int _rcv_pack(struct sk_buff *skb, struct net_device *dev);   \
	ARRAY_SPREAD_ADD_NAMED(__net_pack_registry,                          \
			__net_pack_##_type, {                                        \
				.type = _type,                                           \
				.rcv_pack = _rcv_pack                                    \
			})

/* Help Eclipse CDT. */
#ifdef __CDT_PARSER__
#define EMBOX_NET_PACK(_type, _rcv_pack)
#endif


#endif /* EMBOX_NET_PACK_H_ */
