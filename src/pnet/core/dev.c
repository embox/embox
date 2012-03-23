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
#include <pnet/repo.h>

#include <util/member.h>
#include <net/netdevice.h>

#include <embox/unit.h>
#include <pnet/pnet_pack.h>

EMBOX_UNIT_INIT(net_dev_init);

#if 0
static int tx_hnd(struct pnet_pack * pack) {

	struct net_device *dev = ((struct pnet_dev *) pack->node)->dev;
	struct sk_buff *skb = pack->data;
	skb->dev = dev;
	dev->netdev_ops->ndo_start_xmit(skb, dev);

	return NET_HND_SUPPRESSED; /* not to be processed further */
}


static int entry_tx_hnd(struct pnet_pack * pack) {
	pack->node = pack->node->tx_dfault; //TODO
	return 0;
}
#endif

/* single entry for all devices
       devices
       o  o  o
	\ | /
	  o
      dev_entry

*/

net_node_t pnet_dev_register(struct net_device *dev) {
	net_node_t node = pnet_node_alloc(0, NULL);
	struct pnet_dev *node_dev = (struct pnet_dev *) node;

	node_dev->dev = dev;
	dev->pnet_node = node;
	node->rx_dfault = pnet_get_module("devs entry");

//	pnet_node_attach(node, NET_RX_DFAULT, pnet_get_module("devs entry");

	return node;
}

struct net_device *pnet_get_net_device(net_node_t node) {
	return ((struct pnet_dev *) node)->dev;
}

static int net_dev_init(void) {
	struct net_device *dev;
	net_node_t node;

	netdev_foreach(dev) {
		if (dev) {
			node = pnet_dev_register(dev);
		}
	}

	return 0;
}

PNET_NODE_DEF("devs entry", {});

#if 0
PNET_PROTO_DEF("dev", {
	.tx_hnd = tx_hnd
});
#endif

