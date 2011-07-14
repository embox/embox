/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PROTO_SELF_H_
#define FRAMEWORK_NET_PROTO_SELF_H_

#include __impl_x(framework/net/proto/self_impl.h)

#define EMBOX_NET_PROTO(_type, _handler, _err_handler, _init)   \
	static int _handler(sk_buff_t *pack); 						\
	static net_protocol_t _##_type = { 							\
			.type = _type,										\
			.handler = _handler,								\
			.err_handler = _err_handler							\
		};														\
		__EMBOX_NET_PROTO(_##_type, _init)

#endif /* FRAMEWORK_NET_PROTO_SELF_H_ */
