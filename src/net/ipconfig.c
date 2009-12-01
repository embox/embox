/**
 * @file ipconfig.c
 * @date 15.07.09
 * @author Nikolay Korotky
 * @details Automatic Configuration of IP -- use DHCP, BOOTP, or
 *  user-supplied information to configure own IP address.
 */

#include "common.h"
#include "net/net.h"
#include "net/if_device.h"
#include "misc.h"
#include "kernel/init.h"

DECLARE_INIT("net", net_init, INIT_NET_LEVEL);

/**
 * initialize protocol's stack
 * now only UDP ARP and so on
 * not support TCP protocol
 */
static int net_init() {
    eth_init();
    inet_dev_init();

    //TODO: inet_add_protocol
    icmp_init();
    udp_init();
    return 0;
}
