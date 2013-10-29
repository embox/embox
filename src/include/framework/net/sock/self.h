/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_SOCK_SELF_H_
#define FRAMEWORK_NET_SOCK_SELF_H_

#include __impl_x(framework/net/sock/self_impl.h)

#define EMBOX_NET_SOCK(_family, _type, _protocol, _is_default,  \
		_ops)                                                   \
	static const struct sock_proto_ops _ops;                    \
	__EMBOX_NET_SOCK(_family##_type##_protocol, _family, _type, \
			_protocol, _is_default, _ops)

#endif /* FRAMEWORK_NET_SOCK_SELF_H_ */
