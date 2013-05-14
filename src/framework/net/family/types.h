/**
 * @file
 * @brief Type declarations shared between net framework and each net family suite.
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_TYPES_H_
#define FRAMEWORK_NET_FAMILY_TYPES_H_

/**
 * Prototypes
 */
struct socket;

/**
 * Each netpack implements this interface.
 */
struct net_family {
	int family;
	int (*init)(void);    /* initializer of this packet family */
	int (*fini)(void);    /* initializer of this packet family */
	int (*create)(struct socket *sock, int type, int protocol);
};

#endif /* FRAMEWORK_NET_FAMILY_TYPES_H_ */
