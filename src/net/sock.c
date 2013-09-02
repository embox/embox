/**
 * @file
 * @brief Generic socket support routines.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <embox/net/family.h>
#include <embox/net/sock.h>
#include <errno.h>
#include <hal/ipl.h>
#include <kernel/task/io_sync.h>
#include <mem/misc/pool.h>
#include <net/sock.h>
#include <net/skbuff.h>
#include <string.h>
#include <util/math.h>

static struct sock * sock_alloc(const struct sock_ops *ops) {
	ipl_t sp;
	struct sock *sk;

	assert(ops != NULL);

	sp = ipl_save();
	{
		sk = pool_alloc(ops->sock_pool);
	}
	ipl_restore(sp);

	return sk;
}

static void sock_free(struct sock *sk) {
	ipl_t sp;

	assert(sk != NULL);
	assert(sk->ops != NULL);

	sp = ipl_save();
	{
		pool_free(sk->ops->sock_pool, sk);
	}
	ipl_restore(sp);
}

static void sock_opt_init(struct sock_opt *opt, int family,
		int type, int protocol) {
	static const struct timeval
		default_rcvtimeo = SOCK_OPT_DEFAULT_RCVTIMEO,
		default_sndtimeo = SOCK_OPT_DEFAULT_SNDTIMEO;

	assert(opt != NULL);

	memset(opt, 0, sizeof *opt);
	opt->so_domain = family;
	opt->so_protocol = protocol;
	opt->so_rcvbuf = SOCK_OPT_DEFAULT_RCVBUF;
	opt->so_rcvlowat = SOCK_OPT_DEFAULT_RCVLOWAT;
	memcpy(&opt->so_rcvtimeo, &default_rcvtimeo,
			sizeof opt->so_rcvtimeo);
	opt->so_sndbuf = SOCK_OPT_DEFAULT_SNDBUF;
	opt->so_sndlowat = SOCK_OPT_DEFAULT_SNDLOWAT;
	memcpy(&opt->so_sndtimeo, &default_sndtimeo,
			sizeof opt->so_sndtimeo);
	opt->so_type = type;
}

static void sock_init(struct sock *sk, int family, int type,
		int protocol, const struct family_ops *f_ops,
		const struct sock_ops *ops) {
	assert(sk != NULL);
	assert(f_ops != NULL);
	assert(ops != NULL);

	list_link_init(&sk->lnk);
	sock_opt_init(&sk->opt, family, type, protocol);
	skb_queue_init(&sk->rx_queue);
	skb_queue_init(&sk->tx_queue);
	sk->state = 0;
	sk->shutdown_flag = 0;
	sk->f_ops = f_ops;
	sk->ops = ops;
	io_sync_init(&sk->ios, 0, 0);
}

int sock_create_ext(int family, int type, int protocol,
		int need_hash, struct sock **out_sk) {
	int ret;
	struct sock *new_sk;
	const struct net_family *nfamily;
	const struct net_family_type *nftype;
	const struct net_sock *nsock;

	if (out_sk == NULL) {
		return -EINVAL;
	}

	nfamily = net_family_lookup(family);
	if (nfamily == NULL) {
		return -EAFNOSUPPORT;
	}

	nftype = net_family_type_lookup(nfamily, type);
	if (nftype == NULL) {
		return -EPROTOTYPE;
	}

	nsock = net_sock_lookup(family, type, protocol);
	if (nsock == NULL) {
		return -EPROTONOSUPPORT;
	}

	new_sk = sock_alloc(nsock->ops);
	if (new_sk == NULL) {
		return -ENOMEM;
	}

	sock_init(new_sk, family, type, nsock->protocol,
			nftype->ops, nsock->ops);

	assert(new_sk->f_ops != NULL);
	ret = new_sk->f_ops->init(new_sk);
	if (ret != 0) {
		sock_release(new_sk);
		return ret;
	}

	assert(new_sk->ops != NULL);
	if (new_sk->ops->init != NULL) {
		ret = new_sk->ops->init(new_sk);
		if (ret != 0) {
			sock_close(new_sk);
			return ret;
		}
	}

	if (need_hash) {
		sock_hash(new_sk);
	}

	*out_sk = new_sk;

	return 0;
}

int sock_create(int family, int type, int protocol,
		struct sock **out_sk) {
	return sock_create_ext(family, type, protocol, 1, out_sk);
}

void sock_release(struct sock *sk) {
	if (sk == NULL) {
		return; /* error: invalid argument */
	}

	sock_unhash(sk);
	skb_queue_purge(&sk->rx_queue);
	skb_queue_purge(&sk->tx_queue);
	sock_free(sk);
}

void sock_hash(struct sock *sk) {
	if (sk == NULL) {
		return; /* error: invalid argument */
	}
	else if (!list_alone_element(sk, lnk)) {
		return; /* error: already in hash */
	}

	assert(sk->ops != NULL);
	list_add_last_element(sk, sk->ops->sock_list, lnk);
}

void sock_unhash(struct sock *sk) {
	if (sk == NULL) {
		return; /* error: invalid argument */
	}
	else if (list_alone_element(sk, lnk)) {
		return; /* error: not hashed */
	}

	list_unlink_element(sk, lnk);
}

struct sock * sock_iter(const struct sock_ops *ops) {
	if (ops == NULL) {
		return NULL; /* error: invalid argument */
	}

	return list_first_element(ops->sock_list, struct sock, lnk);
}

struct sock * sock_next(const struct sock *sk) {
	if (sk == NULL) {
		return NULL; /* error: invalid argument */
	}
	else if (list_alone_element(sk, lnk)) {
		return NULL; /* error: not hashed */
	}

	assert(sk->ops != NULL);

	return list_next_element(sk, sk->ops->sock_list,
			struct sock, lnk);
}

struct sock * sock_lookup(const struct sock *sk,
		const struct sock_ops *ops,
		sock_lookup_tester_ft tester,
		const struct sk_buff *skb) {
	struct sock *next_sk;
	ipl_t ipl;

	if ((ops == NULL) || (tester == NULL)) {
		return NULL; /* error: invalid arguments */
	}

	next_sk = sk != NULL ? sock_next(sk) : sock_iter(ops);

	ipl = ipl_save();

	while (next_sk != NULL) {
		if (tester(next_sk, skb)) {
			ipl_restore(ipl);
			return next_sk;
		}

		next_sk = sock_next(next_sk);
	}

	ipl_restore(ipl);

	return NULL; /* error: no such entity */
}

void sock_rcv(struct sock *sk, struct sk_buff *skb,
		unsigned char *p_data, size_t size) {
	if ((sk == NULL) || (skb == NULL) || (p_data == NULL)) {
		return; /* error: invalid argument */
	}

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		skb_free(skb);
		return; /* error: socket is down */
	}

	skb->sk = sk;
	skb->p_data = p_data;
	skb->p_data_end = p_data + size;

	skb_queue_push(&sk->rx_queue, skb);

	io_sync_enable(&sk->ios, IO_SYNC_READING);
}

int sock_close(struct sock *sk) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->f_ops != NULL);
	if (sk->f_ops->close == NULL) {
		sock_release(sk);
		return 0;
	}

	return sk->f_ops->close(sk);
}

int sock_common_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags, int stream_mode) {
	struct sk_buff *skb;
	char *buff;
	size_t buff_sz, total_len, len;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	assert(msg->msg_iov != NULL);
	assert(msg->msg_iov->iov_base != NULL);
	buff = msg->msg_iov->iov_base;
	buff_sz = msg->msg_iov->iov_len;
	total_len = 0;

	while (1) {
		io_sync_disable(&sk->ios, IO_SYNC_READING);
		skb = skb_queue_front(&sk->rx_queue);
		if (skb == NULL) {
			if (total_len == 0) {
				return -EAGAIN;
			}
			break;
		}
		io_sync_enable(&sk->ios, IO_SYNC_READING);

		len = min(buff_sz, skb->p_data_end - skb->p_data);

		memcpy(buff, skb->p_data, len);
		buff += len;
		buff_sz -= len;
		skb->p_data += len;
		total_len += len;

		if (!stream_mode || (skb->p_data >= skb->p_data_end)) {
			skb_free(skb);
		}

		if (!stream_mode || (buff_sz == 0)) {
			/* disable reading if needed */
			io_sync_disable(&sk->ios, IO_SYNC_READING);
			if (NULL != skb_queue_front(&sk->rx_queue)) {
				io_sync_enable(&sk->ios, IO_SYNC_READING);
			}

			/* and exit */
			break;
		}
	}

	msg->msg_iov->iov_len = total_len;

	return 0;
}
