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

#include <net/net_sock.h>

#include <kernel/sched/sched_lock.h>
#include <kernel/task/resource/idesc_event.h>
#include <net/sock_wait.h>

#include <util/err.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_TCP_SOCK    OPTION_GET(NUMBER, amount_tcp_sock)
#define MAX_SIMULTANEOUS_TX_PACK  OPTION_GET(NUMBER, max_simultaneous_tx_pack)

#include <config/embox/net/socket.h>
#define MODOPS_CONNECT_TIMEOUT \
	OPTION_MODULE_GET(embox__net__socket, NUMBER, connect_timeout)


static const struct sock_proto_ops tcp_sock_ops_struct;

const struct sock_proto_ops *const tcp_sock_ops = &tcp_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, 1, tcp_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET6, SOCK_STREAM, IPPROTO_TCP, 1, tcp_sock_ops_struct);

#if OPTION_GET(NUMBER, def_tcp_win_val) == 0
#define TCP_WINDOW_VALUE_DEFAULT  16384 /* Default size of widnow */
#else
#define TCP_WINDOW_VALUE_DEFAULT    (OPTION_GET(NUMBER, def_tcp_win_val))
#endif
#define TCP_WINDOW_FACTOR_DEFAULT     7 /* Default factor of widnow */

#define MAX_TCP_OPT_SIZE        128  /* 128 is max size of tcp options length */
#define MAX_HEADER_SIZE    (IP_MIN_HEADER_SIZE + IP_MAX_OPTLEN + \
		TCP_MIN_HEADER_SIZE + ETH_HEADER_SIZE + MAX_TCP_OPT_SIZE)

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

	tcp_sk->state = TCP_CLOSED;
	tcp_sk->self.seq = tcp_sk->last_ack;
	memcpy(&tcp_sk->self.wind, &self_wind_default, sizeof(tcp_sk->self.wind));
	tcp_sk->rem.wind.factor = 0;
	tcp_sk->parent = NULL;
	INIT_LIST_HEAD(&tcp_sk->conn_lnk);
	/* INIT_LIST_HEAD(&tcp_sk->conn_ready); */
    tcp_sk->listening.is_listening = 0;
    tcp_sk->accepted = 0;
	INIT_LIST_HEAD(&tcp_sk->listening.conn_wait);
	INIT_LIST_HEAD(&tcp_sk->listening.conn_free);
	tcp_sk->lock = 0;
	/* timerclear(&sock.tcp_sk->syn_time); */
	timerclear(&tcp_sk->ack_time);
	timerclear(&tcp_sk->rcv_time);
	tcp_sk->dup_ack = 0;
	tcp_sk->rexmit_mode = 0;

	return 0;
}

/* Form and send reset packet to interrupt connection */
int send_rst_from_socket(struct tcp_sock *tcp_sk){
    struct sk_buff *skb;
    in_port_t dst_port;
    in_port_t src_port;
    struct tcphdr *tcph;

    skb = NULL; /* alloc new pkg */
    int ret = alloc_prep_skb(tcp_sk, 0, NULL, &skb);
    if (ret) {
        return ret; /* error: see ret */
    }

    tcph = tcp_hdr(skb);
    dst_port = sock_inet_get_dst_port(to_sock(tcp_sk));
    src_port = sock_inet_get_src_port(to_sock(tcp_sk));
    tcp_build(tcph, dst_port, src_port, TCP_MIN_HEADER_SIZE,
            tcp_sk->self.wind.value);
    tcph->rst = 1;
    tcp_sk->rem.seq++;
    tcp_set_ack_field(tcph, tcp_sk->rem.seq);
    send_seq_from_sock(tcp_sk, skb);
    skb_free(skb);

    return 0;
}

/* Reset established connections that weren't accepted on listening socket*/
static void reset_unaccepted_ready(struct tcp_sock *parent){
    struct tcp_sock *sock_tcp;

    list_for_each_entry(sock_tcp, &parent->conn_ready, conn_lnk) {
        send_rst_from_socket(sock_tcp);
        tcp_sock_set_state(sock_tcp, TCP_CLOSED);
    }
    return;
}

static int tcp_close(struct sock *sk) {
	int ret;
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
			ret = -EBADF;
			break;
		case TCP_CLOSED:
			tcp_sock_release(tcp_sk);
			ret = 0;
			break;
		case TCP_LISTEN:
            reset_unaccepted_ready(tcp_sk);
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
			tcp_sock_set_state(tcp_sk, TCP_CLOSED);
			tcp_sock_release(tcp_sk);
			ret = 0;
			break;
		case TCP_SYN_RECV:
		case TCP_ESTABIL:
		case TCP_CLOSEWAIT:
		{
			enum tcp_sock_state new_state;
			in_port_t dst_port;
			in_port_t src_port;

			skb = NULL; /* alloc new pkg */
			ret = alloc_prep_skb(tcp_sk, 0, NULL, &skb);
			if (ret) {
				break; /* error: see ret */
			}
			new_state = (tcp_sk->state == TCP_CLOSEWAIT) ? TCP_LASTACK :
					TCP_FINWAIT_1;
			tcp_sock_set_state(tcp_sk, new_state);

			tcph = tcp_hdr(skb);
			dst_port = sock_inet_get_dst_port(to_sock(tcp_sk));
			src_port = sock_inet_get_src_port(to_sock(tcp_sk));
			tcp_build(tcph, dst_port, src_port, TCP_MIN_HEADER_SIZE,
					tcp_sk->self.wind.value);
			tcph->fin = 1;
			tcp_set_ack_field(tcph, tcp_sk->rem.seq);
			send_seq_from_sock(tcp_sk, skb);

			ret = 0;
			break;
		} /* TCP_CLOSEWAIT */
		}
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

	return ret;
}

static int tcp_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addr_len,
		int flags) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	struct tcp_sock *tcp_sk;
	int ret;
	static const uint8_t magic_opts[] = {
		TCP_OPT_KIND_MSS, 0x04,     /* Maximum segment size: */
		0x40, 0x0C,                 /* 16396 bytes           */
		TCP_OPT_KIND_NOP,           /* No-Operation          */
		TCP_OPT_KIND_WS, 0x03,      /* Window scale:         */
		TCP_WINDOW_FACTOR_DEFAULT   /* 7 (multiply by 128)   */
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
		{
			in_port_t dst_port;
			in_port_t src_port;

			/* make skb with options */
			skb = NULL; /* alloc new pkg */
			ret = alloc_prep_skb(tcp_sk, sizeof magic_opts, NULL, &skb);
			if (ret != 0) {
				break;
			}
			tcp_sock_set_state(tcp_sk, TCP_SYN_SENT);
			tcph = tcp_hdr(skb);
			dst_port = sock_inet_get_dst_port(to_sock(tcp_sk));
			src_port = sock_inet_get_src_port(to_sock(tcp_sk));
			tcp_build(tcph, dst_port, src_port,
					TCP_MIN_HEADER_SIZE + sizeof(magic_opts),
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
		} /*TCP_CLOSED */
		}
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

	return ret;
}

/* tcp_sock_listen_fetch() returns the next free allocated socket for
 * the new connection 
 * is called from tcp layer, from tcp_st_listen()*/
struct tcp_sock *tcp_sock_listen_fetch(struct tcp_sock *tcp_sk) {
    assert(tcp_sk->listening.wait_queue_len > 0);

    tcp_sk->listening.wait_queue_len--;
    struct tcp_sock *tcp_newsk = mcast_out(tcp_sk->listening.conn_free.next,
            struct tcp_sock, conn_lnk);
    list_del_init(&tcp_newsk->conn_lnk);

    return tcp_newsk;
}

/* tcp_sock_listen_alloc() allocate a new socket for accepting a new connection*/
int tcp_sock_listen_alloc(struct tcp_sock *tcp_sk) {
	assert(tcp_sk != NULL);
    struct tcp_listen_info *listen = &tcp_sk->listening;

    if (listen->wait_queue_len == listen->backlog) {
        return 0;
    }

	tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
	do{
        struct sock *newsk;
        struct tcp_sock *tcp_newsk;

        newsk = sock_create(to_sock(tcp_sk)->opt.so_domain,
                SOCK_STREAM, IPPROTO_TCP);
        if (ptr2err(newsk) != 0) {
            log_info("could not sock_create() ptr2err(%d)", ptr2err(newsk));
            break;
        }

        tcp_newsk = to_tcp_sock(newsk);
        tcp_newsk->parent = tcp_sk;

        list_add_tail(&tcp_newsk->conn_lnk, &tcp_sk->listening.conn_free);
        listen->wait_queue_len++; /* one more socket in queue ready for accept*/
	}while(0);
	tcp_sock_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);

	return listen->wait_queue_len;
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
            tcp_sk->listening.is_listening = 1;
            tcp_sk->listening.backlog = backlog;
            tcp_sk->listening.wait_queue_len = 0;
            /*tcp_sock_listen_alloc(tcp_sk);*/
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
    tcp_newsk->accepted = 1;

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

    /* Previously here, if no sockets for a queue could be allocated
     * we returned -ENOBUFS, which doesn't seem reasonable */
	/*if (0 == tcp_sock_alloc_missing_backlog(tcp_sk)) {
		return -ENOBUFS;
	}*/

	/* waiting anyone */
	tcp_newsk = NULL;
	tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
	{
		do {
			tcp_newsk = accept_get_connection(tcp_sk);
			if (tcp_newsk) {
				break;
			}
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

static int tcp_write(struct tcp_sock *tcp_sk, struct msghdr * msg) {
	size_t iov_len;
	int full_len;
	void *pb;
	struct sk_buff *skb;
	int ret;
	size_t skb_len;
	int tran_len;
	int cp_len;
	int cp_off = 0;
	int i;

	full_len = 0;
	tran_len = 0;
	for (i = 0; i < msg->msg_iovlen; i ++) {
		full_len += msg->msg_iov[i].iov_len;
	}

	iov_len = 0;
	skb_len = 0;

	for (i = 0; tran_len < full_len; ) {
		if (0 == iov_len) {
			pb = msg->msg_iov[i].iov_base;
			iov_len = msg->msg_iov[i].iov_len;
			i++;
		}

		if (0 == skb_len) {
			in_port_t src_port, dst_port;

			cp_off = 0;
			skb_len = min((full_len - tran_len), (IP_MAX_PACKET_LEN - MAX_HEADER_SIZE));
			skb = NULL; /* alloc new pkg */

			ret = alloc_prep_skb(tcp_sk, 0, &skb_len, &skb);
			if (ret != 0) {
				break;
			}

			dst_port = sock_inet_get_dst_port(to_sock(tcp_sk));
			src_port = sock_inet_get_src_port(to_sock(tcp_sk));
			tcp_build(skb->h.th, dst_port, src_port, TCP_MIN_HEADER_SIZE,
					tcp_sk->self.wind.value);
		}

		cp_len = min(iov_len, skb_len);

		memcpy(((void *)(skb->h.th + 1) + cp_off), pb, cp_len);
		iov_len -= cp_len;
		pb += cp_len;
		skb_len -= cp_len;
		tran_len += cp_len;
		cp_off += cp_len;

		if (0 == skb_len) {
			/* Fill TCP header */
			skb->h.th->psh = (iov_len == 0);
			tcp_set_ack_field(skb->h.th, tcp_sk->rem.seq);
			send_seq_from_sock(tcp_sk, skb);
			cp_off = 0;
		}
	}

	return tran_len;
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

		len = tcp_write(tcp_sk, msg);
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

static int tcp_getsockopt(struct sock *sk, int level, int optname,
        	void *optval, socklen_t *optlen) {
	int val;	

	switch (optname) {
	case TCP_NODELAY:
		val = 0;
		break;
	case TCP_MAXSEG:
		val = 1460;
		break;
	default:
		return -ENOPROTOOPT;
	}

	if (*optlen < sizeof(val)) {
		return -EINVAL;
	}
	memcpy(optval, &val, sizeof(val));
	*optlen = sizeof(val);

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
	.getsockopt = tcp_getsockopt,
	.setsockopt = tcp_setsockopt,
	.shutdown   = tcp_shutdown,
	.sock_pool  = &tcp_sock_pool,
	.sock_list  = &tcp_sock_list
};
