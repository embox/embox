/**
 * @file
 * @brief
 *
 * @date 27.05.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <net/l0/net_tx.h>
#include <net/l3/icmpv6.h>
#include <net/l3/ipv6.h>
#include <net/l2/ethernet.h>
#include <net/lib/icmpv6.h>
#include <net/lib/ipv6.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <string.h>

static int ndp_xmit(struct sk_buff *skb,
		struct net_device *dev) {
	struct net_header_info hdr_info;

	assert(skb != NULL);
	assert(dev != NULL);

	hdr_info.type = ETH_P_IPV6;
	hdr_info.src_hw = dev->dev_addr;
	hdr_info.dst_hw = dev->broadcast;

	return net_tx(skb, &hdr_info);
}

int ndp_send(uint8_t type, uint8_t code, const void *body,
		size_t body_sz, struct net_device *dev) {
	struct sk_buff *skb;
	struct in_device *in_dev;
	struct in6_addr dst_ip6;

	assert(dev != NULL);

	skb = skb_alloc(dev->hdr_len + IP6_HEADER_SIZE
			+ ICMP6_MIN_HEADER_SIZE + body_sz);
	if (skb == NULL) {
		return -ENOMEM;
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;
	skb->h.raw = skb->nh.raw + IP6_HEADER_SIZE;

	in_dev = inetdev_get_by_dev(dev);
	assert(in_dev != NULL);

	inet_pton(AF_INET6, "ff02::1:ff02:10", &dst_ip6);
	ip6_build(skb->nh.ip6h, ICMP6_MIN_HEADER_SIZE + body_sz,
			IPPROTO_ICMPV6, 255, &in_dev->ifa6_address, &dst_ip6);

	icmp6_build(skb->h.icmp6h, type, code, body, body_sz);
	icmp6_set_check_field(skb->h.icmp6h, skb->nh.ip6h);

	return ndp_xmit(skb, dev);
}
