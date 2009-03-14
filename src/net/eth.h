/*
 * eth.h
 *
 *  Created on: Mar 5, 2009
 *      Author: anton
 */

#ifndef ETH_H_
#define ETH_H_
#define NET_TYPE_ALL_PROTOCOL 0



#define IP_PROTOCOL_TYPE (unsigned short)0x0800
/*
typedef struct _IPV4_HEADER
{
	unsigned char dest_addr[4];
	unsigned char flags;//broad_cast
	unsigned char src_addr[4];

}IPV4_HEADER;*/
typedef void (*ETH_LISTEN_CALLBACK)(void * pack);
int eth_init();
int eth_send (net_packet *pack);
int eth_listen (void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback);
unsigned char *eth_get_ipaddr(void *handler);
net_device *eth_get_netdevice(void *handler);

#endif /* ETH_H_ */
