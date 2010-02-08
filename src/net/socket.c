/**
 * @file socket.c
 *
 * @brief An implementation of the SOCKET network access protocol.
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */

#include <stdio.h>
#include <string.h>
#include <common.h>
#include <kernel/module.h>
#include <net/skbuff.h>
#include <net/socket.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/if_ether.h>
#include <net/net_pack_manager.h>
#include <net/protocol.h>

/**
 * Close a socket
 */
void sock_release(struct socket *sock) {
	if (sock->ops) {
		sock->ops->release(sock);
		sock->ops = NULL;
	}
}

int __init sock_init(void) {
	extern inet_protosw_t *__ipstack_sockets_start, *__ipstack_sockets_end;
	inet_protosw_t ** p_netsock = &__ipstack_sockets_start;
	for (; p_netsock < &__ipstack_sockets_end; p_netsock++) {
		/*register socket type*/
		 TRACE("Added sock type 0x%X for proto 0x%X\n", (* p_netsock)->type, (* p_netsock)->protocol);
	}
	/* Initialize sock. */
	//sk_init();

	/* Initialize skbuff. */
	skb_init();
	return 0;
}

int socket(int domain, int type, int protocol) {
	return 0;
}

int connect (int sockfd, const struct sockaddr *addr, int addrlen) {
	return 0;
}

int bind(int sockfd, const struct sockaddr *addr, int addrlen) {
	return 0;
}

int close(int sockfd) {
	return 0;
}

int send(int sockfd, const void *buf, int len, int flags) {
	return len;
}

int recv(int sockfd, void *buf, int len, int flags) {
	return len;
}
