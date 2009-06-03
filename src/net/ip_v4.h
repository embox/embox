/**
 * \file ip_v4.h
 *
 * \date Mar 11, 2009
 * \author anton
 */

#ifndef IP_V4_H_
#define IP_V4_H_

#define IP_PROTOCOL_TYPE (unsigned short)0x0800
#define ICMP_PROTO_TYPE (unsigned short)0x01
#define UDP_PROTO_TYPE (unsigned short)0x11

typedef struct _iphdr {
	__extension__ unsigned char version:4, ihl:4; /* version = 4, ihl >= 5 */
	unsigned char tos;
	unsigned short tot_len;
	unsigned short id;
	unsigned short frag_off;
	unsigned char ttl;
	unsigned char proto;
	unsigned short check;
	unsigned char saddr[4];
	unsigned char daddr[4];
} __attribute__((packed)) iphdr;

#define IP_HEADER_SIZE	(sizeof(iphdr))

int ip_received_packet(struct _net_packet *pack);

#endif /* IP_V4_H_ */
