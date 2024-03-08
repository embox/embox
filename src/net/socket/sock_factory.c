/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <hal/ipl.h>
#include <mem/misc/pool.h>
#include <net/sock.h>

#include "family.h"
#include "net_sock.h"

#include <util/err.h>
#include <security/security.h>

static struct sock * sock_alloc(
		const struct sock_family_ops *f_ops,
		const struct sock_proto_ops *p_ops) {
	ipl_t sp;
	struct sock *sk;
	struct proto_sock *p_sk;

	assert(f_ops != NULL);
	assert(p_ops != NULL);

	sp = ipl_save();
	{
		sk = pool_alloc(f_ops->sock_pool);
		if (sk == NULL) {
			ipl_restore(sp);
			log_error("could not alloc sock_family");
			return NULL;
		}

		if (p_ops->sock_pool != NULL) {
			p_sk = pool_alloc(p_ops->sock_pool);
			if (p_sk == NULL) {
				pool_free(f_ops->sock_pool, sk);
				ipl_restore(sp);
				log_error("could not alloc sock_proto");
				return NULL;
			}
		}
		else {
			p_sk = NULL;
		}
	}
	ipl_restore(sp);

	assert(((p_sk == NULL) && (p_ops->sock_pool == NULL))
			|| ((p_sk != NULL) && (p_ops->sock_pool != NULL)));

	sk->p_sk = p_sk;
	if (p_sk != NULL) {
		p_sk->sk = sk;
	}

	return sk;
}

static void sock_free(struct sock *sk) {
	ipl_t sp;

	assert(sk != NULL);
	assert(sk->f_ops != NULL);
	assert(sk->p_ops != NULL);
	assert(((sk->p_sk == NULL) && (sk->p_ops->sock_pool == NULL))
			|| ((sk->p_sk != NULL)
				&& (sk->p_ops->sock_pool != NULL)));

	sp = ipl_save();
	{
		if (sk->p_sk != NULL) {
			pool_free(sk->p_ops->sock_pool, sk->p_sk);
		}
		pool_free(sk->f_ops->sock_pool, sk);
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

extern const struct idesc_ops task_idx_ops_socket;

static void sock_init(struct sock *sk, int family, int type,
		int protocol, const struct sock_family_ops *f_ops,
		const struct sock_proto_ops *p_ops,
		const struct net_pack_out_ops *o_ops) {
	assert(sk != NULL);
	assert(f_ops != NULL);
	assert(p_ops != NULL);

	dlist_head_init(&sk->lnk);
	sock_opt_init(&sk->opt, family, type, protocol);
	skb_queue_init(&sk->rx_queue);
	skb_queue_init(&sk->tx_queue);
	sk->rx_data_len = 0;
	sock_set_state(sk, SS_UNKNOWN);
	sk->shutdown_flag = 0;
	sk->p_sk = sk->p_sk; /* setup in sock_alloc() */
	sk->f_ops = f_ops;
	sk->p_ops = p_ops;
	sk->o_ops = o_ops;
	sk->src_addr = sk->dst_addr = NULL;
	sk->addr_len = 0;
	sk->err = 0;

	idesc_init(&sk->idesc, &task_idx_ops_socket, O_RDWR);
	sock_xattr_init(sk);
	security_sock_create(sk);
}


struct sock *sock_create(int family, int type, int protocol) {
	int ret;
	struct sock *new_sk;
	const struct net_family *nfamily;
	const struct net_family_type *nftype;
	const struct net_sock *nsock;

	nfamily = net_family_lookup(family);
	if (nfamily == NULL) {
		return err2ptr(EAFNOSUPPORT);
	}

	nftype = net_family_type_lookup(nfamily, type);
	if (nftype == NULL) {
		return err2ptr(EPROTOTYPE);
	}

	nsock = net_sock_lookup(family, type, protocol);
	if (nsock == NULL) {
		return err2ptr(EPROTONOSUPPORT);
	}

	new_sk = sock_alloc(nftype->ops, nsock->ops);
	if (new_sk == NULL) {
		log_info("could not alloc type %d prot %d", nftype->type, nsock->protocol);
		return err2ptr(ENOMEM);
	}

	sock_init(new_sk, family, type, nsock->protocol,
			nftype->ops, nsock->ops,
			nfamily->out_ops != NULL ? *nfamily->out_ops : NULL);

	assert(new_sk->f_ops != NULL);
	ret = new_sk->f_ops->init(new_sk);
	if (ret != 0) {
		sock_release(new_sk);
		return err2ptr(-ret);
	}

	assert(new_sk->p_ops != NULL);
	if (new_sk->p_ops->init != NULL) {
		ret = new_sk->p_ops->init(new_sk);
		if (ret != 0) {
			sock_close(new_sk);
			return err2ptr(-ret);
		}
	}

	sock_hash(new_sk);


	return new_sk;
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


