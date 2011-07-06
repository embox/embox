/**
 * @file
 * @brief The external API for accessing available netpacks.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_API_H_
#define FRAMEWORK_NET_API_H_

#include __impl_x(framework/net/api_impl.h)

#define net_foreach(net_ptr) \
		__net_foreach(net_ptr)

#endif /* FRAMEWORK_NET_API_H_ */
