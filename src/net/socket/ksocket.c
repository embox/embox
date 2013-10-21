/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <net/netdevice.h>

#include <util/math.h>

#include <net/sock.h>
#include <util/sys_log.h>
#include <net/socket/ksocket.h>

#include <framework/mod/options.h>
#include <kernel/time/time.h>

#define MODOPS_CONNECT_TIMEOUT OPTION_GET(NUMBER, connect_timeout)

int ksocket(int family, int type, int protocol,
		struct sock **out_sk) {
	int ret;
	struct sock *new_sk;

	if (out_sk == NULL) {
		return -EINVAL;
	}

	ret = sock_create(family, type, protocol, &new_sk);
	if (ret != 0) {
		return ret;
	}

	sock_set_state(new_sk, SS_UNCONNECTED);

	if (type != SOCK_STREAM) {
		io_sync_enable(&new_sk->ios, IO_SYNC_WRITING);
	}

	*out_sk = new_sk;

	return 0;
}

int ksocket_close(struct sock *sk) {
	if (sk == NULL) {
		return -EBADF;
	}

	sock_set_state(sk, SS_DISCONNECTING);

	if (0 != sock_close(sk)) {
		LOG_WARN("ksocket_close", "can't close socket");
	}

	/* sock_set_state(sk, SS_CLOSED); */

	return 0;
}

int kbind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;

	if (sk == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	if (sock_state_bound(sk)) {
		return -EINVAL;
	}
	else if (sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
#if 0
	else if (!sr_is_saddr_free(sk, (struct sockaddr *)addr)) {
		return -EADDRINUSE;
	}
#endif

	assert(sk->f_ops != NULL);
	if (sk->f_ops->bind == NULL) {
		return -ENOSYS;
	}

	ret = sk->f_ops->bind(sk, addr, addrlen);
	if (ret != 0) {
		return ret;
	}

	sock_set_state(sk, SS_BOUND);

	return 0;
}

int kconnect(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	int ret;

	if (sk == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	if (sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if ((sk->opt.so_type == SOCK_STREAM)
			&& sock_state_connected(sk)) {
		return -EISCONN;
	}
	else if (sock_state_listening(sk)) {
		return -EISCONN;
	}
	else if (sock_state_connecting(sk)) {
		return -EALREADY;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->connect == NULL) {
		return -ENOSYS;
	}

	if (!sock_state_bound(sk)) {
		if (sk->f_ops->bind_local == NULL) {
			return -EINVAL;
		}
		ret = sk->f_ops->bind_local(sk);
		if (ret != 0) {
			return ret;
		}
		/* sock_set_state(sk, SS_BOUND); */
	}

	sock_set_state(sk, SS_CONNECTING);

	ret = sk->f_ops->connect(sk, (struct sockaddr *)addr,
			addrlen, flags);
	if ((ret == -EINPROGRESS) && !(flags & O_NONBLOCK)) {
		/* lock until a connection is established */
		ret = io_sync_wait(&sk->ios, IO_SYNC_WRITING,
				MODOPS_CONNECT_TIMEOUT);
		if (ret == -ETIMEDOUT) {
			/* shutdown connection */
			if (sk->f_ops->shutdown != NULL) {
				(void)sk->f_ops->shutdown(sk, SHUT_RDWR);
			}
		}
		else if ((ret == 0) && !io_sync_ready(&sk->ios,
					IO_SYNC_WRITING)) {
			/* if writing not ready then connection is reset */
			ret = -ECONNRESET;
		}
	}
	if (ret != 0) {
		LOG_ERROR("ksocket_connect",
				"unable to connect on socket");
		sock_set_state(sk, SS_BOUND);
		if (ret == -EINPROGRESS) { /* FIXME */
			sock_set_state(sk, SS_CONNECTED);
		}
		return ret;
	}

	sock_set_state(sk, SS_CONNECTED);

	return 0;
}

int klisten(struct sock *sk, int backlog) {
	int ret;

	if (sk == NULL) {
		return -EBADF;
	}
	backlog = backlog > 0 ? backlog : 1;

	if (sk->opt.so_type != SOCK_STREAM) {
		return -EOPNOTSUPP;
	}
	else if (sock_state_connecting(sk)
			|| sock_state_connected(sk)) {
		return -EINVAL;
	}
	else if (!sock_state_bound(sk)) {
		if (sk->f_ops->bind_local == NULL) {
			return -EINVAL;
		}
		ret = sk->f_ops->bind_local(sk);
		if (ret != 0) {
			return ret;
		}
		sock_set_state(sk, SS_BOUND);
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->listen == NULL) {
		return -ENOSYS;
	}

	ret = sk->f_ops->listen(sk, backlog);
	if (ret != 0) {
		LOG_ERROR("ksocket_listen",
				"error setting socket in listening state");
		return ret;
	}

	sock_set_state(sk, SS_LISTENING);
	sk->opt.so_acceptconn = 1;
	sk->shutdown_flag |= SHUT_WR + 1;

	return 0;
}

int kaccept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct sock **out_sk) {
	int ret;
	struct sock *new_sk;

	if (sk == NULL) {
		return -EBADF;
	}
	else if (((addr == NULL) && (addrlen != NULL))
			|| ((addr != NULL) && (addrlen == NULL))
			|| ((addrlen != NULL) && (*addrlen <= 0))
			|| (out_sk == NULL)) {
		return -EINVAL;
	}

	if (sk->opt.so_type != SOCK_STREAM) {
		return -EOPNOTSUPP;
	}
	else if (!sock_state_listening(sk)) {
		LOG_ERROR("ksocket_accept",
				"accepting socket should be in listening state");
		return -EINVAL;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->accept == NULL) {
		return -ENOSYS;
	}

	if (!(flags & O_NONBLOCK)) {
		ret = io_sync_wait(&sk->ios, IO_SYNC_READING,
				IO_SYNC_TIMEOUT_INFINITE);
		if (ret != 0) {
			return ret;
		}
	}

	ret = sk->f_ops->accept(sk, addr, addrlen,
			flags, &new_sk);
	if (ret != 0) {
		LOG_ERROR("ksocket_accept",
				"error while accepting a connection");
		return ret;
	}

	sock_set_state(new_sk, SS_ESTABLISHED);

	io_sync_enable(&new_sk->ios, IO_SYNC_WRITING);

	*out_sk = new_sk;

	return 0;
}

int ksendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	int ret;
	unsigned long timeout;

	if (sk == NULL) {
		return -EBADF;
	}
	else if ((msg == NULL) || (msg->msg_iov == NULL)
			|| (msg->msg_iovlen == 0)) {
		return -EINVAL;
	}

	if (sk->shutdown_flag & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	assert(sk->f_ops != NULL);
	switch (sk->opt.so_type) {
	default:
		if (!sock_state_bound(sk)) {
			if (sk->f_ops->bind_local == NULL) {
				return -EINVAL;
			}
			ret = sk->f_ops->bind_local(sk);
			if (ret != 0) {
				return ret;
			}
			sock_set_state(sk, SS_BOUND);
		}
		if (msg->msg_name == NULL) {
			if (msg->msg_namelen != 0) {
				return -EINVAL;
			}
			else if (!sock_state_connected(sk)) {
				return -EDESTADDRREQ;
			}
		}
		else if (msg->msg_namelen <= 0) {
			return -EINVAL;
		}
		break;
	case SOCK_STREAM:
		if (!sock_state_connected(sk)) {
			return -ENOTCONN;
		}
		else if ((msg->msg_name != NULL)
				|| (msg->msg_namelen != 0)) {
			return -EISCONN;
		}
		break;
	}

	/* TODO remove this */
	if (msg->msg_flags != 0) {
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}
	else if (msg->msg_iovlen != 1) {
		LOG_ERROR("ksendmsg", "only one msg_iov allowed");
		return -EINVAL;
	}

	if (sk->f_ops->sendmsg == NULL) {
		return -ENOSYS;
	}

	timeout = timeval_to_ms(&sk->opt.so_sndtimeo);
	if (timeout == 0) {
		timeout = IO_SYNC_TIMEOUT_INFINITE;
	}

	if (!(flags & O_NONBLOCK)) {
		ret = io_sync_wait(&sk->ios, IO_SYNC_WRITING, timeout);
		if (ret != 0) {
			return ret;
		}
	}

	return sk->f_ops->sendmsg(sk, msg, flags);
}

int krecvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	int ret;
	unsigned long timeout;

	if (sk == NULL) {
		return -EBADF;
	}
	else if ((msg == NULL) || (msg->msg_iov == NULL)
			|| (msg->msg_iovlen == 0)) {
		return -EINVAL;
	}

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		return -EPIPE;
	}
	else if ((sk->opt.so_type == SOCK_STREAM)
			&& !sock_state_connected(sk)) {
		return -ENOTCONN;
	}

	/* TODO remove this */
	if (msg->msg_flags != 0) {
		LOG_ERROR("krecvmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}
	else if (msg->msg_iovlen != 1) {
		LOG_ERROR("krecvmsg", "only one msg_iov allowed");
		return -EINVAL;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	timeout = timeval_to_ms(&sk->opt.so_rcvtimeo);
	if (timeout == 0) {
		timeout = IO_SYNC_TIMEOUT_INFINITE;
	}

	if (!(flags & O_NONBLOCK)) {
		ret = io_sync_wait(&sk->ios, IO_SYNC_READING, timeout);
		if (ret != 0) {
			return ret;
		}
	}

	return sk->f_ops->recvmsg(sk, msg, flags);
}

int kshutdown(struct sock *sk, int how) {
	if (sk == NULL) {
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

	sk->shutdown_flag |= (how + 1);

	if (!sock_state_connected(sk)
			&& !sock_state_listening(sk)) {
		return -ENOTCONN;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->shutdown == NULL) {
		return 0;
	}

	return sk->f_ops->shutdown(sk, how);
}

int kgetsockname(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen) {
	if (sk == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->getsockname == NULL) {
		return -ENOSYS;
	}

	return sk->f_ops->getsockname(sk, addr, addrlen);
}

int kgetpeername(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen) {
	if (sk == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return -EINVAL;
	}

	if (!sock_state_connected(sk)) {
		return -ENOTCONN;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->getpeername == NULL) {
		return -ENOSYS;
	}

	return sk->f_ops->getpeername(sk, addr, addrlen);
}

#define CASE_GETSOCKOPT(test_name, field, expression) \
	case test_name:                                   \
		memcpy(optval, &sk->opt.field,                \
				min(*optlen, sizeof sk->opt.field));  \
		expression;                                   \
		*optlen = min(*optlen, sizeof sk->opt.field); \
		break

int kgetsockopt(struct sock *sk, int level, int optname,
		void *optval, socklen_t *optlen) {
	if (sk == NULL) {
		return -EBADF;
	}
	else if ((optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return -EINVAL;
	}

	if (level != SOL_SOCKET) {
		assert(sk->f_ops != NULL);
		if (sk->f_ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->f_ops->getsockopt(sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	CASE_GETSOCKOPT(SO_ACCEPTCONN, so_acceptconn, );
	CASE_GETSOCKOPT(SO_BINDTODEVICE, so_bindtodevice,
			strncpy(optval, &sk->opt.so_bindtodevice->name[0],
				*optlen);
			*optlen = min(*optlen,
				strlen(&sk->opt.so_bindtodevice->name[0]));
			break);
	CASE_GETSOCKOPT(SO_BROADCAST, so_broadcast, );
	CASE_GETSOCKOPT(SO_DOMAIN, so_domain, );
	CASE_GETSOCKOPT(SO_DONTROUTE, so_dontroute, );
	CASE_GETSOCKOPT(SO_ERROR, so_error,
			sk->opt.so_error = 0);
	CASE_GETSOCKOPT(SO_LINGER, so_linger, );
	CASE_GETSOCKOPT(SO_OOBINLINE, so_oobinline, );
	CASE_GETSOCKOPT(SO_PROTOCOL, so_protocol, );
	CASE_GETSOCKOPT(SO_RCVBUF, so_rcvbuf, );
	CASE_GETSOCKOPT(SO_RCVLOWAT, so_rcvlowat, );
	CASE_GETSOCKOPT(SO_RCVTIMEO, so_rcvtimeo,
			if (*optlen > sizeof sk->opt.so_rcvtimeo) {
				return -EDOM;
			});
	CASE_GETSOCKOPT(SO_SNDBUF, so_sndbuf, );
	CASE_GETSOCKOPT(SO_SNDLOWAT, so_sndlowat, );
	CASE_GETSOCKOPT(SO_SNDTIMEO, so_sndtimeo,
			if (*optlen > sizeof sk->opt.so_sndtimeo) {
				return -EDOM;
			});
	CASE_GETSOCKOPT(SO_TYPE, so_type, );
	}

	return 0;
}

#define CASE_SETSOCKOPT(test_name, field, expression) \
	case test_name:                                   \
		expression;                                   \
		if (optlen != sizeof sk->opt.field) {         \
			return -EINVAL;                           \
		}                                             \
		memcpy(&sk->opt.field, optval, optlen);       \
		break

int ksetsockopt(struct sock *sk, int level, int optname,
		const void *optval, socklen_t optlen) {
	if (sk == NULL) {
		return -EBADF;
	}
	else if ((optval == NULL) || (optlen < 0)) {
		return -EINVAL;
	}

	if (level != SOL_SOCKET) {
		assert(sk->f_ops != NULL);
		if (sk->f_ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->f_ops->setsockopt(sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	case SO_ACCEPTCONN:
	case SO_DOMAIN:
	case SO_ERROR:
	case SO_PROTOCOL:
	case SO_TYPE:
		return -EINVAL;
	CASE_SETSOCKOPT(SO_BINDTODEVICE, so_bindtodevice,
			optval = netdev_get_by_name(optval);
			if (optval == NULL) {
				return -ENODEV;
			}
			optlen = sizeof optval);
	CASE_SETSOCKOPT(SO_BROADCAST, so_broadcast, );
	CASE_SETSOCKOPT(SO_DONTROUTE, so_dontroute, );
	CASE_SETSOCKOPT(SO_LINGER, so_linger, );
	CASE_SETSOCKOPT(SO_OOBINLINE, so_oobinline, );
	CASE_SETSOCKOPT(SO_RCVBUF, so_rcvbuf, );
	CASE_SETSOCKOPT(SO_RCVLOWAT, so_rcvlowat, );
	CASE_SETSOCKOPT(SO_RCVTIMEO, so_rcvtimeo,
			if (optlen > sizeof sk->opt.so_rcvtimeo) {
				return -EDOM;
			});
	CASE_SETSOCKOPT(SO_SNDBUF, so_sndbuf, );
	CASE_SETSOCKOPT(SO_SNDLOWAT, so_sndlowat, );
	CASE_SETSOCKOPT(SO_SNDTIMEO, so_sndtimeo,
			if (optlen > sizeof sk->opt.so_sndtimeo) {
				return -EDOM;
			});
	}

	return 0;
}
