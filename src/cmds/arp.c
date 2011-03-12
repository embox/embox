/**
 * @file
 * @brief Manipulate the system ARP cache.
 *
 * @date 24.04.09
 * @author Nikolay Korotky
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <net/arp.h>
#include <netutils.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: arp [-i if] [-s|d] [-a host] [-m hwaddr] [-h]\n");
}

static int print_arp_cache(void *ifdev) {
	size_t i;
	unsigned char mac[18];
	net_device_t *net_dev;
	struct in_addr addr;
	for (i = 0; i < ARP_CACHE_SIZE; i++) {
		if ((arp_tables[i].state == 1) &&
		   (ifdev == NULL || ifdev == arp_tables[i].if_handler)) {
			net_dev = arp_tables[i].if_handler->dev;
			macaddr_print(mac, arp_tables[i].hw_addr);
			addr.s_addr = arp_tables[i].pw_addr;
			TRACE("%s\t\t%s\t%s\t%c\t%s\n", inet_ntoa(addr),
				arp_tables[i].if_handler->dev->type==1?"ether":"",
				mac, arp_tables[i].flags==ATF_COM?'C':'P', net_dev->name);
		}
	}
	return 0;
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	struct in_addr addr;
	unsigned char hwaddr[ETH_ALEN];
	in_device_t *ifdev = NULL;
	int op = -1;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "hdsa:m:i:");
		switch(nextOption) {
		case 'h':
			print_usage();
			return 0;
		case 'd':
			op = 0;
			break;
		case 's':
			op = 1;
			break;
		case 'a':
			if (0 == inet_aton(optarg, &addr)) {
				LOG_ERROR("wrong ip addr format (%s)\n", optarg);
				return -1;
			}
			break;
		case 'm':
			if (NULL == macaddr_scan((unsigned char *) optarg, hwaddr)) {
				LOG_ERROR("wrong mac addr format %s\n", optarg);
				return -1;
			}
			break;
		case 'i':
			if (NULL == (ifdev = inet_dev_find_by_name(optarg))) {
				LOG_ERROR("can't find interface %s\n", optarg);
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	switch(op) {
	case 0:
		arp_delete_entity(ifdev, addr.s_addr, hwaddr);
		return 0;
	case 1:
		arp_add_entity(ifdev, addr.s_addr, hwaddr, ATF_PERM);
		return 0;
	default:
		break;
	}
	printf("Address\t\t\tHWtype\tHWaddress\t\tFlags\tIface\n");
	print_arp_cache(ifdev);
	return 0;
}
