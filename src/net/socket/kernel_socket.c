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
#include <string.h>

#include <net/sock.h>
#include <util/sys_log.h>
#include <net/kernel_socket.h>
#include <net/socket_registry.h>

int kernel_socket_create(int family, int type, int protocol, struct socket **psock,
		struct sock *sk, struct proto_ops *sk_ops) {
	int res;
	struct socket *sock;
	const struct net_proto_family *pf;

	if (!is_a_valid_sock_type(type)) {
		return -EPROTOTYPE;
	}

	if ((family == PF_INET) && (type == SOCK_PACKET)) {
		family = PF_PACKET;
	}

	/* check for family support inside */
	pf = socket_repo_get_family(family);
	if (pf == NULL) {
		return -EAFNOSUPPORT;
	}

	if (pf->create == NULL) {
		LOG_ERROR("kernel_socket_create", "no create() method for protocol family");
		return -EAFNOSUPPORT;
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
	sock->state = SS_UNCONNECTED;

	if (sk == NULL) {
		res = pf->create(sock, protocol);
		if (res < 0) {
			socket_free(sock);
			return res;
		}
	}
	else {
		if (sk_ops != NULL) {
			sock->sk = sk;
			sock->ops = sk_ops;
			sk->sk_socket = sock;
		}
		else {
			return -EINVAL;
		}
	}

	/* compare addresses method should be set, else we can't go on */
	if (sock->ops->compare_addresses == NULL){
		LOG_ERROR("kernel_socket_create", "packet family has no compare_addresses() method");
		return -EAFNOSUPPORT;
	}

	/* TODO here we must be code for trying socket (permition and so on)
	 err = security_socket_post_create(sock, family, type, protocol, kern);
	 */

	sock_set_ready(sock->sk);
	/* newly created socket is UNCONNECTED for sure */
	sk_set_connection_state(sock, UNCONNECTED);
	/* addr socket entry to registry */
	if (0 > (res = sr_add_socket_to_registry(sock))) {
		return res;
	}
	/* set default socket options */
	so_options_init(&sock->socket_node->options, type);
	*psock = sock; /* and save structure */

	return ENOERR;
}

/* should be understood as close method */
int kernel_socket_release(struct socket *sock) {
	int res;

	sk_set_connection_state(sock, DISCONNECTING);

	/* since we are releaseing no more connections can be accepted */
	if (sk_is_listening(sock))
		sock->socket_node->options.so_acceptconn = 0;

	/* socket will be unbound, if it is bound else nothing happens */
	sr_remove_saddr(sock);  /* unset saddr in registry */
	/* remove socketentry from registry */
	if (0 < (res = sr_remove_socket_from_registry(sock))){
		LOG_WARN("kernel_socket_release","couldn't remove entry from registry");
		/* return res; */
	}

	res = ENOERR;
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

	/* NOTE: return values that are not processed yet (mostly for
		  nonblocking sockets):
		 -EALREADY An assignment request is already in progress for the
		 specified socket.
		 -EINPROGRESS O_NONBLOCK is set for the file descriptor for
		 the socket and the assignment cannot be immediately performed; the
		 assignment shall be performed asynchronously. -- this probably should be
		 in socket-interface level bind() // ttimkk
		 -ENOBUFS Insufficient resources were available to complete the call.
	*/

	if (!sock->ops->bind) {
		return -EOPNOTSUPP;
	}

	/* if addrlen is not equal to sizeof(sockaddr) -EINVAL is returned
	   like in linux. posix doesn't mention this */
	if(addrlen != sizeof(struct sockaddr)) {
		return -EINVAL;
	}

	/* rebinding is forbidden for now, the check for the protocol
	   rebinding permission should be added to meet posix*/
	if (sk_is_bound(sock)) {
		return -EINVAL;
	}

	/* find out via registry if address is occupied */
	if (!sr_is_saddr_free(sock, (struct sockaddr *)addr)) {
		return -EADDRINUSE;
	}

	/* find out if socket sock is registered in the system */
	if (!sr_socket_exists(sock)) {
		return -ENOTSOCK;
	}

	/* if sockaddr structure's family field doesn't make sense here
	   return EAFNOSUPPORT. further address availability should be
	   checked in place by each protocol family*/
	if (sock->sk->sk_family != addr->sa_family) {
		return -EAFNOSUPPORT;
	}

	/* try to bind */
	res = sock->ops->bind(sock, (struct sockaddr *) addr, addrlen);
	if (res < 0) {  /* If something went wrong */
		LOG_ERROR("kernel_socket_bind", "error binding socket");
		/* Set the state to UNCONNECTED */
		sk_set_connection_state(sock, UNCONNECTED);
		return res;
	} else {
		sk_set_connection_state(sock, BOUND);  /* Everything turned out fine */
		sr_set_saddr(sock, addr);  /* set saddr in registry */
	}
	return ENOERR;
}

int kernel_socket_listen(struct socket *sock, int backlog) {
	int res;
	/* TODO come up with an idea about listening queue */
	/* -EACCES may be returned if the process doesn't have enough privileges */
	/* TODO the situation when socket is shut down should be handled */
	/* NOTE if insufficient resources are available ENOBUFS is the errno */

	/* no listen method is supported for that type of socket */
	if (!sock->ops->listen){
		return -EOPNOTSUPP;
	}

	/* find out if socket sock is registered in the system */
	if (!sr_socket_exists(sock)){
		return -ENOTSOCK;
	}

	/* the socket is unbound */
	if (!sk_is_bound(sock)){  /* the socket should first be bound to an address */
		/* debug_printf("Socket should be bound to accept", */
		/* 						 "kernel_socket", "kernel_socket_listen"); */
		LOG_INFO("kernel_socket_listen", "socket should be bound to accept");
		return -EDESTADDRREQ;
	}

	/* is the socket already connected */
	if (sk_is_connected(sock)){
		return -EINVAL;
	}

	/* try to listen */
	res = sock->ops->listen(sock, backlog);
	if (res < 0){  /* If something went wrong */
		/* debug_printf("Error setting socket in listening state", */
		/* 						 "kernel_sockets", "kernel_socket_listen"); */
		LOG_ERROR("kernel_socket_listen", "error setting socket in listening state");
		/* socket was bound, so set back BOUND */
		sk_set_connection_state(sock, BOUND);
		return res;
	}
	else {
		sk_set_connection_state(sock, LISTENING);/* Everything turned out fine*/
		sock->socket_node->options.so_acceptconn = 1; /* socket options */
	}
	return res;
 }

int kernel_socket_accept(struct socket *sock, struct socket **accepted,
		struct sockaddr *addr, socklen_t *addrlen) {
	int res;
	struct sock *newsk;

	/* TODO EAGAIN or EWOULDBLOCK in case of non-blocking socket and absence
	   of incoming connections should be returned */
	/* TODO ECONNABORTED in case of connection abortion */
	/* TODO EINTR in case of interruption by a signal prior to valid connection
		 arrival */
	/* TODO EPROTO is the errno in case we are having some kind of troubles with
	   protocol */

	/* accept method is not set */
	if (!sock->ops->accept) {
		return -EOPNOTSUPP;
	}

	/* find out if socket sock is registered in the system */
	if (!sr_socket_exists(sock)) {
		return -ENOTSOCK;
	}

	/* is the socket accepting connections */
	if (!sk_is_listening(sock)) {  /* we should connect to a listening socket */
		LOG_ERROR("kernel_socket_accept",
				"socket accepting a connection should be in listening state");
		return -EINVAL;
	}

	/* try to accept */
	res = sock->ops->accept(sock->sk, &newsk, addr, addrlen);
	if (res < 0) { /* If something went wrong */
		/* debug_printf("Error while accepting a connection", */
		/* 						 "kernel_sockets", "kernel_socket_accept"); */
		LOG_ERROR("kernel_socket_accept", "error while accepting a connection");
		return res;
	}

	/* create socket with the same type, protocol and family as 'sock' */
	res = kernel_socket_create(sock->sk->__sk_common.skc_family, sock->sk->sk_type,
			sock->sk->sk_protocol, accepted, newsk, (struct proto_ops *)sock->ops);
	if (res < 0) {
		sk_common_release(newsk);
		return res;
	}
	/* set state */
	sk_set_connection_state(*accepted, ESTABLISHED);
	return res;
 }

int kernel_socket_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	int res;

	/* EACCES may ber returnedf in case of a lack of priveleges */

	/* EADDRNOTAVAIL. probably the best place - is specific
	   protocol implementation*/

	/* how should this be interpreted? */
	if (!sock->ops->connect){
		return -EOPNOTSUPP;
	}

	/* find out if socket sock is registered in the system */
	if (!sr_socket_exists(sock)) {
		return -ENOTSOCK;
	}

	/* invalid address family or length of addr*/
	if ((sizeof(struct sockaddr) != addrlen) ||
		 (sock->sk->sk_family != addr->sa_family)) {
		return -EINVAL;
	}

	/* is the socket already connected */
	if ((sock->type == SOCK_STREAM) &&
		 sk_is_connected(sock)) {
		return -EISCONN;
	}

	/* is socket listening? */
	if (sk_is_listening(sock)) {
		return -EOPNOTSUPP;
	}

	/* here is the place to check wheather the socket is bound
	   to a local address. if it is not, try to bind it.
	   is EADDRINUSE the situation when connect() is trying to
	   bind to an address already in use? */
	if (!sk_is_bound(sock)) {
		/* a method to get a local address which fits the network in
		   which the host we are trying to connect to is situated */
		/* kernel_socket_bind(sock, &localaddress, sizeof(struct sockaddr)); */
	}

	/* if the socket is non blocking, and it is in connecting state
	   at the same time with connect() being called*/
	/* (only blocking sockets are implemented now, but
	   in future this code should work, or else shall be
	   modified)*/
	if (sk_get_connection_state(sock) == CONNECTING) {
		return -EALREADY;
	}
	sk_set_connection_state(sock, CONNECTING);

	/* try to connect */
	/* specific connect method can return ECONREFUSED, ENETUNREACH,
	   EPROTOTYPE, ETIMEDOUT, ECONNRESET, EHOSTUNREACH, ELOOP,
	   ENAMETOLONG, ENETDOWN*/
	res = sock->ops->connect(sock, (struct sockaddr *) addr, addrlen, flags);
	if (res < 0) {
		/* in case of non-blocking sockets(for the future)
		   here should be handled situation when connect is trying to
		   finish asynchronously */
		LOG_ERROR("kernel_socket_connect", "unable to connect on socket");
		sk_set_connection_state(sock, BOUND);
	}
	else {
		sk_set_connection_state(sock, CONNECTED);
	}

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
		char *optval, socklen_t *optlen) {
	int res;

	/* sock is not NULL */
	if (!sock) {
		return -ENOTSOCK;
	}

	/* check if such socket exists */
	if (!sr_socket_exists(sock)) {
		return -ENOTSOCK;
	}

	if (level == SOL_SOCKET) {
		res = so_get_socket_option(&sock->socket_node->options, optname, optval,
				optlen);
		/* clear pending error if it was retrieved */
		if (optname == SO_ERROR && res >= 0) {
			sk_clear_pending_error(sock->sk);
		}
	}
	else {
		if (sock->ops->getsockopt) {
			res = sock->ops->getsockopt(sock, level, optname, optval, optlen);
		}
		else {
			/* if no getsockopt or setsockopt method is set, it should
			   probably be interpreted as no options available to get or set */
			res = -ENOPROTOOPT;
		}
	}

	return res;
}

int kernel_socket_setsockopt(struct socket *sock, int level, int optname,
		char *optval, socklen_t optlen) {
	int res;

	/* sock is not NULL */
	if (!sock) {
		return -ENOTSOCK;
	}

	/* check if such socket exists */
	if (!sr_socket_exists(sock)) {
		return -ENOTSOCK;
	}

	if (level == SOL_SOCKET) {
		res = so_set_socket_option(&sock->socket_node->options, optname, optval,
				optlen);
	}
	else {
		if (sock->ops->setsockopt) {
			res = sock->ops->setsockopt(sock, level, optname, optval, optlen);
		}
		else {
			/* if no getsockopt or setsockopt method is set, it should
			   probably be interpreted as no options available to get or set */
			res = -ENOPROTOOPT;
		}
	}
	return res;
}

int kernel_socket_sendmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *m,	size_t total_len) {
	return sock->ops->sendmsg(iocb, sock, m, total_len);
}

int kernel_socket_recvmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *m, size_t total_len, int flags) {
	return sock->ops->recvmsg(iocb, sock, m, total_len, flags);
}

int kernel_socket_shutdown(struct socket *sock) {
	return ENOERR;
}
