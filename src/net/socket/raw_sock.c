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


#include <net/protocol.h>
#include <net/sock.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/raw.h>
#include <net/inet_common.h>

#include <embox/net/sock.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_RAW_SOCK OPTION_GET(NUMBER, amount_raw_sock)

static struct proto raw_prot;

EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_IP, raw_prot, inet_raw_ops, 0, true);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_ICMP, raw_prot, inet_raw_ops, 0, false);

static struct raw_sock *raw_table[MODOPS_AMOUNT_RAW_SOCK];

static int raw_rcv_skb(struct sock *sk, struct sk_buff *skb);

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
			 sk_it->sk_protocol == protocol) {
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
	int res;
	size_t i;
	struct sock *sk;
	struct sk_buff *cloned;

	assert(skb != NULL);

	for (i = 0; i < sizeof raw_table / sizeof raw_table[0]; ++i) {
		sk = (struct sock *)raw_table[i];
		if ((sk != NULL) && (sk->sk_protocol == skb->nh.iph->proto)) {
			cloned = skb_duplicate(skb); // TODO without skb_clone()
			if (cloned == NULL) {
				continue;
				//return -ENOMEM;
			}
			res = raw_rcv_skb(sk, cloned);
			if (res < 0) {
				skb_free(cloned);
			}
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

static int raw_init_sock(struct sock *sk) {
	return ENOERR;
}

static void raw_close(struct sock *sk, long timeout) {
	sk_common_release(sk);
}

static int raw_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	sock_queue_rcv_skb(sk, skb);
	return ENOERR;
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

static int raw_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		size_t len) {
	struct inet_sock *inet = inet_sk(sk);
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

static int raw_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags) {
	struct sk_buff *skb;

	skb = skb_queue_front(sk->sk_receive_queue);
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

static int raw_bind(struct sock *sk, struct sockaddr *uaddr, int addr_len) {
	struct inet_sock *inet = inet_sk(sk);
	struct sockaddr_in *addr = (struct sockaddr_in *)uaddr;
	inet->rcv_saddr = inet->saddr = addr->sin_addr.s_addr;
	return ENOERR;
}

static int raw_setsockopt(struct sock *sk, int level, int optname,
			char *optval, int optlen) {
	return ENOERR;
}

static int raw_getsockopt(struct sock *sk, int level, int optname,
			char *optval, int *optlen) {
	return ENOERR;
}

static int raw_ioctl(struct sock *sk, int cmd, unsigned long arg) {
	return ENOERR;
}

int raw_connect(struct sock *sk,
				struct sockaddr *uaddr, int addr_len) {
	return ENOERR;
}

static struct proto raw_prot = {
	.name = "RAW",
	.init = raw_init_sock,
	.bind = raw_bind,
	.connect = raw_connect,
	.setsockopt = raw_setsockopt,
	.getsockopt = raw_getsockopt,
	.sendmsg = raw_sendmsg,
	.recvmsg = raw_recvmsg,
	.close = raw_close,
//	.disconnect = udp_disconnect,
	.ioctl = raw_ioctl,
	.backlog_rcv = raw_rcv_skb,
	.hash = raw_hash,
	.unhash = raw_unhash,
	.obj_size = sizeof(struct raw_sock),
};
