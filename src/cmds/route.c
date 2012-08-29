/**
 * @file
 * @brief Similar to the standard Unix route.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <net/route.h>
#include <net/inetdevice.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

EMBOX_CMD(exec);

enum {
	LEN_DEST  = 15,
	LEN_GW    = 15,
	LEN_NMASK = 15,
	LEN_FLAGS = 5,
	LEN_IFACE = 5,
};

enum {
	OFFSET_START,
	OFFSET_DEST  = OFFSET_START,
	OFFSET_GW    = OFFSET_DEST + LEN_DEST + 1,
	OFFSET_NMASK = OFFSET_GW + LEN_GW + 1,
	OFFSET_FLAGS = OFFSET_NMASK + LEN_NMASK + 1,
	OFFSET_IFACE = OFFSET_FLAGS + LEN_FLAGS + 1,
	OFFSET_EOS   = OFFSET_IFACE + LEN_IFACE + 1,
	BUFF_SZ
};

static void print_usage(void) {
	printf("Usage: route [-nmgdh] [add|del]\n");
}

static int exec(int argc, char *argv[]) {
	int opt;
	in_device_t *ifdev;
	struct rt_entry *rt;
	size_t l;
	struct in_addr net, mask, gw;
	char buff[BUFF_SZ], *str;

	ifdev = NULL;
	net.s_addr = mask.s_addr = gw.s_addr = INADDR_ANY;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "n:m:d:g:h"))) {
		switch ((char)opt) {
		case 'n':
			if (!inet_aton(optarg, &net)) {
				printf("Unknown host\n");
				return -1;
			}
			break;
		case 'm':
			if (!inet_aton(optarg, &mask)) {
				printf("Unknown mask\n");
				return -1;
			}
			break;
		case 'g':
			if (!inet_aton(optarg, &gw)) {
				printf("Unknown gateway\n");
				return -1;
			}
			break;
		case 'd':
			if ((ifdev = inet_dev_find_by_name(optarg)) == NULL) {
				printf("route: unknown iface %s\n", optarg);
				return -1;
			}
			break;
		case '?':
			printf("Invalid option '-%c'\n", optopt);
			return -1;
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}
	/* ToDo: check that mask is correct (leading ones etc) */
	net.s_addr = net.s_addr & mask.s_addr;

	if (!strcmp(argv[argc - 1], "add")) {
		/* ToDo:
		 * 	reject route if there is a route with the same length of the mask
		 * 	We don't allow the similar routes in the kernel
		 */
		return rt_add_route(ifdev->dev, net.s_addr, mask.s_addr, gw.s_addr, ((gw.s_addr == INADDR_ANY) ? RTF_UP : RTF_UP | RTF_GATEWAY));
	} else if (!strcmp(argv[argc - 1], "del")) {
		return rt_del_route(ifdev->dev, net.s_addr, mask.s_addr, gw.s_addr);
	} else {
		printf("Destination     Gateway         Genmask         Flags Iface\n");

		buff[OFFSET_EOS] = '\0';
		rt = rt_fib_get_first();
		while (rt != NULL) {
			memset(&buff[0], ' ', (BUFF_SZ - 1) * sizeof(char));

			/* Get route info */
			net.s_addr = rt->rt_dst;
			gw.s_addr = rt->rt_gateway;
			mask.s_addr = rt->rt_mask;

			/* Parse network */
			str = inet_ntoa(net);
			l = strlen(str);
			memcpy(&buff[OFFSET_DEST], str, (l < LEN_DEST ? l : LEN_DEST) * sizeof(char));

			/* Parse gateway */
			str = (gw.s_addr == INADDR_ANY) ? "*" : inet_ntoa(gw);
			l = strlen(str);
			memcpy(&buff[OFFSET_GW], str, (l < LEN_GW ? l : LEN_GW) * sizeof(char));

			/* Parse mask */
			str = inet_ntoa(mask);
			l = strlen(str);
			memcpy(&buff[OFFSET_NMASK], str, (l < LEN_NMASK ? l : LEN_NMASK) * sizeof(char));


			l = OFFSET_FLAGS;
			if (rt->rt_flags & RTF_UP) {
				buff[l++] = 'U';
			}
			if (rt->rt_flags & RTF_GATEWAY) {
				buff[l++] = 'G';
			}

			str = rt->dev->name;
			l = strlen(str);
			memcpy(&buff[OFFSET_IFACE], str, (l < LEN_IFACE ? l : LEN_IFACE) * sizeof(char));

			printf("%s\n", &buff[OFFSET_START]);  /* print info */

			rt = rt_fib_get_next(rt);
		}
	}
	return 0;
}
