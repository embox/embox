/**
 * \file icmp.h
 *
 * \date 14.03.2009
 * \author sunnya
 */

#ifndef ICMP_H_
#define ICMP_H_

struct _net_packet;
typedef void (*ICMP_CALLBACK)(struct _net_packet* response);
typedef struct _icmphdr {
	unsigned char type;
	unsigned char code;
	unsigned short header_check_summ;
	unsigned char data[0x40];
}icmphdr;

#endif /* ICMP_H_ */
