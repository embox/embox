/**
 * @file
 * @brief Ping hosts by ARP requests/replies.
 *
 * @date 23.12.09
 * @author Nikolay Korotky
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/l0/net_tx.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/lib/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <string.h>

#include <net/util/macaddr.h>
#include <net/l3/arp.h>
#include <net/neighbour.h>
#include <kernel/time/time.h>

#define DEFAULT_INTERVAL (1000 * USEC_PER_MSEC)

static void print_usage(void) {
	printf("Usage: arping [-I if] [-c cnt] host\n");
}

static int send_request(struct net_device *dev, uint16_t pro,
		uint8_t pln, const void *spa, const void *tpa) {
	int ret;
	struct sk_buff *skb;
	struct net_header_info hdr_info;

	skb = skb_alloc(dev->hdr_len
			+ ARP_CALC_HEADER_SIZE(dev->addr_len, pln));
	if (skb == NULL) {
		return -ENOMEM;
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;

	hdr_info.type = ETH_P_ARP;
	hdr_info.src_hw = &dev->dev_addr[0];
	hdr_info.dst_hw = &dev->broadcast[0];
	assert(dev->ops != NULL);
	assert(dev->ops->build_hdr != NULL);
	ret = dev->ops->build_hdr(skb, &hdr_info);
	if (ret != 0) {
		skb_free(skb);
		return ret;
	}

	arp_build(arp_hdr(skb), dev->type, pro, dev->addr_len, pln,
			ARP_OP_REQUEST, &dev->dev_addr[0], spa,
			&dev->broadcast[0], tpa);

	return net_tx(skb, NULL);
}

int main(int argc, char **argv) {
	int opt;
	int cnt = 4, cnt_resp = 0, i;
	struct in_device *in_dev = inetdev_get_by_name("eth0");
	struct in_addr dst;
	char dst_b[] = "xxx.xxx.xxx.xxx";
	char from_b[] = "xxx.xxx.xxx.xxx";
	struct in_addr from;
	unsigned char mac[18], hw_addr[ETH_ALEN];

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "I:c:h"))) {
		switch (opt) {
		case 'I': /* get interface */
			if (NULL == (in_dev = inetdev_get_by_name(optarg))) {
				printf("arping: unknown iface %s\n", optarg);
				return -EINVAL;
			}
			break;
		case 'c': /* get ping cnt */
			if (1 != sscanf(optarg, "%d", &cnt)) {
				printf("arping: bad number of packets to transmit.\n");
				return -EINVAL;
			}
			break;
		case '?':
			printf("Invalid option `-%c'\n", optopt);
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	};

	if (argc == 1) {
		print_usage();
		return 0;
	}

	/* Get destination address. */
	if (0 == inet_aton(argv[argc - 1], &dst)) {
		printf("arping: invalid IP address: %s\n", argv[argc - 1]);
		return -EINVAL;
	}

	strncpy(dst_b, inet_ntoa(dst), sizeof(dst_b));
	from.s_addr = in_dev->ifa_address;
	strncpy(from_b, inet_ntoa(from), sizeof(from_b));
	printf("ARPING %s from %s %s\n", dst_b, from_b, in_dev->dev->name);
	for (i = 1; i <= cnt; i++) {
		neighbour_del(ETH_P_IP, &dst, in_dev->dev);
		send_request(in_dev->dev, ETH_P_IP, sizeof in_dev->ifa_address,
				&in_dev->ifa_address, &dst.s_addr);
		usleep(DEFAULT_INTERVAL);
		if (0 == neighbour_get_haddr(ETH_P_IP, &dst, in_dev->dev,
					ARP_HRD_ETHERNET, sizeof hw_addr, &hw_addr[0])) {
			macaddr_print(mac, hw_addr);
			printf("Unicast reply from %s [%s]  %dms\n", dst_b, mac, 0);
			cnt_resp++;
		}
	}
	printf("Sent %d probes (%d broadcast(s))\n", cnt, 1);
	printf("Received %d response(s)\n", cnt_resp);

	return 0;
}
