/**
 * @file
 * @brief
 *
 * @date 17.05.13
 * @author Ilia Vaprol
 */

#include <netinet/in.h>
#include <stddef.h>
#include <embox/net/proto.h>
#include <net/l3/icmpv6.h>
#include <net/lib/icmpv6.h>
#include <net/skbuff.h>
#include <net/util/checksum.h>
#include <arpa/inet.h>
#include <kernel/printk.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <string.h>
#include <net/l3/ipv6.h>
#include <net/lib/ipv6.h>
#include <errno.h>
#include <util/binalign.h>
#include <net/neighbour.h>

EMBOX_NET_PROTO(ETH_P_IPV6, IPPROTO_ICMPV6, icmp6_rcv, NULL);

int icmp6_send(struct sk_buff *skb, uint8_t type, uint8_t code,
		const void *body, size_t body_sz) {
	struct in_device *in_dev;
	struct in6_addr src_ip6, dst_ip6;

	assert(skb != NULL);
	assert(skb->nh.ip6h != NULL);

	in_dev = inetdev_get_by_dev(skb->dev);
	assert(in_dev != NULL);

	memcpy(&src_ip6, &in_dev->ifa6_address, sizeof src_ip6);
	memcpy(&dst_ip6, &skb->nh.ip6h->saddr, sizeof dst_ip6);
	ip6_build(skb->nh.ip6h, ip6_data_length(skb->nh.ip6h),
			IPPROTO_ICMPV6, 255, &src_ip6, &dst_ip6);

	icmp6_build(skb->h.icmp6h, type, code, body, body_sz);
	icmp6_set_check_field(skb->h.icmp6h, skb->nh.ip6h);

	if (ip6_out_ops == NULL) {
		return -ENOSYS;
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
		return 0;
	}

	if (sizeof *icmp6h + sizeof *echo_req > ntohs(
				skb->nh.ip6h->payload_len)) {
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

	if (skb->nh.ip6h->hop_limit != 255) {
		skb_free(skb);
		return 0; /* error: not neighbour */
	}

	if (icmp6h->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	len = ntohs(skb->nh.ip6h->payload_len);
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
			(void)neighbour_add(ETH_P_IPV6, &skb->nh.ip6h->saddr,
					sizeof skb->nh.ip6h->saddr, in_dev->dev,
					ARPG_HRD_ETHERNET, ops->ll_addr,
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

	if (skb->nh.ip6h->hop_limit != 255) {
		skb_free(skb);
		return 0; /* error: not neighbour */
	}

	if (icmp6h->code != 0) {
		skb_free(skb);
		return 0; /* error: bad code */
	}

	len = ntohs(skb->nh.ip6h->payload_len);
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
					ARPG_HRD_ETHERNET, ops->ll_addr,
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

	if (sizeof *icmp6h > ntohs(skb->nh.ip6h->payload_len)) {
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	icmp6h = icmp6_hdr(skb);
	switch (icmp6h->type) {
	default:
		printk("icmp6_rcv: unknown type: %hhu\n", icmp6h->type);
		break;
	case ICMP6_ECHO_REQUEST:
		printk("icmp6_rcv: echo-request\n");
		return icmp6_hnd_echo_request(icmp6h, &icmp6h->body[0].echo,
				skb);
	case ICMP6_ECHO_REPLY:
		printk("icmp6_rcv: echo-reply\n");
		break;
	case NDP_NEIGHBOR_SOLICIT:
		printk("icmp6_rcv: neighbor-solicit\n");
		return ndp_hnd_neighbor_solicit(icmp6h,
				&icmp6h->body[0].neighbor_solicit, skb);
	case NDP_NEIGHBOR_ADVERT:
		printk("icmp6_rcv: neighbor-advert\n");
		return ndp_hnd_neighbor_advert(icmp6h,
				&icmp6h->body[0].neighbor_advert, skb);
	}

	skb_free(skb);
	return 0;
}
