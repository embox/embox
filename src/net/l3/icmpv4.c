/**
 * @file
 * @brief Internet Control Message Protocol (ICMPv4)
 * @details RFC 792
 *
 * @date 14.03.09
 * @author Alexander Batyukov
 * @author Nikolay Korotky
 * 		- remove callback interface
 * 		- major refactoring
 * @author Vladimir Sokolov
 * @author Ilia Vaprol
 */


#include <errno.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>

#include <util/math.h>

#include <net/inetdevice.h>
#include <net/l3/ipv4/ip.h>
#include <net/l3/icmpv4.h>
#include <net/lib/icmpv4.h>
#include <net/socket/raw.h>
#include <net/l2/ethernet.h>
#include <net/if_packet.h>
#include <net/lib/ipv4.h>


#include <embox/net/pack.h>
#include <embox/net/proto.h>

#include <kernel/time/ktime.h>

EMBOX_NET_PROTO(ETH_P_IP, IPPROTO_ICMP, icmp_rcv,
		net_proto_handle_error_none);

static int icmp_send(uint8_t type, uint8_t code, const void *body,
		size_t body_sz, struct sk_buff *skb) {
	int ret;
	size_t size;

	if (ip_out_ops == NULL) {
		skb_free(skb);
		return -ENOSYS; /* error: not implemented */
	}

	size = sizeof *icmp_hdr(skb) + body_sz;
	assert(ip_out_ops->make_pack != NULL);
	ret = ip_out_ops->make_pack(NULL, NULL, &size, &skb);
	if (ret != 0) {
		skb_free(skb);
		return ret; /* error: see ret */
	}
	else if (size != sizeof *icmp_hdr(skb) + body_sz) {
		skb_free(skb);
		return -EMSGSIZE; /* error: message is too big */
	}

	icmp_build(icmp_hdr(skb), type, code, body, body_sz);
	icmp_set_check_field(icmp_hdr(skb), ip_hdr(skb));

	assert(ip_out_ops->snd_pack != NULL);
	return ip_out_ops->snd_pack(skb);
}

static int icmp_notify_an_error(const struct icmphdr *icmph,
		const void *msg, size_t msg_sz, uint16_t extra_info,
		int only_raw, struct sk_buff *skb) {
	const struct iphdr *emb_iph;
	uint32_t error_info;

	emb_iph = msg;
	assert(emb_iph != NULL);

	if ((msg_sz < IP_MIN_HEADER_SIZE)
			|| (IP_HEADER_SIZE(emb_iph) < IP_MIN_HEADER_SIZE)
			|| (ntohs(emb_iph->tot_len) < IP_HEADER_SIZE(emb_iph))
			|| (msg_sz < IP_HEADER_SIZE(emb_iph))) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	if (!ip_check_version(emb_iph)) {
		skb_free(skb);
		return 0; /* error: not ipv4 */
	}

	if (ip_hdr(skb)->daddr != emb_iph->saddr) {
		skb_free(skb);
		return 0; /* error: not my embedded packet */
	}

	error_info = extra_info << 16 | icmph->code << 8 | icmph->type;

	raw_err(skb, error_info);

	if (!only_raw) {
		const struct net_proto *nproto;
		nproto = net_proto_lookup(ETH_P_IP, emb_iph->proto);
		if (nproto != NULL) {
			assert(nproto->handle_error != NULL);
			nproto->handle_error(skb, error_info);
		}
	}

	return 0;
}

static int icmp_hnd_dest_unreach(const struct icmphdr *icmph,
		const struct icmpbody_dest_unreach *dest_unreach,
		struct sk_buff *skb) {
	size_t len;

	switch (icmph->code) {
	default:
		skb_free(skb);
		return 0; /* error: bad code */
	case ICMP_NET_UNREACH:
	case ICMP_HOST_UNREACH:
	case ICMP_PROT_UNREACH:
	case ICMP_PORT_UNREACH:
	case ICMP_FRAG_NEEDED:
	case ICMP_SR_FAILED:
		break;
	}

	len = ip_data_length(ip_hdr(skb));
	if (sizeof *icmph + sizeof *dest_unreach > len) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}
	len -= sizeof *icmph + sizeof *dest_unreach;

	return icmp_notify_an_error(icmph, &dest_unreach->msg[0], len,
			icmph->code == ICMP_FRAG_NEEDED
				? ntohs(dest_unreach->mtu) : 0,
			icmph->code == ICMP_FRAG_NEEDED, skb);
}

static int icmp_hnd_source_quench(const struct icmphdr *icmph,
		const struct icmpbody_source_quench *source_quench,
		struct sk_buff *skb) {
	size_t len;

	if (icmph->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	len = ip_data_length(ip_hdr(skb));
	if (sizeof *icmph + sizeof *source_quench > len) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}
	len -= sizeof *icmph + sizeof *source_quench;

	return icmp_notify_an_error(icmph, &source_quench->msg[0],
			len, 0, 0, skb);
}

static int icmp_hnd_echo_request(const struct icmphdr *icmph,
		const struct icmpbody_echo *echo_req,
		struct sk_buff *skb) {
	size_t len;
	struct icmpbody_echo *echo_rep;

	if (icmph->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	len = ip_data_length(ip_hdr(skb));
	if (sizeof *icmph + sizeof *echo_req > len) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}
	len -= sizeof *icmph + sizeof *echo_req;

	echo_rep = &icmp_hdr(skb)->body[0].echo;

	return icmp_send(ICMP_ECHO_REPLY, 0, echo_rep,
			sizeof *echo_rep + len, skb);
}

static int icmp_hnd_param_prob(const struct icmphdr *icmph,
		const struct icmpbody_param_prob *param_prob,
		struct sk_buff *skb) {
	size_t len;

	switch (icmph->code) {
	default:
		skb_free(skb);
		return 0; /* error: bad code */
	case ICMP_PTR_ERROR:
	case ICMP_PTR_UNUSED:
		break;
	}

	len = ip_data_length(ip_hdr(skb));
	if (sizeof *icmph + sizeof *param_prob > len) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}
	len -= sizeof *icmph + sizeof *param_prob;

	return icmp_notify_an_error(icmph, &param_prob->msg[0], len,
			icmph->code == ICMP_PTR_ERROR ? param_prob->ptr : 0,
			(icmph->code == ICMP_PTR_ERROR)
				&& (param_prob->ptr < IP_HEADER_SIZE(
						(const struct iphdr *)&param_prob->msg[0])),
			skb);
}

static int icmp_hnd_timestamp_request(const struct icmphdr *icmph,
		const struct icmpbody_timestamp *tstamp_req,
		struct sk_buff *skb) {
	uint32_t msec_since_12am;
	struct icmpbody_timestamp *tstamp_rep;

	if (icmph->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	if (sizeof *icmph + sizeof *tstamp_req
			!= ip_data_length(ip_hdr(skb))) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	msec_since_12am = (ktime_get_ns() / NSEC_PER_MSEC) % (
				SEC_PER_DAY * MSEC_PER_SEC);

	tstamp_rep = &icmp_hdr(skb)->body[0].timestamp;
	tstamp_rep->orig = tstamp_req->trans;
	tstamp_rep->recv = tstamp_rep->trans = htonl(msec_since_12am);

	return icmp_send(ICMP_TIMESTAMP_REPLY, 0, tstamp_rep,
			sizeof *tstamp_rep, skb);
}

static int icmp_rcv(struct sk_buff *skb) {
	struct icmphdr *icmph;
	uint16_t old_check;

	if (sizeof *icmph > ip_data_length(ip_hdr(skb))) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	if (NULL == skb_declone(skb)) {
		skb_free(skb);
		return -ENOMEM; /* error: can't declone data */
	}

	icmph = icmp_hdr(skb);
	assert(icmph != NULL);

	old_check = icmph->check;
	icmp_set_check_field(icmph, ip_hdr(skb));
	if (old_check != icmph->check) {
		skb_free(skb);
		return 0; /* error: bad checksum */
	}

	switch (icmph->type) {
	default:
		//printk("icmp_rcv: unknown type: %hhu\n", icmph->type);
		break; /* error: unknown type */
	case ICMP_ECHO_REPLY:
	case ICMP_TIMESTAMP_REPLY:
	case ICMP_INFO_REPLY:
		break;
	case ICMP_DEST_UNREACH:
		return icmp_hnd_dest_unreach(icmph,
				&icmph->body[0].dest_unreach, skb);
	case ICMP_SOURCE_QUENCH:
		return icmp_hnd_source_quench(icmph,
				&icmph->body[0].source_quench, skb);
	case ICMP_REDIRECT:
	case ICMP_TIME_EXCEED:
	case ICMP_INFO_REQUEST:
		break; /* error: not implemented */
	case ICMP_ECHO_REQUEST:
		return icmp_hnd_echo_request(icmph, &icmph->body[0].echo,
				skb);
	case ICMP_PARAM_PROB:
		return icmp_hnd_param_prob(icmph,
				&icmph->body[0].param_prob, skb);
	case ICMP_TIMESTAMP_REQUEST:
		return icmp_hnd_timestamp_request(icmph,
				&icmph->body[0].timestamp, skb);
	}

	skb_free(skb);
	return 0;
}

int icmp_discard(struct sk_buff *skb, uint8_t type, uint8_t code,
		...) {
	struct {
		union {
			struct icmpbody_dest_unreach dest_unreach;
			struct icmpbody_source_quench source_quench;
			struct icmpbody_redirect redirect;
			struct icmpbody_time_exceed time_exceed;
			struct icmpbody_param_prob param_prob;
		} __attribute__((packed));
		char __body_msg_storage[ICMP_DISCARD_MAX_SIZE];
	} __attribute__((packed)) body;
	va_list extra;
	uint8_t *body_msg;
	size_t body_msg_sz;

	if (!(ip_is_local(ip_hdr(skb)->saddr, false, false)
				|| ip_is_local(ip_hdr(skb)->daddr, false, false))
			|| (ip_hdr(skb)->frag_off & htons(IP_OFFSET))
			|| (ip_data_length(ip_hdr(skb)) < ICMP_DISCARD_MIN_SIZE)
			|| (ip_hdr(skb)->proto != IPPROTO_ICMP)
			|| (skb->h.raw = skb->nh.raw + IP_HEADER_SIZE(ip_hdr(skb)),
				ICMP_TYPE_ERROR(icmp_hdr(skb)->type))) {
		skb_free(skb);
		return 0; /* error: inappropriate packet */
	}

	switch (type) {
	default:
		assertf(0, "bad type for discard");
		body_msg = (uint8_t *)&body.__body_msg_storage[0];
		break; /* error: bad type for discard */
	case ICMP_DEST_UNREACH:
		assertf(code < __ICMP_DEST_UNREACH_MAX,
				"incorrect code for type");
		va_start(extra, code);
		body.dest_unreach.zero = 0;
		body.dest_unreach.mtu = code != ICMP_FRAG_NEEDED ? 0
				: htons((uint16_t)va_arg(extra, int));
		va_end(extra);
		body_msg = &body.dest_unreach.msg[0];
		break;
	case ICMP_SOURCE_QUENCH:
		assertf(code == 0, "incorrect code for type");
		body.source_quench.zero = 0;
		body_msg = &body.source_quench.msg[0];
		break;
	case ICMP_REDIRECT:
		assertf(code < __ICMP_REDIRECT_MAX,
				"incorrect code for type");
		va_start(extra, code);
		memcpy(&body.redirect.gateway,
				va_arg(extra, struct in_addr *),
				sizeof body.redirect.gateway);
		va_end(extra);
		body_msg = &body.redirect.msg[0];
		break;
	case ICMP_TIME_EXCEED:
		assertf(code < __ICMP_TIME_EXCEED_MAX,
				"incorrect code for type");
		body.time_exceed.zero = 0;
		body_msg = &body.time_exceed.msg[0];
		break;
	case ICMP_PARAM_PROB:
		assertf(code < __ICMP_PARAM_PROB_MAX,
				"incorrect code for type");
		va_start(extra, code);
		body.param_prob.ptr = code != ICMP_PTR_ERROR ? 0
				: (uint8_t)va_arg(extra, int);
		body.param_prob.zero1 = body.param_prob.zero2 = 0;
		va_end(extra);
		body_msg = &body.param_prob.msg[0];
		break;
	}

	body_msg_sz = min(ip_data_length(ip_hdr(skb)),
			sizeof body.__body_msg_storage);
	memcpy(body_msg, ip_hdr(skb), body_msg_sz);

	if (NULL == skb_declone(skb)) {
		skb_free(skb);
		return -ENOMEM; /* error: can't declone data */
	}

	return icmp_send(type, code, &body, sizeof body
				- sizeof body.__body_msg_storage + body_msg_sz,
			skb);
}
