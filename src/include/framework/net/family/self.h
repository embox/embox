/**
 * @file
 * @brief
 *
 * @date 12.05.13
 * @author Ilia Vaprol
 */

#ifndef FRAMEWORK_NET_FAMILY_SELF_H_
#define FRAMEWORK_NET_FAMILY_SELF_H_

#include __impl_x(framework/net/family/self_impl.h)

#define EMBOX_NET_FAMILY(_family, _types, _out_ops)        \
	static const struct net_family_type _types[];          \
	__EMBOX_NET_FAMILY(_family, _family, _types, _out_ops)

#endif /* FRAMEWORK_NET_FAMILY_SELF_H_ */
