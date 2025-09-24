/**
 * @file
 * @brief
 *
 * @date 05.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <net/net_sock.h>
#include <stddef.h>
#include <lib/libds/array.h>

ARRAY_SPREAD_DEF(const struct net_sock, __net_sock_registry);

const struct net_sock * net_sock_lookup(int family, int type,
		int protocol) {
	const struct net_sock *nsock;

	net_sock_foreach(nsock) {
		if ((nsock->family == family)
				&& (nsock->type == type)
				&& ((nsock->protocol == protocol)
					|| ((protocol == 0)
						&& (nsock->is_default)))) {
			return nsock;
		}
	}

	return NULL;
}
