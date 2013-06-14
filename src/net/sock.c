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
#include <kernel/event.h>
#include <kernel/task/io_sync.h>
#include <mem/misc/pool.h>
#include <net/sock.h>
#include <net/skbuff.h>
#include <string.h>

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

	sock_opt_init(&sk->opt, family, type, protocol);
	skb_queue_init(&sk->rx_queue);
	skb_queue_init(&sk->tx_queue);
	sk->state = 0;
	sk->shutdown_flag = 0;
	sk->f_ops = f_ops;
	sk->ops = ops;
	sk->sk_socket = NULL;
	sk->sk_encap_rcv = NULL;
	event_init(&sk->sock_is_not_empty, "sock_is_not_empty");
}

int sock_create_ext(int family, int type, int protocol,
		struct sock **out_sk) {
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

	sock_init(new_sk, family, type, protocol, nftype->ops,
			nsock->ops);

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

	*out_sk = new_sk;

	return 0;
}

int sock_create(int family, int type, int protocol,
		struct sock **out_sk) {
	int ret;
	struct sock *new_sk;

	if (out_sk == NULL) {
		return -EINVAL;
	}

	ret = sock_create_ext(family, type, protocol, &new_sk);
	if (ret != 0) {
		return ret;
	}
	assert(new_sk != NULL);

	sock_hash(new_sk);

	*out_sk = new_sk;

	return 0;
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
	struct sock **iter;

	if (sk == NULL) {
		return; /* error: invalid argument */
	}

	assert(sk->ops != NULL);
	if (sk->ops->sock_table == NULL) {
		return; /* error: not supported */
	}

	for (iter = sk->ops->sock_table;
			iter < sk->ops->sock_table + sk->ops->sock_table_sz;
			++iter) {
		if (*iter == NULL) {
			*iter = sk;
			break;
		}
	}

	/* error: no memory */
}

void sock_unhash(struct sock *sk) {
	struct sock **iter;

	if (sk == NULL) {
		return; /* error: invalid argument */
	}

	assert(sk->ops != NULL);
	if (sk->ops->sock_table == NULL) {
		return; /* error: not supported */
	}

	for (iter = sk->ops->sock_table;
			iter < sk->ops->sock_table + sk->ops->sock_table_sz;
			++iter) {
		if (*iter == sk) {
			*iter = NULL;
			break;
		}
	}

	/* error: not such entity */
}

struct sock * sock_iter(const struct sock_ops *ops) {
	if (ops == NULL) {
		return NULL; /* error: invalid argument */
	}
	else if (ops->sock_table == NULL) {
		return NULL; /* error: not supported */
	}

	return *ops->sock_table;
}

struct sock * sock_next(const struct sock *sk) {
	struct sock **iter;

	if (sk == NULL) {
		return NULL; /* error: invalid argument */
	}

	assert(sk->ops != NULL);
	if (sk->ops->sock_table == NULL) {
		return NULL; /* error: not supported */
	}

	for (iter = sk->ops->sock_table;
			iter < sk->ops->sock_table + sk->ops->sock_table_sz;
			++iter) {
		if (*iter == sk) {
			break;
		}
	}

	while (++iter < sk->ops->sock_table + sk->ops->sock_table_sz) {
		if (*iter != NULL) {
			return *iter;
		}
	}

	return NULL;
}

void sock_rcv(struct sock *sk, struct sk_buff *skb) {
	if ((sk == NULL) || (skb == NULL)) {
		return; /* error: invalid argument */
	}

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		skb_free(skb);
		return; /* error: socket is down */
	}

	skb_queue_push(&sk->rx_queue, skb);
	event_notify(&sk->sock_is_not_empty);

	if (sk->sk_socket != NULL) {
		idx_io_enable(sk->sk_socket->desc_data, IDX_IO_READING);
	}
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
