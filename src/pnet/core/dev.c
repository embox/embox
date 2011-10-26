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

#include <embox/unit.h>

EMBOX_UNIT_INIT(net_dev_init);

OBJALLOC_DEF(net_devs, struct net_dev, CONFIG_NET_DEVICES_QUANTITY);

static int tx_hnd(net_packet_t pack) {
	net_dev_t dev = (net_dev_t) pack->node->node_addr;
	dev->ops->tx(pack, dev);
	return NET_HND_SUPPRESSED; /* not to be processed further */
}

static struct net_proto dev_proto;

/* single entry for all devices
       devices
	o o o
	\ | /
	  o
      dev_entry

*/
static struct net_node dev_entry;

static struct net_proto dev_entry_proto;

static int entry_tx_hnd(net_packet_t pack) {
	pack->node = pack->node->tx_dfault; //TODO
	return 0;
}

net_dev_t pnet_dev_register(net_dev_ops_t dev_ops) {
	net_dev_t dev = objalloc(&net_devs);

	dev->ops = dev_ops;
	dev->id = ((int) dev - (int) net_devs.storage) / sizeof (struct net_dev);
	dev->node = pnet_node_alloc((net_addr_t) dev, &dev_proto);

	dev->node->rx_dfault = &dev_entry;

	return dev;
}

net_node_t pnet_dev_get_entry(void) {
	return &dev_entry;
}

static int net_dev_init(void) {
	pnet_proto_init(&dev_proto, 0, NULL, tx_hnd);

	pnet_proto_init(&dev_entry_proto, 0, NULL, entry_tx_hnd);

	dev_entry.proto = &dev_entry_proto;
        dev_entry.rx_dfault = pnet_get_node_null();

	return 0;
}
