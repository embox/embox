/*
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
#include <net/in.h>
#include <net/route.h>
#include <net/bootp.h>
#include <kernel/time/time.h>
#include <kernel/time/timer.h>
#include <errno.h>
#include <string.h>
#include <prom/prom_printf.h>

static void process_options(in_device_t *in_dev, struct bootphdr *r) {
	struct in_addr addr;
	unsigned int magic_number = 0x63538263;
	unsigned char *op = r->options;
	const unsigned char *op_begin = op;

	if (*((unsigned int*)op) != magic_number)
		return;

	op += sizeof(magic_number);

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
			addr.s_addr = *((in_addr_t*)op);
			inet_dev_set_mask(in_dev, addr.s_addr);
			op += IP_ADDR_LEN;
			break;
		default:
			op++;
			break;
		}
	}
}

int bootp_receive(struct sock *sk, struct sk_buff *skb) {
	struct bootphdr *r;
	in_device_t *in_dev;
	net_device_t *dev;
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
