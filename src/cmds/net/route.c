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
#include <unistd.h>
#include <net/l3/route.h>
#include <net/inetdevice.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/array.h>
#include <errno.h>
#include <arpa/inet.h>

EMBOX_CMD(exec);

struct route_args {
	char with_a;
	char with_n;
	char with_add_or_del; char add;
	char with_tar; struct in_addr tar;
	char with_net_or_host; char net;
	char with_netmask; struct in_addr netmask;
	char with_gw; struct in_addr gw;
	char with_iface; char iface[IFNAMSIZ];
};

static int route_args_not_empty(struct route_args *args) {
	return args->with_add_or_del || args->with_tar || args->with_net_or_host
			|| args->with_netmask || args->with_gw || args->with_iface;
}

static int route_show_all(struct route_args *args) {
	struct rt_entry *rt;

	printf("Destination     Gateway         Genmask         Flags Iface\n");

	for (rt = rt_fib_get_first(); rt != NULL; rt = rt_fib_get_next(rt)) {
		printf("%-15s ", (rt->rt_dst != INADDR_ANY) || args->with_n
				? inet_ntoa(*(struct in_addr *)&rt->rt_dst) : "default");

		printf("%-15s ", (rt->rt_gateway != INADDR_ANY) || args->with_n
				? inet_ntoa(*(struct in_addr *)&rt->rt_gateway) : "*");

		printf("%-15s ", inet_ntoa(*(struct in_addr *)&rt->rt_mask));

		printf("%c", rt->rt_flags & RTF_UP ? 'U' : ' ');
		printf("%c", rt->rt_flags & RTF_GATEWAY ? 'G' : ' ');
		printf("   ");

		printf(" %5s", rt->dev->name);

		printf("\n");
	}

	return 0;
}

static int exec(int argc, char **argv) {
	int i;
	struct in_device *iface;
	struct route_args args;

	memset(&args, 0, sizeof args);

	for (i = 1; i < argc; ++i) {
		if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
			printf("%s [-hn] [-A family] {add|del} <target> [gw <Gw]"
					" [netmask <Nm>] [[dev] If]\n",
					argv[0]);
			return 0;
		}
		else if (!strcmp("-A", argv[i])) {
			args.with_a = 1;
			if (strcmp("inet", argv[++i])) {
				return -EINVAL;
			}
		}
		else if (!strcmp("-n", argv[i])) args.with_n = 1;
		else if (!args.with_add_or_del) {
			args.with_add_or_del = 1;
			if (strcmp("add", argv[i]) && strcmp("del", argv[i])) {
				printf("%s: invalid operation\n", argv[i]);
				return -EINVAL;
			}
			args.add = argv[i][0] == 'a';
		}
		else if (!strcmp("-net", argv[i])) {
			args.with_net_or_host = 1;
			args.net = argv[i][1] == 'n';
		}
		else if (!strcmp("-host", argv[i])) {
			args.with_net_or_host = args.with_netmask = 1;
			args.net = argv[i][1] == 'n';
			inet_aton("255.255.255.255", &args.netmask);
		}
		else if (!args.with_tar) {
			args.with_tar = 1;
			if (!strcmp("default", argv[i])) {
				args.tar.s_addr = INADDR_ANY;
			}
			else if (!inet_aton(argv[i], &args.tar)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("netmask", argv[i])) {
			args.with_netmask = 1;
			if (args.net == 0 && strcmp(argv[i + 1], "255.255.255.255")) {
				printf("Non-32 mask for host does not make any sense\n");
				return -EINVAL;
			}

			if (!inet_aton(argv[++i], &args.netmask)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("gw", argv[i])) {
			args.with_gw = 1;
			if (!inet_aton(argv[++i], &args.gw)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!args.with_iface) {
			args.with_iface = 1;
			if (!strcmp("dev", argv[i])) ++i;
			strncpy(&args.iface[0], argv[i], ARRAY_SIZE(args.iface));
		}
		else {
			printf("%s: unknown argument\n", argv[i]);
			return -EINVAL;
		}
	}

	if (args.with_iface) {
		iface = inetdev_get_by_name(&args.iface[0]);
		if (iface == NULL) {
			printf("%s: unknown interface\n", &args.iface[0]);
			return -ENODEV;
		}
	}
	else {
		iface = NULL;
	}

	if (args.with_add_or_del && !args.with_tar) {
		return -EINVAL;
	}

	return !route_args_not_empty(&args) ? route_show_all(&args)
			: args.add ? rt_add_route(iface != NULL ? iface->dev : NULL,
				args.tar.s_addr, args.netmask.s_addr, args.gw.s_addr,
				args.gw.s_addr == INADDR_ANY ? RTF_UP : RTF_UP | RTF_GATEWAY)
			: rt_del_route(iface != NULL ? iface->dev : NULL, args.tar.s_addr, args.netmask.s_addr,
				args.gw.s_addr);
}
