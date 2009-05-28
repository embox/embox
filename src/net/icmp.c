/**
 * \file icmp.c
 *
 * \date 14.03.2009
 * \author sunnya
 */
#include "types.h"
#include "common.h"
#include "net.h"
#include "eth.h"
#include "net_pack_manager.h"
#include "icmp.h"
#include "ip_v4.h"

#define ICMP_TYPE_ECHO_REQ  8
#define ICMP_TYPE_ECHO_RESP 0
#define CB_INFO_SIZE	0x10

typedef struct _ICMP_CALLBACK_INFO {
	ICMP_CALLBACK cb;
	void *ifdev;
	unsigned short ip_id;
	unsigned char type;
} ICMP_CALLBACK_INFO;

ICMP_CALLBACK_INFO cb_info[CB_INFO_SIZE];

static int callback_alloc(ICMP_CALLBACK cb, void *ifdev, unsigned short ip_id,
		unsigned char type) {
	int i;
	if (NULL == cb || NULL == ifdev)
		return -1;
	for (i = 0; i < array_len(cb_info); i++) {
		if (NULL == cb_info[i].cb) {
			cb_info[i].cb = cb;
			cb_info[i].ifdev = ifdev;
			cb_info[i].ip_id = ip_id;
			cb_info[i].type = type;
			return i;
		}
	}
	return -1;
}

static int interface_abort(void *ifdev) {
	int i;
	if (NULL == ifdev)
		return -1;
	for (i = 0; i < array_len(cb_info); i++) {
		if (cb_info[i].ifdev == ifdev) {
			cb_info[i].cb = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type = 0;
			return i;
		}
	}
	return -1;
}

static int callback_free(ICMP_CALLBACK cb, void *ifdev, unsigned short ip_id,
		unsigned char type) {
	int i;
	if (NULL == cb) {
		return -1;
	}
	for (i = 0; i < array_len(cb_info); i++) {
		if (cb_info[i].cb == cb && ifdev == cb_info[i].ifdev) {
			cb_info[i].cb = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type = 0;
			return i;
		}
	}
	return -1;
}

static ICMP_CALLBACK callback_find(void *ifdev, unsigned short ip_id,
		unsigned char type) {
	int i;
	for (i = 0; i < array_len(cb_info); i++) {
		if (ifdev == cb_info[i].ifdev && type == cb_info[i].type) {
			return cb_info[i].cb;
		}
	}
	return NULL;
}

typedef int (*PACKET_HANDLER)(net_packet *pack);

static PACKET_HANDLER received_packet_handlers[256];

int icmp_abort_echo_request(void *ifdev) {

	interface_abort(ifdev);
}

static unsigned short calc_checksumm(unsigned char *hdr, int size) {
	return ~ptclbsum(hdr, size);
}

static unsigned short ip_id;

static inline int build_icmp_packet(net_packet *pack, unsigned char type,
		unsigned char code, int ttl, unsigned char srcaddr[4], unsigned char dstaddr[4]) {
	pack->h.raw = pack->nh.raw = pack->data + (pack->netdev->addr_len * 2 + 2)
			+ sizeof(iphdr);
	memset(pack->h.raw, 0, sizeof(icmphdr));

	//fill icmp header
	pack->h.icmph->type = type;
	pack->h.icmph->code = code;
	pack->h.icmph->header_check_summ = calc_checksumm(pack->h.raw,
			sizeof(icmphdr));

	//fill ip header
	pack->nh.raw = pack->data + (pack->netdev->addr_len * 2 + 2);
	pack->nh.iph->version = 4;
	pack->nh.iph->ihl = sizeof(iphdr) >> 2;
	memcpy(pack->nh.iph->daddr, dstaddr, sizeof(pack->nh.iph->daddr));
	memcpy(pack->nh.iph->saddr, srcaddr, sizeof(pack->nh.iph->saddr));
	pack->nh.iph->frag_off = 0;
	pack->nh.iph->check = 0;
	pack->nh.iph->tot_len = sizeof(icmphdr);
	pack->nh.iph->tos = 0;
	pack->nh.iph->ttl = ttl;
	pack->nh.iph->proto = ICMP_PROTO_TYPE;
	pack->nh.iph->id = ip_id++;
	pack->nh.iph->check = calc_checksumm(pack->nh.raw, sizeof(iphdr));
	return sizeof(icmphdr) + sizeof(iphdr);
}

//implementation handlers for received msgs
static int icmp_get_echo_answer(net_packet *pack) { //type 0
	ICMP_CALLBACK cb;
	if (NULL == (cb = callback_find(pack->ifdev, pack->nh.iph->id,
			ICMP_TYPE_ECHO_RESP)))
		return -1;
	cb(pack);
	//unregister
	callback_free(cb, pack->ifdev, pack->nh.iph->id, ICMP_TYPE_ECHO_RESP);
	return 0;
}

static int icmp_get_echo_request(net_packet *recieved_pack) { //type 8
	net_packet *pack = net_packet_copy(recieved_pack);
	if(find_interface_by_addr(pack->nh.iph->daddr)) {
		return 0;
	}

	int i;

	//fill icmp header
	pack->h.icmph->type = ICMP_TYPE_ECHO_RESP;
	memset(pack->h.raw + pack->nh.iph->tot_len - sizeof(iphdr) + 1, 0, 64);

	LOG_DEBUG("\npacket icmp\n");
	for (i = 0; i < pack->nh.iph->tot_len + 64; i ++) {
		if (0 == i % 4) {
			LOG_DEBUG("\t ");
		}
		LOG_DEBUG("%2X",  pack->h.raw[i]);
	}
	LOG_DEBUG("%X\n",  pack->h.icmph->header_check_summ);
	pack->h.icmph->header_check_summ = 0;
	pack->h.icmph->header_check_summ = calc_checksumm(pack->h.raw,pack->nh.iph->tot_len - sizeof(iphdr) + 1 );
	LOG_DEBUG("%X\n",  pack->h.icmph->header_check_summ);

	//fill ip header
	memcpy (pack->nh.iph->saddr, recieved_pack->nh.iph->daddr, sizeof (pack->nh.iph->saddr));
	memcpy (pack->nh.iph->daddr, recieved_pack->nh.iph->saddr, sizeof (pack->nh.iph->daddr));
	pack->nh.iph->id ++;
	pack->nh.iph->check = 0;
	pack->nh.iph->ttl = 64;
	pack->nh.iph->frag_off = 0;
	pack->nh.iph->check = calc_checksumm(pack->nh.raw, sizeof(iphdr));

	pack->len -= sizeof(machdr);
	eth_send(pack);
	return 0;
}

int icmp_send_echo_request(void *ifdev, unsigned char dstaddr[4], int ttl,
		ICMP_CALLBACK callback) { //type 8
	net_packet *pack = net_packet_alloc();
	if (pack == 0) {
		return -1;
	}
	pack->ifdev = ifdev;
	pack->netdev = eth_get_netdevice(ifdev);
	pack->len = build_icmp_packet(pack, ICMP_TYPE_ECHO_REQ, 0, ttl,
					eth_get_ipaddr(ifdev), dstaddr);
	pack->protocol = IP_PROTOCOL_TYPE;

	if (-1 == callback_alloc(callback, ifdev, pack->nh.iph->id,
			ICMP_TYPE_ECHO_RESP)) {
		net_packet_free(pack);
		return -1;
	}
	return eth_send(pack);
}

//set all realized handlers
int icmp_init() {
	received_packet_handlers[0] = icmp_get_echo_answer;
	received_packet_handlers[8] = icmp_get_echo_request;
	return 0;
}

//receive packet
int icmp_received_packet(net_packet *pack) {
	icmphdr *icmp = pack->h.icmph;
	//TODO check summ icmp?
	if (NULL != received_packet_handlers[icmp->type]) {
		return received_packet_handlers[icmp->type](pack);
	}
	return -1;
}

