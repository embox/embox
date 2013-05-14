/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_PROTO_SELF_H_
#define FRAMEWORK_NET_PROTO_SELF_H_

#include __impl_x(framework/net/proto/self_impl.h)

#include <stddef.h>

#define EMBOX_NET_PROTO(_type, _handle, _handle_error)      \
	static int _handle(struct sk_buff *skb);                \
	__EMBOX_NET_PROTO(_type, _type, _handle, _handle_error, \
			NULL, NULL)

#define EMBOX_NET_PROTO_INIT(_type, _handle, _handle_error, \
		_init)                                              \
	static int _handle(struct sk_buff *skb);                \
	static int _init(void);                                 \
	__EMBOX_NET_PROTO(_type, _type, _handle, _handle_error, \
			_init, NULL)

#define EMBOX_NET_PROTO_INIT_FINI(_type, _handle, _handle_error, \
		_init, _fini)                                            \
	static int _handle(struct sk_buff *skb);                     \
	static int _init(void);                                      \
	static int _fini(void);                                      \
	__EMBOX_NET_PROTO(_type, _type, _handle, _handle_error,      \
			_init, _fini)

#endif /* FRAMEWORK_NET_PROTO_SELF_H_ */
