/**
 * @file
 * @brief Similar to the standard Unix route.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <net/route.h>
#include <net/inetdevice.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: route [-nmgdh] [add|del]\n");
}

static int exec(int argc, char **argv) {
	int opt;
	in_device_t *ifdev = NULL;
	in_addr_t net  = INADDR_ANY;
	in_addr_t mask = INADDR_ANY;
	in_addr_t gw   = INADDR_ANY;
	struct rt_entry *rt;
	getopt_init();
	do {
		opt = getopt(argc, argv, "n:m:d:g:h");
		switch(opt) {
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
			gw = inet_addr(optarg);
			break;
		case 'd':
			if (NULL == (ifdev = inet_dev_find_by_name(optarg))) {
				TRACE("route: unknown iface %s\n", optarg);
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	if (!strcmp(argv[argc - 1], "add")) {
		in_device_t *idev = ifdev;
		int flags = (gw == INADDR_ANY) ? RTF_UP : RTF_UP|RTF_GATEWAY;
		rt_add_route(idev->dev, net, mask, gw, flags);
		return 0;
	} else if (!strcmp(argv[argc - 1], "del")) {
		in_device_t *idev = ifdev;
		rt_del_route(idev->dev, net, mask, gw);
	} else {
		printf("Destination\t\tGateway   \t\tGenmask   \t\tFlags\tIface\n");
		rt = rt_fib_get_first();
		while (rt != NULL) {
			struct in_addr net_addr, mask_addr, gw_addr;
			char *s_net, *s_mask, *s_gw;
			net_addr.s_addr = rt->rt_dst;
			s_net = inet_ntoa(net_addr);
			printf("%s  \t\t", s_net);
			free(s_net);
			gw_addr.s_addr = rt->rt_gateway;
			s_gw = inet_ntoa(gw_addr);
			printf("%s  \t\t", s_gw);
			free(s_gw);
			mask_addr.s_addr = rt->rt_mask;
			s_mask = inet_ntoa(mask_addr);
			printf("%s  \t\t", s_mask);
			free(s_mask);
			if (rt->rt_flags & RTF_UP) {
				printf("U");
			}
			if (rt->rt_flags & RTF_GATEWAY) {
				printf("G");
			}
			if (rt->rt_flags & RTF_REJECT) {
				printf("R");
			}
			printf("\t%s\n", rt->dev->name);
			rt = rt_fib_get_next();
		}
	}
	return 0;
}
