/**
 * @file
 * @brief PF_INET protocol family socket handler.
 *
 * @date 01.12.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <embox/net/pack.h>
#include <framework/net/sock/api.h>
#include <stddef.h>
#include <linux/aio.h>
#include <net/protocol.h>
#include <net/in.h>
#include <net/sock.h>
#include <net/socket.h>
#include <net/inet_sock.h>
#include <net/port.h>
#include <net/tcp.h>

EMBOX_NET_PACK(ETH_P_IP, ip_rcv, inet_init);

static struct inet_protosw *inet_proto_find(int type, int protocol) {
	struct inet_protosw *p_netsock = NULL;
	const struct net_sock *net_sock_ptr;

	net_sock_foreach(net_sock_ptr) {
		p_netsock = net_sock_ptr->netsock;
		if (p_netsock->type != type) {
			continue;
		}

		return p_netsock;
//		if (IPPROTO_IP == protocol || p_netsock->protocol == protocol) {
//			return p_netsock;
//		}
	}
	return NULL;
}

/* AF_INET socket create */
static int inet_create(struct socket *sock, int protocol) {
	int err;
	struct sock *sk;
	struct inet_sock *inet;
	struct inet_protosw *p_netsock = NULL;

	if (NULL == (p_netsock = inet_proto_find(sock->type, protocol))) {
		return -EPROTONOSUPPORT;
	}

	sk = sk_alloc(PF_INET, 0, (struct proto *) p_netsock->prot);
	if (sk == NULL) {
		return -ENOMEM;
	}

	sock->sk = sk;
	sock->ops = p_netsock->ops;
	sk->sk_type = sock->type;
	sk->sk_protocol = p_netsock->protocol;

	inet = inet_sk(sk);
	inet->id = 0;
	inet->uc_ttl = -1; /* TODO socket setup more options  */
	inet->mc_ttl = 1;

	if (sk->sk_prot->init != NULL) {
		if(0 != (err = sk->sk_prot->init(sk))) {
			sk_common_release(sk);
			return err;
		}
	}

	return ENOERR;
}

int inet_release(struct socket *sock) {
	struct sock *sk;
	struct inet_sock *inet;

	sock_lock(sock->sk);
	sk = sock->sk;
	inet = inet_sk(sk);
	socket_port_unlock(inet->sport, inet->sport_type);

	if (sk == NULL) {
		return -EINVAL;
	}

	sock_unlock(sock->sk);
	sk->sk_prot->close(sk, 0);
	sock->sk = NULL;

	return ENOERR;
}

int inet_bind(struct socket *sock, struct sockaddr *addr, int addr_len) {
	int res;
	struct sock *sk;
	struct sockaddr_in *addr_in;
	struct inet_sock *inet;

	sock_lock(sock->sk);
	sk = sock->sk;
	if (sk->sk_prot->bind != NULL) {
		res = sk->sk_prot->bind(sk, addr, addr_len);
		if (res < 0) {
			return res;
		}
	}

	inet = inet_sk(sk);
	addr_in = (struct sockaddr_in *)addr;

	socket_set_port_type(sock);
	if (socket_port_is_busy(addr_in->sin_port, inet->sport_type)) {
		return -EBUSY;
	}
	socket_port_lock(addr_in->sin_port, inet->sport_type);
	inet->rcv_saddr = inet->saddr = addr_in->sin_addr.s_addr;
	inet->sport = addr_in->sin_port;
	inet->daddr = 0;
	inet->dport = 0;
	sock_unlock(sock->sk);

	return ENOERR;
}

int inet_dgram_connect(struct socket *sock, struct sockaddr * addr,
			int addr_len, int flags) {
	struct sock *sk;
	struct inet_sock *inet;
	sk = sock->sk;
	inet = inet_sk(sock->sk);
	inet->sport = socket_get_free_port(inet->sport_type);
	socket_set_port_type(sock);
	socket_port_lock(inet->sport, inet->sport_type);

	return sk->sk_prot->connect(sk, addr, addr_len);
}

int inet_sendmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size) {
	struct sock *sk;
	sk = sock->sk;
	return sk->sk_prot->sendmsg(iocb, sk, msg, size);
}

//FIXME add other states handling
int inet_stream_connect(struct socket *sock, struct sockaddr * addr,
			int addr_len, int flags) {
	int err;

	sock_lock(sock->sk);
	switch (sock->state) {
	default:
		err = -EINVAL;
		goto release;
	case SS_CONNECTED:
		err = -EISCONN;
		goto release;
	case SS_CONNECTING:
		err = -EALREADY;
		goto release;
	case SS_UNCONNECTED:
		err = -EISCONN;
		if (sock->sk->sk_state != TCP_CLOSE)
			goto release;
		err = sock->sk->sk_prot->connect(sock->sk, addr, addr_len);
		if (err < 0)
			goto release;
		break;
	}
	sock_unlock(sock->sk);

release:
	inet_release(sock);
	return err;
}

int inet_listen(struct socket *sock, int backlog) {
	struct sock *sk = sock->sk;

	return sk->sk_prot->listen(sk, backlog);
}

static int inet_accept(socket_t *sock, sockaddr_t *addr, int *addr_len) {
	struct sock *sk = sock->sk;

	return sk->sk_prot->accept(sk, addr, addr_len);
}

/* uses for create socket */
struct net_proto_family inet_family_ops = {
	.family = PF_INET,
	.create = inet_create,
};

const struct proto_ops inet_dgram_ops = {
	.family            = PF_INET,
	.release           = inet_release,
	.bind              = inet_bind,
	.connect           = inet_dgram_connect,
	.sendmsg           = inet_sendmsg,
	.recvmsg           = sock_common_recvmsg,
};

const struct proto_ops inet_stream_ops = {
	.family            = PF_INET,
	.release           = inet_release,
	.bind              = inet_bind,
	.connect           = inet_stream_connect,
	.accept            = inet_accept,
	.listen            = inet_listen,
	.sendmsg           = inet_sendmsg,
	.recvmsg           = sock_common_recvmsg,
};

static int inet_init(void) {
	return sock_register(&inet_family_ops);
}
