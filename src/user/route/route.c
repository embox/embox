/**
 * \file route.c
 * \date 16.11.09
 * \author sikmir
 */

#include "shell_command.h"
#include "in.h"
#include "net/route.h"
#include "net/inetdevice.h"

#define COMMAND_NAME     "route"
#define COMMAND_DESC_MSG "manipilate routing table"
#define HELP_MSG         "Usage: route [-nmgdh] [add|del]"
static const char *man_page =
	#include "route_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;
	void *ifdev;
	in_addr_t net  = INADDR_ANY;
	in_addr_t mask = INADDR_ANY;
	in_addr_t gw   = INADDR_ANY;
	struct rt_entry *rt;
        getopt_init();
        do {
                nextOption = getopt(argsc, argsv, "n:m:d:g:h");
                switch(nextOption) {
                case 'h':
                        show_help();
                        return 0;
		case 'n':
		        net = inet_addr(optarg);
		        break;
		case 'm':
		        mask = inet_addr(optarg);
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
	} while(-1 != nextOption);

	if(!strcmp(argsv[argsc - 1], "add")) {
		in_device_t *idev = (struct in_device*)ifdev;
		int flags = (gw == INADDR_ANY) ? RTF_UP : RTF_UP|RTF_GATEWAY;
		rt_add_route(idev->dev, net, mask, gw, flags);
		return 0;
	} else if(!strcmp(argsv[argsc - 1], "del")) {
		in_device_t *idev = (struct in_device*)ifdev;
		rt_del_route(idev->dev, net, mask, gw);
	} else {
		printf("Destination\t\tGateway   \t\tGenmask   \t\tFlags\tIface\n");
		rt = rt_fib_get_first();
		while(rt != NULL) {
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
			if(rt->rt_flags & RTF_UP) {
				printf("U");
			}
			if(rt->rt_flags & RTF_GATEWAY) {
				printf("G");
			}
			if(rt->rt_flags & RTF_REJECT) {
				printf("R");
			}
			printf("\t%s\n", rt->dev->name);
			rt = rt_fib_get_next();
		}
	}
	return 0;
}
