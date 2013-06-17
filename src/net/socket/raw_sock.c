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
#include <util/array.h>
#include <util/list.h>

#include <embox/net/sock.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_RAW_SOCK OPTION_GET(NUMBER, amount_raw_sock)

static const struct sock_ops raw_sock_ops_struct;
const struct sock_ops *const raw_sock_ops = &raw_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_IP, 0, raw_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, raw_sock_ops_struct);

static int raw_rcv_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->nh.iph != NULL);
	return sk->opt.so_protocol == skb->nh.iph->proto;
}

int raw_rcv(struct sk_buff *skb) {
	struct sock *sk;
	struct sk_buff *cloned;

	sk = NULL;

	while (1) {
		sk = sock_lookup(sk, raw_sock_ops, raw_rcv_tester, skb);
		if (sk == NULL) {
			break;
		}

		cloned = skb_share(skb, SKB_SHARE_DATA); // TODO without skb_clone()
		if (cloned == NULL) {
			continue;
			//return -ENOMEM;
		}

		sock_rcv(sk, cloned);
	}

	return 0;
}

static int raw_err_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	const struct inet_sock *inet_sk;
	const struct iphdr *emb_pack_iphdr;

	inet_sk = (const struct inet_sock *)sk;
	assert(inet_sk != NULL);

	assert(skb != NULL);
	assert(skb->h.raw != NULL);
	emb_pack_iphdr = (const struct iphdr *)(skb->h.raw
			+ IP_HEADER_SIZE(skb->nh.iph) + ICMP_HEADER_SIZE);

	return !(inet_sk->daddr != emb_pack_iphdr->saddr && inet_sk->daddr)
			&& !(inet_sk->rcv_saddr != emb_pack_iphdr->daddr && inet_sk->rcv_saddr)
			/* sk_it->sk_bound_dev_if struct sock doesn't have device binding? */
			&& sk->opt.so_protocol == emb_pack_iphdr->proto;
}

void raw_err(struct sk_buff *skb, uint32_t info) {
	struct sock *sk;

	sk = NULL;

	/* notify all sockets matching source, dest address and protocol */
	while (1) {
		sk = sock_lookup(sk, raw_sock_ops, raw_err_tester, skb);
		if (sk == NULL) {
			break;
		}

		/* notify socket about an error */
		ip_v4_icmp_err_notify(sk, skb->h.icmph->type,
				skb->h.icmph->code);
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

POOL_DEF(raw_sock_pool, struct raw_sock, MODOPS_AMOUNT_RAW_SOCK);
static LIST_DEF(raw_sock_list);

static const struct sock_ops raw_sock_ops_struct = {
	.sendmsg   = raw_sendmsg,
	.recvmsg   = raw_recvmsg,
	.sock_pool = &raw_sock_pool,
	.sock_list = &raw_sock_list
};
