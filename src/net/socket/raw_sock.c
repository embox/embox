/**
 * @file
 * @brief Implements raw socket function.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/net/sock.h>
#include <errno.h>
#include <net/l3/ipv4/ip.h>
#include <net/l3/icmpv4.h>
#include <net/skbuff.h>
#include <net/sock.h>
#include <net/socket/inet_sock.h>
#include <net/socket/raw.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <util/dlist.h>

static const struct sock_proto_ops raw_sock_ops_struct;
const struct sock_proto_ops *const raw_sock_ops
		= &raw_sock_ops_struct;

EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0,
		raw_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_UDP, 0,
		raw_sock_ops_struct);
EMBOX_NET_SOCK(AF_INET, SOCK_RAW, IPPROTO_TCP, 0,
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

	assert(sk);
	assert(msg);
	assert(msg->msg_iov);
	assert(msg->msg_iov->iov_base);
	assert(sk->o_ops);
	assert(sk->o_ops->make_pack);

	data_len = msg->msg_iov->iov_len;
	skb = NULL;
	sockaddr = (const struct sockaddr *)msg->msg_name;

	ret = sk->o_ops->make_pack(sk, sockaddr, &data_len, &skb);

	if (ret != 0) {
		return ret;
	}

	if (data_len < msg->msg_iov->iov_len) {
		skb_free(skb);
		return -EMSGSIZE;
	}

	assert(sk);
	assert(skb->h.raw);

	memcpy(skb->h.raw, msg->msg_iov->iov_base, data_len);

	assert(sk->o_ops->snd_pack != NULL);
	return sk->o_ops->snd_pack(skb);
}

static DLIST_DEFINE(raw_sock_list);

static const struct sock_proto_ops raw_sock_ops_struct = {
	.sendmsg   = raw_sendmsg,
	.recvmsg   = sock_nonstream_recvmsg,
	.sock_list = &raw_sock_list
};
