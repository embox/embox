/**
 * @file
 * @brief
 *
 * @date 18.06.12
 * @author Ilia Vaprol
 */
#include <util/log.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <poll.h>
#include <sys/time.h>

#include <util/math.h>

#include <net/l4/tcp.h>
#include <net/lib/tcp.h>
#include <net/l3/ipv4/ip.h>
#include <net/l2/ethernet.h>
#include <net/sock.h>

#include <kernel/time/time.h>
#include <kernel/sched.h>

#include <mem/misc/pool.h>
#include <netinet/in.h>

#include "net_sock.h"

#include <kernel/sched/sched_lock.h>
#include <fs/idesc_event.h>
#include <net/sock_wait.h>

#include <util/err.h>

#include <framework/mod/options.h>
#define MODOPS_AMOUNT_TCP_SOCK OPTION_GET(NUMBER, amount_tcp_sock)

#include <config/embox/net/socket.h>
#define MODOPS_CONNECT_TIMEOUT \
	OPTION_MODULE_GET(embox__net__socket, NUMBER, connect_timeout)

#define MAX_SIMULTANEOUS_TX_PACK OPTION_GET(NUMBER, max_simultaneous_tx_pack)
static const struct sock_proto_ops tcp_sock_ops_struct;
const struct sock_proto_ops *const tcp_sock_ops
		= &tcp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, 1,
		tcp_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET6, SOCK_STREAM, IPPROTO_TCP, 1,
		tcp_sock_ops_struct);

#define MAX_HEADER_SIZE    (IP_MIN_HEADER_SIZE + IP_MAX_OPTLEN + \
		TCP_MIN_HEADER_SIZE + ETH_HEADER_SIZE + 128 /* 128 is max size of tcp options length */)


/************************ Socket's functions ***************************/
static int tcp_init(struct sock *sk) {
	static const struct tcp_wind self_wind_default = {
		.value = TCP_WINDOW_VALUE_DEFAULT,
		.factor = TCP_WINDOW_FACTOR_DEFAULT,
		.size = TCP_WINDOW_VALUE_DEFAULT << TCP_WINDOW_FACTOR_DEFAULT
	};
	struct tcp_sock *tcp_sk;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	log_debug("sk %p", to_sock(tcp_sk));

	tcp_sk->p_sk = tcp_sk->p_sk; /* already initialized */
	tcp_sk->state = TCP_CLOSED;
	tcp_sk->self.seq = tcp_sk->last_ack;
	memcpy(&tcp_sk->self.wind, &self_wind_default,
			sizeof tcp_sk->self.wind);
	tcp_sk->rem.wind.factor = 0;
	tcp_sk->parent = NULL;
	INIT_LIST_HEAD(&tcp_sk->conn_lnk);
	/* INIT_LIST_HEAD(&tcp_sk->conn_ready); */
	INIT_LIST_HEAD(&tcp_sk->conn_wait);
	INIT_LIST_HEAD(&tcp_sk->conn_free);
	tcp_sk->free_wait_queue_len = tcp_sk->free_wait_queue_max = 0;
	tcp_sk->lock = 0;
	/* timerclear(&sock.tcp_sk->syn_time); */
	timerclear(&tcp_sk->ack_time);
	timerclear(&tcp_sk->rcv_time);
	tcp_sk->dup_ack = 0;
	tcp_sk->rexmit_mode = 0;

	return 0;
}

static int tcp_close(struct sock *sk) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	struct tcp_sock *tcp_sk;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	log_debug("sk %p", to_sock(tcp_sk));

	tcp_sock_lock(tcp_sk, TCP_SYNC_STATE);
	{
		assert(tcp_sk->state < TCP_MAX_STATE);
		switch (tcp_sk->state) {
		default:
			return -EBADF;
		case TCP_CLOSED:
			tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);
			tcp_sock_release(tcp_sk);
			return 0;
		case TCP_LISTEN:
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
			tcp_sock_set_state(tcp_sk, TCP_CLOSED);
			tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);
			tcp_sock_release(tcp_sk);
			return 0;
		case TCP_SYN_RECV:
		case TCP_ESTABIL:
		case TCP_CLOSEWAIT:
			skb = NULL; /* alloc new pkg */
			if (0 != alloc_prep_skb(tcp_sk, 0, NULL, &skb)) {
				break; /* error: see ret */
			}
			tcp_sock_set_state(tcp_sk,
					tcp_sk->state == TCP_CLOSEWAIT ? TCP_LASTACK
						: TCP_FINWAIT_1);
			tcph = tcp_hdr(skb);
			tcp_build(tcph,
					sock_inet_get_dst_port(to_sock(tcp_sk)),
					sock_inet_get_src_port(to_sock(tcp_sk)),
					TCP_MIN_HEADER_SIZE, tcp_sk->self.wind.value);
			tcph->fin = 1;
			tcp_set_ack_field(tcph, tcp_sk->rem.seq);
			send_seq_from_sock(tcp_sk, skb);
			break;
		}
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

	return 0;
}

static int tcp_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addr_len,
		int flags) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	struct tcp_sock *tcp_sk;
	int ret;
	static const __u8 magic_opts[] = {
		TCP_OPT_KIND_MSS, 0x04,     /* Maximum segment size:             */
				0x40, 0x0C,               /* 16396 bytes         */
		TCP_OPT_KIND_NOP,           /* No-Operation                      */
		TCP_OPT_KIND_WS, 0x03,      /* Window scale:                     */
				TCP_WINDOW_FACTOR_DEFAULT /* 7 (multiply by 128) */
	};

	(void)addr;
	(void)addr_len;
	(void)flags;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	log_debug("sk %p", to_sock(tcp_sk));

	tcp_sock_lock(tcp_sk, TCP_SYNC_STATE);
	{
		assert(tcp_sk->state < TCP_MAX_STATE);
		switch (tcp_sk->state) {
		default:
			ret = -EISCONN;
			break;
		case TCP_CLOSED:
			/* make skb with options */
			skb = NULL; /* alloc new pkg */
			ret = alloc_prep_skb(tcp_sk, sizeof magic_opts, NULL, &skb);
			if (ret != 0) {
				break;
			}
			tcp_sock_set_state(tcp_sk, TCP_SYN_SENT);
			tcph = tcp_hdr(skb);
			tcp_build(tcph,
					sock_inet_get_dst_port(to_sock(tcp_sk)),
					sock_inet_get_src_port(to_sock(tcp_sk)),
					TCP_MIN_HEADER_SIZE + sizeof magic_opts,
					tcp_sk->self.wind.value);
			tcph->syn = 1;
			memcpy(&tcph->options, &magic_opts[0], sizeof magic_opts);
			send_seq_from_sock(tcp_sk, skb);
			//FIXME hack use common lock/unlock systems for socket
			sched_lock();
			{
				tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);
				ret = sock_wait(sk, POLLOUT | POLLERR, MODOPS_CONNECT_TIMEOUT);
				tcp_sock_lock(tcp_sk, TCP_SYNC_STATE);
			}
			sched_unlock();
			if (ret == -EAGAIN) {
				ret = -EINPROGRESS;
				break;
			}
			if (tcp_sock_get_status(tcp_sk) == TCP_ST_NOTEXIST) {
				ret = -ECONNRESET;
				break;
			}
			if (tcp_sock_get_status(tcp_sk) == TCP_ST_SYNC) {
				ret = 0;
			}

			break;
		}
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

	return ret;
}

static int tcp_sock_alloc_missing_backlog(struct tcp_sock *tcp_sk) {
	int to_alloc;

	assert(tcp_sk != NULL);

	tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
	{
		to_alloc = tcp_sk->free_wait_queue_max - tcp_sk->free_wait_queue_len;
		/* to_alloc allowed to be negative. It could be if backlog had set and then
		 * was reduced.
		 */

		while (to_alloc > 0) {
			struct sock *newsk;
			struct tcp_sock *tcp_newsk;

			newsk = sock_create(to_sock(tcp_sk)->opt.so_domain,
					SOCK_STREAM, IPPROTO_TCP);
			if (err(newsk) != 0) {
				break;
			}

			to_alloc--;

			tcp_newsk = to_tcp_sock(newsk);
			tcp_newsk->parent = tcp_sk;

			list_add_tail(&tcp_newsk->conn_lnk, &tcp_sk->conn_free);
			tcp_sk->free_wait_queue_len++;
		}
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);

	return tcp_sk->free_wait_queue_len;
}

static int tcp_listen(struct sock *sk, int backlog) {
	int ret;
	struct tcp_sock *tcp_sk;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	log_debug("sk %p", to_sock(tcp_sk));

	tcp_sock_lock(tcp_sk, TCP_SYNC_STATE);
	{
		assert(tcp_sk->state < TCP_MAX_STATE);
		switch (tcp_sk->state) {
		default:
			ret = -EINVAL; /* error: connection already exists */
			break;
		case TCP_CLOSED:
		case TCP_LISTEN:
			if (backlog <= 0) {
				ret = -EINVAL; /* error: invalid backlog */
				break;
			}
			tcp_sk->free_wait_queue_max = backlog;
			/* this could be not first listen call, adjusting backlog queue */
			tcp_sock_alloc_missing_backlog(tcp_sk);
			tcp_sock_set_state(tcp_sk, TCP_LISTEN);
			ret = 0;
			break;
		}
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

	return ret;
}

static inline struct tcp_sock *accept_get_connection(struct tcp_sock *tcp_sk) {
	struct tcp_sock *tcp_newsk;
	/* get first socket from */

	if (list_empty(&tcp_sk->conn_ready)) {
		return NULL;
	}

	tcp_newsk = list_entry(tcp_sk->conn_ready.next, struct tcp_sock, conn_lnk);

	/* check if reading was enabled for socket that already released */
	if (tcp_sock_get_status(tcp_newsk) == TCP_ST_NONSYNC) {
		//TODO have to create socket before
		return NULL;
	}

	/* delete new socket from list */
	list_del_init(&tcp_newsk->conn_lnk);
	assert(tcp_sk->free_wait_queue_len > 0);
	--tcp_sk->free_wait_queue_len;

	return tcp_newsk;
}

static int tcp_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addr_len, int flags, struct sock **newsk) {
	struct tcp_sock *tcp_sk, *tcp_newsk;
	int ret;

	(void)addr;
	(void)addr_len;
	(void)flags;

	assert(sk != NULL);
	assert(newsk != NULL);

	tcp_sk = to_tcp_sock(sk);
	log_debug("sk %p, st%d", to_sock(tcp_sk), tcp_sk->state);

	assert(tcp_sk->state < TCP_MAX_STATE);
	if (tcp_sk->state != TCP_LISTEN) {
		return -EINVAL; /* error: the socket is not accepting connections */
	}

	if (0 == tcp_sock_alloc_missing_backlog(tcp_sk)) {
		return -ENOBUFS;
	}

	/* waiting anyone */
	tcp_newsk = NULL;
	tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
	{
		do {
			tcp_newsk = accept_get_connection(tcp_sk);
			if (tcp_newsk) {
				break;
			}
			tcp_sock_alloc_missing_backlog(tcp_sk);
			ret = sock_wait(sk, POLLIN | POLLERR, SCHED_TIMEOUT_INFINITE);
		} while (!ret);
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);

	if (!tcp_newsk) {
		if (0 > ret) {
			return ret;
		} else {
			return -ECONNRESET; /* FIXME */
		}
	}

	if (tcp_sock_get_status(tcp_newsk) == TCP_ST_NOTEXIST) {
		tcp_sock_release(tcp_newsk);
		return -ECONNRESET;
	}

	assert(tcp_sock_get_status(tcp_newsk) == TCP_ST_SYNC);
	sk->rx_data_len--;
	*newsk = to_sock(tcp_newsk);

	return 0;
}

static int tcp_write(struct tcp_sock *tcp_sk, void *buff, size_t len) {
	void *pb;
	struct sk_buff *skb;
	int ret;

	pb = buff;
	while (len != 0) {
		/* Previous comment: try to send wholly msg
		 * We must pass no more than 64k bytes to underlaying IP level */
		size_t bytes = min(len, IP_MAX_PACKET_LEN - MAX_HEADER_SIZE);
		skb = NULL; /* alloc new pkg */

		ret = alloc_prep_skb(tcp_sk, 0, &bytes, &skb);
		if (ret != 0) {
			break;
		}

		log_debug("sending len %d", bytes);

		tcp_build(skb->h.th,
				sock_inet_get_dst_port(to_sock(tcp_sk)),
				sock_inet_get_src_port(to_sock(tcp_sk)),
				TCP_MIN_HEADER_SIZE, tcp_sk->self.wind.value);

		memcpy(skb->h.th + 1, pb, bytes);
		pb += bytes;
		len -= bytes;
		/* Fill TCP header */
		skb->h.th->psh = (len == 0);
		tcp_set_ack_field(skb->h.th, tcp_sk->rem.seq);
		send_seq_from_sock(tcp_sk, skb);
	}
	return pb - buff;
}

#if MAX_SIMULTANEOUS_TX_PACK > 0
static int tcp_wait_tx_ready(struct sock *sk, int timeout) {
	int ret;

	ret = 0;
	sched_lock();
	{
		while (MAX_SIMULTANEOUS_TX_PACK <= skb_queue_count(&sk->tx_queue)) {
			ret = sock_wait(sk, POLLOUT | POLLERR, timeout);
			if (ret < 0) {
				break;
			}
		}
	}
	sched_unlock();
	return ret;
}
#else
static inline int tcp_wait_tx_ready(struct sock *sk, int timeout) {
	return 0;
}
#endif

#define REM_WIND_MAX_SIZE (1460 * 100) /* FIXME use txqueuelen for netdev */
static int tcp_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct tcp_sock *tcp_sk;
	size_t len;
	int ret, timeout;

	(void)flags;

	assert(sk);
	assert(msg);

	timeout = timeval_to_ms(&sk->opt.so_sndtimeo);
	if (timeout == 0) {
		timeout = SCHED_TIMEOUT_INFINITE;
	}

	tcp_sk = to_tcp_sock(sk);
	log_debug("sk %p", to_sock(tcp_sk));

sendmsg_again:
	assert(tcp_sk->state < TCP_MAX_STATE);
	switch (tcp_sk->state) {
	default:
		return -ENOTCONN;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
		sched_lock();
		{
			ret = sock_wait(sk, POLLOUT | POLLERR, timeout);
		}
		sched_unlock();
		if (ret != 0) {
			return ret;
		}
		goto sendmsg_again;
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
		sched_lock();
		{
			while ((min(tcp_sk->rem.wind.size, REM_WIND_MAX_SIZE)
						<= tcp_sk->self.seq - tcp_sk->last_ack)
					|| tcp_sk->rexmit_mode) {
				ret = sock_wait(sk, POLLOUT | POLLERR, timeout);
				if (ret != 0) {
					sched_unlock();
					return ret;
				}
			}
		}
		sched_unlock();

		len = tcp_write(tcp_sk, msg->msg_iov->iov_base, msg->msg_iov->iov_len);
		ret = tcp_wait_tx_ready(sk, timeout);
		if (0 > ret) {
			return ret;
		}
		return len;
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -EPIPE;
	}
}

static int tcp_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	struct tcp_sock *tcp_sk;

	assert(sk);
	assert(msg);

	tcp_sk = to_tcp_sock(sk);

	log_debug("sk %p", to_sock(tcp_sk));

	assert(tcp_sk->state < TCP_MAX_STATE);
	switch (tcp_sk->state) {
	default:
		return -ENOTCONN;
	case TCP_CLOSEWAIT:
		if (to_sock(tcp_sk)->rx_data_len == 0) {
			msg->msg_iov->iov_len = 0;
			return 0; /* no more data to receive */
		}
		/* fallthrough */
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
		return sock_stream_recvmsg(to_sock(tcp_sk), msg, flags);
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -EBADF;
	}
}

/* TODO */
static int tcp_shutdown(struct sock *sk, int how) {
	if (!(how & (SHUT_WR + 1))) {
		return 0;
	}

	tcp_sock_set_state(to_tcp_sock(sk), TCP_CLOSED);

	/*tcp_send_fin()*/
	return 0;
}

static int tcp_setsockopt(struct sock *sk, int level, int optname,
			const void *optval, socklen_t optlen) {

	switch (optname) {
	case TCP_NODELAY:
		/* TODO just ignoring for now... */
		break;
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

POOL_DEF(tcp_sock_pool, struct tcp_sock, MODOPS_AMOUNT_TCP_SOCK);
static DLIST_DEFINE(tcp_sock_list);

static const struct sock_proto_ops tcp_sock_ops_struct = {
	.init       = tcp_init,
	.close      = tcp_close,
	.connect    = tcp_connect,
	.listen     = tcp_listen,
	.accept     = tcp_accept,
	.sendmsg    = tcp_sendmsg,
	.recvmsg    = tcp_recvmsg,
	.setsockopt = tcp_setsockopt,
	.shutdown   = tcp_shutdown,
	.sock_pool  = &tcp_sock_pool,
	.sock_list  = &tcp_sock_list
};
