/**
 * @file
 * @brief The external API for accessing available netfamilies.
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_API_H_
#define FRAMEWORK_NET_FAMILY_API_H_

#include __impl_x(framework/net/family/api_impl.h)

struct net_family;
struct net_family_type;

#define net_family_foreach(net_family_ptr) \
	__net_family_foreach(net_family_ptr)

#define net_family_type_foreach(net_family_type_ptr, \
		net_family_ptr)                              \
	__net_family_type_foreach(net_family_type_ptr,   \
			net_family_ptr)

extern const struct net_family * net_family_lookup(int family);

extern const struct net_family_type * net_family_type_lookup(
		const struct net_family *nfamily, int type);

#endif /* FRAMEWORK_NET_FAMILY_API_H_ */
