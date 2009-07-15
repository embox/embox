/**
 * \file net_device.h
 *
 * \date Mar 4, 2009
 * \author anton
 */

#ifndef NET_DEVICE_H_
#define NET_DEVICE_H_

#include "arp.h"
#include "mac.h"
#include "ip_v4.h"
#include "icmp.h"
#include "udp.h"

struct sock;

typedef struct _net_packet {
	struct _net_device      *netdev;
	void                    *ifdev;
	struct sock             *sk;
	unsigned short          protocol;
	unsigned int            len;
	union {
		//tcphdr	*th;
		udphdr	        *uh;
		icmphdr	        *icmph;
		//igmphdr	*igmph;
		//iphdr	        *ipiph;
		//ipv6hdr	*ipv6h;
		unsigned char	*raw;
	} h;

	union {
		iphdr	        *iph;
		//ipv6hdr       *ipv6h;
		arphdr	        *arph;
		unsigned char	*raw;
	} nh;

	union {
		machdr          *mach;
	  	unsigned char 	*raw;
	} mac;

	unsigned char data[1518];
}net_packet;

typedef struct _net_device_stats {
	int rx_count;
	int tx_count;
	int rx_err;
	int tx_err;
}net_device_stats;

typedef struct _net_device {
	char          name[6];
	unsigned char hw_addr[6];
	unsigned long state;
	unsigned char type;
	unsigned char addr_len;
	unsigned char flags;

	int (*open)(struct _net_device *dev);
	int (*stop)(struct _net_device *dev);
	int (*hard_start_xmit) (net_packet *pack, struct _net_device *dev);
	int (*hard_header) (net_packet *pack, struct _net_device *dev, unsigned short type, void *daddr, void *saddr, unsigned tot_len);
	int (*rebuild_header)(net_packet * pack);
	net_device_stats *(*get_stats)(struct _net_device *dev);
	int (*set_mac_address)(struct _net_device *dev, void *addr);
}net_device;

net_device *find_net_device(const char *name);
net_device *alloc_etherdev();
void free_etherdev(net_device *dev);

#endif /* NET_DEVICE_H_ */
