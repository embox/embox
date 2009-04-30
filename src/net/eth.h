/**
 * \file eth.h
 *
 * \date Mar 5, 2009
 * \author anton
 */

#ifndef ETH_H_
#define ETH_H_
#define NET_TYPE_ALL_PROTOCOL 0

#include "net_device.h"


typedef void (*ETH_LISTEN_CALLBACK)(void * pack);
/**
 * Init ethernet.
 * @return count founded devices.
 */
int eth_init();
void *eth_get_ifdev_by_name(const char *if_name);

/**
 * Show interace (IP/MAC address)
 */

/**
 * Set MAC address (hw,sw)
 */
int eth_set_macaddr (void *ifdev, unsigned char macaddr[6]);

/**
 * Set IP address (sw)
 */
int eth_set_ipaddr (void *ifdev, unsigned char ipaddr[4]);

/**
 * Print IP address
 */

/**
 * Print MAC-address
 */
void macaddr_print(unsigned char *addr, int len);

/**
 * Send Ethernet packet
 */
int eth_send (net_packet *pack);
int eth_listen (void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback);
unsigned char *eth_get_ipaddr(void *handler);
net_device *eth_get_netdevice(void *handler);

#endif /* ETH_H_ */
