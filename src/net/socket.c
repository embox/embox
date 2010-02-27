/**
 * @file
 * @brief An implementation of the SOCKET network access protocol.
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */
#include <stdio.h>
#include <string.h>
#include <common.h>
#include <kernel/module.h>
#include <net/socket.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/kernel_socket.h>
#include <linux/init.h>

int __init sock_init(void) {
	extern inet_protosw_t *__ipstack_sockets_start, *__ipstack_sockets_end;
	inet_protosw_t ** p_netsock = &__ipstack_sockets_start;
	kernel_sock_init();
	sk_init();
	for (; p_netsock < &__ipstack_sockets_end; p_netsock++) {
		/*register socket type*/
		TRACE("Added sock type 0x%X for proto 0x%X\n",
				(* p_netsock)->type, (* p_netsock)->protocol);
	}

	/* Initialize skbuff. */
	skb_init();
	return 0;
}

/* TODO: remove all below from here */

int socket(int domain, int type, int protocol) {
	struct socket *res;
	sock_create_kern(domain, type, protocol, &res);
	return sock_get_fd(res);
}

int connect (int sockfd, const struct sockaddr *addr, int addrlen) {
	struct socket *sock = sockfd_lookup(sockfd);
	return kernel_connect(sock, addr, addrlen, 0);
}

int bind(int sockfd, const struct sockaddr *addr, int addrlen) {
	struct socket *sock = sockfd_lookup(sockfd);
	return kernel_bind(sock, addr, addrlen);
}

int close(int sockfd) {
	struct socket *sock = sockfd_lookup(sockfd);
	kernel_sock_release(sock);
	return 0;
}

int send(int sockfd, const void *buf, int len, int flags) {
	struct socket *sock = sockfd_lookup(sockfd);
	//TODO: temporary XXX
	struct iovec iov = {
		.iov_base = (void*)buf,
	 	.iov_len = len
	};
	static struct msghdr m;
	m.msg_iov = &iov;
	kernel_sendmsg(NULL, sock, &m, len);
	return len;
}

int recv(int sockfd, void *buf, int len, int flags) {
	struct socket *sock = sockfd_lookup(sockfd);
	//TODO: temporary XXX
	struct iovec iov = {
		.iov_base = (void*)buf,
		.iov_len = len
	};
	static struct msghdr m;
	m.msg_iov = &iov;
	kernel_recvmsg(NULL, sock, &m, len, flags);
	return m.msg_iov->iov_len;
}
