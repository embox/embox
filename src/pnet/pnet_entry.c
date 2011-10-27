/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <pnet/prior_path.h>

#include <net/skbuff.h>
#include <net/netdevice.h>

#include <pnet/core.h>
#include <pnet/node.h>

static net_node_t pnet_get_dev_by_device(struct net_device *dev) {
	return pnet_dev_get_entry();
}

int netif_receive_skb(sk_buff_t *skb) {
	net_packet_t pack;
	net_node_t node = pnet_get_dev_by_device(skb->dev);

        pack = pnet_pack_alloc(node, NET_PACKET_RX, skb->data, skb->len);

	pack->skbuf = skb;

	pnet_rx_thread_add(pack);

	return 0;
}
