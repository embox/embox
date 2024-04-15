/**
 * @file
 * @brief Network socket
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <errno.h>
#include <string.h>
#include <embox/unit.h>

#include <pnet/core/types.h>
#include <pnet/core/core.h>
#include <pnet/core/node.h>
#include <pnet/core/repo.h>
#include <pnet/pack/pnet_pack.h>
#include <pnet/core/dev.h>

#include <net/netdevice.h>
#include <lib/libds/hashtable.h>

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
	node->name = dev->name;

	return node;
}

struct net_device *pnet_get_net_device(net_node_t node) {
	return ((struct pnet_dev *) node)->dev;
}

PNET_NODE_DEF("devs entry", {});

static int net_dev_init(void) {
	struct net_device *dev;
	net_node_t entry;

	netdev_foreach(dev) {
		if (dev) {
			pnet_dev_register(dev);
		}
	}

	entry = pnet_get_module("pnet entry");
	entry->rx_dfault = pnet_get_module("devs entry");

	return 0;
}
