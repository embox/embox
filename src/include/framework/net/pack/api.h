/**
 * @file
 * @brief The external API for accessing available netpacks.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PACK_API_H_
#define FRAMEWORK_NET_PACK_API_H_

#include __impl_x(framework/net/pack/api_impl.h)

struct net_pack;
struct net_pack_out;

#define net_pack_foreach(net_pack_ptr) \
	__net_pack_foreach(net_pack_ptr)

extern const struct net_pack * net_pack_lookup(
		unsigned short type);

#endif /* FRAMEWORK_NET_PACK_API_H_ */
