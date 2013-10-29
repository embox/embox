/**
 * @file
 * @brief
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PACK_SELF_H_
#define FRAMEWORK_NET_PACK_SELF_H_

#include __impl_x(framework/net/pack/self_impl.h)

struct net_device;
struct sk_buff;

#define EMBOX_NET_PACK(_type, _rcv_pack)      \
	static int _rcv_pack(struct sk_buff *skb, \
			struct net_device *dev);          \
	__EMBOX_NET_PACK(_type, _type, _rcv_pack)

#define EMBOX_NET_PACK_OUT(_family, _ops)        \
	static const struct net_pack_out_ops _ops;   \
	__EMBOX_NET_PACK_OUT(_family, _family, _ops)

#endif /* FRAMEWORK_NET_PACK_SELF_H_ */
