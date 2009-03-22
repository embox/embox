/*
 * arp.h
 *
 *  Created on: Mar 11, 2009
 *      Author: anton
 */

#ifndef ARP_H_
#define ARP_H_

typedef struct _arphdr
{
	unsigned short htype;// = 0x0001;//ethernet
	unsigned short ptype;// = 0x0800;//ip
	unsigned char hlen;// hardware addr len
	unsigned char plen;// protocol addr len
	unsigned short oper;//1 req 2 resp
	unsigned char sha[6];//Sender hardware address
	unsigned char spa[4];//Sender protocol address
	unsigned char tha[6];//Target hardware address
	unsigned char tpa[4];//Target protocol address
}arphdr;
#define ARP_PROTOCOL_TYPE ((unsigned short)0x0806)
typedef struct _ARP_RESULT
{
	void *if_handler;
	unsigned char *hw_dstaddr;
}ARP_RESULT;

struct _net_packet;
struct _net_packet *arp_resolve_addr (struct _net_packet * pack, unsigned char dst_addr[4]);
int arp_received_packet(struct _net_packet *pack);

#endif /* ARP_H_ */
