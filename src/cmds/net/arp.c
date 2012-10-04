/**
 * @file
 * @brief Manipulate the system ARP cache.
 *
 * @date 24.04.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <net/util.h>
#include <net/if_arp.h>
#include <net/neighbour.h>
#include <net/inetdevice.h>
#include <stdio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: arp [-i if] [-s|d] [-a host] [-m hwaddr] [-h]\n");
}

static int print_arp_entity(const struct neighbour *n,
		struct in_device *in_dev) {
	struct in_addr addr;
	unsigned char mac[18];

	if ((in_dev->dev == n->dev) || (in_dev == NULL)) {
		macaddr_print(mac, &n->haddr[0]);
		addr.s_addr = *(in_addr_t *)&n->paddr[0];
		printf("%15s %6s %17s %5s %5s\n", inet_ntoa(addr),
				(n->dev->type == ARPG_HRD_ETHERNET) ? "ether" : "",
				mac, (!(n->flags & NEIGHBOUR_FLAG_PERMANENT) ? "C" : "P"),
				n->dev->name);
	}
	return 0;
}

static void print_arp_cache(struct in_device *in_dev) {
	printf("Address         HWtype  HWaddress         Flags Iface\n");
	neighbour_foreach((neighbour_foreach_handler_t)&print_arp_entity, in_dev);
}

static int exec(int argc, char **argv) {
	int opt;
	struct in_addr addr;
	unsigned char hwaddr[ETH_ALEN];
	in_device_t *ifdev = NULL;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hd:s:a:m:i:"))) {
		switch (opt) {
		case 'd':
			if (0 == inet_aton(optarg, &addr)) {
				printf("arp: invalid IP address: %s\n", optarg);
				return -1;
			}
			if (ifdev == NULL) {
				printf("arp: please first specify the interface\n");
				return -1;
			}
			//TODO checked interface and use default
			return neighbour_del(NULL, 0, (const unsigned char *)&addr,
					sizeof addr, ifdev->dev);
		case 's':
			if (0 == inet_aton(optarg, &addr)) {
				printf("arp: invalid IP address: %s\n", optarg);
				return -1;
			}
			if(argc <= optind) {
				print_usage();
				return -1;
			}
			optarg = argv[optind++];
			if (NULL == macaddr_scan((const unsigned char *) optarg, hwaddr)) {
				printf("arp: invalid MAC address: %s\n", optarg);
				return -1;
			}
			if (ifdev == NULL) {
				printf("arp: please first specify the interface\n");
				return -1;
			}
			//TODO checked interface and use default
			return neighbour_add(&hwaddr[0], sizeof hwaddr,
					(const unsigned char *)&addr, sizeof addr, ifdev->dev,
					NEIGHBOUR_FLAG_PERMANENT);
		case 'a':
			if (0 == inet_aton(optarg, &addr)) {
				printf("arp: invalid IP address: %s\n", optarg);
				return -1;
			}
			break;
		case 'm':
			if (NULL == macaddr_scan((const unsigned char *) optarg, hwaddr)) {
				printf("arp: invalid MAC address: %s\n", optarg);
				return -1;
			}
			break;
		case 'i':
			if (NULL == (ifdev = inet_dev_find_by_name(optarg))) {
				printf("arp: can't find interface %s\n", optarg);
				return -1;
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

	print_arp_cache(ifdev);

	return 0;
}
