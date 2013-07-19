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

/**
 * Each netpack implements this interface.
 */
struct net_pack {
	unsigned short type;  /* type of packet */
	int (*init)(void);    /* initializer of this packet family */
	int (*fini)(void);
	int (*handle)(struct sk_buff *skb,
			struct net_device *dev); /* packet handler */
};

#endif /* FRAMEWORK_NET_PACK_TYPES_H_ */
