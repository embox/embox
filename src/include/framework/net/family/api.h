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

#define net_family_foreach(net_family_ptr) \
	__net_family_foreach(net_family_ptr)

extern const struct net_family * net_family_lookup(int family);

#endif /* FRAMEWORK_NET_FAMILY_API_H_ */
