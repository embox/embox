/**
 * \file icmp.c
 *
 * \date 14.03.2009
 * \author sunnya
 */
#include "string.h"
#include "conio.h"
#include "common.h"
#include "net/skbuff.h"
#include "net/net.h"
#include "net/if_device.h"
#include "net/eth.h"
#include "net/icmp.h"
#include "net/net_pack_manager.h"
#include "net/ip.h"
#include "net/if_ether.h"

#define CB_INFO_SIZE        0x10

typedef void (*ICMP_CALLBACK)(struct sk_buff* response);

typedef struct _ICMP_CALLBACK_INFO {
	ICMP_CALLBACK      cb;
	void               *ifdev;
	unsigned short     ip_id;
	unsigned char      type;
} ICMP_CALLBACK_INFO;

ICMP_CALLBACK_INFO cb_info[CB_INFO_SIZE];

static int callback_alloc(ICMP_CALLBACK cb, void *ifdev, unsigned short ip_id,
		unsigned char type) {
	int i;
	if (NULL == cb || NULL == ifdev)
		return -1;
	for (i = 0; i < array_len(cb_info); i++) {
		if (NULL == cb_info[i].cb) {
			cb_info[i].cb    = cb;
			cb_info[i].ifdev = ifdev;
			cb_info[i].ip_id = ip_id;
			cb_info[i].type  = type;
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
			cb_info[i].cb    = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type  = 0;
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
			cb_info[i].cb    = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type  = 0;
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

typedef int (*PACKET_HANDLER)(sk_buff_type *pack);

static PACKET_HANDLER received_packet_handlers[NR_ICMP_TYPES];

int icmp_abort_echo_request(void *ifdev) {
	interface_abort(ifdev);
}

unsigned short calc_checksumm(unsigned char *hdr, int size) {
	return ~ptclbsum(hdr, size);
}

/**
 * Fill ICMP header
 */
static int rebuild_icmp_header(sk_buff_type *pack, unsigned char type, unsigned char code) {
	icmphdr *hdr = pack->h.icmph;
	hdr->type    = type;
	hdr->code    = code;
	hdr->header_check_summ = 0;
	hdr->header_check_summ = calc_checksumm(pack->h.raw, ICMP_HEADER_SIZE);
	return 0;
}

static inline int build_icmp_packet(sk_buff_type *pack, unsigned char type,
		unsigned char code, unsigned char ttl, unsigned char srcaddr[4], unsigned char dstaddr[4]) {
	pack->h.raw = pack->nh.raw = pack->data + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	memset(pack->h.raw, 0, ICMP_HEADER_SIZE);
	rebuild_icmp_header(pack, type, code);

	pack->nh.raw = pack->data + ETH_HEADER_SIZE;
	rebuild_ip_header(pack, ttl, ICMP_PROTO_TYPE, 0, ICMP_HEADER_SIZE, srcaddr, dstaddr);

	return ICMP_HEADER_SIZE + IP_HEADER_SIZE;
}

/**
 * implementation handlers for received msgs
 */
static int icmp_get_echo_reply(sk_buff_type *pack) {
	LOG_DEBUG("icmp get echo reply\n");
	ICMP_CALLBACK cb;
	if (NULL == (cb = callback_find(pack->ifdev, pack->nh.iph->id,
			ICMP_ECHOREPLY)))
		return -1;
	cb(pack);
	//unregister
	callback_free(cb, pack->ifdev, pack->nh.iph->id, ICMP_ECHOREPLY);
	return 0;
}

/**
 * Handle ICMP_DEST_UNREACH.
 */
static int icmp_unreach(sk_buff_type *pack) {
	iphdr *iph;
	icmphdr *icmph;
	icmph = pack->h.icmph;
	iph   = (iphdr*)pack->data;
	if (iph->ihl < 5)
		return -1;

	if (icmph->type == ICMP_DEST_UNREACH) {
		switch (icmph->code & 15) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
		        break;
		//TODO:
		default:
		        break;
		}
		if (icmph->code > NR_ICMP_UNREACH)
		        return -1;
	}
	return 0;
}

/**
 * Handle ICMP_ECHO ("ping") requests.
 */
static int icmp_echo(sk_buff_type *recieved_pack) {
	LOG_DEBUG("icmp get echo request\n");
	sk_buff_type *pack = skb_copy(recieved_pack, 0);
	if(inet_dev_find_by_ip(pack->nh.iph->daddr)) {
		return 0;
	}

	//fill icmp header
	pack->h.icmph->type = ICMP_ECHOREPLY;
	memset(pack->h.raw + pack->nh.iph->tot_len - IP_HEADER_SIZE + 1, 0, 64);

/*	LOG_DEBUG("\npacket icmp\n");
	int i;
	for (i = 0; i < pack->nh.iph->tot_len + 64; i ++) {
		if (0 == i % 4) {
			LOG_DEBUG("\t ");
		}
		LOG_DEBUG("%2X",  pack->h.raw[i]);
	}
	LOG_DEBUG("%X\n",  pack->h.icmph->header_check_summ);
*/	pack->h.icmph->header_check_summ = 0;
	pack->h.icmph->header_check_summ = calc_checksumm(pack->h.raw, ICMP_HEADER_SIZE );

	//fill ip header
//	rebuild_ip_header(pack, 64, ICMP_PROTO_TYPE, pack->nh.iph->id++, pack->nh.iph->len,
//			    recieved_pack->nh.iph->saddr, recieved_pack->nh.iph->daddr);

	memcpy (pack->nh.iph->saddr, recieved_pack->nh.iph->daddr, sizeof (pack->nh.iph->saddr));
	memcpy (pack->nh.iph->daddr, recieved_pack->nh.iph->saddr, sizeof (pack->nh.iph->daddr));
	pack->nh.iph->id ++;
	pack->nh.iph->ttl      = 64;
	pack->nh.iph->frag_off = 0;
	pack->nh.iph->check    = 0;
	pack->nh.iph->check    = calc_checksumm(pack->nh.raw, IP_HEADER_SIZE);

	pack->len -= ETH_HEADER_SIZE;
	eth_send(pack);
	return 0;
}

int icmp_send_echo_request(void *ifdev, unsigned char dstaddr[4], int ttl,
		ICMP_CALLBACK callback) { //type 8
	LOG_DEBUG("icmp send echo request\n");
	//TODO icmp req must be variable length
	sk_buff_type *pack = alloc_skb(0x100, 0);
	if ( pack == NULL ) {
		return -1;
	}
	pack->ifdev  = ifdev;
	pack->netdev = (struct net_device *)inet_dev_get_netdevice(ifdev);
	pack->len    = build_icmp_packet(pack, ICMP_ECHO, 0, ttl,
					inet_dev_get_ipaddr(ifdev), dstaddr);
	pack->protocol = ETH_P_IP;

	if (-1 == callback_alloc(callback, ifdev, pack->nh.iph->id,
			ICMP_ECHOREPLY)) {
		kfree_skb(pack);
		return -1;
	}
	return eth_send(pack);
}

void icmp_send(sk_buff_type *pack, int type, int code) {
	//TODO:
	switch(type) {
	case ICMP_ECHO:
	case ICMP_DEST_UNREACH:
		break;
	}
}

int icmp_init() {
	received_packet_handlers[ICMP_ECHOREPLY]    = icmp_get_echo_reply;
	received_packet_handlers[ICMP_DEST_UNREACH] = icmp_unreach;
	received_packet_handlers[ICMP_ECHO]         = icmp_echo;
	//TODO: other types
	return 0;
}

int icmp_rcv(sk_buff_type *pack) {
	LOG_DEBUG("icmp packet received\n");
	icmphdr *icmph = pack->h.icmph;
	net_device_stats *stats = pack->netdev->get_stats(pack->netdev);
	/**
	 * 18 is the highest 'known' ICMP type. Anything else is a mystery
	 * RFC 1122: 3.2.2  Unknown ICMP messages types MUST be silently
	 *                discarded.
	 */
	if (icmph->type > NR_ICMP_TYPES) {
		LOG_ERROR("unknown type of ICMP packet\n");
		stats->rx_err += 1;
		return -1;
	}
	/*
	 * RFC 1122: 3.2.2.6 An ICMP_ECHO to broadcast MAY be
	 *        silently ignored.
	 * RFC 1122: 3.2.2.8 An ICMP_TIMESTAMP MAY be silently
	 *        discarded if to broadcast/multicast.
	 */
        if ( 0 /* (IFF_BROADCAST | IFF_MULTICAST) */) {
    		if (icmph->type == ICMP_ECHO ||
            	    icmph->type == ICMP_TIMESTAMP) {
    			return -1;
    		}
    		if (icmph->type != ICMP_ECHO &&
            	    icmph->type != ICMP_TIMESTAMP &&
            	    icmph->type != ICMP_ADDRESS &&
            	    icmph->type != ICMP_ADDRESSREPLY) {
    			return -1;
    		}
        }

	//TODO: check summ icmp? not need, if ip checksum is ok.
	unsigned short tmp = icmph->header_check_summ;
	icmph->header_check_summ = 0;
	if( tmp !=  calc_checksumm(pack->h.raw, ICMP_HEADER_SIZE)) {
		LOG_ERROR("bad icmp checksum\n");
		return -1;
	}
	if (NULL != received_packet_handlers[icmph->type]) {
		return received_packet_handlers[icmph->type](pack);
	}
	return -1;
}
