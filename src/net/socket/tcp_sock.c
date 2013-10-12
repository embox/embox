/**
 * @file
 * @brief
 *
 * @date 18.06.12
 * @author Ilia Vaprol
 */

#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/uio.h>
#include <util/array.h>
#include <util/math.h>
#include <net/if_ether.h>

#include <net/l4/tcp.h>
#include <sys/socket.h>
#include <net/l3/ipv4/ip.h>
#include <net/sock.h>
#include <net/l3/route.h>
#include <net/inetdevice.h>

#include <kernel/time/time.h>

#include <mem/misc/pool.h>
#include <util/indexator.h>
#include <netinet/in.h>
#include <embox/net/sock.h>
#include <kernel/task/io_sync.h>

#include <framework/mod/options.h>
#define MODOPS_AMOUNT_TCP_SOCK OPTION_GET(NUMBER, amount_tcp_sock)
#define MODOPS_AMOUNT_TCP_PORT OPTION_GET(NUMBER, amount_tcp_port)

static const struct sock_proto_ops tcp_sock_ops_struct;
const struct sock_proto_ops *const tcp_sock_ops = &tcp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, 1, tcp_sock_ops_struct);

/************************ Socket's functions ***************************/
static int tcp_init(struct sock *sk) {
	struct tcp_sock *tcp_sk;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	debug_print(3, "tcp_init: sk %p\n", to_sock(tcp_sk));

	tcp_sk->p_sk = tcp_sk->p_sk; /* already initialized */
	tcp_sk->state = TCP_CLOSED;
	tcp_sk->self.seq = tcp_sk->last_ack;
	tcp_sk->self.wind = TCP_WINDOW_DEFAULT;
	tcp_sk->parent = NULL;
	INIT_LIST_HEAD(&tcp_sk->conn_wait);
	tcp_sk->conn_wait_len = tcp_sk->conn_wait_max = 0;
	tcp_sk->lock = 0;
	/* timerclear(&sock.tcp_sk->syn_time); */
	timerclear(&tcp_sk->ack_time);
	timerclear(&tcp_sk->rcv_time);

	return 0;
}

static int tcp_close(struct sock *sk) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	struct tcp_sock *tcp_sk;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	debug_print(3, "tcp_close: sk %p\n", to_sock(tcp_sk));

	tcp_obj_lock(tcp_sk, TCP_SYNC_STATE);
	{
		assert(tcp_sk->state < TCP_MAX_STATE);
		switch (tcp_sk->state) {
		default:
			return -EBADF;
		case TCP_CLOSED:
			tcp_obj_unlock(tcp_sk, TCP_SYNC_STATE);
			tcp_free_sock(tcp_sk);
			return 0;
		case TCP_LISTEN:
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
			tcp_set_st(tcp_sk, TCP_CLOSED);
			tcp_obj_unlock(tcp_sk, TCP_SYNC_STATE);
			tcp_free_sock(tcp_sk);
			return 0;
		case TCP_SYN_RECV:
		case TCP_ESTABIL:
		case TCP_CLOSEWAIT:
			skb = alloc_prep_skb(0, 0);
			if (skb == NULL) {
				break;
			}
			tcp_set_st(tcp_sk, tcp_sk->state == TCP_CLOSEWAIT ? TCP_LASTACK
						: TCP_FINWAIT_1);
			build_tcp_packet(0, 0, tcp_sk, skb);
			tcph = tcp_hdr(skb);
			tcph->fin = 1;
			tcph->ack = 1;
			send_data_from_sock(tcp_sk, skb);
			break;
		}
	}
	tcp_obj_unlock(tcp_sk, TCP_SYNC_STATE);

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
		/**
		 * TODO
		 * This is a bad way to get perrmission on remote machine, but it's
		 * works
		 */
		0x02, 0x04,      /* Maximum segment size:             */
				0x40, 0x0C,             /* 16396 bytes  */
		0x04, 0x02,      /* TCP SACK Permitted Option: True   */
		0x08, 0x0A,      /* Timestamps:                       */
				0x00, 0x0E, 0x3C, 0xCD, /* TSval 933069 */
				0x00, 0x00, 0x00, 0x00, /* TSecr 0      */
		0x01,            /* No-Operation                      */
		0x03, 0x03, 0x07 /* Window scale: 7 (multiply by 128) */
	};

	(void)addr;
	(void)addr_len;
	(void)flags;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	debug_print(3, "tcp_connect: sk %p\n", to_sock(tcp_sk));

	tcp_obj_lock(tcp_sk, TCP_SYNC_STATE);
	{
		assert(tcp_sk->state < TCP_MAX_STATE);
		switch (tcp_sk->state) {
		default:
			ret = -EISCONN;
			break;
		case TCP_CLOSED:
			/* make skb with options */
			skb = alloc_prep_skb(sizeof magic_opts, 0);
			if (skb == NULL) {
				ret = -ENOMEM;
				break;
			}
			tcp_set_st(tcp_sk, TCP_SYN_SENT);
			build_tcp_packet(sizeof magic_opts, 0,
					tcp_sk, skb);
			tcph = tcp_hdr(skb);
			tcph->syn = 1;
			memcpy(&tcph->options, &magic_opts[0], sizeof magic_opts);
			send_data_from_sock(tcp_sk, skb);

			ret = -EINPROGRESS;
			break;
		}
	}
	tcp_obj_unlock(tcp_sk, TCP_SYNC_STATE);

	return ret;
}

static int tcp_listen(struct sock *sk, int backlog) {
	int ret;
	struct tcp_sock *tcp_sk;

	tcp_sk = to_tcp_sock(sk);
	assert(tcp_sk != NULL);

	debug_print(3, "tcp_listen: sk %p\n", to_sock(tcp_sk));

	tcp_obj_lock(tcp_sk, TCP_SYNC_STATE);
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
			tcp_set_st(tcp_sk, TCP_LISTEN);
			tcp_sk->conn_wait_max = backlog;
			ret = 0;
			break;
		}
	}
	tcp_obj_unlock(tcp_sk, TCP_SYNC_STATE);

	return ret;
}

static int tcp_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addr_len, int flags, struct sock **newsk) {
	struct tcp_sock *tcp_sk, *tcp_newsk;

	(void)addr;
	(void)addr_len;
	(void)flags;

	assert(sk != NULL);
	assert(newsk != NULL);

	tcp_sk = to_tcp_sock(sk);
	debug_print(3, "tcp_accept: sk %p, st%d\n",
			to_sock(tcp_sk), tcp_sk->state);

	assert(tcp_sk->state < TCP_MAX_STATE);
	switch (tcp_sk->state) {
	default:
		return -EINVAL; /* error: the socket is not accepting connections */
	case TCP_LISTEN:
		/* waiting anyone */
		tcp_obj_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
		{
			io_sync_disable(&to_sock(tcp_sk)->ios,
					IO_SYNC_READING);
			if (list_empty(&tcp_sk->conn_wait)) {
				tcp_obj_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);
				return -EAGAIN;
			}

			/* get first socket from */
			tcp_newsk = list_entry(tcp_sk->conn_wait.next,
					struct tcp_sock, conn_wait);

			/* delete new socket from list */
			list_del_init(&tcp_newsk->conn_wait);
			assert(tcp_sk->conn_wait_len != 0);
			--tcp_sk->conn_wait_len;

			/* enable reading if queue not empty */
			if (!list_empty(&tcp_sk->conn_wait)
					&& io_sync_ready(&to_sock(list_entry(
								tcp_sk->conn_wait.next, struct tcp_sock,
									conn_wait))->ios,
						IO_SYNC_WRITING)) {
				io_sync_enable(&to_sock(tcp_sk)->ios, IO_SYNC_READING);
			}
		}
		tcp_obj_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);

		debug_print(3, "tcp_accept: newsk %p for %s:%hu\n",
				to_sock(tcp_newsk),
				inet_ntoa(to_inet_sock(to_sock(tcp_newsk))->dst_in.sin_addr),
				ntohs(to_inet_sock(to_sock(tcp_newsk))->dst_in.sin_port));

		if (tcp_st_status(tcp_newsk) == TCP_ST_NOTEXIST) {
			tcp_free_sock(tcp_newsk);
			return -ECONNRESET;
		}

		assert(io_sync_ready(&to_sock(tcp_newsk)->ios, IO_SYNC_WRITING));

		*newsk = to_sock(tcp_newsk);

		return 0;
	}
}

static int tcp_sendmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	struct sk_buff *skb;
	struct tcp_sock *tcp_sk;
	size_t bytes, max_len;
	char *buff;
	size_t len = msg->msg_iov->iov_len;

	(void)flags;

	assert(sk != NULL);
	assert(msg != NULL);
	assert(len == msg->msg_iov->iov_len);

	tcp_sk = to_tcp_sock(sk);
	debug_print(3, "tcp_sendmsg: sk %p\n", to_sock(tcp_sk));

	assert(tcp_sk->state < TCP_MAX_STATE);
	switch (tcp_sk->state) {
	default:
		return -ENOTCONN;
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
		max_len = (tcp_sk->rem.wind > TCP_MAX_DATA_LEN ?
				TCP_MAX_DATA_LEN : tcp_sk->rem.wind);
		buff = (char *)msg->msg_iov->iov_base;
		while (len != 0) {
			/* Maximum size of data that can be send without tcp window size overflowing */
			int upper_bound;

			while (!(upper_bound = tcp_sk->rem.wind - (tcp_sk->self.seq - tcp_sk->last_ack)));

			assert(upper_bound >= 0, "wind - %d, (self.seq - last_ack) - %d\n",
					tcp_sk->rem.wind, tcp_sk->self.seq - tcp_sk->last_ack);

			bytes = min(upper_bound, (len > max_len ? max_len : len));
			debug_print(3, "tcp_sendmsg: sending len %d\n", bytes);
			skb = alloc_prep_skb(0, bytes);
			if (skb == NULL) {
				if (len != msg->msg_iov->iov_len) {
					break;
				}
				return -ENOMEM;
			}
			build_tcp_packet(0, bytes, tcp_sk, skb);
			memcpy((void *)(skb->h.raw + TCP_MIN_HEADER_SIZE),
					buff, bytes);
			buff += bytes;
			len -= bytes;
			/* Fill TCP header */
			skb->h.th->psh = len != 0 ? 0 : 1;
			skb->h.th->ack = 1;
			send_data_from_sock(tcp_sk, skb);
		}
		msg->msg_iov->iov_len -= len;
		return 0;
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
	int ret;
	struct tcp_sock *tcp_sk;

	if (sk == NULL) {
		return -EINVAL;
	}

	tcp_sk = to_tcp_sock(sk);

	debug_print(3, "tcp_recvmsg: sk %p\n", tcp_sk);

	assert(tcp_sk->state < TCP_MAX_STATE);
	switch (tcp_sk->state) {
	default:
		return -ENOTCONN;
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
		ret = sock_stream_recvmsg(to_sock(tcp_sk), msg, flags);
		if ((ret == -EAGAIN) && (tcp_sk->state == TCP_CLOSEWAIT)) {
			msg->msg_iov->iov_len = 0;
			return 0; /* no more data to receive */
		}
		return ret;
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

	tcp_set_st(to_tcp_sock(sk), TCP_CLOSED);

	/*tcp_send_fin()*/
	return 0;
}

POOL_DEF(tcp_sock_pool, struct tcp_sock, MODOPS_AMOUNT_TCP_SOCK);
INDEX_CLAMP_DEF(tcp_sock_port, 0, MODOPS_AMOUNT_TCP_PORT,
		IPPORT_RESERVED, IPPORT_USERRESERVED - 1);
static LIST_DEF(tcp_sock_list);

static const struct sock_proto_ops tcp_sock_ops_struct = {
	.init      = tcp_init,
	.close     = tcp_close,
	.connect   = tcp_connect,
	.listen    = tcp_listen,
	.accept    = tcp_accept,
	.sendmsg   = tcp_sendmsg,
	.recvmsg   = tcp_recvmsg,
	.shutdown  = tcp_shutdown,
	.sock_pool = &tcp_sock_pool,
	.sock_port = &tcp_sock_port,
	.sock_list = &tcp_sock_list
};
