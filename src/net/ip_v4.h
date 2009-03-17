/*
 * ip_v4.h
 *
 *  Created on: Mar 11, 2009
 *      Author: anton
 */

#ifndef IP_V4_H_
#define IP_V4_H_
typedef struct _iphdr
{
	unsigned char header_size;
	unsigned char type;
	unsigned short len;
	unsigned short frame_id;
	unsigned short frame_offset;
	unsigned char ttl;
	unsigned char proto;
	unsigned short header_check_summ;
	unsigned char src_addr[4];
	unsigned char dst_addr[4];
	unsigned char*raw;
}iphdr;


int ip_received_packet(net_packet *pack);

#endif /* IP_V4_H_ */
