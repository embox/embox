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
#include <net/socket.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>

#include <net/sock.h>
#include <util/sys_log.h>
#include <net/ksocket.h>
#include <net/socket_registry.h>
#include <embox/net/family.h>

static int ksocket_ext(int family, int type, int protocol,
		struct sock *sk, struct proto_ops *sk_ops,
		struct socket **out_sock) {
	int res;
	struct socket *sock;
	const struct net_family *nfamily;

	nfamily = net_family_lookup(family);
	if (nfamily == NULL) {
		return -EAFNOSUPPORT;
	}

	sock = socket_alloc();
	if (sock == NULL) {
		return -ENOMEM;
	}

	sock->type = type;

	if (sk == NULL) {
		assert(nfamily->create != NULL);
		res = nfamily->create(sock, type, protocol);
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
		} }

	sock->sk->sk_encap_rcv = NULL;

	/* compare addresses method should be set, else we can't go on */
	if (sock->ops->compare_addresses == NULL){
		LOG_ERROR("kernel_socket_create", "packet family has no compare_addresses() method");
		return -EAFNOSUPPORT;
	}

	/* addr socket entry to registry */
	if (0 > (res = sr_add_socket_to_registry(sock))) {
		return res;
	}

	/* newly created socket is UNCONNECTED for sure */
	sk_set_connection_state(sock, UNCONNECTED);
	/* set default socket options */
	so_options_init(&sock->socket_node->options, type);

	sock->desc_data = NULL;

	*out_sock = sock; /* and save structure */

	return 0;
}

int ksocket(int family, int type, int protocol,
		struct socket **out_sock) {
	return ksocket_ext(family, type, protocol, NULL, NULL,
			out_sock);
}

int ksocket_close(struct socket *sock) {
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

int kbind(struct socket *sock, const struct sockaddr *addr,
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
	res = sock->ops->bind(sock, addr, addrlen);
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

int kconnect(struct socket *sock, const struct sockaddr *addr,
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

int klisten(struct socket *sock, int backlog) {
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

int kaccept(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct socket **out_sock) {
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
	res = sock->ops->accept(sock->sk, &newsk, addr, addrlen, flags);
	if (res < 0) { /* If something went wrong */
		/* debug_printf("Error while accepting a connection", */
		/* 						 "kernel_sockets", "kernel_socket_accept"); */
		LOG_ERROR("kernel_socket_accept", "error while accepting a connection");
		return res;
	}

	/* create socket with the same type, protocol and family as 'sock' */
	res = ksocket_ext(sock->sk->__sk_common.skc_family, sock->sk->sk_type,
			sock->sk->sk_protocol, newsk, (struct proto_ops *)sock->ops, out_sock);
	if (res < 0) {
		sk_common_release(newsk);
		return res;
	}
	/* set state */
	sk_set_connection_state(*out_sock, ESTABLISHED);
	return res;
}

#include <net/inet_sock.h>
int ksendmsg(struct socket *sock, const struct msghdr *msg, int flags) {
	struct inet_sock *inet;
	struct sockaddr_in *dest_addr;

	assert(sock->sk);

	switch (sock->type) {
	case SOCK_STREAM:
		if (!sk_is_connected(sock)) {
			return -ENOTCONN;
		}
		break;
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_PACKET:
		if (msg->msg_name == NULL) {
			return -EDESTADDRREQ;
		}
		if (msg->msg_namelen != sizeof *dest_addr) {
			return -EINVAL;
		}
		dest_addr = (struct sockaddr_in *)msg->msg_name;
		inet = inet_sk(sock->sk);
		inet->daddr = dest_addr->sin_addr.s_addr;
		inet->dport = dest_addr->sin_port;
		break;
	}

	if (sock->sk->sk_shutdown & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	/* socket is ready for usage and has no data transmitting errors yet */
	sock->sk->sk_err = -1; /* XXX ?? */

	return sock->ops->sendmsg(NULL, sock, (struct msghdr *)msg,
			msg->msg_iov->iov_len, flags);
}

int krecvmsg(struct socket *sock, struct msghdr *msg, int flags) {
	int ret;
	struct inet_sock *inet;
	struct sockaddr_in *dest_addr;

	sched_lock();
	{
		ret = sock->ops->recvmsg(NULL, sock, msg,
				msg->msg_iov->iov_len, flags);
		if ((ret == -EAGAIN) && !(flags & O_NONBLOCK)) {
			EVENT_WAIT(&sock->sk->sock_is_not_empty, 0,
					SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
			ret = sock->ops->recvmsg(NULL, sock, msg,
					msg->msg_iov->iov_len, flags);
		}
	}
	sched_unlock();
	if (ret != 0) {
		return ret;
	}

	if ((msg->msg_name != NULL) && (msg->msg_namelen != 0)) {
		inet = inet_sk(sock->sk);
		dest_addr = (struct sockaddr_in *)msg->msg_name;
		dest_addr->sin_family = AF_INET;
		dest_addr->sin_addr.s_addr = inet->daddr;
		dest_addr->sin_port = inet->dport;
		msg->msg_namelen = sizeof *dest_addr;
	}

	return 0;
}

int kshutdown(struct socket *sock, int how) {
	int res = ENOERR;

	sock->sk->sk_shutdown |= (how + 1);

	if (sock->ops->shutdown) {
		res = sock->ops->shutdown(sock, how);
	}

	return res;
}

int kgetsockname(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen) {
	struct inet_sock *inet;
	struct sockaddr_in *src_addr;

	assert(sock->sk);

	switch (sock->sk->sk_family) {
	case AF_INET:
		if (addr == NULL) {
			SET_ERRNO(EBADF);
			return -1;
		}
		if (*addrlen < sizeof *src_addr) {
			SET_ERRNO(EINVAL);
			return -1;
		}
		src_addr = (struct sockaddr_in *)addr;
		inet = inet_sk(sock->sk);
		src_addr->sin_family = AF_INET;
		src_addr->sin_addr.s_addr = inet->rcv_saddr;
		src_addr->sin_port = inet->sport;
		*addrlen = sizeof *src_addr;
		break;
	default:
		SET_ERRNO(EINVAL);
		return -1;
	}

	return 0;
//	return sock->ops->getname(sock, addr, addrlen, 0);
}

int kgetpeername(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 1);
}

int kgetsockopt(struct socket *sock, int level, int optname,
		void *optval, socklen_t *optlen) {
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

int ksetsockopt(struct socket *sock, int level, int optname,
		const void *optval, socklen_t optlen) {
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
			res = sock->ops->setsockopt(sock, level, optname, (char*)optval, optlen);
		}
		else {
			/* if no getsockopt or setsockopt method is set, it should
			   probably be interpreted as no options available to get or set */
			res = -ENOPROTOOPT;
		}
	}
	return res;
}
