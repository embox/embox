/**
 * @file
 * @brief
 *
 * @date 23.03.12
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <assert.h>

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>


#include <net/skbuff.h>
#include <net/netdevice.h>

EMBOX_UNIT_INIT(net_dev_init);

/* single entry for all devices
       devices
       o  o  o
	\ | /
	  o
      dev_entry

*/
static net_node_t pnet_dev_register(struct net_device *dev) {
	net_node_t node = pnet_node_alloc(0, NULL);
	struct pnet_dev *node_dev = (struct pnet_dev *) node;

	node_dev->dev = dev;
	dev->pnet_node = node;
	node->rx_dfault = pnet_get_module("devs entry");

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

static int net_dev_rx_hnd(struct pnet_pack *pack) {
	struct sk_buff *skb;
	net_node_t node;

	skb = (struct sk_buff *) pack->data;

	node = pnet_get_dev_by_device(skb->dev);
	assert(node);
	pack->node = node;

	return 0;
}

PNET_NODE_DEF("devs resolver", {
	.rx_hnd = net_dev_rx_hnd,
	.tx_hnd = NULL,
});
