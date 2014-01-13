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

#define IP_DEBUG 0
#if IP_DEBUG
#include <arpa/inet.h>
#include <kernel/printk.h>
#define DBG(x) x
#else
#define DBG(x)
#endif

static const struct net_pack_out_ops ip_out_ops_struct;
const struct net_pack_out_ops *const ip_out_ops
		= &ip_out_ops_struct;

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

	/* it's loopback/local or broadcast address? */
	if (ip_is_local(daddr, true, false)) {
		hdr_info.dst_p = NULL;
	}
	else {
		/* get dest ip from route table */
		ret = rt_fib_route_ip(daddr, &daddr);
		if (ret != 0) {
			DBG(printk("ip_xmit: unknown target for %s\n",
						inet_ntoa(*(struct in_addr *)&daddr)));
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
		icmp_discard(skb, ICMP_PARAM_PROB, ICMP_PTR_ERROR,
				(uint8_t)IP_MIN_HEADER_SIZE);
		return -1;
	}

	/* Check TTL and decrease it.
	 * We believe that this skb is ours and we can modify it
	 */
	if (unlikely(iph->ttl <= 1)) {
		icmp_discard(skb, ICMP_TIME_EXCEED, ICMP_TTL_EXCEED);
		return -1;
	}
	iph->ttl--; /* All routes have the same length */
	ip_set_check_field(iph);

	/* Check no route */
	if (!best_route) {
		icmp_discard(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH);
		return -1;
	}

	/* Should we send ICMP redirect */
	if (skb->dev == best_route->dev) {
		struct sk_buff *s_new = skb_copy(skb);
		if (s_new) {
			if (best_route->rt_gateway == INADDR_ANY) {
				icmp_discard(s_new, ICMP_REDIRECT,
						ICMP_HOST_REDIRECT, &iph->daddr);
			}
			else {
				icmp_discard(s_new, ICMP_REDIRECT,
						ICMP_NET_REDIRECT, best_route->rt_gateway);
			}
		}
		/* We can still proceed here */
	}

	if (ip_route(skb, NULL, best_route) < 0) {
		/* So we have something like arp problem */
		if (best_route->rt_gateway == INADDR_ANY) {
			icmp_discard(skb, ICMP_DEST_UNREACH, ICMP_HOST_UNREACH);
		} else {
			icmp_discard(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH);
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
			icmp_discard(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
					  (uint16_t)best_route->dev->mtu); /* Support RFC 1191 */
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
	uint8_t proto;
	in_addr_t src_ip, dst_ip;

	assert((to == NULL) || (to->sa_family == AF_INET));
	assert(data_size != NULL);
	assert(out_skb != NULL);
	assert((sk != NULL) || (*out_skb != NULL));

	in_sk = to_const_inet_sock(sk);
	to_in = (const struct sockaddr_in *)to;

	dst_ip = to_in != NULL ? to_in->sin_addr.s_addr
			: in_sk != NULL ? in_sk->dst_in.sin_addr.s_addr
			: (*out_skb)->nh.iph->saddr; /* make a reply */
	ret = rt_fib_out_dev(dst_ip, in_sk != NULL ? &in_sk->sk : NULL,
			&dev);
	if (ret != 0) {
		DBG(printk("ip_make: unknown device for %s\n",
					inet_ntoa(*(struct in_addr *)&dst_ip)));
		return ret;
	}
	assert(dev != NULL);

	assert(inetdev_get_by_dev(dev) != NULL);
	src_ip = inetdev_get_by_dev(dev)->ifa_address;

	proto = in_sk != NULL ? in_sk->sk.opt.so_protocol
			: (*out_skb)->nh.iph->proto;

	hdr_size = dev->hdr_len + IP_MIN_HEADER_SIZE;
	max_size = min(dev->mtu, skb_max_size());
	if (hdr_size > max_size) {
		DBG(printk("ip_make: hdr_size %zu is too big (max %zu)\n",
					hdr_size, max_size));
		return -EMSGSIZE;
	}

	*data_size = min(*data_size, max_size - hdr_size);

	skb = skb_realloc(hdr_size + *data_size, *out_skb);
	if (skb == NULL) {
		DBG(printk("ip_make: can't realloc packet for size %zu\n",
					hdr_size + *data_size));
		return -ENOMEM;
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;
	skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE;

	ip_build(skb->nh.iph, IP_MIN_HEADER_SIZE + *data_size,
			proto, src_ip, dst_ip);

	*out_skb = skb;

	return 0;
}

static int ip_snd(struct sk_buff *skb) {
	static uint16_t global_id = 1230;

	assert(skb != NULL);

	if (0 != nf_test_skb(NF_CHAIN_OUTPUT, NF_TARGET_ACCEPT, skb)) {
		DBG(printk("ip_snd: dropped by output netfilter\n"));
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
