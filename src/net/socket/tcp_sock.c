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

#include <net/tcp.h>
#include <sys/socket.h>
#include <net/ip.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/inetdevice.h>

#include <kernel/time/time.h>

#include <mem/misc/pool.h>
#include <embox/net/sock.h>
#include <kernel/task/io_sync.h>

#include <framework/mod/options.h>
#define MODOPS_AMOUNT_TCP_SOCK OPTION_GET(NUMBER, amount_tcp_sock)

static const struct sock_ops tcp_sock_ops_struct;
const struct sock_ops *const tcp_sock_ops = &tcp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, 1, tcp_sock_ops_struct);

/************************ Socket's functions ***************************/
static int tcp_init(struct sock *sk) {
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_init: sk %p\n", sock.tcp_sk);

	tcp_set_st(sock, TCP_CLOSED);
	sock.tcp_sk->self.seq = sock.tcp_sk->last_ack;
	sock.tcp_sk->self.wind = TCP_WINDOW_DEFAULT;
	INIT_LIST_HEAD(&sock.tcp_sk->conn_wait);
	sock.tcp_sk->conn_wait_max = 0;
	event_init(&sock.tcp_sk->new_conn, "new_conn");
	sock.tcp_sk->lock = 0;
	timerclear(&sock.tcp_sk->last_activity);
	sock.tcp_sk->oper_timeout = TCP_OPER_TIMEOUT;

	return 0;
}

static int tcp_close(struct sock *sk) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_close: sk %p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	{
		assert(sock.sk->state < TCP_MAX_STATE);
		switch (sock.sk->state) {
		default:
			return -EBADF;
		case TCP_CLOSED:
			tcp_obj_unlock(sock, TCP_SYNC_STATE);
			tcp_free_sock(sock);
			return 0;
		case TCP_LISTEN:
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
			tcp_set_st(sock, TCP_CLOSED);
			tcp_obj_unlock(sock, TCP_SYNC_STATE);
			tcp_free_sock(sock);
			return 0;
		case TCP_SYN_RECV:
		case TCP_ESTABIL:
		case TCP_CLOSEWAIT:
			skb = alloc_prep_skb(0, 0);
			if (skb == NULL) {
				break;
			}
			tcp_set_st(sock, sock.sk->state == TCP_CLOSEWAIT ? TCP_LASTACK : TCP_FINWAIT_1);
			build_tcp_packet(0, 0, sock, skb);
			tcph = tcp_hdr(skb);
			tcph->fin = 1;
			tcph->ack = 1;
			send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
			break;
		}
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return 0;
}

static int tcp_connect(struct sock *sk, const struct sockaddr *addr,
		socklen_t addr_len, int flags) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;
	struct sockaddr_in *addr_in;
	struct rt_entry *rte;
	int ret;
	struct timeval started;
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

	assert(sk != NULL);
	assert(addr != NULL);

	if (addr_len != sizeof(struct sockaddr_in)) {
		return -EINVAL;
	}

	sock.sk = sk;
	debug_print(3, "tcp_connect: sk %p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	{
		assert(sock.sk->state < TCP_MAX_STATE);
		switch (sock.sk->state) {
		default:
			ret = -EISCONN;
			break;
		case TCP_CLOSED:
			/* XXX setup inet_sock */
			addr_in = (struct sockaddr_in *)addr;
			rte = rt_fib_get_best(addr_in->sin_addr.s_addr, NULL);
			if (rte == NULL) {
				ret = -EHOSTUNREACH;
				break;
			}
			assert(inetdev_get_by_dev(rte->dev) != NULL);
			sock.inet_sk->saddr = inetdev_get_by_dev(rte->dev)->ifa_address; // TODO remove this
			sock.inet_sk->daddr = addr_in->sin_addr.s_addr;
			sock.inet_sk->dport = addr_in->sin_port;
			/* make skb with options */
			skb = alloc_prep_skb(sizeof magic_opts, 0);
			if (skb == NULL) {
				ret = -ENOMEM;
				break;
			}
			tcp_set_st(sock, TCP_SYN_SENT);
			build_tcp_packet(sizeof magic_opts, 0, sock, skb);
			tcph = tcp_hdr(skb);
			tcph->syn = 1;
			memcpy(&tcph->options, &magic_opts[0], sizeof magic_opts);
			send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
			tcp_get_now(&started);
			ret = 0;
			while (tcp_st_status(sock) == TCP_ST_NONSYNC) {
				if (tcp_is_expired(&started, sock.tcp_sk->oper_timeout)) {
					ret = -ETIMEDOUT;
					break;
				}
			}
			if (ret == 0) {
				ret = tcp_st_status(sock) == TCP_ST_SYNC ? 0 : -ECONNRESET;
			}
			break;
		}
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return ret;
}

static int tcp_listen(struct sock *sk, int backlog) {
	union sock_pointer sock;
	int ret;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_listen: sk %p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	{
		assert(sock.sk->state < TCP_MAX_STATE);
		switch (sock.sk->state) {
		default:
			ret = -EINVAL; /* error: connection already exists */
			break;
		case TCP_CLOSED:
		case TCP_LISTEN:
			if (backlog <= 0) {
				ret = -EINVAL; /* error: invalid backlog */
				break;
			}
			tcp_set_st(sock, TCP_LISTEN);
			sock.tcp_sk->conn_wait_max = backlog;
			ret = 0;
			break;
		}
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return ret;
}
#include <kernel/thread/sched_lock.h>
static int tcp_accept(struct sock *sk,
		struct sockaddr *addr, int *addr_len, int flags, struct sock **newsk) {
	int ret;
	union sock_pointer sock, newsock;
	struct sockaddr_in *addr_in;
	struct timeval started;

	assert(sk != NULL);
	assert(newsk != NULL);
	assert((addr == NULL) || (addr_len != NULL));

	sock.sk = sk;
	debug_print(3, "tcp_accept: sk %p, st%d\n", sock.tcp_sk, sock.sk->state);

	assert(sock.sk->state < TCP_MAX_STATE);
	switch (sock.sk->state) {
	default:
		return -EINVAL; /* error: the socket is not accepting connections */
	case TCP_LISTEN:
		/* waiting anyone */
		sched_lock(); {
			tcp_obj_lock(sock, TCP_SYNC_CONN_QUEUE); {
				if (list_empty(&sock.tcp_sk->conn_wait)) { /* TODO sync this */
					if (flags & O_NONBLOCK) {
						tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);
						sched_unlock();
						return -EAGAIN;
					}
					/* TODO: event condition */
					ret = EVENT_WAIT_OR_INTR(&sock.tcp_sk->new_conn, 0,
							EVENT_TIMEOUT_INFINITE);
					if (ret != 0) {
						tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);
						sched_unlock();
						return ret;
					}
				}
			} tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);
		} sched_unlock();

		tcp_obj_lock(sock, TCP_SYNC_CONN_QUEUE);
		{
			assert(!list_empty(&sock.tcp_sk->conn_wait));
			/* get first socket from */
			newsock.tcp_sk = list_entry(sock.tcp_sk->conn_wait.next, struct tcp_sock, conn_wait);
			/* Delete new socket from list and block listening socket if there are no connections. */
			list_del_init(&newsock.tcp_sk->conn_wait);
			if (list_empty(&sock.tcp_sk->conn_wait)) {
				idx_io_disable(sk->sk_socket->desc_data, IDX_IO_READING);
			}
		}
		tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);

		/* save remote address */
		if (addr != NULL) {
			addr_in = (struct sockaddr_in *)addr;
			addr_in->sin_family = AF_INET;
			addr_in->sin_port = newsock.inet_sk->dport;
			addr_in->sin_addr.s_addr = newsock.inet_sk->daddr;
			*addr_len = sizeof *addr_in;
		}
		debug_print(3, "tcp_accept: newsk %p for %s:%d\n", newsock.tcp_sk,
				inet_ntoa(*(struct in_addr *)&newsock.inet_sk->daddr), (int)ntohs(newsock.inet_sk->dport));
		/* wait until something happened */
		tcp_get_now(&started);
		while (tcp_st_status(newsock) == TCP_ST_NONSYNC) {
			if (tcp_is_expired(&started, sock.tcp_sk->oper_timeout)) {
				tcp_free_sock(newsock);
				return -ETIMEDOUT;
			}
		}

		if (tcp_st_status(newsock) == TCP_ST_NOTEXIST) {
			tcp_free_sock(newsock);
			return -ECONNRESET;
		}

		*newsk = newsock.sk;
		return 0;
		/* case TCP_LISTEN */
	} /* switch(sock.sk->state) */
}

static int tcp_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct sk_buff *skb;
	union sock_pointer sock;
	size_t bytes, max_len;
	char *buff;
	size_t len = msg->msg_iov->iov_len;

	assert(sk != NULL);
	assert(msg != NULL);
	assert(len == msg->msg_iov->iov_len);

	sock.sk = sk;
	debug_print(3, "tcp_sendmsg: sk %p\n", sock.tcp_sk);

	assert(sock.sk->state < TCP_MAX_STATE);
	switch (sock.sk->state) {
	default:
		return -ENOTCONN;
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
		max_len = (sock.tcp_sk->rem.wind > TCP_MAX_DATA_LEN ?
				TCP_MAX_DATA_LEN : sock.tcp_sk->rem.wind);
		buff = (char *)msg->msg_iov->iov_base;
		while (len != 0) {
			/* Maximum size of data that can be send without tcp window size overflowing */
			int upper_bound;

			while (!(upper_bound = sock.tcp_sk->rem.wind - (sock.tcp_sk->self.seq - sock.tcp_sk->last_ack)));

			assert(upper_bound >= 0, "wind - %d, (self.seq - last_ack) - %d\n",
					sock.tcp_sk->rem.wind, sock.tcp_sk->self.seq - sock.tcp_sk->last_ack);

			bytes = min(upper_bound, (len > max_len ? max_len : len));
			debug_print(3, "tcp_sendmsg: sending len %d\n", bytes);
			skb = alloc_prep_skb(0, bytes);
			if (skb == NULL) {
				if (len != msg->msg_iov->iov_len) {
					break;
				}
				return -ENOMEM;
			}
			build_tcp_packet(0, bytes, sock, skb);
			memcpy((void *)(skb->h.raw + TCP_MIN_HEADER_SIZE),
					buff, bytes);
			buff += bytes;
			len -= bytes;
			/* Fill TCP header */
			skb->h.th->psh = len != 0 ? 0 : 1;
			skb->h.th->ack = 1;
			send_data_from_sock(sock, skb);
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

static int tcp_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	int ret;

	if (sk == NULL) {
		return -EINVAL;
	}

	debug_print(3, "tcp_recvmsg: sk %p\n", sk);

	assert(sk->state < TCP_MAX_STATE);
	switch (sk->state) {
	default:
		return -ENOTCONN;
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
		ret = sock_stream_recvmsg(sk, msg, flags);
		if ((ret == -EAGAIN) && (sk->state == TCP_CLOSEWAIT)) {
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

	/*tcp_send_fin()*/
	return 0;
}

POOL_DEF(tcp_sock_pool, struct tcp_sock, MODOPS_AMOUNT_TCP_SOCK);
static LIST_DEF(tcp_sock_list);

static const struct sock_ops tcp_sock_ops_struct = {
	.init      = tcp_init,
	.close     = tcp_close,
	.connect   = tcp_connect,
	.listen    = tcp_listen,
	.accept    = tcp_accept,
	.sendmsg   = tcp_sendmsg,
	.recvmsg   = tcp_recvmsg,
	.shutdown  = tcp_shutdown,
	.sock_pool = &tcp_sock_pool,
	.sock_list = &tcp_sock_list
};
