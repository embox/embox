/**
 * @file
 * @brief
 *
 * @date 25.10.2011
 * @author Anton Kozlov
 */
#include <stdio.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>
#include <net/skbuff.h>

static int stamp = 1;
static void print_pack(struct pnet_pack *pack) {
	struct sk_buff *skb;

	printf("%d: ", stamp++);
	switch(pack->dir) {
	case PNET_PACK_DIRECTION_RX:
		printf("RX");
		break;
	case PNET_PACK_DIRECTION_TX:
		printf("TX");
		break;
	};
	skb = (struct sk_buff *) pack->data;
	printf(": %d bytes\n", skb->len);
}

static int net_info_tx_hnd(struct pnet_pack *pack) {
	print_pack(pack);
	return NET_HND_FORWARD_DEFAULT;
}

static int net_info_rx_hnd(struct pnet_pack *pack) {
	print_pack(pack);
	return NET_HND_FORWARD_DEFAULT;
}

PNET_NODE_DEF("info printer", {
	.rx_hnd = net_info_rx_hnd,
	.tx_hnd = net_info_tx_hnd
});
