/**
 * @file
 * @brief Implements raw socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <net/l3/ipv4/ip.h>
#include <net/l3/icmpv4.h>
#include <net/skbuff.h>
#include <net/sock.h>
#include <net/socket/inet_sock.h>
#include <net/socket/raw.h>
#include <net/netdevice.h>

#include <lib/libds/dlist.h>

#include <net/net_sock.h>
#include <embox/net/pack.h>

static const struct sock_proto_ops raw_sock_ops_struct;
const struct sock_proto_ops *const raw_sock_ops
		= &raw_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0,
		raw_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_UDP, 0,
		raw_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_TCP, 0,
		raw_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_RAW, 0,
		raw_sock_ops_struct);

static int raw_rcv_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	const struct inet_sock *in_sk;

	in_sk = (const struct inet_sock *)sk;
	assert(in_sk != NULL);
	assert(in_sk->src_in.sin_family == AF_INET);
	assert(in_sk->dst_in.sin_family == AF_INET);

	assert(skb != NULL);
	assert(skb->nh.iph != NULL);

	return ((in_sk->src_in.sin_addr.s_addr == skb->nh.iph->daddr)
				|| (in_sk->src_in.sin_addr.s_addr == INADDR_ANY))
			&& ((in_sk->dst_in.sin_addr.s_addr == skb->nh.iph->saddr)
				|| (in_sk->dst_in.sin_addr.s_addr == INADDR_ANY))
			&& (in_sk->sk.opt.so_protocol == skb->nh.iph->proto)
			&& ((in_sk->sk.opt.so_bindtodevice == skb->dev)
				|| (in_sk->sk.opt.so_bindtodevice == NULL));
}

extern uint16_t skb_get_secure_level(const struct sk_buff *skb);
extern uint16_t sock_get_secure_level(const struct sock *sk);

int raw_rcv(const struct sk_buff *skb) {
	struct sock *sk;
	struct sk_buff *cloned;

	assert(skb != NULL);
	assert(skb->dev != NULL);

	sk = NULL;

	while (1) {

		sk = sock_lookup(sk, raw_sock_ops, raw_rcv_tester, skb);
		if (sk == NULL) {
			break;
		}
		/* if we have socket with secure label we have to check secure level */
		if (sock_get_secure_level(sk) >	skb_get_secure_level(skb)) {
			return 0;
		}

		cloned = skb_clone(skb);
		if (cloned == NULL) {
			return -ENOMEM;
		}

		sock_rcv(sk, cloned, cloned->nh.raw,
				cloned->len - skb->dev->hdr_len);
	}

	return 0;
}

static int raw_err_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	const struct inet_sock *in_sk;
	const struct iphdr *emb_pack_iphdr;

	in_sk = (const struct inet_sock *)sk;
	assert(in_sk != NULL);
	assert(in_sk->src_in.sin_family == AF_INET);
	assert(in_sk->dst_in.sin_family == AF_INET);

	assert(skb != NULL);
	assert(skb->h.raw != NULL);
	emb_pack_iphdr = (const struct iphdr *)(skb->h.raw
			+ IP_HEADER_SIZE(skb->nh.iph) + ICMP_MIN_HEADER_SIZE);

	return (((in_sk->src_in.sin_addr.s_addr == skb->nh.iph->daddr)
					&& (in_sk->src_in.sin_addr.s_addr == emb_pack_iphdr->saddr))
				|| (in_sk->src_in.sin_addr.s_addr == INADDR_ANY))
			&& (((in_sk->dst_in.sin_addr.s_addr == skb->nh.iph->saddr)
					&& (in_sk->dst_in.sin_addr.s_addr == emb_pack_iphdr->daddr))
				|| (in_sk->dst_in.sin_addr.s_addr == INADDR_ANY))
			&& (in_sk->sk.opt.so_protocol == skb->nh.iph->proto)
			&& ((in_sk->sk.opt.so_bindtodevice == skb->dev)
				|| (in_sk->sk.opt.so_bindtodevice == NULL));
}

void raw_err(const struct sk_buff *skb, int error_info) {
	struct sock *sk;

	sk = NULL;

	while (1) {
		sk = sock_lookup(sk, raw_sock_ops, raw_err_tester, skb);
		if (sk == NULL) {
			break;
		}

		sock_set_so_error(sk, error_info);
	}
}

static int raw_sendmsg(struct sock *sk, struct msghdr *msg, int flags) {
	int ret;
	size_t data_len;
	struct sk_buff *skb;
	const struct sockaddr *sockaddr;
	int fill_iphdr = 1;

	assert(sk);
	assert(msg);
	assert(msg->msg_iov);
	assert(msg->msg_iov->iov_base);
	assert(sk->o_ops);
	assert(sk->o_ops->make_pack);

	data_len = msg->msg_iov->iov_len;
	skb = NULL;
	sockaddr = (const struct sockaddr *)msg->msg_name;

	if (sk->opt.so_type == SOCK_RAW) {
		struct inet_sock *inet = to_inet_sock(sk);

		if (sk->opt.so_protocol == IPPROTO_RAW) {
			/* It is Linux specific behaviour. FreeBSD does not set this flag by default
			 * when protocol is IPPROTO_RAW.
			 * See http://sock-raw.org/papers/sock_raw */
			inet->opt.hdrincl = 1;
		}

		if (inet->opt.hdrincl) {
			fill_iphdr = 0;
		}
	}

	if (fill_iphdr) {
		ret = sk->o_ops->make_pack(sk, sockaddr, &data_len, &skb);
	} else {
		size_t actual_data_len;
		if (data_len < sizeof(struct iphdr)) {
			return -EMSGSIZE;
		}
		actual_data_len = data_len - sizeof(struct iphdr);
		ret = sk->o_ops->make_pack(sk, sockaddr, &actual_data_len, &skb);
		data_len = actual_data_len + sizeof(struct iphdr);
	}

	if (ret != 0) {
		return ret;
	}

	if (data_len < msg->msg_iov->iov_len) {
		skb_free(skb);
		return -EMSGSIZE;
	}

	assert(sk);
	assert(skb->h.raw);

	if (fill_iphdr) {
		memcpy(skb->h.raw, msg->msg_iov->iov_base, data_len);
	} else {
		/* Rewrite IP header made by sk->o_ops->make_pack above +
		 * write @c actual_data_len payload */
		memcpy(skb->nh.raw, msg->msg_iov->iov_base, data_len);
	}

	assert(sk->o_ops->snd_pack != NULL);
	ret = sk->o_ops->snd_pack(skb);
	if (0 > ret) {
		return ret;
	}
	return data_len;
}

static DLIST_DEFINE(raw_sock_list);

static const struct sock_proto_ops raw_sock_ops_struct = {
	.sendmsg   = raw_sendmsg,
	.recvmsg   = sock_dgram_recvmsg,
	.sock_list = &raw_sock_list
};
