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
 * @author Vladimir Sokolov
 */


	/* ToDo: svv: remove not required includes */
#include <string.h>
#include <net/inetdevice.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <linux/init.h>
#include <embox/net/proto.h>
#include <err.h>
#include <errno.h>
#include <assert.h>
#include <net/raw.h>
#include <net/socket.h>
#include <util/math.h>
#include <framework/net/proto/api.h>


EMBOX_NET_PROTO_INIT(IPPROTO_ICMP, icmp_rcv, NULL, icmp_init);

/**
 * Packet's handler, corresponding to packet's type.
 * This specifies what to do with each type of ICMP.
 */
typedef int (*icmp_control)(sk_buff_t *skb);

/* Is packet described by skb is multicast one at levels 2 or 3 */
static inline bool is_packet_multicast(sk_buff_t *skb) {
	return (skb->pkt_type != PACKET_HOST) || !(ip_is_local(ntohl(skb->nh.iph->daddr), false, false));
}

static int icmp_discard(sk_buff_t *skb) {
	/* nothing to do here */
	return -1;
}

/* Brief check that embedded header in incoming ICMP is correct */
static inline bool is_ip_header_correct_brief(iphdr_t *iph) {
	uint len;

	/**
	 *   Is the datagram acceptable? General rules:
	 *   1.  Length at least the size of an ip header
	 *   2.  Version of 4
	 *   3.  Checksums correctly
	 *   4.  Doesn't have a bogus length
	 */
	if (iph->ihl < 5 || iph->version != 4) {
		return false;
	}

	len = ntohs(iph->tot_len);
	if (len < IP_HEADER_SIZE(iph)) {
		return false;
	}

	/* Don't be paranoid. Skip CRC. */
	return true;
}

/* Handle ICMP_DEST_UNREACH, ICMP_TIME_EXCEED, ICMP_PARAMETERPROB and ICMP_QUENCH */
static int icmp_unreach(sk_buff_t *skb) {
	iphdr_t *iph = ip_hdr(skb);
	icmphdr_t *icmph = icmp_hdr(skb);
	iphdr_t *iph_embedded = (iphdr_t *)(skb->h.raw + ICMP_HEADER_SIZE);
	net_device_stats_t *stats = skb->dev->netdev_ops->ndo_get_stats(skb->dev);
	uint32_t info;
	bool give_to_only_raw = false;

		/* Drop ICMP if it send not to unicast address */
	if (unlikely(is_packet_multicast(skb))) {
		return -1;
	}

		/* Note: 20 additional bytes. It's enough for:
		 *	TCP 20+
		 *	UDP 20
		 *	IPIP 20+
		 *	ICMP (ping) - it's User Space problems. We can't help here (ToDo: or call raw_err() )
		 */
	if (unlikely((ntohs(iph->tot_len) < (IP_HEADER_SIZE(iph) + ICMP_HEADER_SIZE + IP_MIN_HEADER_SIZE + 20)))) {
		/* Nothing to work with */
		stats->rx_length_errors++;
		return -1;
	}

	if (unlikely(!(is_ip_header_correct_brief(iph_embedded)))) {
		return -1;
	}

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
					break;
				case ICMP_FRAG_NEEDED:
					LOG_WARN("fragmentation needed but DF is set");
					/* They don't like our IP header */
					give_to_only_raw = true;
					break;
				default:
					return -1;
			}
			break;
		case ICMP_SOURCE_QUENCH:
			break;
		case ICMP_TIME_EXCEEDED:
			return -1;
		case ICMP_PARAMETERPROB:
			if (icmph->un.ih_pptr < IP_HEADER_SIZE(iph_embedded)) {
					/* They don't like our IP header */
				give_to_only_raw = true;
			}
			break;
		default:
			return -1;
	}

	    /* Get rid of it. It's a garbage. All error handlers get the original ICMP */
	info = icmph->type;
	info |= (icmph->code << 8);
	info |= (icmph->un.echo.sequence << 16);

	/* Notify all raw/udp/tcp sockets */

		/* ToDo: What's this and should we notify about ICMP_FRAG_NEEDED || ICMP_PARAMETERPROB?
		 * Should we clone skb first to avoid future intersections?
		 */
	raw_err(skb, info);					/* Give skb as is. Probably they want the whole packet (for ICMP etc) */

		/* We can notify upper protocols (for example that some TCP Windows were lost), but the problem is with IP itself */
	if (give_to_only_raw) {
		return ENOERR;	/* Probably raw_err() took it */
	}

	/* Walk by protocols here and notify propriate protocol */
	{
		const struct net_proto *net_proto_ptr = NULL;

		net_proto_foreach(net_proto_ptr) {
			net_protocol_t *p_netproto = net_proto_ptr->netproto;
			if (p_netproto->type == iph_embedded->proto) {

#if 0 /* at least udp_err() tries to accept the original ICMP */
					/* This skb contains ???.IP1.ICMP.IP2.NNN. How could NNN work with it?
					 * Please note, that NNN header might be incorrect (or we can even only get part of it)
					 * So try to fix skb
					 */
				skb_shifthead(skb, -(IP_HEADER_SIZE(iph) + ICMP_HEADER_SIZE));
				skb->h.raw = skb->nh.raw + IP_HEADER_SIZE(iph_embedded);
					/* Now:
					 *	LL - corrupted
					 *	IP - reasonably correct
					 *	TL - it may contain useful information at start
					 */
#endif
				if (p_netproto->err_handler)
				{
					p_netproto->err_handler(skb, info);		/* handler must free this skb */
					return ENOERR;
				} else {
					/* This protocol refuses errors processing */
					return -1;
				}
			}
		}
	}

	return ENOERR;	/* Probably raw_err() took it */
}

static int icmp_redirect(sk_buff_t *skb) {
	LOG_WARN("Our routes might be incorrect\n");
	icmp_discard(skb);
	return -1;
}

static int icmp_echo(sk_buff_t *skb) {
	sk_buff_t *reply;

		/* ToDo: optimization:
		 * 	move all kfree_skb() into subfunctions like it and get rid of this skb_clone()
		 *	we should use the original packet structures
		 */
	if (!likely(reply = skb_clone(skb, 0)))
		return -1;

		/* Fix IP header */
	{
		in_device_t *idev = in_dev_get(reply->dev);				/* Requires symmetric routing */
		__be16 ip_id = inet_dev_get_id(idev);
		__be16 tot_len = reply->nh.iph->tot_len;

			/* Replace not unicast addresses */
		__in_addr_t daddr = ip_is_local(ntohl(reply->nh.iph->daddr), false, false) ? reply->nh.iph->daddr : htonl(idev->ifa_address);

		init_ip_header(reply->nh.iph, ICMP_PROTO_TYPE, ip_id, tot_len, 0, daddr, reply->nh.iph->saddr);
	}
		/* Fix ICMP header */
	reply->h.icmph->type = ICMP_ECHOREPLY;
	icmp_send_check_skb(reply);

	return ip_send_packet(NULL, reply);
}


#if 0	/* Obsoleted stuff from Linux kernel */
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

void icmp_send(sk_buff_t *skb_in, __be16 type, __be16 code, __be32 info) {
		/* Determine how many data we can take from the original datagram
		 * Note:
		 *	We suggest that routing is symmetric (if we get packet from A from device B, then we can reply back through device B)
		 *	That all income packets have device (no ICMP packets generated in User Space) (not checked)
		 */
	const uint realloc_shift = IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE;
	uint ret_len = min((realloc_shift + ntohs(skb_in->nh.iph->tot_len)), skb_in->dev->mtu);
	uint ip_ret_len = min(ret_len, 576);							/* See RCF 1812 4.3.2.3 */
	sk_buff_t *skb;

	/*
	 * RFC 1122: 3.2.2 MUST send at least the IP header and 8 bytes of header.
	 *   MAY send more (we do)								(till ip_ret_len restriction)
	 *   MUST NOT change this header information.
	 *   MUST NOT reply to a multicast/broadcast IP address					(Implemented)
	 *   MUST NOT reply to a multicast/broadcast MAC address				(Implemented)
	 *   MUST reply to only the first fragment						(Implemented)
	 *---------
	 *   It's a bad idea to send ICMP error to an ICMP error				(Implemented)
	 */
		/* Don't reply for not unicast address of any kind */
	if (unlikely(is_packet_multicast(skb_in) || (skb_in->nh.iph->frag_off & htons(IP_OFFSET)))) {
		kfree_skb(skb_in);
		return;
	}
		/* Don't reply to ICMP Error.
		 * We don't check that it's corrupted. We already have sufficient reason to drop it
		 */
	if (unlikely((skb_in->nh.iph->proto == IPPROTO_ICMP) && !(ICMP_INFOTYPE(skb_in->h.icmph->type)))) {
		kfree_skb(skb_in);
		return;
	}

		/* Check presence of extra space for new headers */
	if (unlikely(skb = skb_checkcopy_expand(skb_in, realloc_shift, 0, 0))) {
		kfree_skb(skb_in);
		return;
	}

		/* Relink skb and build content */
	{
		iphdr_t *iph_in = skb->nh.iph;						/* Original IP header */
		iphdr_t *iph;
		icmphdr_t *icmph;

		skb_shifthead(skb, realloc_shift);
		iph = skb->nh.iph;									/* IP header is in correct place. We'll fill it later */
		skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE;
		icmph = skb->h.icmph;								/* ICMP header follows IP header. We'll fill it later */
															/* Link Layer will be build after routing. It may not be ready yet */

			/* Assemble IP header */
		{
			in_device_t *idev = in_dev_get(skb->dev);			/* Requires symmetric routing */
			__be16 ip_id = inet_dev_get_id(idev);
			__be16 tot_len = htons(ip_ret_len);

			init_ip_header(iph, ICMP_PROTO_TYPE, ip_id, tot_len, iph_in->tos, htonl(idev->ifa_address), iph_in->saddr);
		}

			/* Assemble ICMP header */
		icmph->type = type;
		icmph->code = code;
		icmph->un.gateway = info;
		icmp_send_check_skb(skb);

	}

	ip_send_packet(NULL, skb);
}

static int icmp_init(void) {
		/* Nothing to init. Everything is ready */
	return ENOERR;
}

static int ping_rcv(struct sk_buff *skb) {
		/* Kernel doesn't need this answer. User Space stuff might process it if it wants to */
	return ENOERR;
}

/*
 * This table is the definition how we handle ICMP
 */
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
 * Real processing code
 * @param skb received packet
 */
static inline int __icmp_rcv(sk_buff_t *pack) {
	icmphdr_t *icmph = pack->h.icmph;
	net_device_stats_t *stats = pack->dev->netdev_ops->ndo_get_stats(pack->dev);
	__be16 orig_crc = icmph->checksum;

	if (unlikely(ntohs(pack->nh.iph->tot_len) < (IP_HEADER_SIZE(pack->nh.iph) + ICMP_HEADER_SIZE))) {
		LOG_WARN("icmp length is too small\n");
		stats->rx_length_errors++;
		return -1;
	}

	icmp_send_check_skb(pack);
	if (unlikely(icmph->checksum != orig_crc)) {
		LOG_WARN("bad icmp checksum\n");
		stats->rx_crc_errors++;
		return -1;
	}

	/* RFC 1122: 3.2.2  Unknown ICMP messages types MUST be silently discarded (in Kernel)*/
	if (unlikely(icmph->type >= NR_ICMP_TYPES)) {
		LOG_WARN("Unsupported type of ICMP packet %i\n", icmph->type);
		return -1;
	}

#if 0	/* anton wants it to be keeped */
	/* ToDo: properly fix statistics if you are going to add it in future */

	/* RFC 1122: 3.2.2.6  An ICMP_ECHO to broadcast MAY be silently ignored. */
	if (IFF_BROADCAST && (icmph->type == ICMP_ECHO)) {
		return -1;
	}

	/* RFC 1122: 3.2.2.8  An ICMP_TIMESTAMP MAY be silently discarded if to broadcast/multicast. */
	if ((IFF_BROADCAST | IFF_MULTICAST) && (icmph->type == ICMP_TIMESTAMP)) {
		 return -1;
	}
#endif

	assert(icmp_handlers[icmph->type] != NULL);
	return icmp_handlers[icmph->type](pack);
}


/**
 * Receive packet.
 * Check basic asserts(). Nothing special just common parts.
 * @param skb received packet
 */
static int icmp_rcv(sk_buff_t *pack) {
	int res;
	iphdr_t *iph;
	icmphdr_t *icmph;
	net_device_stats_t *stats;

	assert(pack != NULL);

	iph = pack->nh.iph;
	icmph = pack->h.icmph;
	stats = pack->dev->netdev_ops->ndo_get_stats(pack->dev);		/* Why not just get stat from pack->dev? */

	assert(iph != NULL);
	assert(icmph != NULL);
	assert(stats != NULL);

	res = __icmp_rcv(pack);
	stats->rx_err += (res < 0);
	kfree_skb(pack);
	return res;
}

