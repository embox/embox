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
		TRACE("Added sock type 0x%X for proto 0x%02X\n",
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

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	struct socket *sock = sockfd_lookup(sockfd);
	return kernel_connect(sock, addr, addrlen, 0);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	struct socket *sock = sockfd_lookup(sockfd);
	return kernel_bind(sock, addr, addrlen);
}

int close(int sockfd) {
	struct socket *sock = sockfd_lookup(sockfd);
	kernel_sock_release(sock);
	return 0;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen) {
	struct socket *sock = sockfd_lookup(sockfd);
	struct sockaddr_in *daddr = (struct sockaddr_in *)dest_addr;
	struct inet_sock *inet = inet_sk(sock->sk);
	struct iovec iov;
	struct msghdr m;
	//TODO: temporary XXX
	iov.iov_base = (void*)buf;
	iov.iov_len = len;
	m.msg_iov = &iov;
	inet->dport = daddr->sin_port;
	if(inet->sport == 0) {
		//TODO:
		inet->sport = 666;
	}
	inet->daddr = daddr->sin_addr.s_addr;
	if(kernel_sendmsg(NULL, sock, &m, len) < 0) {
		return -1;
	}
	return len;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
			struct sockaddr *src_addr, socklen_t *addrlen) {
	struct socket *sock = sockfd_lookup(sockfd);
	struct sockaddr_in *saddr = (struct sockaddr_in *)src_addr;
	struct inet_sock *inet = inet_sk(sock->sk);
	struct iovec iov;
	struct msghdr m;
	//TODO: temporary XXX
	iov.iov_base = (void*)buf;
	iov.iov_len = len;
	m.msg_iov = &iov;
	m.msg_iov->iov_len = kernel_recvmsg(NULL, sock, &m, len, flags);
	saddr->sin_port = inet->dport;
	saddr->sin_addr.s_addr = inet->daddr;
	return m.msg_iov->iov_len;
}
