/**
 * @file
 * @brief
 *
 * @date 24.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_SOCKET_INET6_SOCK_H_
#define NET_SOCKET_INET6_SOCK_H_

#include <net/sock.h>
#include <netinet/in.h>

struct inet6_sock {
	struct sock sk; /* Base socket class (MUST BE FIRST) */
	struct sockaddr_in6 src_in6;
	struct sockaddr_in6 dst_in6;
};

static inline struct inet6_sock * to_inet6_sock(struct sock *sk) {
	return (struct inet6_sock *)sk;
}

static inline const struct inet6_sock * to_const_inet6_sock(
		const struct sock *sk) {
	return (const struct inet6_sock *)sk;
}

#endif /* NET_SOCKET_INET6_SOCK_H_ */
