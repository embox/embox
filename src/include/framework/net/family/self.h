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

#include <stddef.h>

#define EMBOX_NET_FAMILY(_family, _types)                    \
	static const struct net_family_type _types[];            \
	__EMBOX_NET_FAMILY(_family, _family, _types, NULL, NULL)

#define EMBOX_NET_FAMILY_INIT(_family, _types, _init)         \
	static const struct net_family_type _types[];             \
	static int _init(void);                                   \
	__EMBOX_NET_FAMILY(_family, _family, _types, _init, NULL)

#define EMBOX_NET_FAMILY_INIT_FINI(_family, _types, _init,     \
		_fini)                                                 \
	static const struct net_family_type _types[];              \
	static int _init(void);                                    \
	static int _fini(void);                                    \
	__EMBOX_NET_FAMILY(_family, _family, _types, _init, _fini)

#endif /* FRAMEWORK_NET_FAMILY_SELF_H_ */
