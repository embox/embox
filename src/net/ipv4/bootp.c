/**
 * @file
 * @brief BOOTP implementation (client) over IPv4.
 *
 * @date 01.08.2012
 * @author Alexander Kalmuk
 */

#include <net/skbuff.h>
#include <net/inet_sock.h>
#include <net/inetdevice.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/bootp.h>
#include <errno.h>
#include <util/hashtable.h>

static void process_options(struct in_device *in_dev, struct bootphdr *r) {
	struct in_addr addr;
	in_addr_t mask;
	unsigned char *op = r->options;
	const unsigned char *op_begin = op;

	if (*((unsigned int*)op) != DHCP_MAGIC_COOKIE) {
		return;
	}

	op += sizeof(DHCP_MAGIC_COOKIE);

	while (*op != TAG_END && op < op_begin + VEND_LEN) {
		switch(*op) {
		case TAG_PAD:
			op++;
			break;
		case TAG_GATEWAY: /* add default route */
			op += 2;
			rt_add_route(in_dev->dev, INADDR_ANY,
						INADDR_ANY, *(in_addr_t*)op, 0);
			op += IP_ADDR_LEN;
			break;
		case TAG_SUBNET_MASK: /* set mask */
			op += 2;
			mask = *((in_addr_t*)op);
			addr.s_addr = mask;
			inet_dev_set_mask(in_dev, addr.s_addr);
			rt_add_route(in_dev->dev, r->siaddr & mask, mask,
						INADDR_ANY, 0);
			op += IP_ADDR_LEN;
			break;
		default:
			op++;
			break;
		}
	}
}

int bootp_client_send(int sock, bootphdr_t *bphdr, struct net_device *dev, struct sockaddr_in *dst) {
	bphdr->op = BOOTPREQUEST;
	bphdr->htype = HTYPE_ETHERNET;
	bphdr->hlen = ETH_ALEN;

	memcpy(bphdr->chaddr, dev->dev_addr, ETH_ALEN);
	*(unsigned int*)bphdr->options = DHCP_MAGIC_COOKIE;

	return sendto(sock, (void*) bphdr, sizeof(*bphdr), 0, (struct sockaddr *)dst, sizeof(*dst));
}

int bootp_receive(struct sock *sk, struct sk_buff *skb) {
	struct bootphdr *r;
	struct in_device *in_dev;
	struct net_device *dev;
	udphdr_t *udph = udp_hdr(skb);
	struct inet_sock *inet = inet_sk(sk);

	/* check for BOOTP packet */
	if (ntohs(inet->sport) != BOOTP_PORT_CLIENT ||
			ntohs(udph->source) != BOOTP_PORT_SERVER)
		goto free_and_drop;

	r = (struct bootphdr*)(skb->h.raw + UDP_HEADER_SIZE);
	/* process only server replies */
	if (r->op != BOOTPREPLY)
		goto free_and_drop;

	netdev_foreach(dev) {
		if (!strncmp((void*)dev->dev_addr, (void*)skb->mac.raw, ETH_ALEN))
			break;
	}
	in_dev = in_dev_get(dev);

	if (r->ciaddr == 0) {
		inet_dev_set_ipaddr(in_dev, r->yiaddr);
	}

	process_options(in_dev, r);

	return ENOERR;

free_and_drop:
	skb_free(skb);
	return -NET_RX_DROP;
}
