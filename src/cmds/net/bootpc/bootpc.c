/**
 * @file
 * @brief Simple BOOTP client
 *
 * @date 30.07.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/lib/bootp.h>
#include <net/l2/ethernet.h>
#include <netpacket/packet.h>
#include <net/l3/arp.h>
#include <net/l4/udp.h>
#include <net/lib/ipv4.h>
#include <net/lib/udp.h>

#include <framework/mod/options.h>

#define MODOPS_TIMEOUT OPTION_GET(NUMBER, timeout)

static void print_usage(void) {
	printf("Usage: bootpc <ifname>\n");
}

static int bootp_prepare(struct net_device *dev) {
	int ret;

	ret = netdev_flag_up(dev, IFF_UP);
	if (ret) {
		return ret;
	}

	return 0;
}

static int bootp_process(struct bootphdr *bph, struct net_device *dev) {
	struct in_device *in_dev;
	in_addr_t ip_addr;
	in_addr_t ip_gate;
	in_addr_t ip_mask;
	int ret;

	in_dev = inetdev_get_by_dev(dev);
	if (in_dev == NULL) {
		return -EINVAL;
	}

	ret = bootp_get_ip(bph, &ip_addr);
	if (ret) {
		return ret;
	}

	ret = bootp_get_mask(bph, &ip_mask);
	if (ret != 0) {
		return ret;
	}

	ret = bootp_get_gateway(bph, &ip_gate);
	if (ret != 0) {
		return ret;
	}

	rt_del_route_if(dev);

	inetdev_set_addr(in_dev, ip_addr);

	inetdev_set_mask(in_dev, ip_mask);
	rt_add_route(in_dev->dev, ip_addr & ip_mask, ip_mask, INADDR_ANY, 0);

	rt_add_route(in_dev->dev, 0, 0, ip_gate,	RTF_UP | RTF_GATEWAY);

	return 0;
}

static inline int bootp_make_discover(struct net_device *dev, void *buff) {
	struct ethhdr *ethhdr;
	struct iphdr *iphdr;
	struct udphdr *udphdr;
	struct bootphdr *bootphdr;

/* fill in Ethernet layer (L2) */
	ethhdr = buff;

	memset(ethhdr->h_dest, 0xff, ETH_ALEN); /* brodcast */
	memcpy(ethhdr->h_source, dev->dev_addr, ETH_ALEN);
	ethhdr->h_proto = htons(ETH_P_IP);

	/* fill in IP layer (L3) */
	iphdr = buff + sizeof(struct ethhdr);

	iphdr->version = 4;
	iphdr->ihl = IP_MIN_HEADER_SIZE / 4;
	iphdr->tos = 0;
	iphdr->tot_len = htons(IP_MIN_HEADER_SIZE + UDP_HEADER_SIZE + sizeof(struct bootphdr));
	iphdr->id = 0;
	iphdr->frag_off = 0;
	iphdr->ttl = 64;
	iphdr->proto = IPPROTO_UDP;
	iphdr->check = 0; /* Filled later */

	/* saddr: default: 0.0.0.0 */
	memset(&iphdr->saddr, 0, sizeof(iphdr->saddr));

	/* daddr: 255.255.255.255 for broadcast*/
	memset(&iphdr->daddr, 0xFF, sizeof(iphdr->saddr));
#if 0

	ip_build(iphdr,
			htons(IP_MIN_HEADER_SIZE + UDP_HEADER_SIZE + sizeof(struct bootphdr)),
			64, IPPROTO_UDP, 0x00000000, 0xFFFFFFFF);
#endif
	/* IP header checksum */
	ip_set_check_field(iphdr);

	/* DHCP layer (L7) */
	bootphdr = buff + sizeof(struct ethhdr) + IP_MIN_HEADER_SIZE + sizeof(struct udphdr);

	bootp_build_request(bootphdr, BOOTPREQUEST, dev->type, dev->addr_len, dev->dev_addr);

	udphdr = buff + sizeof(struct ethhdr) + IP_MIN_HEADER_SIZE;

	udphdr->source = htons(BOOTP_CLIENT_PORT);
	udphdr->dest = htons(BOOTP_SERVER_PORT);

	udphdr->len = htons(UDP_HEADER_SIZE + sizeof(struct bootphdr));
	udphdr->check = 0; /* UDP checksum will be done after dhcp header */

#if 0
	udp_build(udphdr,
			htons(BOOTP_CLIENT_PORT), htons(BOOTP_SERVER_PORT),
			htons(UDP_HEADER_SIZE + sizeof(struct bootphdr)));
#endif
	udp4_set_check_field(udphdr, iphdr);

	return 0;
}

int bootp_client(struct net_device *dev) {
	static const struct timeval timeout = {
		MODOPS_TIMEOUT / MSEC_PER_SEC,
		(MODOPS_TIMEOUT % MSEC_PER_SEC) * USEC_PER_MSEC
	};
	int ret, tx_sock, rx_sock;
	uint8_t req_buff[sizeof(struct ethhdr) + IP_MIN_HEADER_SIZE + sizeof(struct udphdr) + sizeof(struct bootphdr)];
	struct sockaddr_ll link_layer;
	struct sockaddr_in addr;
	struct in_device *in_dev;
	in_addr_t dev_ip_addr;

	ret = bootp_prepare(dev);
	if (ret) {
		return ret;
	}

	tx_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (tx_sock == -1) {
		return -errno;
	}

	/* Set link layer parameters */
	memset(&link_layer, 0, sizeof(struct sockaddr_ll));

	link_layer.sll_family = AF_PACKET;
	link_layer.sll_protocol = htons(ETH_P_ALL);
	link_layer.sll_ifindex = dev->index;
	link_layer.sll_hatype = ARP_HRD_ETHERNET;
	link_layer.sll_pkttype = PACKET_OTHERHOST;
	link_layer.sll_halen = 6;

	if (-1 == bind(tx_sock, (struct sockaddr *)&link_layer, sizeof(struct sockaddr_ll))) {
		ret = -errno;
		goto error;
	}

	if (-1 == setsockopt(tx_sock, SOL_SOCKET, SO_BINDTODEVICE,
				&dev->name[0], strlen(&dev->name[0]))) {
		ret = -errno;
		goto error;
	}

	rx_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (tx_sock == -1) {
		return -errno;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(BOOTP_CLIENT_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(rx_sock, (struct sockaddr *)&addr, sizeof addr)) {
		ret = -errno;
		goto error_rx;
	}

	if (-1 == setsockopt(rx_sock, SOL_SOCKET, SO_BINDTODEVICE,
				&dev->name[0], strlen(&dev->name[0]))) {
		ret = -errno;
		goto error_rx;
	}
	if (-1 == setsockopt(rx_sock, SOL_SOCKET, SO_RCVTIMEO,
				&timeout, sizeof timeout)) {
		ret = -errno;
		goto error_rx;
	}

	bootp_make_discover(dev, req_buff);

	in_dev = inetdev_get_by_dev(dev);
	if (in_dev == NULL) {
		ret = -EINVAL;
		goto error_rx;
	}
	dev_ip_addr = in_dev->ifa_address;
	inetdev_set_addr(in_dev, 0);

	if (-1 == sendto(tx_sock, req_buff, sizeof(req_buff), 0,
			(struct sockaddr *) &link_layer, sizeof(struct sockaddr_ll))) {
		ret = -errno;
		goto error_rx;
	}

	if (-1 == recv(rx_sock, req_buff, sizeof(req_buff), 0)) {
		ret = -errno;
		inetdev_set_addr(in_dev, dev_ip_addr);
		goto error_rx;
	}

	ret = bootp_process((void*)req_buff, dev);
	if (ret != 0) {
		inetdev_set_addr(in_dev, dev_ip_addr);
	}
error_rx:
	close(rx_sock);
error:
	close(tx_sock);
	return ret;
}

int main(int argc, char **argv) {
	int opt;
	struct net_device *dev;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			/* FALLTHROUGH */
		default:
			print_usage();
			return 0;
		}
	}

	if (argc < 2) {
		printf("no interface was specified\n");
		return -EINVAL;
	}

	dev = netdev_get_by_name(argv[argc - 1]);
	if (dev == NULL) {
		printf("can't find interface %s\n", argv[argc - 1]);
		return -EINVAL;
	}

	return bootp_client(dev);
}
