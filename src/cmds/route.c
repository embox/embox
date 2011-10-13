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

#define OFFSET_START   0
#define OFFSET_DEST    (0 + OFFSET_START)
#define OFFSET_GATEWAY (16 + OFFSET_DEST)
#define OFFSET_GENMASK (16 + OFFSET_GATEWAY)
#define OFFSET_FLAGS   (16 + OFFSET_GENMASK)
#define OFFSET_IFACE   (6 + OFFSET_FLAGS)
#define OFFSET_EOS     (6 + OFFSET_IFACE)
#define BUFF_SZ        (1 + OFFSET_EOS)

static void print_usage(void) {
	printf("Usage: route [-nmgdh] [add|del]\n");
}

static int exec(int argc, char *argv[]) {
	int i, opt;
	in_device_t *ifdev;
	struct rt_entry *rt;
	size_t iterator;
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
		/* set end of string */
		buff[OFFSET_EOS] = '\0';

		memset(&buff[OFFSET_START], ' ', OFFSET_EOS); /* clear string */
		memcpy(&buff[OFFSET_DEST], "Destination", 11);
		memcpy(&buff[OFFSET_GATEWAY], "Gateway", 7);
		memcpy(&buff[OFFSET_GENMASK], "Genmask", 7);
		memcpy(&buff[OFFSET_FLAGS], "Flags", 5);
		memcpy(&buff[OFFSET_IFACE], "Iface", 5);

		printf("%s\n", &buff[OFFSET_START]);  /* print title */

		rt = rt_fib_get_first(&iterator);
		while (rt != NULL) {
			memset(&buff[OFFSET_START], 0, OFFSET_EOS); /* clear string */

			addr.s_addr = rt->rt_dst;
			str = inet_ntoa(addr);
			strncpy(&buff[OFFSET_DEST], str, strlen(str));


			addr.s_addr = rt->rt_gateway;
			str = inet_ntoa(addr);
			strncpy(&buff[OFFSET_GATEWAY], str, strlen(str));

			addr.s_addr = rt->rt_mask;
			str = inet_ntoa(addr);
			strncpy(&buff[OFFSET_GENMASK], str, strlen(str));


			i = OFFSET_FLAGS;
			if (rt->rt_flags & RTF_UP) {
				buff[i++] = 'U';
			}
			if (rt->rt_flags & RTF_GATEWAY) {
				buff[i++] = 'G';
			}
			if (rt->rt_flags & RTF_REJECT) {
				buff[i++] = 'R';
			}

			str = rt->dev->name;
			memcpy(&buff[OFFSET_IFACE], str, strlen(str));

			printf("%s\n", &buff[OFFSET_START]);  /* print info */

			rt = rt_fib_get_next(&iterator);
		}
	}
	return 0;
}
