/**
 * \file arpc.c
 * \date 24.04.09
 * \author sikmir
 */

#include "types.h"
#include "shell_command.h"
#include "net/skbuff.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"
#include "net/arp.h"
#include "misc.h"

#define COMMAND_NAME     "arp"
#define COMMAND_DESC_MSG "manipulate the system ARP cache"
#define HELP_MSG         "Usage: arp [-i if] [-s|d] [-a host] [-m hwaddr] [-h]"
static const char *man_page =
	#include "arp_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int print_arp_cache(void *ifdev) {
	int i;
	char ip[15], mac[18];
	net_device_t *net_dev;
	for(i=0; i<ARP_CACHE_SIZE; i++) {
		if((arp_table[i].is_busy == 1) &&
		   (ifdev == NULL || ifdev == arp_table[i].if_handler)) {
			net_dev = inet_dev_get_netdevice(arp_table[i].if_handler);
			ipaddr_print(ip, arp_table[i].pw_addr);
			macaddr_print(mac, arp_table[i].hw_addr);
			TRACE("%s\t\t%d\t%s\t%d\t%s\n", ip, inet_dev_get_netdevice(arp_table[i].if_handler)->type,
							 mac, net_dev->flags, net_dev->name);
		}
	}
	return 0;
}

static int exec(int argsc, char **argsv) {
	int nextOption;
	unsigned char addr[4];
	unsigned char hwaddr[6];
	void *ifdev = NULL;
	int op = -1;
	getopt_init();
	do {
	        nextOption = getopt(argsc, argsv, "hdsa:m:i:");
	        switch(nextOption) {
	        case 'h':
	    		show_help();
	    		return 0;
	    	case 'd':
	    		op = 0;
	    	        break;
	    	case 's':
	    		op = 1;
	    	        break;
	    	case 'a':
	    	        if (NULL == ipaddr_scan(optarg, addr)) {
	    	                LOG_ERROR("wrong ip addr format (%s)\n", optarg);
	    	                return -1;
	    	        }
	    	        break;
	    	case 'm':
	    	        if (NULL == macaddr_scan(optarg, hwaddr)) {
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
	} while(-1 != nextOption);

	switch(op) {
        case 0:
                arp_delete_entity(ifdev, addr, hwaddr);
                return 0;
        case 1:
                arp_add_entity(ifdev, addr, hwaddr);
                return 0;
	default:
		break;
	}
	printf("Address\t\t\tHWtype\tHWaddress\t\tFlags\tIface\n");
	print_arp_cache(ifdev);
	return 0;
}
