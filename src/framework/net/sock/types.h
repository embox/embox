/**
 * @file
 * @brief Type declarations shared between net sockets framework and each net socket suite.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SOCK_TYPES_H_
#define FRAMEWORK_NET_SOCK_TYPES_H_
#if 0
/**
 * Prototypes
 */
struct mod;
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
#endif
#endif /* FRAMEWORK_NET_SOCK_TYPES_H_ */
