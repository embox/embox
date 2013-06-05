/**
 * @file
 * @brief Generic socket support routines.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

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
		sk = pool_alloc(ops->obj_pool);
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
		pool_free(sk->ops->obj_pool, sk);
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

int sock_create(int family, int type, int protocol,
		const struct sock_ops *ops, struct sock **out_sk) {
	int ret;
	struct sock *new_sk;

	if ((ops == NULL) || (out_sk == NULL)) {
		return -EINVAL;
	}

	new_sk = sock_alloc(ops);
	if (new_sk == NULL) {
		return -ENOMEM;
	}

	sock_opt_init(&new_sk->opt, family, type, protocol);
	skb_queue_init(&new_sk->rx_queue);
	skb_queue_init(&new_sk->tx_queue);
	new_sk->state = 0;
	new_sk->shutdown_flag = 0;
	new_sk->ops = ops;
	new_sk->sk_socket = NULL;
	new_sk->sk_encap_rcv = NULL;
	event_init(&new_sk->sock_is_not_empty, "sock_is_not_empty");

	if (ops->init != NULL) {
		ret = ops->init(new_sk);
		if (ret != 0) {
			sock_free(new_sk);
			return ret;
		}
	}

	if (ops->hash != NULL) {
		ops->hash(new_sk);
	}

	*out_sk = new_sk;

	return 0;
}

void sock_release(struct sock *sk) {
	if (sk == NULL) {
		return; /* error: invalid argument */
	}

	assert(sk->ops != NULL);
	if (sk->ops->unhash != NULL) {
		sk->ops->unhash(sk);
	}

	skb_queue_purge(&sk->rx_queue);
	skb_queue_purge(&sk->tx_queue);
	sock_free(sk);
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
