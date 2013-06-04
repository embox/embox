/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <net/socket.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <net/netdevice.h>

#include <util/math.h>

#include <net/sock.h>
#include <util/sys_log.h>
#include <net/ksocket.h>
#include <net/socket_registry.h>
#include <embox/net/family.h>

static int ksocket_ext(int family, int type, int protocol,
		struct sock *sk, struct family_ops *sk_ops,
		struct socket **out_sock) {
	int res;
	struct socket *sock;
	const struct net_family *nfamily;

	assert(out_sock != NULL);

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

	sock->desc_data = NULL;

	*out_sock = sock; /* and save structure */

	return 0;
}

int ksocket(int family, int type, int protocol,
		struct socket **out_sock) {
	if (out_sock == NULL) {
		return -EINVAL;
	}

	return ksocket_ext(family, type, protocol, NULL, NULL,
			out_sock);
}

int ksocket_close(struct socket *sock) {
	if (sock == NULL) {
		return -EBADF;
	}

	assert(sr_socket_exists(sock));

	sk_set_connection_state(sock, DISCONNECTING);

	sr_remove_saddr(sock);

	if (0 != sr_remove_socket_from_registry(sock)) {
		LOG_WARN("ksocket_close",
				"couldn't remove entry from registry");
	}

	assert(sock->ops != NULL);
	if (sock->ops->release != NULL) {
		assert(sock->sk != NULL);
		if (0 != sock->ops->release(sock->sk)) {
			LOG_WARN("ksocket_close", "couldn't release socket");
		}
	}
	else {
		LOG_WARN("ksocket_close", "no release method");
	}

	socket_free(sock);

	return 0;
}

int kbind(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	if (sk_is_bound(sock)) {
		return -EINVAL;
	}
	else if (sock->sk->sk_family != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if (!sr_is_saddr_free(sock, (struct sockaddr *)addr)) {
		return -EADDRINUSE;
	}

	assert(sock->ops != NULL);
	if (sock->ops->bind == NULL) {
		return -EOPNOTSUPP;
	}

	assert(sock->sk != NULL);
	ret = sock->ops->bind(sock->sk, addr, addrlen);
	if (ret != 0) {
		return ret;
	}

	sk_set_connection_state(sock, BOUND);
	sr_set_saddr(sock, addr);

	return 0;
}

int kconnect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	if (sock->sk->sk_family != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if ((sock->type == SOCK_STREAM)
			&& sk_is_connected(sock)) {
		return -EISCONN;
	}
	else if (sk_is_listening(sock)) {
		return -EOPNOTSUPP;
	}
	else if (sk_get_connection_state(sock) == CONNECTING) {
		return -EALREADY;
	}

	assert(sock->ops != NULL);
	if (sock->ops->connect == NULL) {
		return -ENOSYS;
	}

#if 0
	if (!sk_is_bound(sock)) {
		/* TODO */
		/* kernel_socket_bind(sock, &localaddress, sizeof(struct sockaddr)); */
		return -EINVAL;
	}
#endif

	sk_set_connection_state(sock, CONNECTING);

	assert(sock->sk != NULL);
	ret = sock->ops->connect(sock->sk, (struct sockaddr *)addr,
			addrlen, flags);
	if (ret != 0) {
		LOG_ERROR("ksocket_connect", "unable to connect on socket");
		sk_set_connection_state(sock, BOUND);
		return ret;
	}

	sk_set_connection_state(sock, CONNECTED);

	return 0;
}

int klisten(struct socket *sock, int backlog) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}

	assert(sr_socket_exists(sock));

	if (!sk_is_bound(sock)) {
		return -EDESTADDRREQ;
	}
	else if (sk_is_connected(sock)){
		return -EINVAL;
	}

	assert(sock->ops != NULL);
	if (sock->ops->listen == NULL) {
		return -ENOSYS;
	}

	assert(sock->sk != NULL);
	ret = sock->ops->listen(sock->sk, backlog >= 0 ? backlog : 0);
	if (ret != 0) {
		LOG_ERROR("ksocket_listen",
				"error setting socket in listening state");
		sk_set_connection_state(sock, BOUND);
		return ret;
	}

	sk_set_connection_state(sock, LISTENING);

	return 0;
}

int kaccept(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct socket **out_sock) {
	int ret;
	struct sock *new_sk;
	struct socket *new_sock;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0) || (out_sock == NULL)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	if (!sk_is_listening(sock)) {
		LOG_ERROR("ksocket_accept",
				"accepting socket should be in listening state");
		return -EINVAL;
	}

	assert(sock->ops != NULL);
	if (sock->ops->accept == NULL) {
		return -EOPNOTSUPP;
	}

	assert(sock->sk != NULL);
	ret = sock->ops->accept(sock->sk, addr, addrlen,
			flags, &new_sk);
	if (ret != 0) {
		LOG_ERROR("ksocket_accept",
				"error while accepting a connection");
		return ret;
	}

	ret = ksocket_ext(sock->sk->sk_family, sock->sk->sk_type,
			sock->sk->sk_protocol, new_sk,
			(struct family_ops *)sock->ops, &new_sock);
	if (ret != 0) {
		sk_common_release(new_sk);
		return ret;
	}

	sk_set_connection_state(new_sock, ESTABLISHED);

	*out_sock = new_sock;

	return 0;
}

int ksendmsg(struct socket *sock, struct msghdr *msg, int flags) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if (msg == NULL) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	switch (sock->type) {
	default:
		if (msg->msg_name == NULL) {
			if (msg->msg_namelen != 0) {
				return -EINVAL;
			}
			else if (!sk_is_connected(sock)) {
				return -EDESTADDRREQ;
			}
		}
		else if (msg->msg_namelen <= 0) {
			return -EINVAL;
		}
		break;
	case SOCK_STREAM:
		if (!sk_is_connected(sock)) {
			return -ENOTCONN;
		}
		else if ((msg->msg_name != NULL)
				|| (msg->msg_namelen != 0)) {
			return -EISCONN;
		}
		break;
	}

	assert(sock->sk != NULL);
	if (sock->sk->sk_shutdown & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	if (msg->msg_flags != 0) { /* TODO remove this */
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}

	/* socket is ready for usage and has no data transmitting errors yet */
	sock->sk->sk_err = -1; /* XXX ?? */

	assert(sock->ops != NULL);
	if (sock->ops->sendmsg == NULL) {
		return -ENOSYS;
	}

	return sock->ops->sendmsg(sock->sk, msg, flags);
}

int krecvmsg(struct socket *sock, struct msghdr *msg, int flags) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	else if (msg == NULL) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	if ((sock->type == SOCK_STREAM)
			&& !sk_is_connected(sock)) {
		return -ENOTCONN;
	}

	assert(sock->sk != NULL);
	if (sock->sk->sk_shutdown & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	if (msg->msg_flags != 0) { /* TODO remove this */
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}

	assert(sock->ops != NULL);
	if (sock->ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	ret = sock->ops->recvmsg(sock->sk, msg, flags);
	if ((ret == -EAGAIN) && !(flags & O_NONBLOCK)) {
		EVENT_WAIT(&sock->sk->sock_is_not_empty, 0,
				SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
		ret = sock->ops->recvmsg(sock->sk, msg, flags);
	}

	return ret;
}

int kshutdown(struct socket *sock, int how) {
	if (sock == NULL) {
		return -EBADF;
	}
	switch (how) {
	default:
		return -EINVAL;
	case SHUT_RD:
	case SHUT_WR:
	case SHUT_RDWR:
		break;
	}

	assert(sr_socket_exists(sock));

	if (!sk_is_connected(sock)){
		return -ENOTCONN;
	}

	assert(sock->sk != NULL);
	sock->sk->sk_shutdown |= (how + 1);

	assert(sock->ops != NULL);
	if (sock->ops->shutdown == NULL) {
		return 0;
	}

	return sock->ops->shutdown(sock->sk, how);
}

int kgetsockname(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	assert(sock->ops != NULL);
	if (sock->ops->getsockname == NULL) {
		return -ENOSYS;
	}

	assert(sock->sk != NULL);
	return sock->ops->getsockname(sock->sk, addr, addrlen);
}

int kgetpeername(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	assert(sock->ops != NULL);
	if (sock->ops->getpeername == NULL) {
		return -ENOSYS;
	}

	assert(sock->sk != NULL);
	return sock->ops->getpeername(sock->sk, addr, addrlen);
}

int kgetsockopt(struct socket *sock, int level, int optname,
		void *optval, socklen_t *optlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	if (level != SOL_SOCKET) {
		assert(sock->ops != NULL);
		if (sock->ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		assert(sock->sk != NULL);
		return sock->ops->getsockopt(sock->sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	case SO_BINDTODEVICE:
		assert(sock->socket_node != NULL);
		memcpy(optval, &sock->sk->sk_so.so_bindtodev,
				min(*optlen, sizeof(void *)));
		*optlen = *optlen > sizeof(void *) ? sizeof(void *) : *optlen;
		return 0;
	case SO_ACCEPTCONN:
		*((unsigned int*)optval) = sock->sk->sk_so.so_acceptconn;
		break;
	case SO_ERROR:
		*((unsigned int*)optval) = sock->sk->sk_so.so_error;
		sock->sk->sk_so.so_error = 0;  /* clear pending error. posix */
		sk_clear_pending_error(sock->sk);
		break;
	case SO_TYPE:
		*((unsigned int*)optval) = sock->sk->sk_so.so_type;
		break;
	case SO_BROADCAST:
		*((unsigned int*)optval) = sock->sk->sk_so.so_broadcast;
		break;
	case SO_DEBUG:
		*((unsigned int*)optval) = sock->sk->sk_so.so_debug;
		break;
	case SO_DONTROUTE:
		*((unsigned int*)optval) = sock->sk->sk_so.so_dontroute;
		break;
	case SO_KEEPALIVE:
		*((unsigned int*)optval) = sock->sk->sk_so.so_keepalive;
		break;
	case SO_OOBINLINE:
		*((unsigned int*)optval) = sock->sk->sk_so.so_oobinline;
		break;
	case SO_RCVBUF:
		*((unsigned int*)optval) = sock->sk->sk_so.so_rcvbuf;
		break;
	case SO_RCVLOWAT:
		*((unsigned int*)optval) = sock->sk->sk_so.so_rcvlowat;
		break;
	case SO_REUSEADDR:
		*((unsigned int*)optval) = sock->sk->sk_so.so_reuseaddr;
		break;
	case SO_SNDBUF:
		*((unsigned int*)optval) = sock->sk->sk_so.so_sndbuf;
		break;
	case SO_SNDLOWAT:
		*((unsigned int*)optval) = sock->sk->sk_so.so_sndlowat;
		break;
		/* non-integer valued optnames */
	case SO_LINGER:
		if(*optlen != sizeof(struct linger))
			return -EINVAL;
		((struct linger*)optval)->l_onoff = sock->sk->sk_so.so_linger.l_onoff;
		((struct linger*)optval)->l_linger = sock->sk->sk_so.so_linger.l_linger;
		*optlen = sizeof(struct linger);
		return ENOERR;
	case SO_RCVTIMEO:
		((struct timeval*)optval)->tv_sec = sock->sk->sk_so.so_rcvtimeo.tv_sec;
		((struct timeval*)optval)->tv_usec = sock->sk->sk_so.so_rcvtimeo.tv_usec;
		goto optname_check_timeval;
	case SO_SNDTIMEO:
		((struct timeval*)optval)->tv_sec = sock->sk->sk_so.so_sndtimeo.tv_sec;
		((struct timeval*)optval)->tv_usec = sock->sk->sk_so.so_sndtimeo.tv_usec;
		goto optname_check_timeval;
//	case SO_BINDTODEVICE:
		optval = sock->sk->sk_so.so_bindtodev;
		break;
	default:
		return -ENOPROTOOPT;
	}

	if(*optlen != sizeof(unsigned int))
		return -EINVAL;
	*optlen = sizeof(unsigned int);

	return 0;

optname_check_timeval:
	if (*optlen < sizeof(struct timeval))
		return -EINVAL;
	if (*optlen > sizeof(struct timeval))
		return -EDOM;
	*optlen = sizeof(struct timeval);

	return 0;
}

int ksetsockopt(struct socket *sock, int level, int optname,
		const void *optval, socklen_t optlen) {
	struct net_device *dev;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((optval == NULL) || (optlen < 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));

	if (level != SOL_SOCKET) {
		assert(sock->ops != NULL);
		if (sock->ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		assert(sock->sk != NULL);
		return sock->ops->setsockopt(sock->sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	case SO_BINDTODEVICE:
		dev = netdev_get_by_name(optval);
		if (dev == NULL) {
			return -ENODEV;
		}
		assert(sock->socket_node != NULL);
		sock->sk->sk_so.so_bindtodev = dev;
		return 0;
	case SO_ACCEPTCONN:
	case SO_ERROR:
	case SO_TYPE:
		/* cannot set these optnames, only read */
		return -EINVAL;
		break;
		/* integer valued optnames */
	case SO_BROADCAST:
		/* in a specific protocol realization when the address is checked
			 to be broadcast it is also a good idea to check wheather it can
		   broadcast at all (like socket type is SOCK_DGRAM)
		   This check isn't done here, because posix dosn't lay any
		   restrictions on setting this optname for sockets with types
		   other than SOCK_DGRAM */
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_broadcast = *((unsigned int*)optval);
		break;
	case SO_DEBUG:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_debug = *((unsigned int*)optval);
		break;
	case SO_DONTROUTE:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_dontroute = *((unsigned int*)optval);
		break;
	case SO_KEEPALIVE:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_keepalive = *((unsigned int*)optval);
		break;
	case SO_OOBINLINE:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_oobinline = *((unsigned int*)optval);
		break;
	case SO_RCVBUF:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_rcvbuf = *((unsigned int*)optval);
		break;
	case SO_RCVLOWAT:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_rcvlowat = *((unsigned int*)optval);
		break;
	case SO_REUSEADDR:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_reuseaddr = *((unsigned int*)optval);
		break;
	case SO_SNDBUF:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_sndbuf = *((unsigned int*)optval);
		break;
	case SO_SNDLOWAT:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->sk_so.so_sndlowat = *((unsigned int*)optval);
		break;
		/* non-integer valued optnames */
	case SO_LINGER:
		if(optlen != sizeof(struct linger))
			return -EINVAL;
		memcpy(&sock->sk->sk_so.so_linger, optval, optlen);
		break;
	case SO_RCVTIMEO:
		if(optlen < sizeof(struct timeval))
			return -EINVAL;
		if(optlen > sizeof(struct timeval))
			return -EDOM;
		memcpy(&sock->sk->sk_so.so_rcvtimeo, optval, optlen);
		break;
	case SO_SNDTIMEO:
		if(optlen < sizeof(struct timeval))
			return -EINVAL;
		if(optlen > sizeof(struct timeval))
			return -EDOM;
		memcpy(&sock->sk->sk_so.so_sndtimeo, optval, optlen);
		break;
	default:
		return -ENOPROTOOPT;
		break;
	}

	return 0;
}
