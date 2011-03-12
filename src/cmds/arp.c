/**
 * @file
 * @brief Manipulate the system ARP cache.
 *
 * @date 24.04.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <netutils.h>
#include <net/arp.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: arp [-i if] [-s|d] [-a host] [-m hwaddr] [-h]\n");
}

static void print_arp_cache(void *ifdev) {
	size_t i;
	unsigned char mac[18];
	net_device_t *net_dev;
	struct in_addr addr;

	printf("Address\t\t\tHWtype\tHWaddress\t\tFlags\tIface\n");

	for (i = 0; i < ARP_CACHE_SIZE; i++) {
		if ((arp_tables[i].state == 1) && (ifdev == NULL || ifdev
				== arp_tables[i].if_handler)) {
			net_dev = arp_tables[i].if_handler->dev;
			macaddr_print(mac, arp_tables[i].hw_addr);
			addr.s_addr = arp_tables[i].pw_addr;
			printf("%s\t\t%s\t%s\t%c\t%s\n", inet_ntoa(addr),
					arp_tables[i].if_handler->dev->type == 1 ? "ether" : "",
					mac, arp_tables[i].flags == ATF_COM ? 'C' : 'P',
					net_dev->name);
		}
	}
}

static int exec(int argc, char **argv) {
	int opt;
	struct in_addr addr;
	unsigned char hwaddr[ETH_ALEN];
	in_device_t *ifdev = NULL;
	int op = -1;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hdsa:m:i:"))) {
		switch (opt) {
		case 'd':
			op = 0;
			break;
		case 's':
			op = 1;
			break;
		case 'a':
			if (0 == inet_aton(optarg, &addr)) {
				printf("arp: invalid IP address: %s\n", optarg);
				return -1;
			}
			break;
		case 'm':
			if (NULL == macaddr_scan((unsigned char *) optarg, hwaddr)) {
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
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	};

	if (op == -1) {
		print_arp_cache(ifdev);
		return 0;
	}

	if (op) {
		arp_add_entity(ifdev, addr.s_addr, hwaddr, ATF_PERM);
	} else {
		arp_delete_entity(ifdev, addr.s_addr, hwaddr);
	}

	return 0;
}
