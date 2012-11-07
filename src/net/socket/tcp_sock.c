/**
 * @file
 * @brief Implements udp socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 */

#include <net/tcp.h>
#include <net/socket.h>
#include <net/net.h>
#include <net/ip.h>
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <lib/list.h>
#include <net/sock.h>
#include <string.h>
#include <mem/objalloc.h>
#include <embox/net/sock.h>
#include <net/route.h>
#include <net/inetdevice.h>
#include <fcntl.h>

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, tcp_prot, inet_stream_ops, 0, true);

OBJALLOC_DEF(objalloc_tcp_socks, struct tcp_sock, MODOPS_AMOUNT_TCP_SOCK); /* Allocator for tcp_sock structure */


/************************ Socket's functions ***************************/
static int tcp_v4_init_sock(struct sock *sk) {
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_init_sock: sk 0x%p\n", sock.tcp_sk);

	tcp_set_st(sock, TCP_CLOSED);
	sock.tcp_sk->this_unack = 100; // TODO remove constant
	sock.tcp_sk->seq_queue = sock.tcp_sk->this.seq = sock.tcp_sk->this_unack;
	sock.tcp_sk->this.wind = TCP_WINDOW_DEFAULT;
	sock.tcp_sk->lock = 0;
	sock.tcp_sk->last_activity = 0; // TODO 0 or not?
	sock.tcp_sk->oper_timeout = TCP_OPER_TIMEOUT * USEC_PER_MSEC;
	INIT_LIST_HEAD(&sock.tcp_sk->conn_wait);

	event_init(&sock.tcp_sk->new_conn, "new_conn");

	return ENOERR;
}

static int tcp_v4_connect(struct sock *sk, struct sockaddr *addr, int addr_len) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;
	struct sockaddr_in *addr_in;
	struct rt_entry *rte;
	int res;
	useconds_t started, now;
	__u8 magic_opts[] = { /* TODO This is a bad way to get perrmission on remote machine, but it's works */
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

	sock.sk = sk;
	debug_print(3, "tcp_v4_connect: sk 0x%p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	switch (sock.sk->sk_state) {
	default:
		res = -EBADF;
		break;
	case TCP_CLOSED:
	case TCP_SYN_RECV_PRE:
		if (addr_len != sizeof(struct sockaddr_in)) {
			res = -EINVAL;
			break;
		}
		addr_in = (struct sockaddr_in *)addr;
		/* setup inet_sock */
		rte = rt_fib_get_best(addr_in->sin_addr.s_addr, NULL);
		if (rte == NULL) {
			res = -EHOSTUNREACH;
			break;
		}
		assert(in_dev_get(rte->dev) != NULL);
		sock.inet_sk->saddr = in_dev_get(rte->dev)->ifa_address; // TODO remove this
		sock.inet_sk->daddr = addr_in->sin_addr.s_addr;
		sock.inet_sk->dport = addr_in->sin_port;
		/* make skb with options */
		skb = alloc_prep_skb(sizeof magic_opts, 0);
		if (skb == NULL) {
			res = -ENOMEM;
			break;
		}
		tcph = tcp_hdr(skb);
		tcph->syn = 1;
		sock.tcp_sk->seq_queue += tcp_seq_len(skb);
		memcpy(&tcph->options, &magic_opts[0], sizeof magic_opts);
		tcp_set_st(sock, TCP_SYN_SENT);
		send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
		now = started = tcp_get_usec();
		while (tcp_st_status(sock) == TCP_ST_NONSYNC) {
			now = tcp_get_usec();
			if (now - started >= sock.tcp_sk->oper_timeout) {
				break;
			}
		}
		if (now - started >= sock.tcp_sk->oper_timeout) {
			res = -ETIMEDOUT;
		}
		else {
			res = (tcp_st_status(sock) == TCP_ST_SYNC ? ENOERR : -ECONNRESET);
		}
		break;
	case TCP_LISTEN:
		res = -1;
		break;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		res = -1; /* error: connection already exists */
		break;
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return res;
}

static int tcp_v4_listen(struct sock *sk, int backlog) {
	union sock_pointer sock;
	int res;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_listen: sk 0x%p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
	switch (sock.sk->sk_state) {
	default:
		res = -EBADF;
		break;
	case TCP_CLOSED:
	case TCP_LISTEN:
		tcp_set_st(sock, TCP_LISTEN);
		res = ENOERR;
		break;
	case TCP_SYN_RECV_PRE:
		res = -1; /* TODO */
		break;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		res = -1; /* error: connection already exists */
		break;
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);

	return res;
}

static int tcp_v4_accept(struct sock *sk, struct sock **newsk,
		struct sockaddr *addr, int *addr_len) {
	union sock_pointer sock, newsock;
	struct sockaddr_in *addr_in;
	useconds_t started;

	assert(sk != NULL);
	assert(addr != NULL);
	assert(addr_len != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_accept: sk 0x%p, st%d\n", sock.tcp_sk, sock.sk->sk_state);

	switch (sock.sk->sk_state) {
	default: /* TODO another states */
		return -EBADF;
	case TCP_LISTEN:
		/* waiting anyone */
		if (list_empty(&sock.tcp_sk->conn_wait)) { /* TODO sync this */
			event_wait(&sock.tcp_sk->new_conn, EVENT_TIMEOUT_INFINITE);
		}
		assert(!list_empty(&sock.tcp_sk->conn_wait));
		/* get first socket from */
		newsock.tcp_sk = list_entry(sock.tcp_sk->conn_wait.next, struct tcp_sock, conn_wait);
		tcp_obj_lock(sock, TCP_SYNC_CONN_QUEUE);
		list_del_init(&newsock.tcp_sk->conn_wait);
		tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);
		/* save remote address */
		addr_in = (struct sockaddr_in *)addr;
		addr_in->sin_family = AF_INET;
		addr_in->sin_port = newsock.inet_sk->dport;
		addr_in->sin_addr.s_addr = newsock.inet_sk->daddr;
		*addr_len = sizeof *addr_in;
		debug_print(3, "tcp_v4_accept: newsk 0x%p for %s:%d\n", newsock.tcp_sk,
				inet_ntoa(addr_in->sin_addr), (int)ntohs(addr_in->sin_port));
		/* wait until something happened */
		started = tcp_get_usec();
		while (tcp_st_status(newsock) == TCP_ST_NONSYNC) {
			if (tcp_get_usec() - started >= sock.tcp_sk->oper_timeout) {
				tcp_free_sock(newsock);
				return -ETIMEDOUT;
			}
		}
		*newsk = newsock.sk;
		return (tcp_st_status(newsock) == TCP_ST_SYNC ? ENOERR : -ECONNRESET);
	}
}

static int tcp_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
	struct sk_buff *skb;
	union sock_pointer sock;
	size_t bytes, max_len;
	char *buff;

	assert(sk != NULL);
	assert(msg != NULL);
	assert(len == msg->msg_iov->iov_len);

	sock.sk = sk;
	debug_print(3, "tcp_v4_sendmsg: sk 0x%p\n", sock.tcp_sk);

	switch (sock.sk->sk_state) {
	default:
		return -EBADF;
	case TCP_CLOSED:
	case TCP_LISTEN:
	case TCP_SYN_RECV_PRE:
		return -1; /* error: connection does not exist */
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
		return -1; /* TODO save data and send them later */
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
		max_len = (sock.tcp_sk->rem.wind > TCP_MAX_DATA_LEN ?
				TCP_MAX_DATA_LEN : sock.tcp_sk->rem.wind);
		buff = (char *)msg->msg_iov->iov_base;
		while (len > 0) {
			bytes = (len > max_len ? max_len : len);
			debug_print(3, "tcp_v4_sendmsg: sending len %d\n", bytes);
			skb = alloc_prep_skb(0, bytes);
			if (skb == NULL) {
				if (len != msg->msg_iov->iov_len) {
					break;
				}
				return -ENOMEM;
			}
			memcpy((void *)(skb->h.raw + TCP_V4_HEADER_MIN_SIZE),
					buff, bytes);
			buff += bytes;
			len -= bytes;
			/* Fill TCP header */
			skb->h.th->psh = 1; /* XXX not req */
			skb->h.th->ack = 1;
			sock.tcp_sk->seq_queue += tcp_seq_len(skb);
			send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
		}
		msg->msg_iov->iov_len -= len;
		return ENOERR;
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -1; /* error: connection closing */
	}
}

static int tcp_v4_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags) {
	struct sk_buff *skb;
	union sock_pointer sock;
	size_t bytes;
	char *buff, last_iteration;
	useconds_t started;

	assert(sk != NULL);
	assert(msg != NULL);
	assert(len == msg->msg_iov->iov_len);

	sock.sk = sk;
	debug_print(3, "tcp_v4_recvmsg: sk 0x%p\n", sock.tcp_sk);

	started = tcp_get_usec();

check_state:
	switch (sock.sk->sk_state) {
	default:
		return -EBADF;
	case TCP_CLOSED:
	case TCP_LISTEN:
		return -1; /* error: connection does not exist */
	case TCP_SYN_SENT:
	case TCP_SYN_RECV_PRE:
	case TCP_SYN_RECV:
		return -1; /* TODO this in tcp_st_xxx function*/
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
		skb = skb_queue_front(sk->sk_receive_queue);
		if (skb == NULL) {
			if (sock.sk->sk_state == TCP_CLOSEWAIT) {
				msg->msg_iov->iov_len = 0;
				return -ECONNREFUSED; /* no more data to receive */
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
		return ENOERR;
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -ECONNREFUSED; /* error: connection closing */
	}
}

static void tcp_v4_close(struct sock *sk, long timeout) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(3, "tcp_v4_close: sk 0x%p\n", sock.tcp_sk);

	tcp_obj_lock(sock, TCP_SYNC_STATE);
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
		tcph = tcp_hdr(skb);
		tcph->fin = 1;
		tcph->ack = 1;
		sock.tcp_sk->seq_queue += tcp_seq_len(skb);
		tcp_set_st(sock, (sock.sk->sk_state == TCP_CLOSEWAIT ? TCP_LASTACK : TCP_FINWAIT_1));
		send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
		break;
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
		break; /* error: connection closing */
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		break; /* error: connection closing */
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);
}

/* TODO */
int tcp_v4_shutdown(struct sock *sk, int how) {
	if (!(how & (SHUT_WR + 1))) {
		return ENOERR;
	}

	/*tcp_send_fin()*/
	return ENOERR;
}

static void tcp_v4_hash(struct sock *sk) {
	size_t i;

	debug_print(4, "tcp_v4_hash: sk 0x%p\n", sk);
	for (i = 0; i < sizeof tcp_table / sizeof tcp_table[0]; ++i) {
		if (tcp_table[i] == NULL) {
			tcp_table[i] = (struct tcp_sock *)sk;
			break;
		}
	}
}

static void tcp_v4_unhash(struct sock *sk) {
	size_t i;

	debug_print(4, "tcp_v4_unhash: sk 0x%p\n", sk);
	for (i = 0; i < sizeof tcp_table / sizeof tcp_table[0]; ++i) {
		if (tcp_table[i] == (struct tcp_sock *)sk) {
			tcp_table[i] = NULL;
			break;
		}
	}
}

static int allocated = 0; /* for debug */
static struct sock * tcp_v4_sock_alloc(void) {
	struct sock *sk;

	sk = (struct sock *)objalloc(&objalloc_tcp_socks);
	debug_print(6, "tcp_v4_sock_alloc: 0x%p, total %d\n", sk, ++allocated);
	return sk;
}

static void tcp_v4_sock_free(struct sock *sk) {
	debug_print(6, "tcp_v4_sock_free: 0x%p, total %d\n", sk, --allocated);
	objfree(&objalloc_tcp_socks, sk);
}

static int tcp_v4_setsockopt(struct sock *sk, int level, int optname,
			char *optval, int optlen) {
	return ENOERR;
}

static int tcp_v4_getsockopt(struct sock *sk, int level, int optname,
			char *optval, int *optlen) {
	return ENOERR;
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
