/**
 * @file
 *
 * @brief used with linux network stack, optionally routes packs to pnet
 *
 * @date 21.10.2011
 * @author Anton Kozlov
 */

#include <net/in.h>
#include <pnet/prior_path.h>
#include <kernel/thread/api.h>
#include <net/skbuff.h>

#include <pnet/core.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(pnet_linux_init);

net_dev_t pnet_get_dev_by_device(struct net_device *dev) {
	return NULL;
}

int pnet_netif_rx(struct sk_buff *skb) {
	net_packet_t pack;
	net_dev_t dev = pnet_get_dev_by_device(skb->dev);

        pack = net_pack_alloc(dev->node, NET_PACKET_RX, skb->data, skb->len);

	pack->skbuf = skb;

	rx_thread_add(pack);

	return 0;
}

struct net_proto linux_out;

static int pnet_linux_rx(net_packet_t pack) {
//	netif_rx(pack->skbuf);
	return 0;
}

static net_node_t lin_gate = NULL;

net_node_t pnet_get_linux_out(void) {
	return lin_gate;
}

static int pnet_linux_init(void) {
	int res = net_proto_init(&linux_out, 0, pnet_linux_rx, NULL);

	lin_gate = net_node_alloc(0, &linux_out);

	return res;
}

