/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>

#include <pnet/prior_path.h>

#include <net/skbuff.h>
#include <net/netdevice.h>

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>

static net_node_t pnet_get_dev_by_device(struct net_device *dev) {
	net_node_t node = dev->pnet_node;

	if (NULL == node) {
		return pnet_get_module("devs entry");
	}

	return node;
}

int pnet_entry(struct net_packet *pack) {
	net_device_t *dev = pnet_get_net_device(pack->node);
	pack->skbuf->dev = dev;

	netif_rx(pack->skbuf);

	pnet_pack_free(pack);

	return 0;
}

int netif_rx(struct sk_buff *skb) {
	net_device_t *dev;

	if (NULL == skb) {
		return NET_RX_DROP;
	}

	dev = skb->dev;

	if (NULL == dev) {
		kfree_skb(skb);
		return NET_RX_DROP;
	}

	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;
	skb_queue_tail(&(dev->dev_queue), skb);
	netif_rx_schedule(dev);
	return NET_RX_SUCCESS;
}

int netif_receive_skb(sk_buff_t *skb) {
	net_packet_t pack;
	net_node_t node = pnet_get_dev_by_device(skb->dev);
        pack = pnet_pack_alloc_skb(node, NET_PACKET_RX, skb);

	pnet_rx_thread_add(pack);

	return 0;
}
