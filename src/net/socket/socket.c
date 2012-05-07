/**
 * @file
 * @brief An implementation of the SOCKET network access protocol.
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <net/in.h>
#include <net/inet_sock.h>
#include <net/kernel_socket.h>
#include <net/net.h>
#include <net/socket.h>
#include <stddef.h>
#include <types.h>
#include <util/array.h>
#include <kernel/task.h>
#include <net/arp_queue.h>

#include <kernel/thread/api.h>
#include <kernel/thread/event.h>


extern const struct task_res_ops * __task_res_ops[];


static ssize_t this_read(int fd, const void *buf, size_t nbyte) {
	return recvfrom(fd, (void *) buf, nbyte, 0, NULL, 0);
}

static ssize_t this_write(int fd, const void *buf, size_t nbyte) {
	return sendto(fd, buf, nbyte, 0, NULL, 0);
}

static int this_ioctl(int fd, int request, va_list args) {
	return 0;
}

static struct task_res_ops ops = {
	.type = TASK_IDX_TYPE_SOCKET,
	.read = this_read,
	.write = this_write,
	.close = socket_close,
	.ioctl = this_ioctl
};

ARRAY_SPREAD_ADD(__task_res_ops, &ops);

static struct socket *idx2sock(int fd) {
	return (struct socket *) task_self_idx_get(fd)->data;
}

int socket(int domain, int type, int protocol) {
	int res;
	struct socket *sock;

	res = kernel_socket_create(domain, type, protocol, &sock, NULL, NULL);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1; /* return error code */
	}

	res = task_res_idx_alloc(task_self_res(), TASK_IDX_TYPE_SOCKET, sock);
	if (res < 0) { // release socket if can't alloc idx
		kernel_socket_release(sock);
		/* TODO: EMFILE should be returned when no fids left for process to use.
		   If the system cannot allocate more fids at all, ENFILE should be returned */
		SET_ERRNO(EMFILE);
		return -1;
	}
	return res;
}

int connect(int sockfd, const struct sockaddr *daddr, socklen_t daddrlen) {
	struct socket *sock;
	int res;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = kernel_socket_connect(sock, daddr, daddrlen, 0);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}
	return ENOERR;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	int res;
	struct socket *sock;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = kernel_socket_bind(sock, addr, addrlen);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}
	return ENOERR;
}

int listen(int sockfd, int backlog) {
	struct socket *sock;
	int res;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}
	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}
	res = kernel_socket_listen(sock, backlog);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}
	return ENOERR;

}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	struct socket *sock, *new_sock;
	int res;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}
	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = kernel_socket_accept(sock, &new_sock, addr, addrlen);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	res = task_res_idx_alloc(task_self_res(), TASK_IDX_TYPE_SOCKET, new_sock);
	if (res < 0) {
		kernel_socket_release(new_sock);
		SET_ERRNO(EMFILE);  /* also could be ENFILE */
		return -1;
	}
	return res;
}

static size_t sendto_sock(struct socket *sock, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {
	int res;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	iov.iov_base = (void *)buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	inet = inet_sk(sock->sk);
	if ( (sock->type == SOCK_STREAM) || (sock->type ==  SOCK_SEQPACKET) ) {
		/* Quotation: "If sendto() is used on a connection-mode (SOCK_STREAM, SOCK_SEQPACKET)
		 * socket, the arguments dest_addr and addrlen are ignored
		 * (and the error EISCONN MAY be returned when they are not NULL and 0)"
		 *------------
		 * Currently we do nothing and believe that inet has correct pair address-port
		 * from tcp_v4_accept() call. As a compromise with the old code we can
		 * check that new parameters don't change anything. If not - return
		 * an error.
		 */
	} else {
		if(!daddr || (daddrlen < sizeof(struct sockaddr_in))) {
			SET_ERRNO(EDESTADDRREQ);
			return -1;
		} else {
			dest_addr = (struct sockaddr_in *)daddr;
			inet->daddr = dest_addr->sin_addr.s_addr;
			inet->dport = dest_addr->sin_port;
		}
	}

	/* socket is ready for usage and has no data transmitting errors yet */
	sock->sk->sk_err = -1;
	sock_set_ready(sock->sk);

	res = kernel_socket_sendmsg(NULL, sock, &m, len);

	/* if(sock->sk && sock_was_transmitted(sock->sk) != 0) { */
	if(!sock_is_ready(sock->sk)){
		sock_lock(sock->sk);
		/* sleep until the event sock_is_ready fires */
		sched_sleep(&sock->sk->sock_is_ready, MAX_WAIT_TIME);
	  sock_unlock(sock->sk);
		/* res = sock_get_answer(sock->sk); */
	 }

	if(res < 0) {
		return (ssize_t)res;
	}

	return (ssize_t)iov.iov_len;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {
	int res;

	res = sendto_sock(idx2sock(sockfd), buf, len, flags, daddr, daddrlen);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}
	return res;
}

int check_icmp_err(int sockfd) {
	struct socket *sock;
	int err;

	sock = idx2sock(sockfd);
	err = sock->sk->sk_err;
	sock->sk->sk_err = -1;

	return err;
}

static ssize_t recvfrom_sock(struct socket *sock, void *buf, size_t len, int flags,
			struct sockaddr *daddr, socklen_t *daddrlen) {
	int res;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	if (sock == NULL) {
		return -EBADF;
	}

	iov.iov_base = buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	res = kernel_socket_recvmsg(NULL, sock, &m, len, flags);
	if (res < 0) {
		return res;
	}

	inet = inet_sk(sock->sk);
	if ((daddr != NULL) && (daddrlen != NULL)) {
		dest_addr = (struct sockaddr_in *)daddr;
		dest_addr->sin_addr.s_addr = inet->daddr;
		dest_addr->sin_port = inet->dport;
		*daddrlen = sizeof *dest_addr;
	}

	return iov.iov_len; /* return length of received msg */
}


ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
			struct sockaddr *daddr, socklen_t *daddrlen) {
	int res;

	res = recvfrom_sock(idx2sock(sockfd), buf, len, flags, daddr, daddrlen);
	if(res <0){
		SET_ERRNO(-res);
		return -1;
	}
	return res;
}

int socket_close(int sockfd) {
	int res;
	struct socket *sock;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = kernel_socket_release(sock);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}

	return ENOERR;
}

int getsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t *optlen){
	struct socket *sock;
	int res;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}
	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}
	res = kernel_socket_getsockopt(sock, level, optname,
			optval, optlen);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}
	return ENOERR;

}

int setsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t optlen){
	struct socket *sock;
	int res;

	if (sockfd < 0) {
		SET_ERRNO(EBADF);
		return -1;
	}
	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}
	res = kernel_socket_setsockopt(sock, level, optname,
			optval, optlen);
	if(res < 0){
		SET_ERRNO(-res);
		return -1;
	}
	return ENOERR;

}
