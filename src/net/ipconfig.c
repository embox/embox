/**
 * @file ipconfig.c
 *
 * @details Automatic Configuration of IP -- use DHCP, BOOTP,
 * or user-supplied information to configure own IP address.
 * @date 15.07.09
 * @author Nikolay Korotky
 */

#include "net/net.h"
#include "net/inetdevice.h"
#include "kernel/init.h"

DECLARE_INIT("net", net_init, INIT_NET_LEVEL);

/**
 * initialize protocol's stack
 * now only UDP ARP and so on
 * not support TCP protocol
 */
static int net_init() {
    //TODO:
    eth_init();
    inet_dev_init();
    return 0;
}
