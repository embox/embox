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

#include <net/tcp.h>
#include <sys/socket.h>
#include <net/ip.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/inetdevice.h>

#include <kernel/time/time.h>

#include <mem/objalloc.h>
#include <embox/net/sock.h>
#include <kernel/task/io_sync.h>


EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, tcp_prot, inet_stream_ops, 0, true);

OBJALLOC_DEF(objalloc_tcp_socks, struct tcp_sock, MODOPS_AMOUNT_TCP_SOCK);


/************************ Socket's functions ***************************/
static int tcp_v4_init_sock(struct sock *sk) {
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_init_sock: sk %p\n", sock.tcp_sk);

	tcp_set_st(sock, TCP_CLOSED);
	sock.tcp_sk->last_ack = 100; // TODO remove constant
	sock.tcp_sk->self.seq = sock.tcp_sk->last_ack;
	sock.tcp_sk->self.wind = TCP_WINDOW_DEFAULT;
	sock.tcp_sk->lock = 0;
	sock.tcp_sk->last_activity = 0;
	sock.tcp_sk->oper_timeout = TCP_OPER_TIMEOUT * USEC_PER_MSEC;
	INIT_LIST_HEAD(&sock.tcp_sk->conn_wait);

	event_init(&sock.tcp_sk->new_conn, "new_conn");

	return 0;
}

static int tcp_v4_connect(struct sock *sk, struct sockaddr *addr, int addr_len) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;
	struct sockaddr_in *addr_in;
	struct rt_entry *rte;
	int ret;
	useconds_t started, now;
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
	debug_print(3, "tcp_v4_connect: sk %p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	{
		assert(sock.sk->sk_state < TCP_MAX_STATE);
		switch (sock.sk->sk_state) {
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
			now = started = tcp_get_usec();
			while (tcp_st_status(sock) == TCP_ST_NONSYNC) {
				now = tcp_get_usec();
				if (now - started >= sock.tcp_sk->oper_timeout) {
					break;
				}
			}
			if (now - started >= sock.tcp_sk->oper_timeout) {
				ret = -ETIMEDOUT;
			}
			else {
				ret = tcp_st_status(sock) == TCP_ST_SYNC ? 0 : -ECONNRESET;
			}
			break;
		}
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return ret;
}

static int tcp_v4_listen(struct sock *sk, int backlog) {
	union sock_pointer sock;
	int ret;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_listen: sk %p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	{
		assert(sock.sk->sk_state < TCP_MAX_STATE);
		switch (sock.sk->sk_state) {
		default:
			ret = -EINVAL; /* error: connection already exists */
			break;
		case TCP_CLOSED:
		case TCP_LISTEN:
			tcp_set_st(sock, TCP_LISTEN);
			ret = 0;
			break;
		}
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return ret;
}
#include <kernel/thread/sched_lock.h>
static int tcp_v4_accept(struct sock *sk, struct sock **newsk,
		struct sockaddr *addr, int *addr_len, int flags) {
	union sock_pointer sock, newsock;
	struct sockaddr_in *addr_in;
	useconds_t started;

	assert(sk != NULL);
	assert(newsk != NULL);
	assert((addr == NULL) || (addr_len != NULL));

	sock.sk = sk;
	debug_print(3, "tcp_v4_accept: sk %p, st%d\n", sock.tcp_sk, sock.sk->sk_state);

	assert(sock.sk->sk_state < TCP_MAX_STATE);
	switch (sock.sk->sk_state) {
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
					sock_unlock(sk);

					event_wait(&sock.tcp_sk->new_conn, EVENT_TIMEOUT_INFINITE);

					if (!sock_lock(&sk)) {
						tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);
						sched_unlock();
						return -EINVAL;
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
		debug_print(3, "tcp_v4_accept: newsk %p for %s:%d\n", newsock.tcp_sk,
				inet_ntoa(*(struct in_addr *)&newsock.inet_sk->daddr), (int)ntohs(newsock.inet_sk->dport));
		/* wait until something happened */
		started = tcp_get_usec();
		while (tcp_st_status(newsock) == TCP_ST_NONSYNC) {
			if (tcp_get_usec() - started >= sock.tcp_sk->oper_timeout) {
				tcp_free_sock(newsock);
				return -ETIMEDOUT;
			}
		}
		*newsk = newsock.sk;
		return tcp_st_status(newsock) == TCP_ST_SYNC ? 0 : -ECONNRESET;
		/* case TCP_LISTEN */
	} /* switch(sock.sk->sk_state) */
}

static int tcp_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int flags) {
	struct sk_buff *skb;
	union sock_pointer sock;
	size_t bytes, max_len;
	char *buff;

	assert(sk != NULL);
	assert(msg != NULL);
	assert(len == msg->msg_iov->iov_len);

	sock.sk = sk;
	debug_print(3, "tcp_v4_sendmsg: sk %p\n", sock.tcp_sk);

	assert(sock.sk->sk_state < TCP_MAX_STATE);
	switch (sock.sk->sk_state) {
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
			debug_print(3, "tcp_v4_sendmsg: sending len %d\n", bytes);
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

static int tcp_v4_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int flags) {
	struct sk_buff *skb;
	union sock_pointer sock;
	size_t bytes;
	char *buff, last_iteration;
	useconds_t started;

	assert(sk != NULL);
	assert(msg != NULL);
	assert(len == msg->msg_iov->iov_len);

	sock.sk = sk;
	debug_print(3, "tcp_v4_recvmsg: sk %p\n", sock.tcp_sk);

	started = tcp_get_usec();

check_state:
	assert(sock.sk->sk_state < TCP_MAX_STATE);
	switch (sock.sk->sk_state) {
	default:
		return -ENOTCONN;
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
		skb = skb_queue_front(sk->sk_receive_queue);
		if (skb == NULL) {
			if (sock.sk->sk_state == TCP_CLOSEWAIT) {
				msg->msg_iov->iov_len = 0;
				return 0; /* no more data to receive */
			}
			if (tcp_get_usec() - started >= sock.tcp_sk->oper_timeout) {
				return -ETIMEDOUT; /* error: timeout */
			}

			if (flags & O_NONBLOCK) {
				return -EAGAIN;
			}
			/* wait received packet or another state */
			goto check_state;
		}
		last_iteration = 0;
		buff = (char *)msg->msg_iov->iov_base;
		do {
			bytes = tcp_data_left(skb);
			if (bytes > len) {
				bytes = len;
				last_iteration = 1;
			}
			memcpy((void *)buff, skb->p_data, bytes);
			buff += bytes;
			len -= bytes;
			debug_print(3, "tcp_v4_recvmsg: received len %d\n", bytes);
			if (last_iteration) {
				skb->p_data += bytes;
				break;
			}
			skb_free(skb);
		} while ((len > 0) && ((skb = skb_queue_front(sk->sk_receive_queue)) != NULL));
		msg->msg_iov->iov_len -= len;
		return 0;
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -EBADF;
	}
}

static void tcp_v4_close(struct sock *sk, long timeout) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_close: sk %p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	{
		assert(sock.sk->sk_state < TCP_MAX_STATE);
		switch (sock.sk->sk_state) {
		default:
			break; /* error: EBADF */
		case TCP_CLOSED:
			tcp_obj_unlock(sock, TCP_SYNC_STATE);
			tcp_free_sock(sock);
			return; /* error: connection does not exist */
		case TCP_LISTEN:
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
			tcp_set_st(sock, TCP_CLOSED);
			tcp_obj_unlock(sock, TCP_SYNC_STATE);
			tcp_free_sock(sock);
			return;
		case TCP_SYN_RECV:
		case TCP_ESTABIL:
		case TCP_CLOSEWAIT:
			skb = alloc_prep_skb(0, 0);
			if (skb == NULL) {
				break;
			}
			tcp_set_st(sock, sock.sk->sk_state == TCP_CLOSEWAIT ? TCP_LASTACK : TCP_FINWAIT_1);
			build_tcp_packet(0, 0, sock, skb);
			tcph = tcp_hdr(skb);
			tcph->fin = 1;
			tcph->ack = 1;
			send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
			break;
		}
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);
}

/* TODO */
static int tcp_v4_shutdown(struct sock *sk, int how) {
	if (!(how & (SHUT_WR + 1))) {
		return 0;
	}

	/*tcp_send_fin()*/
	return 0;
}

static void tcp_v4_hash(struct sock *sk) {
	size_t i;

	debug_print(4, "tcp_v4_hash: sk %p\n", sk);

	tcp_obj_lock(tcp_sock_default, TCP_SYNC_SOCK_TABLE);
	{
		for (i = 0; i < ARRAY_SIZE(tcp_table); ++i) {
			if (tcp_table[i] == NULL) {
				tcp_table[i] = (struct tcp_sock *)sk;
				break;
			}
		}
	}
	tcp_obj_unlock(tcp_sock_default, TCP_SYNC_SOCK_TABLE);
}

static void tcp_v4_unhash(struct sock *sk) {
	size_t i;

	debug_print(4, "tcp_v4_unhash: sk %p\n", sk);

	tcp_obj_lock(tcp_sock_default, TCP_SYNC_SOCK_TABLE);
	{
		for (i = 0; i < ARRAY_SIZE(tcp_table); ++i) {
			if (tcp_table[i] == (struct tcp_sock *)sk) {
				tcp_table[i] = NULL;
				break;
			}
		}
	}
	tcp_obj_unlock(tcp_sock_default, TCP_SYNC_SOCK_TABLE);
}

static int allocated = 0; /* for debug */
static struct sock * tcp_v4_sock_alloc(void) {
	struct sock *sk;

	sk = (struct sock *)objalloc(&objalloc_tcp_socks); /* TODO sync this */
	debug_print(6, "tcp_v4_sock_alloc: %p, total %d\n", sk, ++allocated);
	return sk;
}

static void tcp_v4_sock_free(struct sock *sk) {
	debug_print(6, "tcp_v4_sock_free: %p, total %d\n", sk, --allocated);
	objfree(&objalloc_tcp_socks, sk);
}

static int tcp_v4_setsockopt(struct sock *sk, int level, int optname,
			char *optval, int optlen) {
	return 0;
}

static int tcp_v4_getsockopt(struct sock *sk, int level, int optname,
			char *optval, int *optlen) {
	return 0;
}

const struct proto tcp_prot = {
		.name       = "TCP",
		.init       = tcp_v4_init_sock,
		.connect    = tcp_v4_connect,
		.listen     = tcp_v4_listen,
		.accept     = tcp_v4_accept,
		.setsockopt = tcp_v4_setsockopt,
		.getsockopt = tcp_v4_getsockopt,
		.sendmsg    = tcp_v4_sendmsg,
		.recvmsg    = tcp_v4_recvmsg,
		.close      = tcp_v4_close,
		.hash       = tcp_v4_hash,
		.unhash     = tcp_v4_unhash,
		.sock_alloc = tcp_v4_sock_alloc,
		.sock_free  = tcp_v4_sock_free,
		.shutdown   = tcp_v4_shutdown,
		.obj_size   = sizeof(struct tcp_sock),
};
