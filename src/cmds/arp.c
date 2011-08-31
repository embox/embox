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

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: arp [-i if] [-s|d] [-a host] [-m hwaddr] [-h]\n");
}

static void print_arp_cache(void *ifdev) {
	unsigned char mac[18];
	net_device_t *net_dev;
	struct in_addr addr;
	struct neighbour *entity;

	entity = neighbour_get_first();

	if (entity == NULL) {
		return;
	}

	printf("Address         HWtype  HWaddress         Flags Iface\n");

	do {
		if ((ifdev == entity->if_handler)
				|| (ifdev == NULL)) {
			net_dev = entity->if_handler->dev;
			macaddr_print(mac, entity->hw_addr);
			addr.s_addr = entity->ip_addr;
			printf("%15s %6s %17s %5s %5s\n", inet_ntoa(addr),
					(net_dev->type == ARPHRD_ETHER) ? "ether" : "",
					mac, entity->flags == ATF_COM ? "C" : "P",
					net_dev->name);
		}
		neighbour_get_next(&entity);
	} while (entity != NULL);
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
		neighbour_add(ifdev, addr.s_addr, hwaddr, ATF_PERM);
	} else {
		neighbour_delete(ifdev, addr.s_addr);
	}

	return 0;
}
