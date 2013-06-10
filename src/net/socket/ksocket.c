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
		struct sock *sk, struct socket **out_sock) {
	int ret;
	struct socket *new_sock;

	assert(out_sock != NULL);

	new_sock = socket_alloc();
	if (new_sock == NULL) {
		return -ENOMEM;
	}

	if (sk == NULL) {
		ret = sock_create(family, type, protocol, &sk);
		if (ret != 0) {
			socket_free(new_sock);
			return ret;
		}
	}

	new_sock->sk = sk;
	new_sock->desc_data = NULL;
	new_sock->socket_node = NULL;

	ret = sr_add_socket_to_registry(new_sock);
	if (ret != 0) {
		sock_close(sk);
		return ret;
	}
	sk_set_connection_state(new_sock, UNCONNECTED);

	*out_sock = new_sock;

	return 0;
}

int ksocket(int family, int type, int protocol,
		struct socket **out_sock) {
	if (out_sock == NULL) {
		return -EINVAL;
	}

	return ksocket_ext(family, type, protocol, NULL, out_sock);
}

int ksocket_close(struct socket *sock) {
	if (sock == NULL) {
		return -EBADF;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	sk_set_connection_state(sock, DISCONNECTING);

	sr_remove_saddr(sock);

	if (0 != sr_remove_socket_from_registry(sock)) {
		LOG_WARN("ksocket_close",
				"couldn't remove entry from registry");
	}

	if (0 != sock_close(sock->sk)) {
		LOG_WARN("ksocket_close", "can't close socket");
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
	assert(sock->sk != NULL);

	if (sk_is_bound(sock)) {
		return -EINVAL;
	}
	else if (sock->sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if (!sr_is_saddr_free(sock, (struct sockaddr *)addr)) {
		return -EADDRINUSE;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->bind == NULL) {
		return -EOPNOTSUPP;
	}

	ret = sock->sk->f_ops->bind(sock->sk, addr, addrlen);
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
	assert(sock->sk != NULL);

	if (sock->sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if ((sock->sk->opt.so_type == SOCK_STREAM)
			&& sk_is_connected(sock)) {
		return -EISCONN;
	}
	else if (sk_is_listening(sock)) {
		return -EOPNOTSUPP;
	}
	else if (sk_get_connection_state(sock) == CONNECTING) {
		return -EALREADY;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->connect == NULL) {
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

	ret = sock->sk->f_ops->connect(sock->sk, (struct sockaddr *)addr,
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
	assert(sock->sk != NULL);

	if (!sk_is_bound(sock)) {
		return -EDESTADDRREQ;
	}
	else if (sk_is_connected(sock)){
		return -EINVAL;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->listen == NULL) {
		return -ENOSYS;
	}

	assert(sock->sk != NULL);
	ret = sock->sk->f_ops->listen(sock->sk, backlog >= 0 ? backlog : 0);
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
	assert(sock->sk != NULL);

	if (!sk_is_listening(sock)) {
		LOG_ERROR("ksocket_accept",
				"accepting socket should be in listening state");
		return -EINVAL;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->accept == NULL) {
		return -EOPNOTSUPP;
	}

	ret = sock->sk->f_ops->accept(sock->sk, addr, addrlen,
			flags, &new_sk);
	if (ret != 0) {
		LOG_ERROR("ksocket_accept",
				"error while accepting a connection");
		return ret;
	}

	ret = ksocket_ext(sock->sk->opt.so_domain,
			sock->sk->opt.so_type, sock->sk->opt.so_protocol,
			new_sk, &new_sock);
	if (ret != 0) {
		sock_release(new_sk);
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
	assert(sock->sk != NULL);

	switch (sock->sk->opt.so_type) {
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

	if (sock->sk->shutdown_flag & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	if (msg->msg_flags != 0) { /* TODO remove this */
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->sendmsg == NULL) {
		return -ENOSYS;
	}

	return sock->sk->f_ops->sendmsg(sock->sk, msg, flags);
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
	assert(sock->sk != NULL);

	if ((sock->sk->opt.so_type == SOCK_STREAM)
			&& !sk_is_connected(sock)) {
		return -ENOTCONN;
	}

	if (sock->sk->shutdown_flag & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	if (msg->msg_flags != 0) { /* TODO remove this */
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	ret = sock->sk->f_ops->recvmsg(sock->sk, msg, flags);
	if ((ret == -EAGAIN) && !(flags & O_NONBLOCK)) {
		EVENT_WAIT(&sock->sk->sock_is_not_empty, 0,
				SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
		ret = sock->sk->f_ops->recvmsg(sock->sk, msg, flags);
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
	assert(sock->sk != NULL);

	if (!sk_is_connected(sock)){
		return -ENOTCONN;
	}

	sock->sk->shutdown_flag |= (how + 1);

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->shutdown == NULL) {
		return 0;
	}

	return sock->sk->f_ops->shutdown(sock->sk, how);
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
	assert(sock->sk != NULL);

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->getsockname == NULL) {
		return -ENOSYS;
	}

	return sock->sk->f_ops->getsockname(sock->sk, addr, addrlen);
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
	assert(sock->sk != NULL);

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->getpeername == NULL) {
		return -ENOSYS;
	}

	return sock->sk->f_ops->getpeername(sock->sk, addr, addrlen);
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
	assert(sock->sk != NULL);

	if (level != SOL_SOCKET) {
		assert(sock->sk->f_ops != NULL);
		if (sock->sk->f_ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sock->sk->f_ops->getsockopt(sock->sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	case SO_BINDTODEVICE:
		assert(sock->socket_node != NULL);
		memcpy(optval, &sock->sk->opt.so_bindtodev,
				min(*optlen, sizeof(void *)));
		*optlen = *optlen > sizeof(void *) ? sizeof(void *) : *optlen;
		return 0;
	case SO_ACCEPTCONN:
		*((unsigned int*)optval) = sock->sk->opt.so_acceptconn;
		break;
	case SO_ERROR:
		*((unsigned int*)optval) = sock->sk->opt.so_error;
		sock->sk->opt.so_error = 0;  /* clear pending error. posix */
		break;
	case SO_TYPE:
		*((unsigned int*)optval) = sock->sk->opt.so_type;
		break;
	case SO_BROADCAST:
		*((unsigned int*)optval) = sock->sk->opt.so_broadcast;
		break;
	case SO_DONTROUTE:
		*((unsigned int*)optval) = sock->sk->opt.so_dontroute;
		break;
	case SO_OOBINLINE:
		*((unsigned int*)optval) = sock->sk->opt.so_oobinline;
		break;
	case SO_RCVBUF:
		*((unsigned int*)optval) = sock->sk->opt.so_rcvbuf;
		break;
	case SO_RCVLOWAT:
		*((unsigned int*)optval) = sock->sk->opt.so_rcvlowat;
		break;
	case SO_SNDBUF:
		*((unsigned int*)optval) = sock->sk->opt.so_sndbuf;
		break;
	case SO_SNDLOWAT:
		*((unsigned int*)optval) = sock->sk->opt.so_sndlowat;
		break;
		/* non-integer valued optnames */
	case SO_LINGER:
		if(*optlen != sizeof(struct linger))
			return -EINVAL;
		((struct linger*)optval)->l_onoff = sock->sk->opt.so_linger.l_onoff;
		((struct linger*)optval)->l_linger = sock->sk->opt.so_linger.l_linger;
		*optlen = sizeof(struct linger);
		return ENOERR;
	case SO_RCVTIMEO:
		((struct timeval*)optval)->tv_sec = sock->sk->opt.so_rcvtimeo.tv_sec;
		((struct timeval*)optval)->tv_usec = sock->sk->opt.so_rcvtimeo.tv_usec;
		goto optname_check_timeval;
	case SO_SNDTIMEO:
		((struct timeval*)optval)->tv_sec = sock->sk->opt.so_sndtimeo.tv_sec;
		((struct timeval*)optval)->tv_usec = sock->sk->opt.so_sndtimeo.tv_usec;
		goto optname_check_timeval;
//	case SO_BINDTODEVICE:
		optval = sock->sk->opt.so_bindtodev;
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
	assert(sock->sk != NULL);

	if (level != SOL_SOCKET) {
		assert(sock->sk->f_ops != NULL);
		if (sock->sk->f_ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sock->sk->f_ops->setsockopt(sock->sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	case SO_BINDTODEVICE:
		dev = netdev_get_by_name(optval);
		if (dev == NULL) {
			return -ENODEV;
		}
		assert(sock->socket_node != NULL);
		sock->sk->opt.so_bindtodev = dev;
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
		sock->sk->opt.so_broadcast = *((unsigned int*)optval);
		break;
	case SO_DONTROUTE:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->opt.so_dontroute = *((unsigned int*)optval);
		break;
	case SO_OOBINLINE:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->opt.so_oobinline = *((unsigned int*)optval);
		break;
	case SO_RCVBUF:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->opt.so_rcvbuf = *((unsigned int*)optval);
		break;
	case SO_RCVLOWAT:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->opt.so_rcvlowat = *((unsigned int*)optval);
		break;
	case SO_SNDBUF:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->opt.so_sndbuf = *((unsigned int*)optval);
		break;
	case SO_SNDLOWAT:
		if(optlen != sizeof(unsigned int))
			return -EINVAL;
		sock->sk->opt.so_sndlowat = *((unsigned int*)optval);
		break;
		/* non-integer valued optnames */
	case SO_LINGER:
		if(optlen != sizeof(struct linger))
			return -EINVAL;
		memcpy(&sock->sk->opt.so_linger, optval, optlen);
		break;
	case SO_RCVTIMEO:
		if(optlen < sizeof(struct timeval))
			return -EINVAL;
		if(optlen > sizeof(struct timeval))
			return -EDOM;
		memcpy(&sock->sk->opt.so_rcvtimeo, optval, optlen);
		break;
	case SO_SNDTIMEO:
		if(optlen < sizeof(struct timeval))
			return -EINVAL;
		if(optlen > sizeof(struct timeval))
			return -EDOM;
		memcpy(&sock->sk->opt.so_sndtimeo, optval, optlen);
		break;
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}
