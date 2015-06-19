/**
 * @file
 * @brief Usermode ethernet driver (supported by emvisor)
 *
 * @author  Anton Kozlov
 * @date    31.03.2013
 */

#include <errno.h>
#include <string.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <arpa/inet.h>
#include <net/skbuff.h>
#include <embox/unit.h>
#include <net/l0/net_entry.h>

#include <kernel/printk.h>
#include <kernel/host.h>

EMBOX_UNIT_INIT(umether_init);

static int umether_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	host_net_tx(hnet, skb->mac.raw, skb->len);

	return 0;
}

static int umether_start(struct net_device *dev) {
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_START);
}

static int umether_stop(struct net_device *dev) {
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_STOP);
}

static int umether_setmac(struct net_device *dev, const void *addr) {
	return ENOERR;
}

static irq_return_t umether_irq(unsigned int irq_num, void *dev_id) {
	struct net_device *dev = (struct net_device *) dev_id;
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);
	struct sk_buff *skb;
	int len;

	while ((len = host_net_rx_count(hnet))) {
		if (!(skb = skb_alloc(len))) {
			return IRQ_NONE;
		}

		host_net_rx(hnet, skb->mac.raw, len);
		skb->dev = dev;

		netif_rx(skb);
	}

	return IRQ_NONE;
}

static const struct net_driver umether_drv_ops = {
	.xmit = umether_xmit,
	.start = umether_start,
	.stop = umether_stop,
	.set_macaddr = umether_setmac,
};

static int umether_init(void) {
	int res = 0;
	struct net_device *nic;
	struct host_net_adp *hnet;

	nic = etherdev_alloc(sizeof(struct host_net_adp));
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->drv_ops = &umether_drv_ops;
	nic->irq = HOST_NET_IRQ;

	hnet = netdev_priv(nic, struct host_net_adp);

	res = host_net_cfg(hnet, HOST_NET_INIT);
	if (res < 0) {
		etherdev_free(nic);
		printk("usermode: can't init network: %s\n", strerror(-res));
		return 0;
	}

	res = irq_attach(HOST_NET_IRQ, umether_irq, IF_SHARESUP, nic,
			"umether");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}
