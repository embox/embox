/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_SOCK_SELF_H_
#define FRAMEWORK_NET_SOCK_SELF_H_

#include __impl_x(framework/net/sock/self_impl.h)

#define EMBOX_NET_SOCK(_type, _protocol, _prot, _ops, _no_check, 	\
						net_proto_family)   						\
	static inet_protosw_t _##_type = { 								\
			.type = _type,											\
			.protocol = _protocol,									\
			.prot = &_prot,                							\
			.ops = &_ops, 											\
			.no_check = _no_check									\
		};															\
		__EMBOX_NET_SOCK(_##_type, net_proto_family)

#endif /* FRAMEWORK_NET_SOCK_SELF_H_ */
