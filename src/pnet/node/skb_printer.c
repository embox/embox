/**
 * @file
 *
 * @brief
 *
 * @date 03.04.2012
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>

#include <net/skbuff.h>
#include <net/netdevice.h>

#include <arpa/inet.h>

static void print_skb(struct sk_buff *skb) {
	int i, j;

	printf("**********\n");
	printf("dev %s\n", skb->dev->name);
	printf("eth type = %#X ", ntohs(skb->mac.ethh->h_proto));
	printf(": %u bytes\n", skb->len);

	printf("data:\n");
	for (i = 0; i < skb->len; i += 16) {
		for (j = 0; j < 16; j += 2) {
			printf("%02x%02x ",
				(uint8_t)skb->mac.raw[i + j],
				(uint8_t)skb->mac.raw[i + j + 1]);
		}
		printf("\n");
	}
	printf("\n");
}

static int handler(struct pnet_pack *pack) {
	struct sk_buff *skb;

	skb = (struct sk_buff *) pack->data;
	print_skb(skb);
	return NET_HND_FORWARD_DEFAULT;
}

PNET_NODE_DEF("skb printer", {
	.rx_hnd = handler,
	.tx_hnd = NULL
});
