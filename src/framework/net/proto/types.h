/**
 * @file
 * @brief Type declarations shared between net protos framework and each net proto suite.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PROTO_TYPES_H_
#define FRAMEWORK_NET_PROTO_TYPES_H_

/**
 * Prototypes
 */
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

#endif /* FRAMEWORK_NET_PROTO_TYPES_H_ */
