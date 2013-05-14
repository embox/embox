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

struct socket;

#define EMBOX_NET_FAMILY(_family, _create)                    \
	static int _create(struct socket *sock, int type,         \
			int protocol);                                    \
	__EMBOX_NET_FAMILY(_family, _family, _create, NULL, NULL)

#define EMBOX_NET_FAMILY_INIT(_family, _create, _init)         \
	static int _create(struct socket *sock, int type,          \
			int protocol);                                     \
	static int _init(void);                                    \
	__EMBOX_NET_FAMILY(_family, _family, _create, _init, NULL)

#define EMBOX_NET_FAMILY_INIT_FINI(_family, _create, _init,     \
		_fini)                                                  \
	static int _create(struct socket *sock, int type,           \
			int protocol);                                      \
	static int _init(void);                                     \
	static int _fini(void);                                     \
	__EMBOX_NET_FAMILY(_family, _family, _create, _init, _fini)


#endif /* FRAMEWORK_NET_FAMILY_SELF_H_ */
