/**
 * @file
 * @brief Ping hosts by ARP requests/replies.
 *
 * @date 23.12.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#include <net/util.h>
#include <net/rarp.h>
#include <net/if_arp.h>
#include <net/neighbour.h>
#include <net/inetdevice.h>
#include <net/ip.h>
#include <string.h>


EMBOX_CMD(exec);

#define DEFAULT_INTERVAL 1000

static void print_usage(void) {
	printf("Usage: arping [-I if] [-c cnt] host\n");
}

static int exec(int argc, char **argv) {
	int opt, ret;
	int cnt = 4, cnt_resp = 0, i;
	in_device_t *in_dev = inet_dev_find_by_name("eth0");
	unsigned char hln = ETH_ALEN, pln = IP_ADDR_LEN, tmp;
	unsigned char sha[MAX_ADDR_LEN], tha[MAX_ADDR_LEN];
	unsigned char spa[MAX_ADDR_LEN], tpa[MAX_ADDR_LEN];
	char *sha_str = "xx.xx.xx.xx.xx.xx", *tha_str = "xx.xx.xx.xx.xx.xx";
	char *spa_str = "xxx.xxx.xxx.xxx", *tpa_str = "xxx.xxx.xxx.xxx";

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "I:c:h"))) {
		switch (opt) {
		case 'I': /* get interface */
			if (NULL == (in_dev = inet_dev_find_by_name(optarg))) {
				printf("arping: unknown iface %s\n", optarg);
				return -1;
			}
			break;
		case 'c': /* get ping cnt */
			if (1 != sscanf(optarg, "%d", &cnt)) {
				printf("arping: bad number of packets to transmit.\n");
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
	};

	if (argc == 1) {
		print_usage();
		return 0;
	}

	/* Get destination hardware address. */
	if (macaddr_scan((const unsigned char *)argv[argc - 1], &tha[0]) == NULL) {
		printf("arping: invalid MAC address: %s\n", argv[argc - 1]);
		return -1;
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
	for (i = 1; i <= cnt; i++) {
		neighbour_del(&tha[0], hln, NULL, 0, in_dev->dev);
		rarp_send(RARP_OPER_REQUEST, ETH_P_IP, hln, pln, &sha[0], NULL,
			&tha[0], NULL, NULL, in_dev->dev);
		usleep(DEFAULT_INTERVAL);
		ret = neighbour_get_protocol_address(&tha[0], hln, in_dev->dev,
				sizeof tpa, &tpa[0], &tmp);
		if (ret == ENOERR) {
			assert(tmp == pln);
			strcpy(&tpa_str[0], inet_ntoa(*(struct in_addr *)&tpa[0]));
			printf("Unicast reply from %s(%s)  %dms\n", &tha_str[0], &tpa_str[0], 0);
			cnt_resp++;
		}
	}
	printf("Sent %d probes (%d broadcast(s))\n", cnt, 1);
	printf("Received %d response(s)\n", cnt_resp);

	return 0;
}

