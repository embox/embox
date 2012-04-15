/**
 * @file
 * @brief The Internet Protocol (IP) output module.
 *
 * @date 03.12.09
 * @author Nikolay Korotky
 * @author Vladimir Sokolov
 */

#include <err.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/inet_sock.h>
#include <net/inetdevice.h>
#include <net/route.h>
#include <linux/init.h>
#include <net/ip_fragment.h>
#include <net/skbuff.h>
#include <net/icmp.h>
#include <util/math.h>



int rebuild_ip_header(sk_buff_t *skb, unsigned char ttl, unsigned char proto,
		unsigned short id, unsigned short len, in_addr_t saddr,
		in_addr_t daddr/*, ip_options_t *opt*/) {
	iphdr_t *hdr = skb->nh.iph;
	hdr->version = 4;
	hdr->ihl = IP_MIN_HEADER_SIZE >> 2 /* + opt->optlen */;
	hdr->saddr = saddr; // Changed as in_addr_t is in network-ordered
	hdr->daddr = daddr;
	hdr->tot_len = htons(len - ETH_HEADER_SIZE);
	hdr->ttl = htons(ttl);
	hdr->id = htons(id);
	hdr->tos = 0;
	/* frag_off will be set during fragmentation decision */
	hdr->proto = proto;
	ip_send_check(hdr);
	return 0;
}

static inline void build_ip_packet(struct inet_sock *sk, sk_buff_t *skb) {

	/* IP header has already been built */
	if (sk->sk.sk_type == SOCK_RAW)
		return;

	/* We use headers in other way then Linux. So data coinsides with LL header
	 * Ethernet was hardcoded in skb allocations, so be careful
	 */
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

		/* Suspicious:
		 *	socket SHOULD NOT set TLL. It's possible, but strange
		 *	socket (!raw || !packet) CAN NOT have information about id. It's not its business.
		 * This functionality belongs to the device. NOT to the socket.
		 * See init_ip_header() usage. It's more correct
		 */
	rebuild_ip_header(skb, sk->uc_ttl, sk->sk.sk_protocol, sk->id, skb->len,
			sk->saddr, sk->daddr/*, sk->opt*/);
	return;
}

int ip_queue_xmit(sk_buff_t *skb, int ipfragok) {
	skb->protocol = ETH_P_IP;
	return dev_queue_xmit(skb);
}

int ip_send_packet(struct inet_sock *sk, sk_buff_t *skb) {
	iphdr_t *iph = ip_hdr(skb);
	in_addr_t daddr = ntohl(iph->daddr);
	struct rt_entry *best_route = rt_fib_get_best(daddr);

	if (!best_route) {
		kfree_skb(skb);
		return -1;
	}

	if (sk) {
		build_ip_packet(sk, skb);
	}

	if (ip_route(skb, best_route) < 0) {
		kfree_skb(skb);
		return -1;
	}

	if (skb->len > best_route->dev->mtu) {
		if (!(skb->nh.iph->frag_off & htons(IP_DF))) {
			struct sk_buff_head *tx_buf = ip_frag(skb, best_route->dev->mtu);
			struct sk_buff *tmp;
			int res = 0;

			kfree_skb(skb);
			while ((res >= 0) && (tmp = skb_dequeue(tx_buf))) {
				res = min(ip_queue_xmit(tmp, 0), res);
			}
			skb_queue_purge(tx_buf);
			return res;
		} else {
			/* if packet size is greater than MTU and we can't fragment it we can't go further */
			kfree_skb(skb);
			return -1;
		}
	}

	return ip_queue_xmit(skb, 0);
}

int ip_forward_packet(sk_buff_t *skb) {
	iphdr_t *iph = ip_hdr(skb);
	in_addr_t daddr = ntohl(iph->daddr);
	int optlen = IP_HEADER_SIZE(iph) - IP_MIN_HEADER_SIZE;
	struct rt_entry *best_route = rt_fib_get_best(daddr);

		/* Drop broadcast and multicast addresses of 2 and 3 layers
		 * Note, that some kinds of those addresses we can't get here, because
		 * they processed in other part of code - see ip_is_local(,true,xxx);
		 * And, of course, loopback packets must not be processed here
		 */
	if ( (eth_packet_type(skb) != PACKET_HOST) || ipv4_is_multicast(daddr) ) {
		kfree_skb(skb);
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
	iph->ttl--;		/* All routes have the same length */
	ip_send_check(iph);

		/* Check no route */
	if (!best_route) {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, 0);
		return -1;
	}

		/* Should we send ICMP redirect */
	if (skb->dev == best_route->dev) {
		struct sk_buff *s_new = skb_copy(skb, 0);
		if (s_new) {
			icmp_send(s_new, ICMP_REDIRECT, (best_route->rt_gateway == INADDR_ANY),
					  htonl(best_route->rt_gateway));
		}
		/* We can still proceed here */
	}

	if (ip_route(skb, best_route) < 0) {
		/* So we have something like arp problem */
		if (best_route->rt_gateway == INADDR_ANY)
		{
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, iph->daddr);
		}
		else
		{
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, htonl(best_route->rt_gateway));
		}
		return -1;
	}

		/* Fragment packet, if it's required */
	if (skb->len > best_route->dev->mtu) {
		if (!(iph->frag_off & htons(IP_DF))) {
			/* We can perform fragmentation */
			struct sk_buff_head *tx_buf = ip_frag(skb, best_route->dev->mtu);
			struct sk_buff *s_tmp;
			int res = 0;

			kfree_skb(skb);
			while ((res >= 0) && (s_tmp = skb_dequeue(tx_buf))) {
				res = min(ip_queue_xmit(s_tmp, 0), res);
			}
			skb_queue_purge(tx_buf);
			return res;
		} else {
			/* Fragmentation is disabled */
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
					  htonl(best_route->dev->mtu << 16));		/* Support RFC 1191 */
			return -1;
		}
	}

	return ip_queue_xmit(skb, 0);
}

void ip_v4_icmp_err_notify(struct sock *sk, int type, int code){

	/* TODO: add more specific error notification */

	sk->sk_err = (type & (code<<8));
}
