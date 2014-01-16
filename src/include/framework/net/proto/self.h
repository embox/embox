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

struct sk_buff;

#define EMBOX_NET_PROTO(_pack, _type, _handle, _handle_error) \
	static int _handle(struct sk_buff *skb);                  \
	static void _handle_error(const struct sk_buff *skb,      \
			int error_info);                                  \
	__EMBOX_NET_PROTO(_pack##_type, _pack, _type, _handle,    \
			_handle_error)

#endif /* FRAMEWORK_NET_PROTO_SELF_H_ */
