/**
 * @file
 * @brief The external API for accessing available netprotos.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PROTO_API_H_
#define FRAMEWORK_NET_PROTO_API_H_

#include __impl_x(framework/net/proto/api_impl.h)

struct net_proto;

#define net_proto_foreach(net_proto_ptr) \
	__net_proto_foreach(net_proto_ptr)

extern const struct net_proto * net_proto_lookup(
		unsigned char type);

#endif /* FRAMEWORK_NET_PROTO_API_H_ */
