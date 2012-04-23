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
#include <util/debug_msg.h>
#include <net/kernel_socket.h>
#include <net/socket_registry.h>

int kernel_socket_create(int family, int type, int protocol, struct socket **psock) {
	int res;
	struct socket *sock;
	const struct net_proto_family *pf;

	if(!is_a_valid_sock_type(type))
		return -EPROTOTYPE;

  if ((family == PF_INET) && (type == SOCK_PACKET)) {
    family = PF_PACKET;
  }

  /* check for family support inside */
  pf = socket_repo_get_family(family);
  if (pf == NULL)
    return -EAFNOSUPPORT;

  if (pf->create == NULL){
    debug_printf("no create() method for protocol family",
                 "kernel_socket", "kernel_socket_create");
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

  res = pf->create(sock, protocol);
	if (res < 0) {
		socket_free(sock);
		return res;
	}

	/* compare addresses method should be set, else we can't go on */
	if(sock->ops->compare_addresses == NULL){
		debug_printf("packet family has no compare_addresses() method",
								 "kernel_socket", "kernel_socket_create");
		return -EAFNOSUPPORT;
	}

	/* TODO here we must be code for trying socket (permition and so on)
	 err = security_socket_post_create(sock, family, type, protocol, kern);
	 */

	/* initialize socket_is_ready event */
	event_init(&sock->sk->sock_is_ready, "socket_is_ready");
	sock_set_ready(sock->sk);
	/* newly created socket is UNCONNECTED for sure */
	sk_set_connection_state(sock, UNCONNECTED);
	/* addr socket entry to registry */
	if( 0 > (res = sr_add_socket_to_registry(sock)))
		return res;
	*psock = sock; /* and save structure */

	return ENOERR;
}

/* should be understood as close method */
int kernel_socket_release(struct socket *sock) {
	int res;

	if(sk_is_connected(sock))
		sk_set_connection_state(sock, DISCONNECTING);

	/* socket will be unbound, if it is bound else nothing happens */
	sr_remove_saddr(sock);  /* unset saddr in registry */
	/* remove socketentry from registry */
	if(0 > (res = sr_remove_socket_from_registry(sock))){
		debug_printf("couldn't remove entry from registry",
								 "kernel_socket", "kernel_socket_release");
		return res;
	}

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

	if(!sock->ops->bind){
		debug_printf("No bind() method", "kernel_socket", "kernel_socket_bind");
		return -EOPNOTSUPP;
	}

	/* if addrlen is not equal to sizeof(sockaddr) -EINVAL is returned
	   like in linux. posix doesn't mention this */
	if(addrlen != sizeof(struct sockaddr)){
		return -EINVAL;
	}

	/* rebinding is forbidden for now, the check for the protocol
	   rebinding permission should be added to meet posix*/
	if(sk_is_bound(sock)){
		return -EINVAL;
	}

	/* find out via registry if address is occupied */
	if(!sr_is_saddr_free(sock, (struct sockaddr *)addr)){
		return -EADDRINUSE;
	}

	/* find out if socket sock is registered in the system */
	if(!sr_socket_exists(sock)){
		return -ENOTSOCK;
	}

	/* if sockaddr structure's family field doesn't make sense here
	   return EAFNOSUPPORT. further address availability should be
	   checked in place by each protocol family*/
	if(sock->sk->sk_family != addr->sa_family){
		return -EAFNOSUPPORT;
	}

	/* try to bind */
	res = sock->ops->bind(sock, (struct sockaddr *) addr, addrlen);
	if(res < 0){  /* If something went wrong */
		debug_printf("error while binding socket",
								 "kernel_sockets", "kernel_socket_bind");
		/* Set the state to UNCONNECTED */
		sk_set_connection_state(sock, UNCONNECTED);
		return res;
	}else{
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
	if(!sock->ops->listen){
		debug_printf("No listen() method", "kernel_socket", "kernel_socket_listen");
		return -EOPNOTSUPP;
	}

	/* find out if socket sock is registered in the system */
	if(!sr_socket_exists(sock)){
		return -ENOTSOCK;
	}

	/* the socket is unbound */
	if(!sk_is_bound(sock)){  /* the socket should first be bound to an address */
		debug_printf("Socket should be bound to accept",
								 "kernel_socket", "kernel_socket_listen");
		return -EDESTADDRREQ;
	}

	/* is the socket already connected */
	if(sk_is_connected(sock)){
		return -EINVAL;
	}

	/* try to listen */
	res = sock->ops->listen(sock, backlog);
	if(res < 0){  /* If something went wrong */
		debug_printf("Error setting socket in listening state",
								 "kernel_sockets", "kernel_socket_listen");
		/* socket was bound, so set back BOUND */
		sk_set_connection_state(sock, BOUND);
		return res;
	}else
		sk_set_connection_state(sock, LISTENING);/* Everything turned out fine*/
	return res;
 }

int kernel_socket_accept(struct socket *sock, struct socket **accepted,
												 struct sockaddr *addr, socklen_t *addrlen) {
	int res;
	struct socket *new_sock;

	/* TODO EAGAIN or EWOULDBLOCK in case of non-blocking socket and absence
	   of incoming connections should be returned */
	/* TODO ECONNABORTED in case of connection abortion */
	/* TODO EINTR in case of interruption by a signal prior to valid connection
		 arrival */
	/* TODO EPROTO is the errno in case we are having some kind of troubles with
	   protocol */

	/* accept method is not set */
	if(!sock->ops->accept){
		debug_printf("No accept() method", "kernel_socket", "kernel_socket_accept");
		return -EOPNOTSUPP;
	}

	/* find out if socket sock is registered in the system */
	if(!sr_socket_exists(sock)){
		return -ENOTSOCK;
	}

	/* is the socket accepting connections */
	if(!sk_is_listening(sock)){  /* we should connect to a listening socket */
		debug_printf("Socket accepting a connection should be in listening state",
								 "kernel_socket", "kernel_socket_accept");
		return -EINVAL;
	}

	/* create socket with the same type, protocol and family as 'sock' */
	res = kernel_socket_create(sock->sk->__sk_common.skc_family,
														 sock->sk->sk_type,
														 sock->sk->sk_protocol, &new_sock);
	if (res < 0) {
		return res;
	}
	/* try to accept */
	res = sock->ops->accept(sock, new_sock, addr, addrlen);
	if (res < 0) { /* If something went wrong */
		debug_printf("Error while accepting a connection",
								 "kernel_sockets", "kernel_socket_accept");
		kernel_socket_release(new_sock);
	}
	else {
		*accepted = new_sock;
		/* set state */
		sk_set_connection_state(new_sock, ESTABLISHED);
	}
	return res;
 }

int kernel_socket_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	int res;

	/* EACCES may ber returnedf in case of a lack of priveleges */

	/* EADDRNOTAVAIL. probably the best place - is specific
	   protocol implementation*/

	/* how should this be interpreted? */
	if(!sock->ops->connect){
		debug_printf("No connect() method", "kernel_socket",
								 "kernel_socket_connect");
		return -EOPNOTSUPP;
	}

	/* find out if socket sock is registered in the system */
	if(!sr_socket_exists(sock)){
		return -ENOTSOCK;
	}

	/* invalid address family or length of addr*/
	if((sizeof(struct sockaddr) != addrlen)||
		 (sock->sk->sk_family != addr->sa_family)){
		return -EINVAL;
	}

	/* is the socket already connected */
	if((sock->type == SOCK_STREAM) &&
		 sk_is_connected(sock)){
		return -EISCONN;
	}

	/* is socket listening? */
	if(sk_is_listening(sock)){
		return -EOPNOTSUPP;
	}

	/* here is the place to check wheather the socket is bound
	   to a local address. if it is not, try to bind it.
		 is EADDRINUSE the situation when connect() is trying to
	   bind to an address already in use? */
	if(!sk_is_bound(sock)){
		/* a method to get a local address which fits the network in
		   which the host we are trying to connect to is situated */
		/* kernel_socket_bind(sock, &localaddress, sizeof(struct sockaddr)); */
	}

	/* if the socket is non blocking, and it is in connecting state
	   at the same time with connect() being called*/
	/* (only blocking sockets are implemented now, but
     in future this code should work, or else shall be
	   modified)*/
	if(sk_get_connection_state(sock) == CONNECTING){
		return -EALREADY;
	}
	sk_set_connection_state(sock, CONNECTING);

	/* try to connect */
	/* specific connect method can return ECONREFUSED, ENETUNREACH,
	   EPROTOTYPE, ETIMEDOUT, ECONNRESET, EHOSTUNREACH, ELOOP,
	   ENAMETOLONG, ENETDOWN*/
	res = sock->ops->connect(sock, (struct sockaddr *) addr, addrlen, flags);
	if(res < 0){
		/* in case of non-blocking sockets(for the future)
		   here should be handled situation when connect is trying to
		   finish asynchronously */
		debug_printf("Unable to connect on socket",
								 "kernel_sockets", "kernel_socket_connect");
		sk_set_connection_state(sock, BOUND);
	}else
		sk_set_connection_state(sock, CONNECTED);

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
	int res;

	if(level == SOL_SOCKET){
		res = so_get_socket_option(&sock->socket_node->options, optname, optval,
															 (socklen_t*)optlen);
	}else{
		res = sock->ops->getsockopt(sock, level, optname, optval, optlen);
	}

	return res;
}

int kernel_socket_setsockopt(struct socket *sock, int level, int optname,
		char *optval, int optlen) {
	int res;

	if(level == SOL_SOCKET){
		res = so_set_socket_option(&sock->socket_node->options, optname, optval,
															 (socklen_t)optlen);
	}else{
		res = sock->ops->setsockopt(sock, level, optname, optval, optlen);
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

int kernel_socket_shutdown(struct socket *sock){
	return ENOERR;
}
