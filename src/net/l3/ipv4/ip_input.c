/**
 * @file
 * @brief The Internet Protocol (IP) module.
 *
 * @date 17.03.09
 * @author Alexandr Batyukov
 * @author Nikolay Korotky
 * @author Vladimir Sokolov
 */

#include <string.h>
#include <errno.h>
#include <embox/net/pack.h>
#include <net/l3/ipv4/ip.h>
#include <net/l3/icmpv4.h>
#include <net/l4/udp.h>
#include <net/socket/raw.h>
#include <net/socket/inet_sock.h>
#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/util/checksum.h>
#include <framework/net/proto/api.h>
#include <net/l3/ipv4/ip_fragment.h>
#include <net/netfilter.h>
#include <net/neighbour.h>
#include <net/if_ether.h>

EMBOX_NET_PACK(ETH_P_IP, ip_rcv);

static int ip_rcv(struct sk_buff *skb, struct net_device *dev) {
	net_device_stats_t *stats = &dev->stats;
	const struct net_proto *nproto;
	iphdr_t *iph = ip_hdr(skb);
	unsigned short tmp;
	unsigned int len;
	int optlen;
	sk_buff_t *complete_skb;

	/**
	 *   RFC1122: 3.1.2.2 MUST silently discard any IP frame that fails the checksum.
	 *   Is the datagram acceptable?
	 *   1.  Length at least the size of an ip header
	 *   2.  Version of 4
	 *   3.  Checksums correctly. [Speed optimisation for later, skip loopback checksums]
	 *   4.  Doesn't have a bogus length
	 */
	if (iph->ihl < 5 || iph->version != 4) {
		//LOG_ERROR("invalid IPv4 header\n");
		stats->rx_err++;
		skb_free(skb);
		return 0; /* error: invalid hdr */
	}

	len = ntohs(iph->tot_len);
	if (skb->len < len || len < IP_HEADER_SIZE(iph)) {
		//LOG_ERROR("invalid IPv4 header length\n");
		stats->rx_length_errors++;
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	tmp = iph->check;
	iph->check = 0;
	if (tmp != ptclbsum(iph, IP_HEADER_SIZE(iph))) {
		//LOG_ERROR("bad ip checksum\n");
		stats->rx_crc_errors++;
		skb_free(skb);
		return 0; /* error: invalid crc */
	}

	/* Setup transport layer header */
	skb->h.raw = skb->nh.raw + IP_HEADER_SIZE(iph);

	/* Validating */
	if (0 != nf_test_skb(NF_CHAIN_INPUT, NF_TARGET_ACCEPT, skb)) {
		stats->rx_dropped++;
		skb_free(skb);
		return 0; /* error: dropped */
	}

	/* Forwarding */
	assert(skb->dev);
	assert(inetdev_get_by_dev(skb->dev));
	if (inetdev_get_by_dev(skb->dev)->ifa_address != 0) {
		/**
		 * FIXME
		 * This check needed for BOOTP protocol
		 * disable forwarding if interface is not set yet
		 */
		/**
		 * Check the destination address, and if it doesn't match
		 * any of own addresses, retransmit packet according to the routing table.
		 */
		if (!ip_is_local(iph->daddr, true, false)) {
			if (0 != nf_test_skb(NF_CHAIN_FORWARD, NF_TARGET_ACCEPT, skb)) {
				stats->rx_dropped++;
				skb_free(skb);
				return 0; /* error: dropped */
			}
			return ip_forward_packet(skb);
		}
	}

	optlen = IP_HEADER_SIZE(iph) - IP_MIN_HEADER_SIZE;
	if (optlen > 0) {
		/* NOTE : maybe it'd be better to copy skb here,
		 * 'cause options may cause modifications
		 * but smart people who wrote linux kernel
		 * say that this is extremely rarely needed
		 */
		ip_options_t *opts = (ip_options_t*)(skb->cb);

		memset(skb->cb, 0, sizeof(skb->cb));
		opts->optlen = optlen;
		if (ip_options_compile(skb, opts)) {
			stats->rx_err++;
			skb_free(skb);
			return 0; /* error: bad ops */
		}
		if (ip_options_handle_srr(skb)) {
			stats->tx_err++;
			skb_free(skb);
			return 0; /* error: can't handle ops */
		}
	}

	/* It's very useful for us to have complete packet even for forwarding
	 * (we may apply any filter, we may perform NAT etc),
	 * but it'll break routing if different parts of a fragmented
	 * packet will use different routes. So they can't be assembled.
	 * See RFC 1812 for details
	 */
	if ((complete_skb = ip_defrag(skb)) == NULL) {
		if (skb == NULL) {
			return 0; /* error: */
		}
		return 0;
	} else {
		skb = complete_skb;
		iph = ip_hdr(complete_skb);
	}

	/* neighbour can be remembered here to prevent discovery on reply
 	 * (if any)
	 */
	neighbour_add(skb->mac.ethh->h_source, ETH_ALEN,
		(void *) &skb->nh.iph->saddr, IP_ADDR_LEN, skb->dev, 0);

	/* When a packet is received, it is passed to any raw sockets
	 * which have been bound to its protocol or to socket with concrete protocol */
	raw_rcv(skb);

	nproto = net_proto_lookup(iph->proto);
	if (nproto != NULL) {
		return nproto->handle(skb);
	}

	skb_free(skb);
	return 0; /* error: nobody wants this packet */
}
