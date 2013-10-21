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

#include <stddef.h>

struct net_device;
struct sk_buff;

#define EMBOX_NET_PACK(_type, _rcv_pack)                  \
	static int _rcv_pack(struct sk_buff *skb,             \
			struct net_device *dev);                      \
	__EMBOX_NET_PACK(_type, _type, _rcv_pack, NULL, NULL)

#define EMBOX_NET_PACK_INIT(_type, _rcv_pack, _init)       \
	static int _rcv_pack(struct sk_buff *skb,              \
			struct net_device *dev);                       \
	static int _init(void);                                \
	__EMBOX_NET_PACK(_type, _type, _rcv_pack, _init, NULL)

#define EMBOX_NET_PACK_INIT_FINI(_type, _rcv_pack, _init, _fini) \
	static int _rcv_pack(struct sk_buff *skb,                    \
			struct net_device *dev);                             \
	static int _init(void);                                      \
	static int _fini(void);                                      \
	__EMBOX_NET_PACK(_type, _type, _rcv_pack, _init, _fini)


#endif /* FRAMEWORK_NET_PACK_SELF_H_ */
