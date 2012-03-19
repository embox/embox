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
	hdr->frag_off = skb->offset;
	hdr->frag_off |= IP_DF;						/* svv: So all packets from our system can't be fragmented??? */
	hdr->frag_off = htons(hdr->frag_off);
	hdr->proto = proto;
	ip_send_check(hdr);
	return 0;
}

static inline void build_ip_packet(struct inet_sock *sk, sk_buff_t *skb) {
	if (sk->sk.sk_type == SOCK_RAW)
	    return;
	skb->nh.raw = skb->data + ETH_HEADER_SIZE;
	rebuild_ip_header(skb, sk->uc_ttl, sk->sk.sk_protocol, sk->id, skb->len,
			sk->saddr, sk->daddr/*, sk->opt*/);
	return;
}

int ip_queue_xmit(sk_buff_t *skb, int ipfragok) {
	skb->protocol = ETH_P_IP;
	return dev_queue_xmit(skb);
}

int ip_send_packet(struct inet_sock *sk, sk_buff_t *skb) {
	struct sk_buff_head *tx_buf;
	struct sk_buff *tmp;
	int res;

	res = 0;

	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;

	if (skb->len > MTU) {
		/* svv: suspicious:
		 *	if offset is just a shift (==value), then we can't use IP_DF here
		 *	if it's conside with information structure in IP header then
		 *	we haven't had it for not raw sockets
		 *	we are going to build ip header later
		 *------
		 *	Which "MTU" should we use? We have different interfaces
		 */
		if (!(skb->offset & IP_DF)) {
			tx_buf = ip_frag(skb);
			while ((tmp = skb_dequeue(tx_buf)) != NULL) {
				res += ip_send_packet(sk, tmp);
			}
			skb_queue_purge(tx_buf);
			return res;
		} else {
			/* if packet size is greater than MTU and we can't fragment it we can't go further */
			kfree_skb(skb);
			return -1;
		}
	}

	if (sk) {
		build_ip_packet(sk, skb);
	}
	if (ip_route(skb, NULL)) {
		kfree_skb(skb);
		return -1;
	}
	ip_send_check(skb->nh.iph);
	return ip_queue_xmit(skb, 0);			/* svv: What about not-Ether carrier ? */
}

int ip_forward_packet(sk_buff_t *skb) {
	iphdr_t *iph = ip_hdr(skb);
	in_addr_t daddr = ntohl(iph->daddr);
	int optlen = IP_HEADER_SIZE(iph) - IP_MIN_HEADER_SIZE;
	int16_t frag_options = ntohs(iph->frag_off);
	struct rt_entry *best_route = rt_fib_get_best(daddr);

		/* Drop broadcast and multicast addresses of 2 and 3 layers
		 * Note, that some kinds of those addresses we can't get here, because
		 * they processed in other part of code - see ip_is_local(,true,xxx);
		 */
	if ( (skb->pkt_type != PACKET_HOST) || ipv4_is_multicast(daddr) ) {
		kfree_skb(skb);
		return 0;
	}

		/* Check TTL and decrease it */
	if (iph->ttl <= 1) {
		icmp_send(skb, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, 0);
		return -1;
	}
	iph->ttl--;		/* All routes have the same length */

		/* IP Options is a security violation */
	if (optlen) {
		icmp_send(skb, ICMP_PARAMETERPROB, 0, htonl(20));
		return -1;
	}

		/* Check no route */
	if (!best_route) {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, 0);
		return -1;
	}

		/* Should we send ICMP redirect */
	if (skb->dev == best_route->dev) {
		struct sk_buff *s_new = skb_copy(skb, 0);		/* ToDo: too extravagant. Useless for more then first fragment */
		if (s_new) {
			icmp_send(s_new, ICMP_REDIRECT, (best_route->rt_gateway == INADDR_ANY), htonl(best_route->rt_gateway));
		}
		/* We can still proceed here */
	}

		/* Fragment packet, if it's required */
	if (skb->len > best_route->dev->mtu) {
		if (!(frag_options & IP_DF)) {
			/* We can perform fragmentation */
			struct sk_buff_head *tx_buf = ip_frag(skb);		/* ToDo: it must take proper MTU */
			struct sk_buff *s_tmp;
			int res = 0;

			while ((s_tmp = skb_dequeue(tx_buf)) != NULL) {
				res += ip_forward_packet(s_tmp);
			}
			skb_queue_purge(tx_buf);
			return res;
		} else {
			/* Fragmentation is disabled */
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, 0);
			return -1;
		}
	}

	if (ip_route(skb, best_route) < 0) {
		/* So we have something like arp problem */
		icmp_send(skb, ICMP_DEST_UNREACH, best_route->rt_gateway == INADDR_ANY ? ICMP_HOST_UNREACH : ICMP_NET_UNREACH, 0);
		return -1;
	}

		/* At least TTL was changed.
		 * But iff it's the only change fast CRC recalculation is possible (ToDo)
		 */
	ip_send_check(iph);

	return ip_queue_xmit(skb, 0);			/* What about not-Ether carrier? */
}

