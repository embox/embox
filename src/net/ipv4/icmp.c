/**
 * @file
 * @brief Implementation of the ICMP protocol layer.
 * @details RFC 792
 *
 * @date 14.03.09
 * @author Alexander Batyukov
 * @author Nikolay Korotky
 * 		- remove callback interface
 * 		- major refactoring
 * @author Ilia Vaprol
 */

#include <string.h>
#include <net/inetdevice.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <net/protocol.h>
#include <net/kernel_socket.h>
#include <linux/init.h>
#include <embox/net/proto.h>
#include <err.h>
#include <errno.h>
#include <assert.h>

EMBOX_NET_PROTO_INIT(IPPROTO_ICMP, icmp_rcv, NULL, icmp_init);

/**
 * ICMP Build xmit assembly blocks
 */
struct icmp_bxm {
	sk_buff_t *skb;         /* For ICMP messages sent with icmp_send, represents
                               the ingress IP packet that triggered the transmission.
                               For ICMP messages sent with icmp_reply, represents an
                               ingress ICMP messages request. */
	int offset;             /* Offset between skb->data and skb->nh.
                               This offset is useful when evaluating how much
                               data can be put into the ICMP payload for those
                               ICMP messages that require it. */
	int data_len;           /* Size of the ICMP payload */
	struct {
		icmphdr_t icmph;    /* Header of the ICMP message to transmit */
		uint32_t times[3];  /* Used by the ICMP_TIMESTAMPREPLY message type */
	} data;
	int head_len;           /* Size of the ICMP header */
#if 0
	struct ip_options replyopts;
	unsigned char  optbuf[40];
#endif
};

/**
 * Packet's handler, corresponding to packet's type. This specifies
 * what to do with each type of ICMP.
 */
typedef int (*icmp_control)(sk_buff_t *skb);

static socket_t *icmp_socket; /* Socket for transmitting ICMP messages */

static icmp_control icmp_handlers[]; /* ICMP control handlers */

/**
 * Driving logic for building and sending ICMP messages.
 * Used by the ICMP protocol to reply to ingress ICMP request messages
 * that require a response (ECHO and TIMESTAMP).
 *
 * @param icmp_param
 * @param skb_in
 */
static int icmp_reply(struct icmp_bxm *icmp_param, sk_buff_t *skb_in) {
	sk_buff_t *skb = skb_copy(icmp_param->skb, 0);
	skb->dev = icmp_param->skb->dev;
	skb->h.icmph->type = icmp_param->data.icmph.type;
	skb->h.icmph->code = icmp_param->data.icmph.code;
	skb->h.icmph->checksum = 0;
	/* TODO checksum must be at network byte order */
	skb->h.icmph->checksum = ptclbsum(skb->h.raw,
				htons(skb->nh.iph->tot_len) - IP_HEADER_SIZE(skb->nh.iph));
	//TODO: kernel_sendmsg(NULL, __icmp_socket, ...);
	ip_send_reply(NULL, icmp_param->skb->nh.iph->daddr,
				icmp_param->skb->nh.iph->saddr, skb, 0);
	return ENOERR;
}

static int icmp_discard(sk_buff_t *skb) {
	/* nothing to do here */
	return -1;
}

static int icmp_unreach(sk_buff_t *skb) {
	/* Handle ICMP_DEST_UNREACH, ICMP_TIME_EXCEED, ICMP_PARAMETERPROB and ICMP_QUENCH */
	iphdr_t *iph;
	icmphdr_t *icmph;
	net_device_stats_t *stats;

	assert(skb != NULL);

	iph = ip_hdr(skb);
	icmph = icmp_hdr(skb);
	stats = skb->dev->netdev_ops->ndo_get_stats(skb->dev);

	assert(iph != NULL);
	assert(icmph != NULL);
	assert(stats != NULL);

	switch (icmph->type) {
	case ICMP_DEST_UNREACH:
		if (icmph->code >= NR_ICMP_UNREACH) {
			return -1;
		}
		switch (icmph->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			/* do nothing */
			break;
		case ICMP_FRAG_NEEDED: // TODO
			LOG_WARN("fragmentation needed but DF is set");
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
	case ICMP_TIME_EXCEEDED:
		/* do nothing */
		break;
	case ICMP_PARAMETERPROB:
		break;
	default:
		return -1;
	}

	return ENOERR;
}

static int icmp_redirect(sk_buff_t *skb) {
	/* Handle ICMP_REDIRECT */
	//TODO: fix ip route before.
	return -1;
}

static int icmp_echo(sk_buff_t *skb) {
	sk_buff_t *skb_reply;

	/* RFC 796:  The data received in the echo message must be returned in the echo reply message. */
	skb_reply = skb_copy(skb, 0);
	skb_reply->dev = skb->dev;
	skb_reply->h.icmph->type = ICMP_ECHOREPLY;

	/* TODO checksum must be at network byte order */
	skb_reply->h.icmph->checksum = 0;
	skb_reply->h.icmph->checksum = ptclbsum(skb_reply->h.raw,
				htons(skb_reply->nh.iph->tot_len) - IP_HEADER_SIZE(skb_reply->nh.iph));
	//TODO: kernel_sendmsg(NULL, __icmp_socket, ...);
	ip_send_reply(NULL, skb->nh.iph->daddr, skb->nh.iph->saddr, skb_reply, 0);
	return ENOERR;
}

/**
 * Handle ICMP Timestamp requests.
 * RFC 1122: 3.2.2.8 MAY implement ICMP timestamp requests.
 *         SHOULD be in the kernel for minimum random latency.
 *         MUST be accurate to a few minutes.
 *         MUST be updated at least at 15Hz.
 */
static int icmp_timestamp(sk_buff_t *skb) {
	//TODO: we can't work with timestamp now.
	struct icmp_bxm icmp_param;
	icmp_param.data.icmph      = *icmp_hdr(skb);
	icmp_param.data.icmph.type = ICMP_TIMESTAMPREPLY;
	icmp_param.data.icmph.code = 0;
	icmp_param.skb             = skb;
	icmp_param.offset          = 0;
	icmp_param.data_len        = 0;
	icmp_param.head_len        = sizeof(icmphdr_t) + 12;
	return icmp_reply(&icmp_param, skb);
}

#define DATA_SIZE(iph) (IP_HEADER_SIZE(iph) + 8)

void icmp_send(sk_buff_t *skb_in, int type, int code, uint32_t info) {
	iphdr_t *iph_in = ip_hdr(skb_in);
	iphdr_t *iph;
	icmphdr_t *icmph;
	struct iovec iov;
	struct msghdr m;
	char packet[IP_HEADER_SIZE(iph_in) + ICMP_HEADER_SIZE + DATA_SIZE(iph_in)];
	/*
	 * RFC 1122: 3.2.2 MUST send at least the IP header and 8 bytes of header.
	 *   MAY send more (we do).
	 *   MUST NOT change this header information.
	 *   MUST NOT reply to a multicast/broadcast IP address.
	 *   MUST NOT reply to a multicast/broadcast MAC address.
	 *   MUST reply to only the first fragment.
	 */
	if ((skb_in->pkt_type != PACKET_HOST) ||
		(iph_in->frag_off & htons(IP_OFFSET))) {
		return;
	}
	iph = (iphdr_t *) packet;
	icmph = (icmphdr_t *) (packet + IP_HEADER_SIZE(iph_in));
	/* build IP header */
	memcpy(iph, iph_in, IP_HEADER_SIZE(iph_in));
	iph->proto = IPPROTO_ICMP;
	iph->tot_len = IP_HEADER_SIZE(iph_in) + ICMP_HEADER_SIZE + DATA_SIZE(iph_in);
	iph->daddr = skb_in->nh.iph->saddr;
	iph->saddr = skb_in->nh.iph->daddr;
	iph->id++;
	iph->frag_off = IP_DF;
	/* build ICMP header */
	memcpy(icmph + ICMP_HEADER_SIZE, iph_in, DATA_SIZE(iph_in));
	icmph->type = type;
	icmph->code = code;
	icmph->un.gateway = info;
	icmph->checksum = 0;
	/* TODO checksum must be at network byte order */
	icmph->checksum = ptclbsum(icmph, htons(iph->tot_len) - IP_HEADER_SIZE(iph));

	iov.iov_base = (void *) packet;
	iov.iov_len = IP_HEADER_SIZE(iph) + ICMP_HEADER_SIZE + DATA_SIZE(iph);
	m.msg_iov = &iov;
	kernel_socket_sendmsg(NULL, icmp_socket, &m, iov.iov_len);

	kfree_skb(skb_in);
}

/*
 * This table is the definition of how we handle ICMP.
 */

static int icmp_init(void) {
	int res;

	res = kernel_socket_create(PF_INET, SOCK_RAW, IPPROTO_ICMP, &icmp_socket);
	if (res < 0) {
		return res;
	}

	return ENOERR;
}

static int ping_rcv(struct sk_buff *skb) {
	return 0;
}

static icmp_control icmp_handlers[NR_ICMP_TYPES] = {
		[ICMP_ECHOREPLY]      = ping_rcv,
		[1]                   = icmp_discard,
		[2]                   = icmp_discard,
		[ICMP_DEST_UNREACH]   = icmp_unreach,
		[ICMP_SOURCE_QUENCH]  = icmp_unreach,
		[ICMP_REDIRECT]       = icmp_redirect,
		[6]                   = icmp_discard,
		[7]                   = icmp_discard,
		[ICMP_ECHO]           = icmp_echo,
		[9]                   = icmp_discard,
		[10]                  = icmp_discard,
		[ICMP_TIME_EXCEEDED]  = icmp_unreach,
		[ICMP_PARAMETERPROB]  = icmp_unreach,
		[ICMP_TIMESTAMP]      = icmp_timestamp,
		[ICMP_TIMESTAMPREPLY] = icmp_discard,
		[ICMP_INFO_REQUEST]   = icmp_discard,
		[ICMP_INFO_REPLY]     = icmp_discard
#if 0
		[ICMP_ADDRESS]        = icmp_address,
		[ICMP_ADDRESSREPLY]   = icmp_address_reply
#endif
};

/**
 * Receive packet.
 *
 * @param skb received packet
 */
static int icmp_rcv(sk_buff_t *pack) {
	int res;
	icmphdr_t *icmph;
	net_device_stats_t *stats;
	uint16_t tmp;

	assert(pack != NULL);

	icmph = pack->h.icmph;
	stats = pack->dev->netdev_ops->ndo_get_stats(pack->dev);

	assert(icmph != NULL);
	assert(stats != NULL);

	/* RFC 1122: 3.2.2  Unknown ICMP messages types MUST be silently discarded */
	if (icmph->type >= NR_ICMP_TYPES) {
		LOG_WARN("unknown type of ICMP packet\n");
		stats->rx_err++;
		return -1;
	}

#if 0
	/* RFC 1122: 3.2.2.6  An ICMP_ECHO to broadcast MAY be silently ignored. */
	if (IFF_BROADCAST && (icmph->type == ICMP_ECHO)) {
		return -1;
	}

	/* RFC 1122: 3.2.2.8  An ICMP_TIMESTAMP MAY be silently discarded if to broadcast/multicast. */
	if ((IFF_BROADCAST | IFF_MULTICAST) && (icmph->type == ICMP_TIMESTAMP)) {
		 return -1;
	}
#endif

	tmp = icmph->checksum; /* TODO checksum must be at network byte order */
	icmph->checksum = 0;
	if (tmp != ptclbsum(pack->h.raw, ntohs(pack->nh.iph->tot_len) - IP_HEADER_SIZE(pack->nh.iph))) {
		LOG_WARN("bad icmp checksum\n");
		stats->rx_err++;
		stats->rx_crc_errors++;
		return -1;
	}

	assert(icmp_handlers[icmph->type] != NULL);
	res = icmp_handlers[icmph->type](pack);
	if (res < 0) {
		stats->rx_err++;
		return res;
	}

	return ENOERR;
}
