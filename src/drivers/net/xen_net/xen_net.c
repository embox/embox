/**
 * @file 
 * @brief Xen PV net driver initialization
 *
 * @date 
 * @author 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <embox/unit.h>
#include <kernel/printk.h>
#include <defines/null.h>

#include <kernel/irq.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <arpa/inet.h>
#include <net/l2/ethernet.h>

#include "xen_net.h"
#include "xs_info.h"
#include "netfront.h"

EMBOX_UNIT_INIT(xen_net_init);

#if 0
static int host_net_rx(struct host_net_adp *hnet, void *buf, int len) {
	memcpy(buf, host_net_scratch_rx, len);

	return len;
}

static int host_net_rx_count(struct host_net_adp *hnet) {
	return 0;
}

static struct net_device * xen_net_dev_alloc(size_t priv_size) {

	return NULL;
}

static void xen_net_dev_free(struct net_device *dev) {}
#endif

static void host_net_tx(struct host_net_adp *hnet, const void *buf, int len) {}

static int host_net_cfg(struct host_net_adp *hnet, enum host_net_op op) {
	return 0;
}

static int xen_net_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	host_net_tx(hnet, skb->mac.raw, skb->len);

	return 0;
}

static int xen_net_start(struct net_device *dev) {
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_START);
}

static int xen_net_stop(struct net_device *dev) {
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_STOP);
}

static int xen_net_setmac(struct net_device *dev, const void *addr) {
	return ENOERR;
}

static irq_return_t xen_net_irq(unsigned int irq_num, void *dev_id) {
#if 0
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
#endif
	return IRQ_NONE;
}

static const struct net_driver xen_net_drv_ops = {
	.xmit = xen_net_xmit,
	.start = xen_net_start,
	.stop = xen_net_stop,
	.set_macaddr = xen_net_setmac,
};

static int xen_net_init(void) {
	int res = 0;
	struct net_device *nic;
	
	nic = etherdev_alloc(sizeof(struct host_net_adp));
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &xen_net_drv_ops;
	nic->irq = HOST_NET_IRQ;

	res = irq_attach(HOST_NET_IRQ, xen_net_irq, IF_SHARESUP, nic,
			"xen_net");
	if (res < 0) {
		return res;
	}

	xenstore_info();

	char nodename[] = "device/vif/0";
	struct netfront_dev *dev = init_netfront(nodename, NULL, NULL, NULL);
	
	printk("nodename: %s\n"
		   "backend: %s\n"
		   "mac %s\n",
		   dev->nodename,
		   dev->backend,
		   dev->mac);
		   
	return 0;
	// return inetdev_register_dev(nic);
}
