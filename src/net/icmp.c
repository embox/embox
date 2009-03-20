/*
 * icmp.c
 *
 *  Created on: 14.03.2009
 *      Author: sunnya
 */

#include "eth.h"
#include "icmp.h"
//#include "ip_v4.h"

#define ICMP_TYPE_ECHO_REQ  8
#define ICMP_TYPE_ECHO_RESP 0


typedef struct _ICMP_CALLBACK_INFO
{
	ICMP_CALLBACK cb;
	void *ifdev;
	unsigned short ip_id;
	unsigned char type;
}ICMP_CALLBACK_INFO;
ICMP_CALLBACK_INFO cb_info[0x10];
static unsigned short ip_id;

static int callback_alloc (	ICMP_CALLBACK cb, void *ifdev, unsigned short ip_id, unsigned char type)
{
	int i;
	if (NULL == cb || NULL == ifdev)
		return -1;
	for (i = 0; i < sizeof (cb_info)/sizeof (cb_info); i ++ )
	{
		if(NULL == cb)
		{
			cb_info[i].cb = cb;
			cb_info[i].ifdev = ifdev;
			cb_info[i].ip_id = ip_id;
			cb_info[i].type = type;
			return i;
		}
	}
	return -1;
}

static int interface_abort (void *ifdev)
{
	int i;
	if (NULL == ifdev)
		return -1;
	for (i = 0; i < sizeof (cb_info)/sizeof (cb_info); i ++ )
	{
		if(cb_info[i].ifdev == ifdev)
		{
			cb_info[i].cb = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type = 0;
			return i;
		}
	}
	return -1;
}

static int callback_free (ICMP_CALLBACK cb, void *ifdev, unsigned short ip_id, unsigned char type)
{
	int i;
	if (NULL == cb)
		return -1;
	for (i = 0; i < sizeof (cb_info)/sizeof (cb_info); i ++ )
	{
		if(cb_info[i].cb == cb && ifdev == cb_info[i].ifdev && cb_info[i].ip_id == ip_id && cb_info[i].type)
		{
			cb_info[i].cb = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type = 0;
			return i;
		}
	}
	return -1;
}

typedef int (*PACKET_HANDLER)(net_packet *pack);

PACKET_HANDLER received_packet_handlers[256];
//PACKET_HANDLER send_packet_handlers[256];

/*
//returns function-handler
PACKET_HANDLER received_packet_handler(unsigned char type){
	return received_packet_handlers[type];
}

PACKET_HANDLER send_packet_handler(unsigned char type){
	return send_packet_handlers[type];
}
ICMP_CALLBACK current_callback;
*/

int icmp_abort_echo_request(void *ifdev) {
//	current_callback = NULL;
	interface_abort(ifdev);
}
static unsigned short calc_checksumm (icmphdr *hdr)
{
	return 0;
}
static inline net_packet *build_icmp_packet(net_packet *pack, unsigned char type, unsigned char code, unsigned char srcaddr[4], unsigned char dstaddr[4])
{
	pack->h.raw = pack->nh.raw = pack->data + (pack->netdev->addr_len * 2 + 2) + sizeof (iphdr);
	memset (pack->h.raw, 0, sizeof (icmphdr));

	//fill icmp header

	pack->h.icmph->type = type;
	pack->h.icmph->code = code;
	pack->h.icmph->header_check_summ = calc_checksumm;
	//fill ip header
	pack->nh.raw = pack->nh.raw = pack->data + (pack->netdev->addr_len * 2 + 2);
	iphdr->header_size = sizeof (iphdr);
	memcpy(iphdr->dst_addr, dstaddr, sizeof(iphdr->dst_addr));
	memcpy(iphdr->src_addr, srcaddr, sizeof(iphdr->src_addr));
}

//implementation handlers for received msgs
int icmp_get_echo_answer(net_packet *pack, ICMP_CALLBACK callback) { //type 0

	if (NULL == current_callback)
		return -1;
	current_callback(pack);
	//unregister
	current_callback = NULL;
	return 0;
}

int icmp_get_echo_request(net_packet *pack, ICMP_CALLBACK callback) { //type 8
//TODO copy pack

	icmphdr *icmphdr;
	icmphdr->type = 0;
	icmphdr->code = 0;
	icmphdr->header_check_summ = 0;
	icmphdr->raw= pack->h.icmph->raw;
	iphdr *iphdr = pack->nh->iph;
	//!!change adresses
	icmp_send_packet(icmphdr, iphdr->dst_addr, iphdr->src_addr, callback);
}


int icmp_send_echo_request(void *ifdev, unsigned char dstaddr[4], ICMP_CALLBACK callback) { //type 8

	net_packet *pack = net_packet_alloc();
	if (pack == 0)
		return -1;

	pack->ifdev = ifdev;
	pack->netdev = eth_get_netdevice(ifdev);
	pack->len = build_icmp_packet(pack, eth_get_ipaddr(ifdev), dstaddr, type);

	if (callback_alloc())
		return 0;
	eth_send(pack);
}

//set all realized handlers
int icmp_init() {
	received_packet_handlers[0] = icmp_get_echo_answer;
	received_packet_handlers[8] = icmp_get_echo_request;

	//send_packet_handlers[0]  =icmp_send_echo_request;
	//send_packet_handlers[8] = icmp_send_echo_request;
}

//receive packet
int icmp_received_packet(net_packet *pack)
{
	PACKET_HANDLER func;
	icmphdr *icmp =pack.h.icmph;

	func = received_packet_handler(icmp->type);
	//check_summ?
	if (NULL != func) func(pack, NULL );
}

//send packet

/*int icmp_send_packet(icmphdr *hdr,unsigned char srcaddr[4], unsigned char dstaddr[4],
					 ICMP_CALLBACK callback)
*/
/*
int icmp_send_packet (void *ifdev, net_packet *pack)
{
	PACKET_HANDLER func;
	net_packet *pack = net_packet_alloc();
	if (pack == 0)
		return -1;
	pack->len = build_icmp_packet(pack, eth_get_ipaddr(ifdev), dstaddr, type);



	//func = send_packet_handler(type);
	//if (NULL != func) func(pack, callback);
	eth_send(pack);
}
*/
