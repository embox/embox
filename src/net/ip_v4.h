/**
 * \file ip_v4.h
 *
 * \date Mar 11, 2009
 * \author anton
 */

#ifndef IP_V4_H_
#define IP_V4_H_

#include "net_device.h"

#define IP_PROTOCOL_TYPE (unsigned short)0x0800
#define ICMP_PROTO_TYPE (unsigned short)0x01
#define UDP_PROTO_TYPE (unsigned short)0x11

typedef struct _iphdr
{
	unsigned char version:4, ihl:4;
	unsigned char tos;
	unsigned short tot_len;
	unsigned short id;
	unsigned short frag_off;
	unsigned char ttl;
	unsigned char proto;
	unsigned short check;
	unsigned char saddr[4];
	unsigned char daddr[4];
}iphdr;


//int ip_received_packet(net_packet *pack);

#endif /* IP_V4_H_ */
