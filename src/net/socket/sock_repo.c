/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */

#include <util/log.h>

#include <stdlib.h>
#include <arpa/inet.h>

#include <net/sock.h>
#include <lib/libds/dlist.h>
#include <hal/ipl.h>

struct sock * sock_iter(const struct sock_proto_ops *p_ops) {
	if (p_ops == NULL) {
		return NULL; /* error: invalid argument */
	}

	return !dlist_empty(p_ops->sock_list) ? dlist_first_entry(p_ops->sock_list, struct sock, lnk) : NULL;
}

struct sock * sock_next(const struct sock *sk) {
	if (sk == NULL) {
		return NULL; /* error: invalid argument */
	}
	else if (dlist_empty_entry(sk, lnk)) {
		return NULL; /* error: not hashed */
	}

	assert(sk->p_ops != NULL);

	return dlist_next_entry_if_not_last(sk, sk->p_ops->sock_list,
			struct sock, lnk);
}

struct sock * sock_lookup(const struct sock *sk,
		const struct sock_proto_ops *p_ops,
		sock_lookup_tester_ft tester,
		const struct sk_buff *skb) {
	ipl_t ipl;
	struct sock *next_sk;

	if ((p_ops == NULL) || (tester == NULL)) {
		return NULL; /* error: invalid arguments */
	}

	next_sk = sk != NULL ? sock_next(sk) : sock_iter(p_ops);

	ipl = ipl_save();
	{
		while (next_sk != NULL) {
			if (tester(next_sk, skb)) {
				ipl_restore(ipl);
				return next_sk;
			}
			next_sk = sock_next(next_sk);
		}
	}
	ipl_restore(ipl);

	return NULL; /* error: no such entity */
}

int sock_addr_is_busy(const struct sock_proto_ops *p_ops,
		sock_addr_tester_ft tester, const struct sockaddr *addr,
		socklen_t addrlen) {
	const struct sock *sk;

	assert(p_ops != NULL);
	assert(tester != NULL);

	sock_foreach(sk, p_ops) {
		if ((sk->addr_len == addrlen)
				&& tester(addr, sk->src_addr)) {
			return 1;
		}
	}

	return 0;
}

int sock_addr_alloc_port(const struct sock_proto_ops *p_ops,
		in_port_t *addrport, sock_addr_tester_ft tester,
		const struct sockaddr *addr, socklen_t addrlen) {
	in_port_t port;
	static uint16_t start_port = IPPORT_RESERVED;

	assert(addrport != NULL);

	if (start_port == IPPORT_RESERVED) {
		start_port = IPPORT_RESERVED + ((rand() & 0xFFFF)) % (IPPORT_USERRESERVED - IPPORT_RESERVED);
	}
	/* This is allocation of ephemeral port (https://en.wikipedia.org/wiki/Ephemeral_port).
	 * Instead of getting random port we increase port number for each
	 * new port allocation request. */
	start_port++;
	if (start_port >= IPPORT_USERRESERVED) {
		start_port = IPPORT_RESERVED;
	}

	for (port = start_port; port < IPPORT_USERRESERVED; ++port) {
		*addrport = htons(port);
		if (!sock_addr_is_busy(p_ops, tester, addr, addrlen)) {
			break;
		}
	}

	return port != IPPORT_USERRESERVED;
}
