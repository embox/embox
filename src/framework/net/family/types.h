/**
 * @file
 * @brief Type declarations shared between net framework and each net family suite.
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_TYPES_H_
#define FRAMEWORK_NET_FAMILY_TYPES_H_

#include <stddef.h>

/**
 * Prototypes
 */
struct family_ops;

struct net_family_type {
	int type;
	const struct family_ops *ops;
};

/**
 * Each netfamily implements this interface.
 */
struct net_family {
	int family;
	const struct net_family_type *types;
	size_t types_sz;
	int (*init)(void);    /* initializer of this packet family */
	int (*fini)(void);    /* initializer of this packet family */
};

#endif /* FRAMEWORK_NET_FAMILY_TYPES_H_ */
