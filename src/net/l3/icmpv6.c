/**
 * @file
 * @brief Internet Control Message Protocol (ICMPv6)
 * for the Internet Protocol Version 6 (IPv6) Specification
 * @details RFC 4443
 *
 * @date 17.05.13
 * @author Ilia Vaprol
 */

#include <netinet/in.h>
#include <stddef.h>
#include <embox/net/proto.h>
#include <net/l3/icmpv6.h>
#include <net/l3/arp.h>
#include <net/l2/ethernet.h>
#include <net/lib/icmpv6.h>
#include <net/lib/ipv6.h>
#include <net/skbuff.h>
#include <net/util/checksum.h>
#include <arpa/inet.h>
#include <util/log.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <string.h>
#include <net/l3/ipv6.h>
#include <errno.h>
#include <util/binalign.h>
#include <net/neighbour.h>

EMBOX_NET_PROTO(ETH_P_IPV6, IPPROTO_ICMPV6, icmp6_rcv,
		net_proto_handle_error_none);

int icmp6_send(struct sk_buff *skb, uint8_t type, uint8_t code,
		const void *body, size_t body_sz) {
	struct in_device *in_dev;
	struct ip6hdr *ip6h;
	struct in6_addr src_ip6, dst_ip6;

	if (NULL == skb_declone(skb)) {
		skb_free(skb);
		return -ENOMEM; /* error: can't declone data */
	}

	assert(skb != NULL);
	in_dev = inetdev_get_by_dev(skb->dev);
	assert(in_dev != NULL);

	ip6h = ip6_hdr(skb);
	assert(ip6h != NULL);

	memcpy(&src_ip6, &in_dev->ifa6_address, sizeof src_ip6);
	memcpy(&dst_ip6, &ip6h->saddr, sizeof dst_ip6);
	ip6_build(ip6h, ip6_data_length(ip6h),
			IPPROTO_ICMPV6, 255, &src_ip6, &dst_ip6);

	icmp6_build(icmp6_hdr(skb), type, code, body, body_sz);
	icmp6_set_check_field(icmp6_hdr(skb), ip6h);

	if (ip6_out_ops == NULL) {
		return -ENOSYS; /* error: not implemented */
	}

	assert(ip6_out_ops->snd_pack != NULL);
	return ip6_out_ops->snd_pack(skb);
}

static int icmp6_hnd_echo_request(const struct icmp6hdr *icmp6h,
		const struct icmp6body_echo *echo_req,
		struct sk_buff *skb) {
	struct icmp6body_echo echo_rep;

	if (icmp6h->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	if (sizeof *icmp6h + sizeof *echo_req > ip6_data_length(
				ip6_hdr(skb))) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	echo_rep.id = echo_req->id;
	echo_rep.seq = echo_req->seq;

	return icmp6_send(skb, ICMP6_ECHO_REPLY, 0, &echo_rep,
			sizeof echo_rep);
}

static int ndp_hnd_neighbor_solicit(const struct icmp6hdr *icmp6h,
		const struct ndpbody_neighbor_solicit *nbr_solicit,
		struct sk_buff *skb) {
	struct in_device *in_dev;
	size_t len;
	const struct ndpoptions_ll_addr *ops;
	struct {
		struct ndpbody_neighbor_advert body;
		struct ndpoptions_ll_addr ops;
		char __ops_ll_addr_storage[MAX_ADDR_LEN];
	} __attribute__((packed)) nbr_advert;

	if (ip6_hdr(skb)->hop_limit != 255) {
		skb_free(skb);
		return 0; /* error: not neighbour */
	}

	if (icmp6h->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	len = ip6_data_length(ip6_hdr(skb));
	if (sizeof *icmp6h + sizeof *nbr_solicit > len) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}
	len -= sizeof *icmp6h + sizeof *nbr_solicit;

	in_dev = inetdev_get_by_dev(skb->dev);
	assert(in_dev != NULL);

	if (0 != memcmp(&in_dev->ifa6_address, &nbr_solicit->target,
				sizeof in_dev->ifa6_address)) {
		skb_free(skb);
		return 0; /* error: not for us */
	}

	if (len != 0) {
		if (sizeof *ops > len) {
			skb_free(skb);
			return 0; /* error: invalid length */
		}
		ops = (const struct ndpoptions_ll_addr *)nbr_solicit->options;
		switch (ops->hdr.type) {
		default:
			skb_free(skb);
			return 0; /* error: unknown option type */
		case NDP_SOURCE_LL_ADDR:
			if (ops->hdr.len * 8 != len) {
				skb_free(skb);
				return 0; /* error: invalid length */
			}
			(void)neighbour_add(ETH_P_IPV6, &ip6_hdr(skb)->saddr,
					sizeof ip6_hdr(skb)->saddr, in_dev->dev,
					ARP_HRD_ETHERNET, ops->ll_addr,
					ops->hdr.len * 8 - sizeof ops->hdr, 0);
			break;
		}
	}

	nbr_advert.body.router = 0;
	nbr_advert.body.solicited = 1;
	nbr_advert.body.override = 0;
	nbr_advert.body.zero1 = nbr_advert.body.zero2 = 0;
	memcpy(&nbr_advert.body.target, &in_dev->ifa6_address,
			sizeof nbr_advert.body.target);
	nbr_advert.ops.hdr.type = NDP_TARGET_LL_ADDR;
	nbr_advert.ops.hdr.len = binalign_bound(sizeof nbr_advert.ops
			+ in_dev->dev->addr_len, 8) / 8;
	assert(in_dev->dev->addr_len
			<= sizeof nbr_advert.__ops_ll_addr_storage);
	memcpy(nbr_advert.ops.ll_addr, &in_dev->dev->dev_addr[0],
			in_dev->dev->addr_len);

	return icmp6_send(skb, NDP_NEIGHBOR_ADVERT, 0,
			&nbr_advert, sizeof nbr_advert.body
				+ sizeof nbr_advert.ops + in_dev->dev->addr_len);
}

static int ndp_hnd_neighbor_advert(const struct icmp6hdr *icmp6h,
		const struct ndpbody_neighbor_advert *nbr_advert,
		struct sk_buff *skb) {
	int ret;
	size_t len;
	const struct ndpoptions_ll_addr *ops;

	if (ip6_hdr(skb)->hop_limit != 255) {
		skb_free(skb);
		return 0; /* error: not neighbour */
	}

	if (icmp6h->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	len = ip6_data_length(ip6_hdr(skb));
	if (sizeof *icmp6h + sizeof *nbr_advert > len) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}
	len -= sizeof *icmp6h + sizeof *nbr_advert;

	if (len != 0) {
		if (sizeof *ops > len) {
			skb_free(skb);
			return 0; /* error: invalid length */
		}
		ops = (const struct ndpoptions_ll_addr *)nbr_advert->options;
		switch (ops->hdr.type) {
		default:
			skb_free(skb);
			return 0; /* error: unknown option type */
		case NDP_TARGET_LL_ADDR:
			if (ops->hdr.len * 8 != len) {
				skb_free(skb);
				return 0; /* error: invalid length */
			}
			ret = neighbour_add(ETH_P_IPV6, &nbr_advert->target,
					sizeof nbr_advert->target, skb->dev,
					ARP_HRD_ETHERNET, ops->ll_addr,
					ops->hdr.len * 8 - sizeof ops->hdr, 0);
			if (ret != 0) {
				skb_free(skb);
				return ret; /* error: see ret */
			}
			break;
		}
	}

	skb_free(skb);
	return 0;
}

static int icmp6_rcv(struct sk_buff *skb) {
	struct icmp6hdr *icmp6h;
	uint16_t old_check;

	if (sizeof *icmp6h > ip6_data_length(ip6_hdr(skb))) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	if (NULL == skb_declone(skb)) {
		skb_free(skb);
		return -ENOMEM; /* error: can't declone data */
	}

	icmp6h = icmp6_hdr(skb);
	assert(icmp6h != NULL);

	old_check = icmp6h->check;
	icmp6_set_check_field(icmp6h, ip6_hdr(skb));
	if (old_check != icmp6h->check) {
		skb_free(skb);
		return 0; /* error: bad checksum */
	}

	switch (icmp6h->type) {
	default:
		log_error("icmp6_rcv: unknown type: %hhu\n", icmp6h->type);
		break; /* error: unknown type */
	case ICMP6_ECHO_REQUEST:
		return icmp6_hnd_echo_request(icmp6h,
				&icmp6h->body[0].echo, skb);
	case ICMP6_ECHO_REPLY:
		break;
	case NDP_NEIGHBOR_SOLICIT:
		return ndp_hnd_neighbor_solicit(icmp6h,
				&icmp6h->body[0].neighbor_solicit, skb);
	case NDP_NEIGHBOR_ADVERT:
		return ndp_hnd_neighbor_advert(icmp6h,
				&icmp6h->body[0].neighbor_advert, skb);
	}

	skb_free(skb);
	return 0;
}
