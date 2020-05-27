/**
 * @file
 * @brief Ping hosts by RARP requests/replies.
 *
 * @date 02.10.12
 * @author Ilia Vaprol
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include <net/util/macaddr.h>
#include <net/l0/net_tx.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/l3/rarp.h>
#include <net/lib/arp.h>
#include <net/neighbour.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/l3/ipv4/ip.h>
#include <string.h>
#include <kernel/time/time.h>

#define DEFAULT_INTERVAL (1000 * USEC_PER_MSEC)

static void print_usage(void) {
	printf("Usage: rarping [-I if] [-c cnt] hwaddr\n");
}

static int send_request(struct net_device *dev, uint16_t pro,
		uint8_t pln, const void *tha) {
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

	hdr_info.type = ETH_P_RARP;
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
			RARP_OP_REQUEST, &dev->dev_addr[0], NULL, tha, NULL);

	return net_tx_direct(skb);
}

int main(int argc, char **argv) {
	int opt, ret;
	int cnt = 4, cnt_resp = 0;
	struct in_device *in_dev;
	char *if_name = "eth0";
	unsigned char hln = ETH_ALEN, pln = IP_ADDR_LEN;
	unsigned char sha[MAX_ADDR_LEN], tha[MAX_ADDR_LEN];
	unsigned char spa[MAX_ADDR_LEN], tpa[MAX_ADDR_LEN];
	char sha_str[] = "xx.xx.xx.xx.xx.xx", tha_str[] = "xx.xx.xx.xx.xx.xx";
	char spa_str[] = "xxx.xxx.xxx.xxx", tpa_str[] = "xxx.xxx.xxx.xxx";

	while (-1 != (opt = getopt(argc, argv, "I:c:h"))) {
		switch (opt) {
		case 'I': /* get interface */
			if_name = optarg;
			break;
		case 'c': /* get ping cnt */
			if (1 != sscanf(optarg, "%d", &cnt)) {
				printf("rarping: bad number of packets to transmit.\n");
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
	}

	if (argc == 1) {
		print_usage();
		return 0;
	}

	in_dev = inetdev_get_by_name(if_name);
	if (NULL == in_dev) {
		printf("rarping: unknown iface %s\n", if_name);
		return -EINVAL;
	}

	if (in_dev->dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		printf("rarping: iface %s don't support ARP\n", if_name);
		return -EINVAL;
	}

	/* Get destination hardware address. */
	if (macaddr_scan((const unsigned char *)argv[argc - 1], &tha[0]) == NULL) {
		printf("rarping: invalid MAC address: %s\n", argv[argc - 1]);
		return -EINVAL;
	}

	/* Get a source hardware and protocol address */
	assert(hln == in_dev->dev->addr_len);
	memcpy(&sha[0], &in_dev->dev->dev_addr[0], hln);
	assert(pln == sizeof in_dev->ifa_address);
	memcpy(&spa[0], &in_dev->ifa_address, pln);

	/* Get addresses in human-readable form */
	macaddr_print((unsigned char *)&tha_str[0], &tha[0]);
	macaddr_print((unsigned char *)&sha_str[0], &sha[0]);
	strcpy(&spa_str[0], inet_ntoa(*(struct in_addr *)&spa[0]));

	printf("RARPING %s from %s(%s) %s\n", &tha_str[0], &sha_str[0],
		&spa_str[0], in_dev->dev->name);
	for (size_t i = 1; i <= cnt; i++) {
		neighbour_clean(in_dev->dev);
		send_request(in_dev->dev, ETH_P_IP, sizeof in_dev->ifa_address,
				&tha[0]);
		usleep(DEFAULT_INTERVAL);
		ret = neighbour_get_paddr(ARP_HRD_ETHERNET, &tha[0], in_dev->dev,
				ETH_P_IP, pln, &tpa[0]);
		if (ret == 0) {
			strcpy(&tpa_str[0], inet_ntoa(*(struct in_addr *)&tpa[0]));
			printf("Unicast reply from %s(%s)  %dms\n", &tha_str[0], &tpa_str[0], -1);
			cnt_resp++;
		}
	}
	printf("Sent %d probes (%d broadcast(s))\n", (int)cnt, 1);
	printf("Received %d response(s)\n", cnt_resp);

	return 0;
}
