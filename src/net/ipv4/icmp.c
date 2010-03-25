/**
 * @file
 * @brief Implementation of the ICMP protocol layer.
 * @details RFC 792
 *
 * @date 14.03.2009
 * @author Alexander Batyukov
 */

#include <string.h>
#include <net/inetdevice.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <net/protocol.h>
#include <linux/init.h>

/**
 * Build xmit assembly blocks
 */
struct icmp_bxm {
	sk_buff_t *skb;
	int offset;
	int data_len;
	struct {
		icmphdr_t icmph;
		uint32_t  times[3];
	} data;
	int head_len;
#if 0
	struct ip_options replyopts;
#endif
	unsigned char  optbuf[40];
};

#define CB_INFO_SIZE        0x10

typedef struct _ICMP_CALLBACK_INFO {
	ICMP_CALLBACK      cb;
	void               *ifdev;
	unsigned short     ip_id;
	unsigned char      type;
} ICMP_CALLBACK_INFO;

ICMP_CALLBACK_INFO cb_info[CB_INFO_SIZE];

static int callback_alloc(ICMP_CALLBACK cb, void *in_dev, unsigned short ip_id,
		unsigned char type) {
	size_t i;
	if (NULL == cb || NULL == in_dev)
		return -1;
	for (i = 0; i < array_len(cb_info); i++) {
		if (NULL == cb_info[i].cb) {
			cb_info[i].cb    = cb;
			cb_info[i].ifdev = in_dev;
			cb_info[i].ip_id = ip_id;
			cb_info[i].type  = type;
			return i;
		}
	}
	return -1;
}

static int interface_abort(void *in_dev) {
	size_t i;
	if (NULL == in_dev)
		return -1;
	for (i = 0; i < array_len(cb_info); i++) {
		if (cb_info[i].ifdev == in_dev) {
			cb_info[i].cb    = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type  = 0;
			return i;
		}
	}
	return -1;
}

static int callback_free(ICMP_CALLBACK cb, void *in_dev, unsigned short ip_id,
		unsigned char type) {
	size_t i;
	if (NULL == cb) {
		return -1;
	}
	for (i = 0; i < array_len(cb_info); i++) {
		if (cb_info[i].cb == cb && in_dev == cb_info[i].ifdev) {
			cb_info[i].cb    = 0;
			cb_info[i].ifdev = 0;
			cb_info[i].ip_id = 0;
			cb_info[i].type  = 0;
			return i;
		}
	}
	return -1;
}

static ICMP_CALLBACK callback_find(void *in_dev, unsigned short ip_id,
		unsigned char type) {
	size_t i;
	for (i = 0; i < array_len(cb_info); i++) {
		if (in_dev == cb_info[i].ifdev && type == cb_info[i].type) {
			return cb_info[i].cb;
		}
	}
	return NULL;
}

/*
 * ICMP control array. This specifies what to do with each ICMP.
 */
struct icmp_control {
	void (*handler)(sk_buff_t *skb);
	short error; /* This ICMP is classed as an error message */
};

static const struct icmp_control icmp_pointers[NR_ICMP_TYPES + 1];

int icmp_abort_echo_request(void *in_dev) {
	return interface_abort(in_dev);
}

/**
 * Fill ICMP header
 */
static int rebuild_icmp_header(sk_buff_t *pack, unsigned char type,
			unsigned char code, unsigned seq, unsigned id) {
	icmphdr_t *hdr = pack->h.icmph;
	hdr->type    = type;
	hdr->code    = code;
	hdr->un.echo.sequence = seq;
	hdr->un.echo.id = id;
	hdr->checksum = 0;
	hdr->checksum = ptclbsum(pack->h.raw, ICMP_HEADER_SIZE);
	return 0;
}

static inline void build_icmp_packet(sk_buff_t *pack, unsigned char type,
		unsigned char code, unsigned char ttl, in_addr_t srcaddr, in_addr_t dstaddr,
		unsigned seq, unsigned id) {
	pack->h.raw = pack->data + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	memset(pack->h.raw, 0, ICMP_HEADER_SIZE);
	rebuild_icmp_header(pack, type, code, seq, id);

	pack->nh.raw = pack->data + ETH_HEADER_SIZE;
	memset(pack->nh.raw, 0, IP_HEADER_SIZE);
	rebuild_ip_header(pack, ttl, ICMP_PROTO_TYPE, 0, pack->len - ETH_HEADER_SIZE, srcaddr, dstaddr);
}

/**
 * implementation handlers for received msgs
 */
static void icmp_get_echo_reply(sk_buff_t *pack) {
	//TODO now ICMP reply haven't to work with callbacks it works through sockets
	ICMP_CALLBACK cb;
	if (NULL == (cb = callback_find(in_dev_get(pack->dev), pack->nh.iph->id,
			ICMP_ECHOREPLY)))
		return;
	cb(pack);
	//unregister
	callback_free(cb, in_dev_get(pack->dev), pack->nh.iph->id, ICMP_ECHOREPLY);
}

/**
 * Handle ICMP_DEST_UNREACH.
 */
static void icmp_unreach(sk_buff_t *pack) {
	iphdr_t *iph;
	icmphdr_t *icmph;
	icmph = pack->h.icmph;
	iph   = (iphdr_t*)pack->data;
	if (iph->ihl < 5)
		return;

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
			return;
	}
}

/*
 * Driving logic for building and sending ICMP messages.
 */
static void icmp_reply(struct icmp_bxm *icmp_param, sk_buff_t *skb) {
	sk_buff_t *pack = skb_copy(icmp_param->skb, 0);
	pack->dev = icmp_param->skb->dev;
	pack->protocol = icmp_param->skb->protocol;
	pack->h.icmph->type = icmp_param->data.icmph.type;
	//TODO:
	memset(pack->h.raw + pack->nh.iph->tot_len - IP_HEADER_SIZE + 1, 0, 64);
	pack->h.icmph->checksum = 0;
	pack->h.icmph->checksum = ptclbsum(pack->h.raw,
				pack->nh.iph->tot_len - IP_HEADER_SIZE );

	ip_send_reply(NULL, icmp_param->skb->nh.iph->daddr,
				icmp_param->skb->nh.iph->saddr, pack, 0);
	kfree_skb(pack);
}

/**
 * Handle ICMP_ECHO ("ping") requests.
 * RFC 1122: 3.2.2.6 MUST have an echo server that answers ICMP echo
 *                requests.
 * RFC 1122: 3.2.2.6 Data received in the ICMP_ECHO request MUST be
 *                included in the reply.
 * RFC 1812: 4.3.3.6 SHOULD have a config option for silently ignoring
 *                echo requests, MUST have default=NOT.
 */
static void icmp_echo(sk_buff_t *pack) {
	struct icmp_bxm icmp_param;
	icmp_param.data.icmph      = *icmp_hdr(pack);
	icmp_param.data.icmph.type = ICMP_ECHOREPLY;
	icmp_param.skb             = pack;
	icmp_param.offset          = 0;
	icmp_param.data_len        = pack->len;
	icmp_param.head_len        = sizeof(icmphdr_t);
	icmp_reply(&icmp_param, pack);
}
#if 0
static void icmp_discard(sk_buff_t *skb) {
}
#endif
int icmp_send_echo_request(void *in_dev, in_addr_t dstaddr, int ttl,
		ICMP_CALLBACK callback, unsigned size, __u16 pattern, unsigned seq) {
	sk_buff_t *pack = alloc_skb(ETH_HEADER_SIZE + IP_HEADER_SIZE +
						ICMP_HEADER_SIZE + size, 0);
	if ( pack == NULL ) {
		return -1;
	}
	//TODO ICMP get net dev
	pack->dev = ((in_device_t*)in_dev)->dev;
	build_icmp_packet(pack, ICMP_ECHO, 0, ttl, inet_dev_get_ipaddr(in_dev),
				dstaddr, seq, /*id*/0);
	memset(pack->h.raw + ICMP_HEADER_SIZE, pattern, size);
	pack->protocol = ETH_P_IP;

	if (-1 == callback_alloc(callback, in_dev, pack->nh.iph->id,
			ICMP_ECHOREPLY)) {
		kfree_skb(pack);
		return -1;
	}
	return dev_queue_xmit(pack);
}

/* RFC 1122: 3.2.2 MUST send at least the IP header and 8 bytes of header.
 *   MAY send more (we do).
 *   MUST NOT change this header information.
 *   MUST NOT reply to a multicast/broadcast IP address.
 *   MUST NOT reply to a multicast/broadcast MAC address.
 *   MUST reply to only the first fragment.
 */
void icmp_send(sk_buff_t *pack, int type, int code, uint32_t info) {
	//TODO:
	switch(type) {
	case ICMP_ECHO:
	case ICMP_DEST_UNREACH:
		break;
	}
}

/*
 * This table is the definition of how we handle ICMP.
 */
static const struct icmp_control icmp_pointers[NR_ICMP_TYPES + 1] = {
	[ICMP_ECHOREPLY] = {
		.handler = icmp_get_echo_reply/*icmp_discard*/,
	},
	[ICMP_DEST_UNREACH] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_ECHO] = {
		.handler = icmp_echo,
	},
};

void __init icmp_init(void) {
}

int icmp_rcv(sk_buff_t *pack) {
	icmphdr_t *icmph = pack->h.icmph;
	net_device_stats_t *stats = pack->dev->netdev_ops->ndo_get_stats(pack->dev);
	uint16_t tmp;
	/**
	 * 18 is the highest 'known' ICMP type. Anything else is a mystery
	 * RFC 1122: 3.2.2  Unknown ICMP messages types MUST be silently
	 *                discarded.
	 */
	if (icmph->type > NR_ICMP_TYPES) {
		LOG_ERROR("unknown type of ICMP packet\n");
		stats->rx_err ++;
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
	tmp = icmph->checksum;
	icmph->checksum = 0;
	if( tmp != ptclbsum(pack->h.raw, pack->nh.iph->tot_len - IP_HEADER_SIZE)) {
		LOG_ERROR("bad icmp checksum\n");
		return -1;
	}
	if (NULL != icmp_pointers[icmph->type].handler) {
		icmp_pointers[icmph->type].handler(pack);
		kfree_skb(pack);
		return 0;
	}
	return -1;
}

net_protocol_t icmp_protocol = {
	.handler = icmp_rcv,
	.type = IPPROTO_ICMP
};

DECLARE_INET_PROTO(icmp_protocol);
