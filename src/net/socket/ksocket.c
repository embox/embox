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

#include <util/math.h>
#include <util/log.h>

#include <net/sock.h>
#include <net/socket/ksocket.h>
#include <net/netdevice.h>

#include <framework/mod/options.h>
#include <kernel/time/time.h>
#include <kernel/task/resource/idesc.h>

#include <util/err.h>

#define MODOPS_CONNECT_TIMEOUT OPTION_GET(NUMBER, connect_timeout)

struct sock *ksocket(int family, int type, int protocol) {
	struct sock *new_sk;

	new_sk = sock_create(family, type, protocol);
	if (0 != ptr2err(new_sk)) {
		return new_sk;
	}

	sock_set_state(new_sk, SS_UNCONNECTED);

	return new_sk;
}

void ksocket_close(struct sock *sk) {
	assert(sk);

	sock_set_state(sk, SS_DISCONNECTING);

	if (0 != sock_close(sk)) {
		log_warning("can't close socket");
	}
	/* sock_set_state(sk, SS_CLOSED); */
}

int kbind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;

	assert(sk);
	assert(addr);
	assert(addrlen > 0);

	if (sock_state_bound(sk)) {
		return -EINVAL;
	}

	if (sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}

	assert(sk->f_ops != NULL);
	/* XXX may be assert? --Alexander */
	if (sk->f_ops->bind == NULL) {
		return -EOPNOTSUPP;
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

	assert(sk);
	assert(addr);
	assert(addrlen > 0);

	if (sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}

	if ((sk->opt.so_type == SOCK_STREAM) && sock_state_connected(sk)) {
		return -EISCONN;
	}

	if (sock_state_listening(sk)) {
		return -EISCONN;
	}

	if (sock_state_connecting(sk)) {
		return -EALREADY;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->connect == NULL) {
		return -EOPNOTSUPP;
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

	ret = sk->f_ops->connect(sk, (struct sockaddr *)addr, addrlen, flags);
	if (ret == -ETIMEDOUT) {
		/* shutdown connection */
		if (sk->f_ops->shutdown != NULL) {
			(void)sk->f_ops->shutdown(sk, SHUT_RDWR);
		}
	}

	if (ret != 0) {
		log_error("unable to connect on socket");
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

	assert(sk);

	backlog = backlog > 0 ? backlog : 1;

	if (sk->opt.so_type != SOCK_STREAM) {
		return -EOPNOTSUPP;
	}

	if (sock_state_connecting(sk) || sock_state_connected(sk)) {
		return -EINVAL;
	}

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

	assert(sk->f_ops != NULL);
	if (sk->f_ops->listen == NULL) {
		return -EOPNOTSUPP;
	}

	ret = sk->f_ops->listen(sk, backlog);
	if (ret != 0) {
		log_error("error setting socket in listening state");
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

	assert(sk);
	assert(out_sk);
	assert(addr || !addrlen);
	assert(!addr || addrlen);
	assert(!addrlen || (*addrlen > 0));

	if (sk->opt.so_type != SOCK_STREAM) {
		return -EOPNOTSUPP;
	}

	if (!sock_state_listening(sk)) {
		log_error("accepting socket should be in listening state");
		return -EINVAL;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->accept == NULL) {
		return -EOPNOTSUPP;
	}

	ret = sk->f_ops->accept(sk, addr, addrlen, flags, &new_sk);
	if (ret != 0) {
		log_error("error while accepting a connection");
		return ret;
	}

	sock_set_state(new_sk, SS_ESTABLISHED);

	*out_sk = new_sk;

	return 0;
}

int ksendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	int ret;

	assert(sk);
	assert(msg);
	assert(msg->msg_iov);

	assert(sk->f_ops);
	assert(sk->f_ops->sendmsg);

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
			if (!sock_state_connected(sk)) {
				return -EDESTADDRREQ;
			}
		} else if (msg->msg_namelen <= 0) {
			return -EINVAL;
		}
		break;
	case SOCK_STREAM:
		if (!sock_state_connected(sk)) {
			return -ENOTCONN;
		}
		if ((msg->msg_name != NULL) || (msg->msg_namelen != 0)) {
			return -EISCONN;
		}
		break;
	}

	if (sk->f_ops->sendmsg == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->f_ops->sendmsg(sk, msg, flags);
}

int krecvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	assert(sk);
	assert(msg);
	assert(msg->msg_iov);

//	if (msg->msg_iov->iov_len == 0) {
//		return 0;
//	}

	if ((sk->opt.so_type == SOCK_STREAM) && !sock_state_connected(sk)) {
		return -ENOTCONN;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->recvmsg == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->f_ops->recvmsg(sk, msg, flags);
}

int kshutdown(struct sock *sk, int how) {
	assert(sk);
	assert(how == SHUT_RD || how == SHUT_WR || how == SHUT_RDWR);

	sk->shutdown_flag |= (how + 1);

	if (!sock_state_connected(sk) && !sock_state_listening(sk)) {
		return -ENOTCONN;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->shutdown == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->f_ops->shutdown(sk, how);
}

int kgetsockname(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen) {
	assert(sk);
	assert(addr);
	assert(addrlen);
	assert(*addrlen > 0);
	assert(sk->f_ops != NULL);

	if (sk->f_ops->getsockname == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->f_ops->getsockname(sk, addr, addrlen);
}

int kgetpeername(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen) {
	assert(sk);
	assert(addr);
	assert(addrlen);
	assert(*addrlen > 0);
	assert(sk->f_ops != NULL);

	if (!sock_state_connected(sk)) {
		return -ENOTCONN;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->getpeername == NULL) {
		return -EOPNOTSUPP;
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
	assert(sk);
	assert(optval);
	assert(optlen);
	assert(*optlen >= 0);

	if (level != SOL_SOCKET) {
		assert(sk->f_ops != NULL);
		if (sk->f_ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}

		return sk->f_ops->getsockopt(sk, level, optname, optval, optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;

	CASE_GETSOCKOPT(SO_ACCEPTCONN, so_acceptconn, );
	CASE_GETSOCKOPT(SO_BINDTODEVICE, so_bindtodevice,
			strncpy(optval, &sk->opt.so_bindtodevice->name[0], *optlen);
			*optlen = min(*optlen, strlen(&sk->opt.so_bindtodevice->name[0]));
			break);
	CASE_GETSOCKOPT(SO_BROADCAST, so_broadcast, );
	CASE_GETSOCKOPT(SO_DOMAIN, so_domain, );
	CASE_GETSOCKOPT(SO_DONTROUTE, so_dontroute, );
	CASE_GETSOCKOPT(SO_ERROR, so_error, sk->opt.so_error = 0);
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
		return 0

static inline int 
ksetsockopt_sol_socket(struct sock *sk, int level,
						int optname, const void *optval, socklen_t optlen) {
	switch (optname) {
	default:
		return -ENOPROTOOPT;

	case SO_ACCEPTCONN:
	case SO_DOMAIN:
	case SO_ERROR:
	case SO_PROTOCOL:
	case SO_TYPE:
		return -EINVAL;
	case SO_BINDTODEVICE: {
		struct net_device *dev = netdev_get_by_name(optval);
		if (dev == NULL) {
			return -ENODEV;
		}
		sk->opt.so_bindtodevice = dev;
		return 0;
	}
	CASE_SETSOCKOPT(SO_REUSEADDR, so_reuseaddr, );
	CASE_SETSOCKOPT(SO_BROADCAST, so_broadcast, );
	CASE_SETSOCKOPT(SO_DONTROUTE, so_dontroute, );
	CASE_SETSOCKOPT(SO_LINGER, so_linger, );
	CASE_SETSOCKOPT(SO_OOBINLINE, so_oobinline, );
	CASE_SETSOCKOPT(SO_RCVBUF, so_rcvbuf, );
	CASE_SETSOCKOPT(SO_RCVLOWAT, so_rcvlowat, );
	CASE_SETSOCKOPT(SO_RCVTIMEO, so_rcvtimeo,
		    if (optlen > sizeof sk->opt.so_rcvtimeo) { return -EDOM; }
		);
	CASE_SETSOCKOPT(SO_SNDBUF, so_sndbuf, );
	CASE_SETSOCKOPT(SO_SNDLOWAT, so_sndlowat, );
	CASE_SETSOCKOPT(SO_SNDTIMEO, so_sndtimeo,
		    if (optlen > sizeof sk->opt.so_sndtimeo) { return -EDOM; }
		);
	}
	return -EINVAL;
}

int ksetsockopt(struct sock *sk, int level, int optname,
		const void *optval, socklen_t optlen) {
	int ret;

	assert(sk);
	assert(optval);
	assert(optlen >= 0);

	if (level == SOL_SOCKET) {
		return ksetsockopt_sol_socket(sk, level, optname, optval, optlen);
	} else {
		ret = -EOPNOTSUPP;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->setsockopt == NULL) {
		return ret;
	}
	return sk->f_ops->setsockopt(sk, level, optname,
			optval, optlen);
}
