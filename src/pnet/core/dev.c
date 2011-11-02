/**
 * @file
 * @brief Network socket
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <errno.h>
#include <string.h>
#include <mem/objalloc.h>

#include <pnet/types.h>
#include <pnet/core.h>
#include <pnet/node.h>

#include <util/member.h>
#include <net/netdevice.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(net_dev_init);

static int tx_hnd(net_packet_t pack) {
	net_node_t node = pack->node;
	struct net_device *dev = member_cast_out(node, struct net_device, net_node);
	struct sk_buff *skb = pack->skbuf;
	skb->dev = dev;
	dev->netdev_ops->ndo_start_xmit(skb, dev);
	return NET_HND_SUPPRESSED; /* not to be processed further */
}

static struct pnet_proto dev_proto = {
	.tx_hnd = tx_hnd
};

/* single entry for all devices
       devices
       o  o  o
	\ | /
	  o
      dev_entry

*/

static int entry_tx_hnd(net_packet_t pack) {
	pack->node = pack->node->tx_dfault; //TODO
	return 0;
}

static struct pnet_proto dev_entry_proto = {
	.tx_hnd = entry_tx_hnd
};

static struct net_node dev_entry = {
	.proto = &dev_entry_proto
};

net_node_t pnet_dev_register(struct net_device *dev) {
	net_node_t node = pnet_node_init(&dev->net_node, 0, &dev_proto);

	pnet_node_attach(node, NET_RX_DFAULT, &dev_entry);

	return node;
}

struct net_device *pnet_get_net_device(net_node_t *node) {
	return member_cast_out(node, struct net_device, net_node);
}

pnet_proto_t pnet_dev_get_proto(void) {
	return &dev_proto;
}

net_node_t pnet_dev_get_entry(void) {
	return &dev_entry;
}

static int net_dev_init(void) {

	return 0;
}
