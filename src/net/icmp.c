/*
 * icmp.c
 *
 *  Created on: 14.03.2009
 *      Author: sunnya
 */

#include "eth.h"
#include "icmp.h"
//#include "ip_v4.h"

typedef int (*PACKET_HANDLER)(net_packet *pack, ETH_LISTEN_CALLBACK callback);

PACKET_HANDLER received_packet_handlers[256];
PACKET_HANDLER send_packet_handlers[256];


//returns function-handler
PACKET_HANDLER received_packet_handler(unsigned char type){
	return received_packet_handlers[type];
}

PACKET_HANDLER send_packet_handler(unsigned char type){
	return send_packet_handlers[type];
}
ETH_LISTEN_CALLBACK current_callback;


int icmp_abort_echo_request() {
	current_callback = NULL;
}

static inline net_packet *build_icmp_packet(icmphdr *hdr,unsigned char srcaddr[4], unsigned char dstaddr[4])
{
	net_packet resp;
	if (NULL == (resp = net_packet_alloc()))
			return -1;
	//fill icmp header
		resp->h.icmph->type = hdr->type;
		resp->h.icmph->code = hdr->code;
		resp->h.icmph->header_check_summ = hdr->header_check_summ;
		resp->h.icmph->raw = hdr->raw;
	//fill ip header
		iphdr *iphdr = resp->nh.iph;
		iphdr->header_size = sizeof (iphdr);
		memcpy(iphdr->dst_addr, dstaddr, sizeof(iphdr->dst_addr));
		memcpy(iphdr->src_addr, srcaddr, sizeof(iphdr->src_addr));

}

//implementation handlers for received msgs
int icmp_get_echo_answer(net_packet *pack, ETH_LISTEN_CALLBACK callback) { //type 0

	if (NULL == current_callback)
		return -1;
	current_callback(pack);
	//unregister
	current_callback = NULL;
	return 0;
}
int icmp_get_echo_request(net_packet *pack, ETH_LISTEN_CALLBACK callback) { //type 8
	icmphdr *icmphdr;
	icmphdr->type = 0;
	icmphdr->code = 0;
	icmphdr->header_check_summ = 0;
	icmphdr->raw= pack->h.icmph->raw;
	iphdr *iphdr = pack->nh->iph;
	//!!change adresses
	icmp_send_packet(icmphdr,iphdr->dst_addr,iphdr->src_addr, callback);
}

//implementation handlers for sended msgs
int icmp_send_echo_request(net_packet *pack, ETH_LISTEN_CALLBACK callback) { //type 8
	current_callback = callback;
}

//set all realized handlers
int icmp_init() {
	received_packet_handlers[0] = icmp_get_echo_answer;
	received_packet_handlers[8] = icmp_get_echo_request;

	send_packet_handlers[0]  =icmp_send_echo_request;
	send_packet_handlers[8] = icmp_send_echo_request;
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

int icmp_send_packet(icmphdr *hdr,unsigned char srcaddr[4], unsigned char dstaddr[4],
					 ETH_LISTEN_CALLBACK callback)
{
	PACKET_HANDLER func;
	net_packet *pack;
	pack = build_icmp_packet(hdr, srcaddr, dstaddr);
	pack->len += sizeof(icmphdr)+sizeof(iphdr);
	func = send_packet_handler(hdr->type);
	if (NULL != func) func(pack, callback);
	eth_send(pack);
}

