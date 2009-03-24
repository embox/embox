/*
 * eth.h
 *
 *  Created on: Mar 5, 2009
 *      Author: anton
 */

#ifndef ETH_H_
#define ETH_H_
#define NET_TYPE_ALL_PROTOCOL 0

#include "net_device.h"


typedef void (*ETH_LISTEN_CALLBACK)(void * pack);
int eth_init();
int eth_set_macaddr (void *ifdev, unsigned char macaddr[6]);
int eth_set_ipaddr (void *ifdev, unsigned char ipaddr[4]);
void macaddr_print(unsigned char *addr, int len);
int eth_send (net_packet *pack);
int eth_listen (void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback);
unsigned char *eth_get_ipaddr(void *handler);
net_device *eth_get_netdevice(void *handler);

#endif /* ETH_H_ */
