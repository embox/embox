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

	return net_tx_direct(skb);
}

int main(int argc, char **argv) {
	int opt;
	int cnt = 4, cnt_resp = 0, i;
	struct in_device *in_dev;
	char *if_name = "eth0";
	struct in_addr dst;
	char dst_b[] = "xxx.xxx.xxx.xxx";
	char from_b[] = "xxx.xxx.xxx.xxx";
	struct in_addr from;
	unsigned char mac[18], hw_addr[ETH_ALEN];
	struct timeval t1, t2, sub_res;
	int ret, microseconds, milliseconds;

	while (-1 != (opt = getopt(argc, argv, "I:c:h"))) {
		switch (opt) {
		case 'I': /* get interface */
			if_name = optarg;
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

	in_dev = inetdev_get_by_name(if_name);
	if (NULL == in_dev) {
		printf("arping: unknown iface %s\n", if_name);
		return -EINVAL;
	}

	if (in_dev->dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		printf("arping: iface %s don't support ARP\n", if_name);
		return -EINVAL;
	}

	/* Get destination address. */
	if (0 == inet_aton(argv[argc - 1], &dst)) {
		printf("arping: invalid IP address: %s\n", argv[argc - 1]);
		return -EINVAL;
	}

	strncpy(dst_b, inet_ntoa(dst), sizeof(dst_b) - 1);
	from.s_addr = in_dev->ifa_address;
	strncpy(from_b, inet_ntoa(from), sizeof(from_b) - 1);
	printf("ARPING %s from %s %s\n", dst_b, from_b, in_dev->dev->name);

	for (i = 1; i <= cnt; i++) {
		neighbour_del(ETH_P_IP, &dst, in_dev->dev);

		usleep(DEFAULT_INTERVAL);

		if (gettimeofday(&t1, NULL) == -1) {
			return -1;
		}

		send_request(in_dev->dev, ETH_P_IP, sizeof in_dev->ifa_address,
				&in_dev->ifa_address, &dst.s_addr);

		/* Repeat until neigbour_get_haddr returns 0 or one second
		 * (default interval) has passed.
		 */
		do {
			ret = neighbour_get_haddr(ETH_P_IP, &dst, in_dev->dev,
								ARP_HRD_ETHERNET, sizeof hw_addr, &hw_addr[0]);
			if (gettimeofday(&t2, NULL) == -1) {
				return -1;
			}
			timersub(&t2, &t1, &sub_res);
		} while (ret != 0 && sub_res.tv_sec < 1);

		if (ret == 0 && sub_res.tv_sec < 1){
			macaddr_print(mac, hw_addr);

			/* To avoid doing floating point arithmetic. */
			microseconds = sub_res.tv_usec;
			milliseconds = microseconds / 1000;
			microseconds = microseconds % 1000;

			printf("Unicast reply from %s [%s]  %d.%03dms\n",
			dst_b, mac, milliseconds, microseconds);
			cnt_resp++;
		}
	}
	printf("Sent %d probes (%d broadcast(s))\n", cnt, 1);
	printf("Received %d response(s)\n", cnt_resp);

	return 0;
}

