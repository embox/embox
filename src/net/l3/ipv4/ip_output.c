/**
 * @file
 * @brief The Internet Protocol (IP) output module.
 *
 * @date 03.12.09
 * @author Nikolay Korotky
 * @author Vladimir Sokolov
 * @author Ilia Vaprol
 */

#include <errno.h>

#include <net/l3/ipv4/ip.h>
#include <net/l4/udp.h>
#include <net/socket/inet_sock.h>
#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/l0/net_tx.h>
#include <net/lib/bootp.h>
#include <net/l3/ipv4/ip_fragment.h>
#include <net/skbuff.h>
#include <net/l3/icmpv4.h>
#include <linux/in.h>
#include <net/netfilter.h>
#include <kernel/printk.h>
#include <net/if_packet.h>
#include <net/if_ether.h>
#include <util/math.h>
#include <embox/net/pack.h>
#include <net/lib/ipv4.h>

static const struct net_pack_out_ops ip_out_ops_struct;
const struct net_pack_out_ops *const ip_out_ops
		= &ip_out_ops_struct;

EMBOX_NET_PACK_OUT(AF_INET, ip_out_ops_struct);

static int ip_xmit(struct sk_buff *skb) {
	int ret;
	in_addr_t daddr;
	struct net_header_info hdr_info;

	assert(skb != NULL);
	assert(skb->nh.iph != NULL);
	daddr = skb->nh.iph->daddr;

	hdr_info.type = ETH_P_IP;
	hdr_info.src_hw = NULL;
	hdr_info.dst_hw = NULL;
	if (ip_is_local(daddr, false, false)) {
		hdr_info.dst_p = NULL;
	}
	else {
		/* get dest ip from route table */
		ret = rt_fib_route_ip(daddr, &daddr);
		if (ret != 0) {
			skb_free(skb);
			return ret;
		}

		hdr_info.dst_p = &daddr;
		hdr_info.p_len = sizeof daddr;
	}

	return net_tx(skb, &hdr_info);
}

/* Fragments skb and sends it to the interface.
 * Returns -1 in case of error
 * As side effect frees incoming skb
 */
static int fragment_skb_and_send(struct sk_buff *skb, struct net_device *dev) {
	int ret;
	struct sk_buff_head tx_buf;
	struct sk_buff *s_tmp;

	skb->dev = dev;
	ret = ip_frag(skb, dev->mtu, &tx_buf);
	if (ret != 0) {
		skb_free(skb);
		return ret;
	}
	skb_free(skb);

	while (NULL != (s_tmp = skb_queue_pop(&tx_buf))) {
		s_tmp->dev = dev;
		ip_set_check_field(s_tmp->nh.iph);
		ret = ip_xmit(s_tmp);
		if (ret != 0) {
			break;
		}
	}
	skb_queue_purge(&tx_buf);
	return ret;
}


int ip_forward(struct sk_buff *skb) {
	iphdr_t *iph = ip_hdr(skb);
	int optlen = IP_HEADER_SIZE(iph) - IP_MIN_HEADER_SIZE;
	struct rt_entry *best_route = rt_fib_get_best(iph->daddr, NULL);

	/* Drop broadcast and multicast addresses of 2 and 3 layers
	 * Note, that some kinds of those addresses we can't get here, because
	 * they processed in other part of code - see ip_is_local(,true,xxx);
	 * And, of course, loopback packets must not be processed here
	 */
	if ((pkt_type(skb) != PACKET_HOST) || ipv4_is_multicast(iph->daddr)) {
		skb_free(skb);
		return 0;
	}

	/* IP Options is a security violation
	 * Try to return packet as close as possible, so check it before ttl processsing (RFC 1812)
	 */
	if (unlikely(optlen)) {
		icmp_send(skb, ICMP_PARAMETERPROB, 0, htonl(IP_MIN_HEADER_SIZE));
		return -1;
	}

	/* Check TTL and decrease it.
	 * We believe that this skb is ours and we can modify it
	 */
	if (unlikely(iph->ttl <= 1)) {
		icmp_send(skb, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, 0);
		return -1;
	}
	iph->ttl--; /* All routes have the same length */
	ip_set_check_field(iph);

	/* Check no route */
	if (!best_route) {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, 0);
		return -1;
	}

	/* Should we send ICMP redirect */
	if (skb->dev == best_route->dev) {
		struct sk_buff *s_new = skb_copy(skb);
		if (s_new) {
			icmp_send(s_new, ICMP_REDIRECT, (best_route->rt_gateway == INADDR_ANY),
					  best_route->rt_gateway);
		}
		/* We can still proceed here */
	}

	if (ip_route(skb, best_route) < 0) {
		/* So we have something like arp problem */
		if (best_route->rt_gateway == INADDR_ANY) {
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, iph->daddr);
		} else {
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, best_route->rt_gateway);
		}
		return -1;
	}

	/* Fragment packet, if it's required */
	if (skb->len > best_route->dev->mtu) {
		if (!(iph->frag_off & htons(IP_DF))) {
			/* We can perform fragmentation */
			return fragment_skb_and_send(skb, best_route->dev);
		} else {
			/* Fragmentation is disabled */
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
					  htonl(best_route->dev->mtu << 16)); /* Support RFC 1191 */
			return -1;
		}
	}

	return ip_xmit(skb);
}

static int ip_make(const struct sock *sk,
		const struct sockaddr *to,
		size_t *data_size, struct sk_buff **out_skb) {
	size_t hdr_size, max_size;
	int ret;
	struct sk_buff *skb;
	struct net_device *dev;
	const struct inet_sock *in_sk;
	const struct sockaddr_in *to_in;
	in_addr_t src_ip, dst_ip;

	assert(data_size != NULL);
	assert((sk != NULL) || (to != NULL));

	in_sk = to_const_inet_sock(sk);
	to_in = (const struct sockaddr_in *)to;

	assert((to_in == NULL) || (to_in->sin_family == AF_INET));

	dst_ip = to_in != NULL ? to_in->sin_addr.s_addr
			: in_sk->dst_in.sin_addr.s_addr;
	ret = rt_fib_out_dev(dst_ip, &in_sk->sk, &dev);
	if (ret != 0) {
		return ret;
	}
	assert(dev != NULL);

	assert(inetdev_get_by_dev(dev) != NULL);
	src_ip = inetdev_get_by_dev(dev)->ifa_address;

	hdr_size = dev->hdr_len + IP_MIN_HEADER_SIZE;
	max_size = min(dev->mtu, skb_max_size());
	if (hdr_size > max_size) {
		return -EMSGSIZE;
	}

	*data_size = min(*data_size, max_size - hdr_size);

	skb = skb_realloc(hdr_size + *data_size, *out_skb);
	if (skb == NULL) {
		return -ENOMEM;
	}

	skb->sk = in_sk != NULL ? &in_sk->sk : NULL;
	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;
	skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE;

	if (in_sk != NULL) {
		ip_build(skb->nh.iph, IP_MIN_HEADER_SIZE + *data_size,
				in_sk->sk.opt.so_protocol, src_ip, dst_ip);
	}

	*out_skb = skb;

	return 0;
}

static int ip_snd(struct sk_buff *skb) {
	static uint16_t global_id = 1230;

	assert(skb != NULL);

	if (0 != nf_test_skb(NF_CHAIN_OUTPUT, NF_TARGET_ACCEPT, skb)) {
		printk("ip_snd: skb %p dropped by netfilter\n", skb);
		skb_free(skb);
		return 0;
	}

	ip_set_id_field(skb->nh.iph, global_id++);
	ip_set_check_field(skb->nh.iph);

	return ip_xmit(skb);
}

static const struct net_pack_out_ops ip_out_ops_struct = {
	.make_pack = &ip_make,
	.snd_pack = &ip_snd
};
