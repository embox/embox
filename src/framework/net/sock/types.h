/**
 * @file
 * @brief Type declarations shared between net sockets framework and each net socket suite.
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SOCK_TYPES_H_
#define FRAMEWORK_NET_SOCK_TYPES_H_

/**
 * Prototypes
 */
struct mod;
struct sock_ops;

/**
 * Each netsock implements this interface.
 */
typedef struct net_sock {
	int family;
	int type;
	int protocol;
	int is_default;
	const struct sock_ops *ops;
	int (*init)(void);
	int (*fini)(void);
	/** The corresponding mod. */
	const struct mod *mod;
} net_sock_t;

#endif /* FRAMEWORK_NET_SOCK_TYPES_H_ */
