/**
 * \file ipconfig.c
 * \date 15.07.09
 * \author sikmir
 * \details  Automatic Configuration of IP -- use DHCP, BOOTP, or
 *  user-supplied information to configure own IP address.
 */

#include "conio/conio.h"
#include "string.h"
#include "common.h"
#include "net/net.h"
#include "net/if_device.h"
#include "misc.h"

/**
 * struct for default config net interfaces
 */
typedef struct _ETH_CONFIG{
    char *iname;
    char *ip;
    char *mac;
} ETH_CONFIG;

/* default configuration */
static const ETH_CONFIG eth_ifs[] = {
#include "ifconfig.inc"
        };

/**
 * initialize protocol's stack
 * now only UDP ARP and so on
 * not support TCP protocol
 */
int net_init() {
    eth_init();
    ifdev_init();

    //TODO: inet_add_protocol
    icmp_init();
    udp_init();
    return 0;
}

/**
 * set default config for net interfaces
 * default config load from ifconfig.inc
 */
int net_config() {
    TRACE("Initializing ifdevs:\n");
    int i, k;
    unsigned char ipaddr[4];
    unsigned char hwaddr[ETH_ALEN];
    char ipbuff[0x20];
    char macbuff[0x20];
    //TODO: bring loopback device up first ("l0" not realized)
    for (k = 0; k < array_len(eth_ifs); k++) {
        if (NULL == ipaddr_scan(eth_ifs[k].ip, ipaddr)) {
            LOG_ERROR("parsing ipadd\n");
            continue;
        }

        if (NULL == macaddr_scan(eth_ifs[k].mac, hwaddr)) {
            LOG_ERROR("parsing mac\n");
            continue;
        }
        ifdev_up(eth_ifs[k].iname);
        ifdev_set_interface((char *)eth_ifs[k].iname, (char *)ipaddr, (char *)hwaddr);
        ipaddr_print(ipbuff, ipaddr);
        macaddr_print(macbuff,hwaddr);
        TRACE("name = %s\tip = %s\tmac = %s\n",eth_ifs[k].iname, ipbuff, macbuff);
    }
    return 0;
}
