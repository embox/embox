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
#include <net/netdevice.h>

#include <pnet/core.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(pnet_linux_init);

net_node_t pnet_get_dev_by_device(struct net_device *dev) {
	return pnet_dev_get_entry();
}

int pnet_netif_rx(struct sk_buff *skb) {
	net_packet_t pack;
	net_node_t node = pnet_get_dev_by_device(skb->dev);

        pack = pnet_pack_alloc(node, NET_PACKET_RX, skb->data, skb->len);

	pack->skbuf = skb;

	pnet_rx_thread_add(pack);

	return 0;
}

struct net_proto linux_out;

static int pnet_linux_rx(net_packet_t pack) {
	netif_rx(pack->skbuf);
	return NET_HND_SUPPRESSED;
}

static struct net_node lin_gate = {
	.proto = &linux_out
};

net_node_t pnet_get_node_linux_gate(void) {
	return &lin_gate;
}

static int pnet_linux_init(void) {
	int res = pnet_proto_init(&linux_out, 0, pnet_linux_rx, NULL);

	return res;
}

