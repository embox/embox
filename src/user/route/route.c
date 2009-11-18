/**
 * \file route.c
 * \date 16.11.09
 * \author sikmir
 */

#include "shell_command.h"
#include "net/route.h"
#include "net/if_device.h"

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
	unsigned char net[4]  = {0x0,0x0,0x0,0x0};
	unsigned char mask[4] = {0x0,0x0,0x0,0x0};
	unsigned int  n_mask  = 0;
	unsigned char gw[4]   = {0x0,0x0,0x0,0x0};
	struct rt_entry *rt;
	char _net[15], _mask[15], _gw[15];
        getopt_init();
        do {
                nextOption = getopt(argsc, argsv, "n:m:d:g:h");
                switch(nextOption) {
                case 'h':
                        show_help();
                        return 0;
		case 'n':
			if (NULL == ipaddr_scan(optarg, net)) {
		                LOG_ERROR("wrong ip addr format (%s)\n", optarg);
		                show_help();
		                return -1;
		        }
			break;
		case 'm':
			if( (NULL == ipaddr_scan(optarg, mask)) ||
			    (1 != sscanf(optarg, "%d", &n_mask)) ) {
		                LOG_ERROR("wrong mask format (%s)\n", optarg);
		                show_help();
		                return -1;
		        }
			break;
		case 'g':
		        if (NULL == ipaddr_scan(optarg, gw)) {
		                LOG_ERROR("wrong ip addr format (%s)\n", optarg);
		                show_help();
		                return -1;
		        }
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
		inet_device_t *idev = (struct inet_device*)ifdev;
		rt_add_route(idev->net_dev, net, mask, gw);
		return 0;
	} else if(!strcmp(argsv[argsc - 1], "del")) {
		inet_device_t *idev = (struct inet_device*)ifdev;
		rt_del_route(idev->net_dev, net, mask, gw);
	} else {
		printf("Destination\t\tGateway\t\tGenmask   \t\tIface\n");
		rt = rt_fib_get_first();
		while(rt != NULL) {
			ipaddr_print(_net, rt->rt_dst);
		        ipaddr_print(_mask, rt->rt_mask);
		        ipaddr_print(_gw, rt->rt_gateway);
		        printf("%s\t\t%s\t\t%s\t\t%s\n", _net, _gw, _mask, rt->dev->name);
			rt = rt_fib_get_next();
		}
	}
	return 0;
}
