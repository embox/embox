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

/**
 * @brief Add new socket type repository:
 * @param _net_proto_family - address family (e.g. AF_INET)
 * @param _type             - type of socket (e.g. SOCK_DGRAM)
 * @param _protocol         - type of protocol (e.g. UDP)
 * @param _prot             - structure containing protocol specific functions
 *                            of _protocol
 * @param _ops              - socket options based on socket type (e.g. connect)
 * @param _no_check         - checksum control on/off
 */
#define EMBOX_NET_SOCK(_net_proto_family, _type, _protocol, _prot, _ops,		\
											 _no_check, _default)																			\
	static inet_protosw_t _##_type_##_protocol = { 				\
			.type = _type,					\
			.protocol = _protocol,				\
			.prot = &_prot,                			\
			.ops = &_ops, 					\
			.no_check = _no_check,				\
      .deflt = _default  \
		};							\
		__EMBOX_NET_SOCK(_##_type_##_protocol, _net_proto_family)

#endif /* FRAMEWORK_NET_SOCK_SELF_H_ */
