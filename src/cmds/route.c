/**
 * @file
 * @brief Similar to the standard Unix route.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
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
	size_t l, iterator;
	in_addr_t net, mask, gw;
	struct in_addr addr;
	char buff[BUFF_SZ], *str;

	ifdev = NULL;
	net = mask = gw = INADDR_ANY;
	getopt_init();
	while ((opt = getopt(argc, argv, "n:m:d:g:h")) != -1) {
		switch ((char)opt) {
		case 'h':
			print_usage();
			return 0;
		case 'n':
			if ((net = inet_addr(optarg)) == INADDR_NONE) {
				printf("Unknown host\n");
				return -1;
			}
			break;
		case 'm':
			if ((mask = inet_addr(optarg)) == INADDR_NONE) {
				printf("Unknown mask\n");
				return -1;
			}
			break;
		case 'g':
			if ((gw = inet_addr(optarg)) == INADDR_NONE) {
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
		default:
			return 0;
		}
	}

	if (!strcmp(argv[argc - 1], "add")) {
		rt_add_route(ifdev->dev, net, mask, gw, ((gw == INADDR_ANY) ? RTF_UP : RTF_UP | RTF_GATEWAY));
		return 0;
	} else if (!strcmp(argv[argc - 1], "del")) {
		rt_del_route(ifdev->dev, net, mask, gw);
	} else {
		printf("Destination     Gateway         Genmask         Flags Iface\n");

		buff[OFFSET_EOS] = '\0';
		rt = rt_fib_get_first(&iterator);
		while (rt != NULL) {
			memset(&buff[0], ' ', (BUFF_SZ - 1) * sizeof(char));

			addr.s_addr = rt->rt_dst;
			str = inet_ntoa(addr);
			l = strlen(str);
			memcpy(&buff[OFFSET_DEST], str, (l < LEN_DEST ? l : LEN_DEST) * sizeof(char));


			addr.s_addr = rt->rt_gateway;
			str = (addr.s_addr == INADDR_ANY) ? "*" : inet_ntoa(addr);
			l = strlen(str);
			memcpy(&buff[OFFSET_GW], str, (l < LEN_GW ? l : LEN_GW) * sizeof(char));

			addr.s_addr = rt->rt_mask;
			str = inet_ntoa(addr);
			l = strlen(str);
			memcpy(&buff[OFFSET_NMASK], str, (l < LEN_NMASK ? l : LEN_NMASK) * sizeof(char));


			l = OFFSET_FLAGS;
			if (rt->rt_flags & RTF_UP) {
				buff[l++] = 'U';
			}
			if (rt->rt_flags & RTF_GATEWAY) {
				buff[l++] = 'G';
			}
			if (rt->rt_flags & RTF_REJECT) {
				buff[l++] = 'R';
			}

			str = rt->dev->name;
			l = strlen(str);
			memcpy(&buff[OFFSET_IFACE], str, (l < LEN_IFACE ? l : LEN_IFACE) * sizeof(char));

			printf("%s\n", &buff[OFFSET_START]);  /* print info */

			rt = rt_fib_get_next(&iterator);
		}
	}
	return 0;
}
