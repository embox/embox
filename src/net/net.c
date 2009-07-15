/**
 * \file net.c
 * \date 15.07.09
 * \author sikmir
 */

#include "common.h"
#include "misc.h"

typedef struct {
        char *iname;
        char *ip;
        char *mac ;
} ETH_CONFIG;

static ETH_CONFIG eth_ifs[] = {
	#include "ifconfig.inc"
};

int net_init() {
	eth_init();
        TRACE("Initializing ifdevs:\n");
        int i, k;
        unsigned char ipaddr[4];
        unsigned char hwaddr[6];
        for (k = 0; k < array_len(eth_ifs); k++) {
    		sscanf(eth_ifs[k].iname, "eth%d", &i);
    		ipaddr_scan(eth_ifs[k].ip, ipaddr);
    		macaddr_scan(eth_ifs[k].mac, hwaddr);
                eth_up(i, ipaddr, hwaddr);
        }
        icmp_init();
        udp_init();
}
