/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <hal/ipl.h>
#include <linux/aio.h>
#include <net/net.h>
#include <net/socket.h>
#include <stddef.h>
#include <types.h>

#include <net/sock.h>
#include <util/debug_msg.h>


int kernel_socket_create(int family, int type, int protocol, struct socket **psock) {
	int res;
	struct socket *sock;
	const struct net_proto_family *pf;

	if ((type < 0) || (type >= SOCK_MAX)) {
		return -EINVAL;
	}
	if ((family == PF_INET) && (type == SOCK_PACKET)) {
		family = PF_PACKET;
	}

	pf = socket_repo_get_family(family);

	if ((pf == NULL) || (pf->create == NULL)) {
		return -EINVAL;
	}

	/* TODO here must be code for trying socket (permition and so on)
	 err = security_socket_create(family, type, protocol, kern);
	 if (err)
	 return err;
	 */

	/*
	 * Allocate the socket and allow the family to set things up. if
	 * the protocol is 0, the family is instructed to select an appropriate
	 * default.
	 */
	sock = socket_alloc();
	if (sock == NULL) {
		return -ENOMEM;
	}

	sock->type = type;

	res = pf->create(sock, protocol);
	if (res < 0) {
		socket_free(sock);
		return res;
	}

	/* TODO here we must be code for trying socket (permition and so on)
	 err = security_socket_post_create(sock, family, type, protocol, kern);
	 */

	/* newly created socket is UNCONNECTED for sure */
	sk_set_connection_state(sock->sk, UNCONNECTED);
	*psock = sock; /* and save structure */

	return res;
}

/* should be understood as close method */
int kernel_socket_release(struct socket *sock) {
	int res;

	if(sk_is_connected(sock->sk))
		sk_set_connection_state(sock->sk, DISCONNECTING);

	if ((sock != NULL) && (sock->ops != NULL)
			&& (sock->ops->release != NULL)) {
		res = sock->ops->release(sock); /* release struct sock */
		if (res < 0) {
			return res;
		}
	}

	socket_free(sock);

	return ENOERR;
}

int kernel_socket_bind(struct socket *sock, const struct sockaddr *addr,
			socklen_t addrlen) {
	int res;

	if(!sock->ops->bind){
		debug_printf("No bind() method", "kernel_socket", "kernel_socket_bind");
		return SK_NO_SUCH_METHOD;
	}

	/* try to bind */
	res = sock->ops->bind(sock, (struct sockaddr *) addr, addrlen);
	if(res){  /* If something went wrong */
		debug_printf("error while binding socket", "kernel_sockets", "kernel_socket_bind");
		sk_set_connection_state(sock->sk, UNCONNECTED);  /* Set the state to UNCONNECTED */
	}else
		sk_set_connection_state(sock->sk, BOUND);  /* Everything turned out fine */
	return res;
}

int kernel_socket_listen(struct socket *sock, int backlog) {
	int res;

	if(!sock->ops->listen){
		debug_printf("No listen() method", "kernel_socket", "kernel_socket_bind");
		return SK_NO_SUCH_METHOD;
	}

	if(!sk_is_bound(sock->sk)){  /* the socket should first be bound to an adress */
		debug_printf("Socket should be bound to accept",
								 "kernel_socket", "kernel_socket_bind");
		return SK_ERR;
	}

	/* try to listen */
	res = sock->ops->listen(sock, backlog);
	if(res){  /* If something went wrong */
		debug_printf("Error setting socket in listening state",
								 "kernel_sockets", "kernel_socket_bind");
		/* Set the state to UNCONNECTEd */
		sk_set_connection_state(sock->sk, UNCONNECTED);
	}else
		sk_set_connection_state(sock->sk, LISTENING);  /* Everything turned out fine */
	return res;
 }

int kernel_socket_accept(struct socket *sock, struct sockaddr *addr, socklen_t *addrlen) {
	int res;

	if(!sock->ops->accept){
		debug_printf("No accept() method", "kernel_socket", "kernel_socket_bind");
		return SK_NO_SUCH_METHOD;
	}

	if(!sk_is_listening(sock->sk)){  /* we should connect to a listening socket */
		debug_printf("Socket cccepting a connection should be in listening state",
								 "kernel_socket", "kernel_socket_bind");
		return SK_ERR;
	}

	/* try to accept */
	res = sock->ops->accept(sock, addr, addrlen);
	if(res)  /* If something went wrong */
		debug_printf("Error while accepting a connection",
								 "kernel_sockets", "kernel_socket_bind");
	else
		sk_set_connection_state(sock->sk, ESTABLISHED);  /* Everything turned out fine */
	return res;
 }

int kernel_socket_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	int res;

	/* Socket's like UDP are 'connectionless'. that means that connect
	   only sets binds the target of the socket to an address.
	   In the cases of stream protocols the meaning is strict - initiate
	   connection to a given host*/

	/* TODO add code for the mentioned above cases */

	if(!sock->ops->connect){
		debug_printf("No connect() method", "kernel_socket", "kernel_socket_bind");
		return SK_NO_SUCH_METHOD;
	}

	/* try to connect */
	sk_set_connection_state(sock->sk, CONNECTING);
	res = sock->ops->connect(sock, (struct sockaddr *) addr, addrlen, flags);
	if(!res){  /* smth's wrong */
		debug_printf("Unable to connect on socket",
								 "kernel_sockets", "kernel_socket_bind");
		sk_set_connection_state(sock->sk, UNCONNECTED);
		return res;
	}else
		sk_set_connection_state(sock->sk, CONNECTED);

	return res;
}

int kernel_socket_getsockname(struct socket *sock,
			struct sockaddr *addr, int *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 0);
}

int kernel_socket_getpeername(struct socket *sock,
			struct sockaddr *addr, int *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 1);
}

int kernel_socket_getsockopt(struct socket *sock, int level, int optname,
		char *optval, int *optlen) {
	return sock->ops->getsockopt(sock, level, optname, optval, optlen);
}

int kernel_socket_setsockopt(struct socket *sock, int level, int optname,
		char *optval, int optlen) {
	return sock->ops->setsockopt(sock, level, optname, optval, optlen);
}

int kernel_socket_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len) {
	sock_set_ready(sock->sk);
	return sock->ops->sendmsg(iocb, sock, m, total_len);
}

int kernel_socket_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len, int flags) {
	return sock->ops->recvmsg(iocb, sock, m, total_len, flags);
}

/* These two dummy functions should do smth. Or maybe they aren't necessary */
int kernel_socket_shutdown(struct socket *sock){
	return ENOERR;
}
