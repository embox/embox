/**
 * @file
 * @brief Implements raw socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/uio.h>
#include <net/if_ether.h>

#include <net/sock.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/raw.h>
#include <mem/misc/pool.h>

#include <embox/net/sock.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_RAW_SOCK OPTION_GET(NUMBER, amount_raw_sock)

EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_IP, 0, raw_prot);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, raw_prot);

static struct raw_sock *raw_table[MODOPS_AMOUNT_RAW_SOCK];

POOL_DEF(raw_sock_pool, struct raw_sock, MODOPS_AMOUNT_RAW_SOCK);

/* static method for getting hash table index of a socket */
static int _raw_get_hash_idx(struct sock *sk) {
	unsigned int i;

	assert(sk != NULL);

	for (i = 0; i < sizeof raw_table / sizeof raw_table[0]; ++i) {
		if (&raw_table[i]->inet.sk == sk) {
			return i;
		}
	}

	return -1;
}

/* looking up for a socket by index in table, proto, source and dest addresses */
static struct sock *_raw_lookup(unsigned int sk_hash_idx, unsigned char protocol,
		unsigned int saddr, unsigned int daddr, struct net_device *dev) {
	/* socket for iterating */
	struct sock * sk_it;
	struct inet_sock *inet;
	int i;

	for (i = sk_hash_idx; i < sizeof raw_table / sizeof raw_table[0]; ++i) {
		sk_it = &raw_table[i]->inet.sk;
		inet = inet_sk(sk_it);
		/* the socket is being searched for by (daddr, saddr, protocol) */
		if (!(inet->daddr != daddr && inet->daddr) &&
			 !(inet->rcv_saddr != saddr && inet->rcv_saddr) &&
			 /* sk_it->sk_bound_dev_if struct sock doesn't have device binding? */
			 sk_it->opt.so_protocol == protocol) {
			return sk_it;
		}
	}

	return NULL;
}

#if 0
static struct sock *raw_lookup(__u8 proto) {
	struct sock *sk;
	size_t i;
	for (i = 0; i < sizeof raw_hash / sizeof raw_hash[0]; i++) {
		sk = (struct sock*) raw_hash[i];
		if (sk && sk->sk_protocol == proto) {
			return sk;
		}
	}
	return NULL;
}
#endif

int raw_rcv(struct sk_buff *skb) {
	size_t i;
	struct sock *sk;
	struct sk_buff *cloned;

	assert(skb != NULL);

	for (i = 0; i < sizeof raw_table / sizeof raw_table[0]; ++i) {
		sk = (struct sock *)raw_table[i];
		if ((sk != NULL) && (sk->opt.so_protocol == skb->nh.iph->proto)) {
			cloned = skb_share(skb, SKB_SHARE_DATA); // TODO without skb_clone()
			if (cloned == NULL) {
				continue;
				//return -ENOMEM;
			}
			sock_rcv(sk, cloned);
		}
	}

	return ENOERR;
}

void raw_err(struct sk_buff *skb, uint32_t info) {
	struct sock *sk = NULL;
	int sk_idx = 0;
	struct iphdr *emb_pack_iphdr;

	emb_pack_iphdr = (struct iphdr *)(skb->h.raw + IP_HEADER_SIZE(skb->nh.iph) + ICMP_HEADER_SIZE);

	/* notify all sockets matching source, dest address and protocol */
	do {
		sk = _raw_lookup(sk_idx, emb_pack_iphdr->proto,
				emb_pack_iphdr->saddr, emb_pack_iphdr->daddr, skb->dev);
		if (sk != NULL) {  /* notify socket about an error */
			ip_v4_icmp_err_notify(sk, skb->h.icmph->type,
					skb->h.icmph->code);
			/* do something else - specific for raw sockets ? */
			sk_idx = _raw_get_hash_idx(sk) + 1;
		}
	} while(sk != NULL);
}

static void raw_hash(struct sock *sk) {
	size_t i;

	for (i = 0; i < sizeof raw_table / sizeof raw_table[0]; ++i) {
		if (raw_table[i] == NULL) {
			raw_table[i] = (struct raw_sock *) sk;
			break;
		}
	}
}

static void raw_unhash(struct sock *sk) {
	size_t i;

	for (i = 0; i < sizeof raw_table / sizeof raw_table[0]; ++i) {
		if (raw_table[i] == (struct raw_sock *)sk) {
			raw_table[i] = NULL;
			break;
		}
	}
}

static int raw_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct inet_sock *inet = inet_sk(sk);
	size_t len = msg->msg_iov->iov_len;
	sk_buff_t *skb = skb_alloc(ETH_HEADER_SIZE + len);

	assert(skb);

	memcpy((void*)((unsigned int)(skb->mac.raw + ETH_HEADER_SIZE)),
					(void*) msg->msg_iov->iov_base, len);
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

		/* Correct until somebody sends:
		 *	IP packet with options
		 *	already fragmented IP packet
		 * Probably we don't need this pointer in later code
		 */
	skb->h.raw = skb->mac.raw + ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE;// + inet->opt->optlen;

	skb->sk = sk;
	ip_send_packet(inet, skb);
	return 0;
}

static int raw_recvmsg(struct sock *sk, struct msghdr *msg, int flags) {
	struct sk_buff *skb;
	size_t len = msg->msg_iov->iov_len;

	skb = skb_queue_front(&sk->rx_queue);
	if (skb && (skb->len > 0)) {
		if (len > (skb->len - ETH_HEADER_SIZE)) {
			len = skb->len - ETH_HEADER_SIZE;
		}
		memcpy((void*) msg->msg_iov->iov_base,
				(void*) (skb->mac.raw + ETH_HEADER_SIZE), len);
		skb_free(skb);
	} else {
		len = 0;
	}

	msg->msg_iov->iov_len = len;

	return 0;
}

static const struct sock_ops raw_prot = {
	.sendmsg = raw_sendmsg,
	.recvmsg = raw_recvmsg,
	.hash = raw_hash,
	.unhash = raw_unhash,
	.obj_pool   = &raw_sock_pool
};
