/**
 * @file
 * @brief The external API for accessing available netpacks.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PACK_API_H_
#define FRAMEWORK_NET_PACK_API_H_

#include __impl_x(framework/net/pack/api_impl.h)

#define net_pack_foreach(net_pack_ptr) \
		__net_pack_foreach(net_pack_ptr)

#endif /* FRAMEWORK_NET_PACK_API_H_ */
