/**
 * @file
 * @brief Manipulate the system ARP cache.
 *
 * @date 24.04.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <net/util/macaddr.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/neighbour.h>
#include <net/inetdevice.h>
#include <net/if_arp.h>
#include <string.h>



EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: arp [-h] [-i if] [-s addr hwaddr|-d addr]\n");
}

static int print_arp_entity(const struct neighbour *n,
		struct in_device *in_dev) {
	struct in_addr addr;
	unsigned char mac[18];

	assert(n != NULL);

	if ((in_dev == NULL) || (in_dev->dev == n->dev)) {
		if (!n->incomplete) {
			macaddr_print(mac, &n->haddr[0]);
		}
		else {
			sprintf((char *)mac, "%s", "(incomplete)");
		}
		memcpy(&addr, n->paddr, n->plen);
		printf("%-15s %-6s  %-17s %-5s %-5s\n", inet_ntoa(addr),
			(n->dev->type == ARPG_HRD_ETHERNET) ? "ether" : "",
			mac, (!(n->flags & NEIGHBOUR_FLAG_PERMANENT) ? "C" : "P"),
			n->dev->name);
	}
	return 0;
}

static void print_arp_cache(struct in_device *in_dev) {
	printf("Address         HWtype  HWaddress         Flags Iface\n");
	neighbour_foreach((neighbour_foreach_ft)&print_arp_entity, in_dev);
}

static int exec(int argc, char **argv) {
	int opt;
	struct in_addr addr;
	unsigned char hwaddr[ETH_ALEN];
	struct in_device *ifdev = NULL;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hd:s:a:m:i:"))) {
		switch (opt) {
		case 'd':
			if (0 == inet_aton(optarg, &addr)) {
				printf("arp: invalid IP address: %s\n", optarg);
				return -EINVAL;
			}
			if (ifdev == NULL) {
				printf("arp: please first specify the interface\n");
				return -EINVAL;
			}
			//TODO checked interface and use default
			return neighbour_del(ETH_P_IP, &addr, ifdev->dev);
		case 's':
			if (0 == inet_aton(optarg, &addr)) {
				printf("arp: invalid IP address: %s\n", optarg);
				return -EINVAL;
			}
			if(argc <= optind) {
				print_usage();
				return -EINVAL;
			}
			optarg = argv[optind++];
			if (NULL == macaddr_scan((const unsigned char *) optarg, hwaddr)) {
				printf("arp: invalid MAC address: %s\n", optarg);
				return -EINVAL;
			}
			if (ifdev == NULL) {
				printf("arp: please first specify the interface\n");
				return -EINVAL;
			}
			//TODO checked interface and use default
			return neighbour_add(ETH_P_IP, &addr, sizeof addr, ifdev->dev,
					ARPG_HRD_ETHERNET, &hwaddr[0], sizeof hwaddr, NEIGHBOUR_FLAG_PERMANENT);
		case 'i':
			if (NULL == (ifdev = inetdev_get_by_name(optarg))) {
				printf("arp: can't find interface %s\n", optarg);
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

	print_arp_cache(ifdev);

	return 0;
}
