/**
 * @file
 * @brief Type declarations shared between net framework and each net packet suite.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PACK_TYPES_H_
#define FRAMEWORK_NET_PACK_TYPES_H_

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
			const struct sockaddr *to, size_t *data_size,
			struct sk_buff **out_skb);
	int (*snd_pack)(struct sk_buff *skb);
};

/**
 * Each netpack implements this interface.
 */
struct net_pack {
	unsigned short type;  /* type of packet */
	int (*rcv_pack)(struct sk_buff *skb,
			struct net_device *dev); /* packet handler */
};

#endif /* FRAMEWORK_NET_PACK_TYPES_H_ */
