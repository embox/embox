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

#include <util/math.h>

#include <net/netdevice.h>
#include <net/inetdevice.h>

#include <net/l3/ipv4/ip.h>
#include <net/l4/udp.h>
#include <net/socket/inet_sock.h>

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
#include <net/l2/ethernet.h>

#include <embox/net/pack.h>
#include <net/lib/ipv4.h>
#include <net/l3/arp.h>

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

	hdr_info.type = ETH_P_IP;
	hdr_info.dst_hw = NULL;
	hdr_info.src_hw = skb->dev->dev_addr;

	/* it's loopback/local or broadcast address? */
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	if (ip_is_local_net_ns(skb->nh.iph->daddr, IP_LOCAL_BROADCAST,
				skb->dev->net_ns)) {
#else
	if (ip_is_local(skb->nh.iph->daddr, IP_LOCAL_BROADCAST)) {
#endif
		hdr_info.dst_p = NULL;
	}
	else {
		/* get dest ip from route table */
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
		ret = rt_fib_route_ip_net_ns(skb->nh.iph->daddr, &daddr,
					skb->dev->net_ns);
#else
		ret = rt_fib_route_ip(skb->nh.iph->daddr, &daddr);
#endif
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
	struct rt_entry *best_route;

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	best_route = rt_fib_get_best_net_ns(iph->daddr,
						NULL, skb->dev->net_ns);
#else
	best_route = rt_fib_get_best(iph->daddr, NULL);
#endif

	if (skb->dev->type != ARP_HRD_NONE)
	{
		/* Drop broadcast and multicast addresses of 2 and 3 layers
		 * Note, that some kinds of those addresses we can't get here, because
		 * they processed in other part of code - see ip_is_local(,true,xxx);
		 * And, of course, loopback packets must not be processed here
		 */
		if ((pkt_type(skb) != PACKET_HOST) || ipv4_is_multicast(iph->daddr)) {
			skb_free(skb);
			return 0;
		}
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
				in_addr_t daddr = iph->daddr; /* Extract addr to avoid unaligned pointer dereference */
				icmp_discard(s_new, ICMP_REDIRECT,
						ICMP_HOST_REDIRECT, &daddr);
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

static in_addr_t ip_get_dest_addr(const struct inet_sock *in_sk,
		const struct sockaddr *to,
		struct sk_buff **out_skb) {

	const struct sockaddr_in *to_in;

	to_in = (const struct sockaddr_in *)to;

	if (to_in != NULL) {
		return to_in->sin_addr.s_addr;
	} else if (in_sk != NULL) {
		return in_sk->dst_in.sin_addr.s_addr;
	} else {
		return (*out_skb)->nh.iph->saddr; /* make a reply */
	}
}

static int ip_make(const struct sock *sk,
		const struct sockaddr *to,
		size_t *data_size,
		struct sk_buff **out_skb) {

	size_t hdr_size, max_size;
	struct sk_buff *skb;
	struct net_device *dev;
	const struct inet_sock *in_sk;
	uint8_t proto;
	in_addr_t src_ip, dst_ip;
	int ret;
	int ip_length;

	assert(out_skb);
	assert(sk || *out_skb);
	assert(data_size);

	assert((to == NULL) || (to->sa_family == AF_INET));

	in_sk = to_const_inet_sock(sk);

	dst_ip = ip_get_dest_addr(in_sk, to, out_skb);

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	net_namespace_p net_ns;
	fill_net_ns_from_sk(net_ns, sk, out_skb);
	ret = rt_fib_out_dev_net_ns(dst_ip, in_sk != NULL ? &in_sk->sk : NULL,
			&dev, net_ns);
#else
	if (*out_skb && (*out_skb)->dev->type == ARP_HRD_NONE)
	{
		dev = (*out_skb)->dev;
		dst_ip =(*out_skb)->nh.iph->daddr;
		ret = 0;
	}
	else
	{
		ret = rt_fib_out_dev(dst_ip, in_sk != NULL ? &in_sk->sk : NULL, &dev);
	}
#endif
	if (ret != 0) {
		DBG(printk("ip_make: unknown device for %s\n",
					inet_ntoa(*(struct in_addr *)&dst_ip)));
		return ret;
	}
	assert(dev != NULL);

	assert(inetdev_get_by_dev(dev) != NULL);
	//src_ip = inetdev_get_by_dev(dev)->ifa_address; /* TODO it's better! */
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	ret = rt_fib_source_ip_net_ns(dst_ip, dev, &src_ip, dev->net_ns);
#else
	ret = rt_fib_source_ip(dst_ip, dev, &src_ip);
#endif
	if (ret != 0) {
		DBG(printk("ip_make: can't resolve source ip for %s\n",
					inet_ntoa(*(struct in_addr *)&dst_ip)));
		return ret;
	}

	proto = in_sk != NULL ? in_sk->sk.opt.so_protocol
			: (*out_skb)->nh.iph->proto;

	if (sk) {
		ip_length = ip_header_size((struct sock *)sk);
	} else {
		ip_length = IP_MIN_HEADER_SIZE;
	}


	hdr_size = dev->hdr_len + ip_length;
	max_size = min(dev->mtu, skb_max_size());
	if (hdr_size > max_size) {
		DBG(printk("ip_make: hdr_size %zu is too big (max %zu)\n",
					hdr_size, max_size));
		return -EMSGSIZE;
	}

#if 0
	/* TODO ip fragmentation accepts packets size <= 64 KB,
	 * so it would be something like this:
	 * *data_size = min(*data_size, 64000 - hdr_size); */
	*data_size = min(*data_size, max_size - hdr_size);
#endif

	skb = skb_realloc(hdr_size + *data_size, *out_skb);
	if (skb == NULL) {
		DBG(printk("ip_make: can't realloc packet for size %zu\n",
					hdr_size + *data_size));
		return -ENOMEM;
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;
	skb->h.raw = skb->nh.raw + ip_length;

	ip_build(skb->nh.iph, ip_length + *data_size,
			64, proto, src_ip, dst_ip);
	if (IP_MIN_HEADER_SIZE < ip_length) {
		ip_header_make_secure((struct sock *)sk, skb);
	}

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

	if (skb->len > skb->dev->mtu) {
		if (!(skb->nh.iph->frag_off & htons(IP_DF))) {
			return fragment_skb_and_send(skb, skb->dev);
		}
	}

	return ip_xmit(skb);
}

static const struct net_pack_out_ops ip_out_ops_struct = {
	.make_pack = &ip_make,
	.snd_pack = &ip_snd
};
