/**
 * @file
 * @brief The Internet Protocol (IP) module.
 *
 * @date 17.03.09
 * @author Alexandr Batyukov
 * @author Nikolay Korotky
 * @author Vladimir Sokolov
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include <util/log.h>

#include <net/l3/ipv4/ip.h>
#include <net/l3/ipv4/ip_options.h>
#include <net/l3/icmpv4.h>
#include <net/l4/udp.h>
#include <net/socket/raw.h>
#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/l3/ipv4/ip_fragment.h>
#include <net/netfilter.h>
#include <net/l2/ethernet.h>
#include <net/lib/ipv4.h>

#include <embox/net/proto.h>
#include <embox/net/pack.h>

EMBOX_NET_PACK(ETH_P_IP, ip_rcv);

extern int ip_forward(struct sk_buff *skb);

static int ip_rcv(struct sk_buff *skb, struct net_device *dev) {
	net_device_stats_t *stats = &dev->stats;
	const struct net_proto *nproto;
	iphdr_t *iph = ip_hdr(skb);
	__u16 old_check;
	size_t ip_len;
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
	if (skb->len < dev->hdr_len + IP_MIN_HEADER_SIZE
			|| IP_HEADER_SIZE(iph) < IP_MIN_HEADER_SIZE
			|| skb->len < dev->hdr_len + IP_HEADER_SIZE(iph)) {
		log_debug("ip_rcv: invalid IPv4 header length");
		stats->rx_length_errors++;
		skb_free(skb);
		return 0; /* error: invalid header length */
	}


	if (iph->version != 4) {
		log_debug("ip_rcv: invalid IPv4 version");
		stats->rx_err++;
		skb_free(skb);
		return 0; /* error: not ipv4 */
	}

	old_check = iph->check;
	ip_set_check_field(iph);
	if (old_check != iph->check) {
		log_debug("ip_rcv: invalid checksum %hx(%hx)",
				ntohs(old_check), ntohs(iph->check));
		stats->rx_crc_errors++;
		skb_free(skb);
		return 0; /* error: invalid crc */
	}

	ip_len = ntohs(iph->tot_len);
	if (ip_len < IP_HEADER_SIZE(iph)
			|| skb->len < dev->hdr_len + ip_len) {
		log_debug("ip_rcv: invalid IPv4 length");
		stats->rx_length_errors++;
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	/* Setup transport layer (L4) header */
	skb->h.raw = skb->nh.raw + IP_HEADER_SIZE(iph);

	/* Validating */
	if (0 != nf_test_skb(NF_CHAIN_INPUT, NF_TARGET_ACCEPT, skb)) {
		log_debug("ip_rcv: dropped by input netfilter");
		stats->rx_dropped++;
		skb_free(skb);
		return 0; /* error: dropped */
	}

	/* Forwarding */
	assert(skb->dev);
	if (!inetdev_get_by_dev(skb->dev)) {
		log_debug("ip_rcv: dropped by input  because inet_dev is not set");
		skb_free(skb);
		return 0; /* didn't set inet dev yet */
	}

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
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
		if (!ip_is_local_net_ns(iph->daddr, IP_LOCAL_BROADCAST,
					skb->dev->net_ns)) {
#else
		if (!ip_is_local(iph->daddr, IP_LOCAL_BROADCAST)) {
#endif
			if (0 != nf_test_skb(NF_CHAIN_FORWARD, NF_TARGET_ACCEPT, skb)) {
				log_debug("ip_rcv: dropped by forward netfilter");
				stats->rx_dropped++;
				skb_free(skb);
				return 0; /* error: dropped */
			}
			return ip_forward(skb);
		}
	}

	memset(skb->cb, 0, sizeof(skb->cb));
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
			log_debug("ip_rcv: invalid options");
			stats->rx_err++;
			skb_free(skb);
			return 0; /* error: bad ops */
		}
		if (ip_options_handle_srr(skb)) {
			log_debug("ip_rcv: can't handle options");
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
	if (ntohs(skb->nh.iph->frag_off) & (IP_MF | IP_OFFSET)) {
		if ((complete_skb = ip_defrag(skb)) == NULL) {
			return 0;
		} else {
			skb = complete_skb;
			iph = ip_hdr(complete_skb);
		}
	}

	/* When a packet is received, it is passed to any raw sockets
	 * which have been bound to its protocol or to socket with concrete protocol */
	raw_rcv(skb);

	nproto = net_proto_lookup(ETH_P_IP, iph->proto);
	if (nproto != NULL) {
		return nproto->handle(skb);
	}

	log_debug("ip_rcv: unknown protocol %d", iph->proto);
	skb_free(skb);
	return 0; /* error: nobody wants this packet */
}
