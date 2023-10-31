/**
 * @file
 * @brief Similar to the standard Unix route.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 * @author Denis Deryugin
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <arpa/inet.h>

#include <net/l3/route.h>
#include <net/inetdevice.h>
#include "stdlib.h"

#define FLAG_N 0x01

enum route_action {ADD, DEL};
enum target_type {NET, HOST};

static int route_show(int flags) {
	struct rt_entry *rt;

	printf("Destination     Gateway         Genmask         Flags  Iface\n");

	for (rt = rt_fib_get_first(); rt != NULL; rt = rt_fib_get_next(rt)) {
		printf("%-15s ", (rt->rt_dst != INADDR_ANY) || (flags & FLAG_N)
				? inet_ntoa(*(struct in_addr *)&rt->rt_dst) : "default");

		printf("%-15s ", (rt->rt_gateway != INADDR_ANY) || (flags & FLAG_N)
				? inet_ntoa(*(struct in_addr *)&rt->rt_gateway) : "*");

		printf("%-15s ", inet_ntoa(*(struct in_addr *)&rt->rt_mask));

		printf("%c", rt->rt_flags & RTF_UP ? 'U' : ' ');
		printf("%c", rt->rt_flags & RTF_GATEWAY ? 'G' : ' ');
		printf("%c", rt->rt_mask == INADDR_BROADCAST  ? 'H' : ' ');
		printf("   ");

		printf(" %5s", rt->dev->name);

		printf("\n");
	}

	return 0;
}

static void print_help() {
	printf("route [-hn] [-A family] {add|del} <target> [gw <Gw]"
					"[netmask <Nm>] [[dev] If]\n");
}

int main(int argc, char **argv) {
	struct net_device *netdev = NULL;
	enum route_action rt_act;
	in_addr_t target, netmask = INADDR_ANY, gw = INADDR_ANY;
	enum target_type tar_t = NET;
	int d_flags = 0x0, i;
	char **tmp;


	for (tmp = argv; *tmp != NULL; tmp++) {
		if (!strcmp(*tmp, "-net") || !strcmp(*tmp, "-host")) {
			*tmp[0] = '#';
		}
	}

	while ((i = getopt(argc, argv, "A:nFV")) != EOF) {
		switch (i) {
			case 'F':
				break;
			case 'A':
				if (strcmp(optarg, "inet")) {
					/* NIY */
					return -EINVAL;
				}
				break;
			case 'n':
				d_flags |= FLAG_N;
				break;
			default:
				print_help();
				return 0;
		}
	}

	argv += optind, argc -= optind;

	if (!argc) {
		return route_show(d_flags);
	}

	if (!strcmp(argv[0], "add")) {
		rt_act = ADD;
	} else if (!strcmp(argv[0], "del")) {
		rt_act = DEL;
	} else {
		print_help();
		return -EINVAL;
	}

	argc--, argv++;

	if (argc) {
		if (!strcmp(argv[0], "#net")) {
			tar_t = NET;
			argc--, argv++;
		} else if (!strcmp(argv[0], "#host")) {
			tar_t = HOST;
			argc--, argv++;
		}
	}

	if (!argc) {
		printf("Missing target.\n");
		return -EINVAL;
	}

	/* Check if the address contains mask and save netmask if given */
	char *slash = strchr(argv[0], '/');
	if (slash != NULL) {
		char *endptr;
		*slash++ = '\0';
		errno = 0;
		int preflen = strtoul(slash, &endptr, 10);
		if ((errno) || (preflen < 0) || (preflen > 32) || (*endptr)) {
			printf("Bad prefixlen or CIDR notation\n");
			return -EINVAL;
		}
		netmask = htonl(~(0xffffffffU >> preflen));
	}

	if (!strcmp(argv[0], "default")) {
		target = INADDR_ANY;
	} else {
		target = inet_addr(argv[0]);
		if (target == INADDR_BROADCAST && strcmp(argv[0], "255.255.255.255")) {
			printf("Unknown target: %s.\n", argv[0]);
			return -EINVAL;
		}
	}
	argv++, argc--;

	if (argc && !strcmp(argv[0], "netmask")) {
		argv++, argc--;
		if (!argc) {
			printf("Missing netmask.\n");
			return -EINVAL;
		}
		netmask = inet_addr(argv[0]);
		if (netmask == INADDR_BROADCAST &&strcmp(argv[0], "255.255.255.255")) {
			printf("Unknown netmask: %s.\n", argv[0]);
			return -EINVAL;
		}

		if ((tar_t == NET && ~netmask & target) || (tar_t == HOST && netmask != INADDR_BROADCAST)) {
			printf("Mask %s does not fit target.\n", argv[0]);
			return -EINVAL;
		}

		if (~htonl(netmask) & (~htonl(netmask) + 1)) {
			printf("Netmask is invalid.\n");
			return -EINVAL;
		}

		argv++, argc--;
	}

	if (argc && !strcmp(argv[0], "gw")) {
		argv++, argc--;
		if (!argc) {
			printf("Missing gateway.\n");
			return -EINVAL;
		}

		if (!strcmp(argv[0], "default")) {
			gw = INADDR_ANY;
		} else {
			gw = inet_addr(argv[0]);
			if (gw == INADDR_BROADCAST && strcmp(argv[0], "255.255.255.255")) {
				printf("Unknown gateway: %s.\n", argv[0]);
				return -EINVAL;
			}
		}
		argv++, argc--;
	}

	if (argc) {
		if (!strcmp(argv[0], "dev")) {
			argc--, argv++;
		}

		if (argc) {
			struct in_device *iface = inetdev_get_by_name(argv[0]);
			if (iface == NULL) {
				printf("Unknown interface: %s.\n", argv[0]);
				return -ENODEV;
			}
			netdev = iface->dev;
		} else {
			printf("Missing device.\n");
		}
	}

	if (rt_act == ADD) {
		return rt_add_route(netdev, target, netmask, gw,
					gw == INADDR_ANY ? RTF_UP : RTF_UP | RTF_GATEWAY);
	} else {
		return rt_del_route(netdev, target, netmask, gw);
	}
}

