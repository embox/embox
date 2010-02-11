/**
 * @file ip.c
 *
 * @brief The Internet Protocol (IP) module.
 * @date 17.03.2009
 * @author Alexandr Batyukov
 * @author Nikolay Korotky
 */
#include <err.h>
#include <net/net.h>
#include <net/skbuff.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/inet_sock.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/route.h>
#include <net/checksum.h>

int ip_rcv(sk_buff_t *pack, net_device_t *dev,
                      packet_type_t *pt, net_device_t *orig_dev) {
	net_device_stats_t *stats = dev->netdev_ops->ndo_get_stats(pack->dev);
	iphdr_t *iph;
	unsigned short tmp;
	unsigned int len;
	pack->h.raw = pack->nh.raw + IP_HEADER_SIZE;
	iph = pack->nh.iph;
	/**
	 *   RFC1122: 3.1.2.2 MUST silently discard any IP frame that fails the checksum.
	 *   Is the datagram acceptable?
	 *   1.  Length at least the size of an ip header
	 *   2.  Version of 4
	 *   3.  Checksums correctly. [Speed optimisation for later, skip loopback checksums]
	 *   4.  Doesn't have a bogus length
	 */
	if (iph->ihl < 5 || iph->version != 4) {
		LOG_ERROR("invalide IPv4 header\n");
		stats->rx_err ++;
		return NET_RX_DROP;
	}
	tmp = iph->check;
	iph->check = 0;
	if (tmp != ptclbsum(pack->nh.raw, IP_HEADER_SIZE)) {
		LOG_ERROR("bad ip checksum\n");
		stats->rx_crc_errors ++;
		return NET_RX_DROP;
	}

	len = ntohs(iph->tot_len);
	if (pack->len < len || len < (iph->ihl * 4)) {
		LOG_ERROR("invalide IPv4 header length\n");
		stats->rx_length_errors ++;
		return NET_RX_DROP;
	}
	/**
	 * Check the destination address, and if it dosn't match
	 * any of own addresses, retransmit packet according to routing table.
	 */
	if(ip_dev_find(pack->nh.iph->daddr) == NULL) {
		if(!ip_route(pack)) {
			dev_queue_xmit(pack);
		}
	        return 0;
	}
	if (ICMP_PROTO_TYPE == iph->proto) {
		icmp_rcv(pack);
	}
	if (UDP_PROTO_TYPE == iph->proto) {
		udp_rcv(pack);
	}
	return NET_RX_SUCCESS;
}
