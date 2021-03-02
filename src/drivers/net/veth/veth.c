
#include <assert.h>
#include <embox/unit.h>
#include <errno.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_entry.h>
#include <net/net_namespace.h>
#include <net/netdevice.h>

static int veth_xmit(struct net_device *dev,
		struct sk_buff *skb) {
	struct net_device_stats *lb_stats;
	size_t skb_len;

	if ((skb == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	skb_len = skb->len;

	lb_stats = &dev->stats;

	skb->dev = (struct net_device *) dev->priv;
	if (netif_rx(skb) == NET_RX_SUCCESS) {
		lb_stats->rx_packets++;
		lb_stats->rx_bytes += skb_len;
	} else {
		lb_stats->rx_err++;
	}

	return 0;
}

static const struct net_driver veth_ops = {
	.xmit = veth_xmit
};

struct net_device *veth_alloc(struct net_device **v1, struct net_device **v2) {
	struct net_device *veth1, *veth2;
	int ret;

	veth1 = etherdev_alloc(sizeof(struct net_device *));
	if (!veth1) {
		return NULL;
	}
	ret = inetdev_register_dev(veth1);
	if (ret != 0) {
		return NULL;
	}

	veth2 = etherdev_alloc(sizeof(struct net_device *));
	if (!veth2) {
		return NULL;
	}
	ret = inetdev_register_dev(veth2);
	if (ret != 0) {
		return NULL;
	}

	assign_net_ns(veth1->net_ns, init_net_ns);
	assign_net_ns(veth2->net_ns, init_net_ns);
	veth1->priv = (void *)veth2;
	veth2->priv = (void *)veth1;
	veth1->drv_ops = &veth_ops;
	veth2->drv_ops = &veth_ops;

	*v1 = veth1;
	*v2 = veth2;
	return (struct net_device *)1;
}
