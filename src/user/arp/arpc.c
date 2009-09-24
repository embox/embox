/**
 * \file arpc.c
 * \date 24.04.09
 * \author sikmir
 */
#include "shell_command.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"
#include "net/arp.h"

#define COMMAND_NAME     "arp"
#define COMMAND_DESC_MSG "manipulate the system ARP cache"
static const char *help_msg =
	#include "arp_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);

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
	    		if (NULL == (ifdev = ifdev_find_by_name(optarg))) {
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
