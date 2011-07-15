/**
 * @file
 * @brief The external API for accessing available netprotos.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PROTO_API_H_
#define FRAMEWORK_NET_PROTO_API_H_

#include __impl_x(framework/net/proto/api_impl.h)

#define net_proto_foreach(net_proto_ptr) \
		__net_proto_foreach(net_proto_ptr)

#endif /* FRAMEWORK_NET_PROTO_API_H_ */
