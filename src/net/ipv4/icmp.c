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
#include <errno.h>
#include <assert.h>

#include <util/math.h>

#include <net/inetdevice.h>
#include <net/icmp.h>
#include <net/raw.h>

#include <embox/net/proto.h>
#include <framework/net/proto/api.h>

#include <kernel/time.h>

#include <err.h>

EMBOX_NET_PROTO_INIT(IPPROTO_ICMP, icmp_rcv, NULL, icmp_init);

/* Is the packet described by skb is multicast/broadcast one at levels 2 or 3
 * Packets with type PACKET_LOOPBACK we treat as broadcast and drop them.
 * There are much more simple interfaces for local error notifications
 * then ICMP
 */
static inline bool is_packet_not_unicast(sk_buff_t *skb) {
	return (eth_packet_type(skb) != PACKET_HOST) ||
		   !(ip_is_local(skb->nh.iph->daddr, false, false));
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
	if (unlikely((iph->ihl < 5 || iph->version != 4))) {
		return false;
	}

	len = ntohs(iph->tot_len);
	if (unlikely((len < IP_HEADER_SIZE(iph)))) {
		return false;
	}

	/* Don't be paranoid. Skip CRC. */

	return true;
}


/* Pre check.
 * Is it possible at all to use the obtained ICMP error
 * Calculates parameters for future processing as side effect
 */
static inline int icmp_unreach_usability_check(sk_buff_t *skb,
							bool *give_to_only_raw, uint32_t *info) {
	iphdr_t *iph = ip_hdr(skb);
	icmphdr_t *icmph = icmp_hdr(skb);
	iphdr_t *iph_embedded = (iphdr_t *)(skb->h.raw + ICMP_HEADER_SIZE);

	*give_to_only_raw = false;

		/* Drop ICMP if it send not to unicast address */
	if (unlikely(is_packet_not_unicast(skb))) {
		return -1;
	}

		/* Note: 20 additional bytes. It's enough for:
		 *	TCP 20+
		 *	UDP 20
		 *	IPIP 20+
		 *	ICMP (ping) - it's User Space problems. They get the reply without us.
		 *	We can't help here (or may be raw_err() will give something more?)
		 */
	if (unlikely((ntohs(iph->tot_len) <
			(IP_HEADER_SIZE(iph) + ICMP_HEADER_SIZE + IP_MIN_HEADER_SIZE + 20)))) {
		net_device_stats_t *stats = &skb->dev->stats;
		/* Nothing to work with */
		stats->rx_length_errors++;
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
					*give_to_only_raw = true;
					*info |= ntohs(icmph->un.frag.mtu) << 16;
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
				*give_to_only_raw = true;
			}
			*info |= icmph->un.ih_pptr << 16;
			break;
		default:
			return -1;
	}

	/* Now it's a real error and we can use iph_embedded.
	 * It's location is correct and addressible.
	 */

	if (unlikely(!(is_ip_header_correct_brief(iph_embedded)))) {
		return -1;
	}

		/* Somebody is doing something nasty
		 * As a side effect it's a kind of protection for our loopback
		 */
	if (unlikely(iph_embedded->saddr != iph->daddr)) {
		return -1;
	}

	return ENOERR;;
}

/* Handle ICMP_DEST_UNREACH, ICMP_TIME_EXCEED, ICMP_PARAMETERPROB and ICMP_QUENCH */
static int icmp_unreach(sk_buff_t *skb) {
	icmphdr_t *icmph = icmp_hdr(skb);
	iphdr_t *iph_embedded = (iphdr_t *)(skb->h.raw + ICMP_HEADER_SIZE);
	bool give_to_only_raw = false;
	int res;

	/* This information is used as a marker for socket(s).
	 * So it allows to use _some_ information when skb
	 * isn't available any more.
	 * If required any protocol can assemble it's own
	 * information based on the whole skb (=> ICMP) data.
	 */
	uint32_t info = icmph->type | (icmph->code << 8);
	if ((res = icmp_unreach_usability_check(skb, &give_to_only_raw, &info)) < 0) {
		return res;
	}

	/* Notify all raw/udp/tcp sockets */

	raw_err(skb, info);

		/* 1) We can notify upper protocols (for example that some TCP windows were lost),
		 * but the problem is with IP itself
		 * 2) Don't call ICMP error handler again (if it exists).
		 * User Space must take care of it
		 */
	if (give_to_only_raw || (iph_embedded->proto == IPPROTO_ICMP)) {
		return ENOERR;	/* Probably raw_err() took it */
	}

	/* Walk by protocols here and notify the appropriate protocol */
	{
		const struct net_proto *net_proto_ptr = NULL;

		net_proto_foreach(net_proto_ptr) {
			net_protocol_t *p_netproto = net_proto_ptr->netproto;
			if (p_netproto->type == iph_embedded->proto) {
				if (likely(p_netproto->err_handler)) {
					p_netproto->err_handler(skb, info);
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
	icmp_discard(skb);	/* Don't check length, it's useless here */
	return -1;
}

/* 1) Assemble IP header for reply (based on request)
 * 2) Fix ICMP check sum
 * 3) Send it
 */
static int icmp_prepare_reply(sk_buff_t *reply) {
	/* Fix IP header */
		in_device_t *idev = in_dev_get(reply->dev);	/* Requires symmetric routing */
		__be16 ip_id = inet_dev_get_id(idev);
		__be16 tot_len = reply->nh.iph->tot_len;

		/* Replace not unicast addresses */
		in_addr_t daddr = ip_is_local(reply->nh.iph->daddr, false, false) ?
							reply->nh.iph->daddr : idev->ifa_address;

		init_ip_header(reply->nh.iph, ICMP_PROTO_TYPE, ip_id, tot_len, 0, daddr, reply->nh.iph->saddr);

		/* Calculate ICMP CRC. Header itself was fixed in caller */
	icmp_send_check_skb(reply);

	return ip_send_packet(NULL, reply);
}


static int icmp_echo(sk_buff_t *skb) {
	sk_buff_t *reply = skb_copy(skb, 0);	/* We are going to fix the data */

	if (!likely(reply))
		return -1;

		/* Mark it as a reply */
	reply->h.icmph->type = ICMP_ECHOREPLY;
	return icmp_prepare_reply(reply);
}

/* Map time into a proper ICMP network format */
static inline __be32 iptime(struct ktimeval *ctime) {
	uint32_t t = (ctime->tv_sec % (24*60*60)) * 1000 + ctime->tv_usec / 1000;
	return (htonl(t));
}

static int icmp_timestamp(sk_buff_t *skb) {
	sk_buff_t *reply;
	__be32 *time_ptr;
	struct ktimeval tv;
	__be32 time_field;
	int i;

	if (unlikely(ntohs(skb->nh.iph->tot_len) !=
				 (IP_HEADER_SIZE(skb->nh.iph) + ICMP_LEN_TIMESTAMP))) {
		net_device_stats_t *stats = &skb->dev->stats;
		LOG_WARN("icmp timestamp request length is too small\n");
		stats->rx_length_errors++;
		return -1;
	}

	if (!likely(reply = skb_copy(skb, 0))) 	/* We are going to fix the data */
		return -1;

	/* Mark it as a reply */
	reply->h.icmph->type = ICMP_TIMESTAMPREPLY;

	/* Fix time fields. Similar fields will do for us */
	get_timeval(&tv);
	time_field = iptime(&tv);
	time_ptr = (__be32 *)(reply->h.raw + ICMP_HEADER_SIZE);
	for ( i = 0; i < 3; i++, time_ptr++) {
		*time_ptr = time_field;
	}

	return icmp_prepare_reply(reply);
}


/* Pre check.
 * Is it possible at all to use the obtained skb to generate ICMP error
 * where true means that we can generate an error
 */
static inline bool icmp_send_usability_check(sk_buff_t *skb_in) {
	/*
	 * RFC 1122: 3.2.2 MUST send at least the IP header and 8 bytes of header.
	 *   MAY send more (we do)						(till ip_ret_len restriction)
	 *   MUST NOT change this header information.
	 *   MUST NOT reply to a multicast/broadcast IP address
	 *   MUST NOT reply to a multicast/broadcast MAC address
	 *   MUST reply to only the first fragment
	 *---------
	 *   It's a bad idea to send ICMP error to an ICMP error
	 */

	/* Don't reply for not unicast address of any kind */
	if (unlikely(is_packet_not_unicast(skb_in)) ||
				 (skb_in->nh.iph->frag_off & htons(IP_OFFSET))) {
		return false;
	}

	/* At least the IP header and 8 bytes of header */
	if (unlikely((ntohs(skb_in->nh.iph->tot_len) < (IP_HEADER_SIZE(skb_in->nh.iph) + 8)))) {
		/* Nothing to report about */
		return false;
	}

	skb_in->h.raw = skb_in->nh.raw + IP_HEADER_SIZE(skb_in->nh.iph);
	/* At least 8 bytes of h.raw are addressable now */

	/* Don't reply to ICMP Error.
	 *	Is there any reason to send errors about replies?
	 *	(No such requirements in RFCs, but it's reasonable)
	 */
	if (unlikely((skb_in->nh.iph->proto == IPPROTO_ICMP) &&
				 !(ICMP_INFOTYPE(skb_in->h.icmph->type)))) {
		return false;
	}

	return true;
}


static inline void __icmp_send(sk_buff_t *skb_in, __be16 type, __be16 code, __be32 info) {
		/* Determine how many data we can take from the original datagram
		 * Note 1:
		 *	We suggest that routing is symmetric:
		 *	(if we get packet from A from device B, then we can reply back through device B)
		 *	That all income packets have device
		 *	(no strange ICMP packets generated in User Space)
		 * Note 2:
		 * 	We generate an error with common length ICMP_HEADER_SIZE
		 */
	const uint realloc_shift = IP_MIN_HEADER_SIZE + ICMP_HEADER_SIZE;
	uint ret_len = min((realloc_shift + ntohs(skb_in->nh.iph->tot_len)), skb_in->dev->mtu);
	uint ip_ret_len = min(ret_len, 576);			/* See RCF 1812 4.3.2.3 */
	sk_buff_t *skb;


	if (!likely(icmp_send_usability_check(skb_in))) {
		kfree_skb(skb_in);
		return;
	}

		/* Check presence of extra space for new headers and modification permission*/
	if (!likely(skb = skb_checkcopy_expand(skb_in, realloc_shift, 0, 0))) {
		kfree_skb(skb_in);
		return;
	}

		/* Relink skb and build content */
	{
		iphdr_t *iph_in = skb->nh.iph;			/* Original IP header */
		iphdr_t *iph;
		icmphdr_t *icmph;

		skb_shifthead(skb, realloc_shift);
		skb->len = ip_ret_len + ETH_HEADER_SIZE;

			/* IP header is in correct place now. We'll fill it later */
		iph = skb->nh.iph;

			/* ICMP header follows IP header. We'll fill it later */
		skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE;
		icmph = skb->h.icmph;

			/* Link Layer will be build after routing. It may not be ready yet */

			/* Assemble IP header */
		{
			in_device_t *idev = in_dev_get(skb->dev);	/* Requires symmetric routing */
			__be16 ip_id = inet_dev_get_id(idev);
			__be16 tot_len = htons(ip_ret_len);

			init_ip_header(iph, ICMP_PROTO_TYPE, ip_id, tot_len, iph_in->tos,
						   idev->ifa_address, iph_in->saddr);
		}

			/* Assemble ICMP header */
		icmph->type = type;
		icmph->code = code;
		icmph->un.gateway = info;
		icmp_send_check_skb(skb);

	}

	ip_send_packet(NULL, skb);
}

/* Unfortunately code might not be safe */
void icmp_send(sk_buff_t *pack, __be16 type, __be16 code, __be32 info) {
	/* Check skb and headers */
	assert(pack);
	assert(pack->dev);
	assert(pack->nh.iph);
	__icmp_send(pack, type, code, info);
}


static int icmp_init(void) {
		/* Nothing to init. Everything is ready */
	return ENOERR;
}

static int ping_rcv(struct sk_buff *skb) {
		/* Kernel doesn't need this answer.
		 * User Space stuff might process it if it wants to
		 */
	return ENOERR;
}

/**
 * Packet's handler, corresponding to packet's type.
 * This specifies what to do with each type of ICMP.
 */
typedef int (*icmp_control)(sk_buff_t *skb);

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
	net_device_stats_t *stats = &pack->dev->stats;
	__be16 orig_crc = icmph->checksum;

	if (unlikely(ntohs(pack->nh.iph->tot_len) <
				 (IP_HEADER_SIZE(pack->nh.iph) + ICMP_HEADER_SIZE))) {
		LOG_WARN("icmp length is obviously too small\n");
		stats->rx_length_errors++;
		return -1;
	}

	icmp_send_check_skb(pack);
	if (unlikely(icmph->checksum != orig_crc)) {
		LOG_WARN("bad icmp checksum\n");
		icmph->checksum = orig_crc;
		stats->rx_crc_errors++;
		return -1;
	}

	/* RFC 1122: 3.2.2 Unknown ICMP messages types MUST be silently discarded (in Kernel)*/
	if (unlikely(icmph->type >= NR_ICMP_TYPES)) {
		LOG_WARN("Unsupported type of ICMP packet %i\n", icmph->type);
		return -1;
	}

	assert(icmp_handlers[icmph->type]);
	return icmp_handlers[icmph->type](pack);
}


/**
 * Receive packet.
 * Check basic asserts(). Nothing special just common parts.
 * @param skb received packet
 * (skb: not modified; freed; privacy could be any, a copy created)
 */
static int icmp_rcv(sk_buff_t *pack) {
	int res;

		/* Check skb and headers */
	assert(pack);
	assert(pack->nh.iph);
	assert(pack->h.icmph);

		/* Check stats-related stuff */
	assert(pack->dev);

	res = __icmp_rcv(pack);
	pack->dev->stats.rx_err += (res < 0);
	kfree_skb(pack);
	return res;
}

