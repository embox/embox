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
#include <net/udp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/kernel_socket.h>
#include <linux/init.h>
#include <embox/net/proto.h>
#include <err.h>
#include <errno.h>
#include <assert.h>

#include <net/raw.h>
#include <net/socket.h>

EMBOX_NET_PROTO_INIT(IPPROTO_ICMP, icmp_rcv, NULL, icmp_init);


#if 0
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
};
#endif

/**
 * Packet's handler, corresponding to packet's type. This specifies
 * what to do with each type of ICMP.
 */
typedef int (*icmp_control)(sk_buff_t *skb);

static socket_t *icmp_socket; /* Socket for transmitting ICMP messages */

static icmp_control icmp_handlers[]; /* ICMP control handlers */

static int icmp_discard(sk_buff_t *skb) {
	/* nothing to do here */
	return -1;
}

static int icmp_unreach(sk_buff_t *skb) {
	/* Handle ICMP_DEST_UNREACH, ICMP_TIME_EXCEED, ICMP_PARAMETERPROB and ICMP_QUENCH */
	iphdr_t *iph;
	icmphdr_t *icmph;
	net_device_stats_t *stats;
	uint32_t info;

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

	info = icmph->type;
	info ^= (icmph->code << 8);
	info ^= (icmph->un.echo.sequence << 16);
	/* Notify to all raw and udp sockets */
	raw_err(skb, info);
	udp_err(skb, info);

	kfree_skb(skb);								/* svv: Check possible conflict with freeing in common function */

	return ENOERR;
}

static int icmp_redirect(sk_buff_t *skb) {
	/* Handle ICMP_REDIRECT */
	//TODO: fix ip route before.
	return -1;
}

static inline int icmp_send_reply(struct sock *sk, in_addr_t saddr, in_addr_t daddr,
		sk_buff_t *skb, unsigned int len) {
	skb->nh.iph->saddr = saddr;
	skb->nh.iph->daddr = daddr;
	skb->nh.iph->id = htons(ntohs(skb->nh.iph->id) + 1);
	skb->nh.iph->frag_off = htons(IP_DF);				/* Why? */
	ip_send_check(skb->nh.iph);
	return ip_queue_xmit(skb, 0);
}

static int icmp_echo(sk_buff_t *skb) {
	sk_buff_t *reply;

	reply = skb_clone(skb, 0);	/* svv: Will lead to problems if our MTU is less then incoming len. See IP_DF flag later */
	assert(reply);
	reply->h.icmph->type = ICMP_ECHOREPLY;
	/* TODO checksum must be at network byte order */
	reply->h.icmph->checksum = 0;
	reply->h.icmph->checksum = ptclbsum(reply->h.raw, htons(reply->nh.iph->tot_len) - IP_HEADER_SIZE(reply->nh.iph));
	//TODO: kernel_sendmsg(NULL, __icmp_socket, ...);
			/* svv: ToDo: daddr might be 255.255.255.255. It can't act as a source
			 * Please note, if we have more that 1 interface, then
			 * daddr might not belong to the device we obtain this packet
			 */
	return icmp_send_reply(NULL, skb->nh.iph->daddr, skb->nh.iph->saddr, reply, 0);
}

static int icmp_timestamp(sk_buff_t *skb) {
#if 0
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
#endif
	return -1;
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
	 *   MAY send more (we do)												(svv: Fine. But we can don't fit interface MTU)
	 *																		(And there is a restriction of 576 bytes)
	 *   MUST NOT change this header information.
	 *   MUST NOT reply to a multicast/broadcast IP address					(Not implemented)
	 *   MUST NOT reply to a multicast/broadcast MAC address				(Implemented)
	 *   MUST reply to only the first fragment								(Implemented)
	 *---------
	 *   It's a bad idea to send ICMP error to an ICMP error				(Not implemented)
	 */
	if ((skb_in->pkt_type != PACKET_HOST) ||
		(iph_in->frag_off & htons(IP_OFFSET))) {
		return;										/* svv: Unlike the common way we don't free this skb. Why? */
	}
	iph = (iphdr_t *) packet;
	icmph = (icmphdr_t *) (packet + IP_HEADER_SIZE(iph_in));
	/* build IP header */
	memcpy(iph, iph_in, IP_HEADER_SIZE(iph_in));
	iph->proto = IPPROTO_ICMP;
	iph->tot_len = IP_HEADER_SIZE(iph_in) + ICMP_HEADER_SIZE + DATA_SIZE(iph_in);
	iph->daddr = skb_in->nh.iph->saddr;
	iph->saddr = skb_in->nh.iph->daddr;
	iph->id++;						/* svv: Possible problems if we already use this id in other flow */
	iph->frag_off = IP_DF;					/* svv: Why the whole Internet must be as small as our MTU? */
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
	kernel_socket_sendmsg(NULL, icmp_socket, &m, iov.iov_len);	/* Sending interface differs from one in icmp_send_reply() */

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
	return ENOERR;
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
	struct sk_buff *skb_tmp;


	assert(pack != NULL);

	/* Remove packet that came to raw socket icmp_socket */
	if ((skb_tmp = skb_recv_datagram(icmp_socket->sk, 0, 0, 0)) != NULL) {
		kfree_skb(skb_tmp);
	}

	icmph = pack->h.icmph;
	stats = pack->dev->netdev_ops->ndo_get_stats(pack->dev);

	assert(icmph != NULL);
	assert(stats != NULL);

	/* RFC 1122: 3.2.2  Unknown ICMP messages types MUST be silently discarded */
	if (icmph->type >= NR_ICMP_TYPES) {
		LOG_WARN("unknown type of ICMP packet\n");
		stats->rx_err++;
		kfree_skb(pack);
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
		kfree_skb(pack);
		return -1;
	}

	assert(icmp_handlers[icmph->type] != NULL);
	res = icmp_handlers[icmph->type](pack);
	stats->rx_err += (res < 0);
	kfree_skb(pack);
	return res;
}
