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

//OBJALLOC_DEF(net_devs, struct net_dev, CONFIG_NET_DEVICES_QUANTITY);

static int tx_hnd(net_packet_t pack) {
	net_node_t node = pack->node;
	struct net_device *dev = member_cast_out(node, struct net_device, net_node);
	struct sk_buff *skb = pack->skbuf;
	skb->dev = dev;
	dev_queue_xmit(skb);

	return NET_HND_SUPPRESSED; /* not to be processed further */
}

static struct pnet_proto dev_proto;

/* single entry for all devices
       devices
	o o o
	\ | /
	  o
      dev_entry

*/
static struct net_node dev_entry;

static struct pnet_proto dev_entry_proto;

static int entry_tx_hnd(net_packet_t pack) {
	pack->node = pack->node->tx_dfault; //TODO
	return 0;
}

net_node_t pnet_dev_register(struct net_device *dev) {
	net_node_t node = pnet_node_init(&dev->net_node, 0, &dev_proto);

	pnet_node_attach(node, NET_RX_DFAULT, &dev_entry);

	return node;
}

pnet_proto_t pnet_dev_get_proto(void) {
	return &dev_proto;
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
