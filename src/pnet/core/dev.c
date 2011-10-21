/**
 * @file
 * @brief Network socket
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <pnet/types.h>
#include <pnet/core.h>
#include <embox/unit.h>

#include <errno.h>
#include <string.h>
#include <mem/objalloc.h>

EMBOX_UNIT_INIT(net_dev_init);

OBJALLOC_DEF(net_devs, struct net_dev, CONFIG_NET_DEVICES_QUANTITY);

static int tx_hnd(net_packet_t pack) {
	net_dev_t dev = (net_dev_t) pack->node->node_addr;
	dev->ops->tx(pack, dev);
	return NET_HND_SUPPRESSED; /* not to be processed further */
}

struct net_proto dev_proto;

net_dev_t net_dev_register(net_dev_ops_t dev_ops) {
	net_dev_t dev = objalloc(&net_devs);
	dev->ops = dev_ops;
	dev->node = net_node_alloc((net_addr_t)	dev, &dev_proto);
	return dev;
}

#if 1
static int net_dev_init(void) {
	net_proto_init(&dev_proto, 0, NULL, tx_hnd);
	return 0;
}
#endif
