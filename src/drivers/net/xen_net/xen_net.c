/**
 * @file 
 * @brief Xen PV net driver initialization
 *
 * @date 
 * @author 
 */
#include <embox/unit.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <kernel/printk.h>
#include <kernel/irq.h>

#include "xen_net.h"
#include "xs_info.h"
#include "netfront.h"
#include <net/l2/ethernet.h>
#include <net/inetdevice.h>
#include <kernel/sched/sched_lock.h>

EMBOX_UNIT_INIT(xen_net_init);

static int xen_net_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct netfront_dev *nic_priv;
	nic_priv = netdev_priv(dev, struct netfront_dev);
	netfront_xmit(nic_priv, skb->mac.raw, skb->len );
	skb_free(skb);
	return ENOERR;
}

static irq_return_t xen_net_irq(unsigned int irq_num, void *dev_id) {
	struct net_device *dev;
	struct netfront_dev *nic_priv;

	dev = dev_id;
	nic_priv = netdev_priv(dev, struct netfront_dev);

	sched_lock();
#ifndef FEATURE_SPLIT_CHANNELS 
	network_tx_buf_gc(nic_priv);
#endif
	network_rx(nic_priv, dev);
	sched_unlock();
	return IRQ_NONE;
}

static irq_return_t xen_net_irq_tx(unsigned int irq_num, void *dev_id) {
	struct net_device *dev;
	struct netfront_dev *nic_priv;

	dev = dev_id;
	nic_priv = netdev_priv(dev, struct netfront_dev);

	sched_lock();
	network_tx_buf_gc(nic_priv);
	sched_unlock();
	return IRQ_NONE;
}

static int xen_net_start(struct net_device *dev) {
	int res;
	struct netfront_dev *nic_priv;
	nic_priv = netdev_priv(dev, struct netfront_dev);
#ifdef FEATURE_SPLIT_CHANNELS 
	dev->irq = nic_priv->evtchn_rx;
	res = irq_attach(nic_priv->evtchn_rx, xen_net_irq, IF_SHARESUP, dev,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
	res = irq_attach(nic_priv->evtchn_tx, xen_net_irq_tx, IF_SHARESUP, dev,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
#else
	dev->irq = nic_priv->evtchn;
	res = irq_attach(nic_priv->evtchn, xen_net_irq, IF_SHARESUP, dev,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
#endif
	return ENOERR;
}

static int xen_net_stop(struct net_device *dev) {
	return ENOERR;
}

static int xen_net_setmac(struct net_device *dev, const void *addr) {
	memcpy(dev->dev_addr, addr, ETH_ALEN);

	return ENOERR;
}

static const struct net_driver xen_net_drv_ops = {
	.xmit = xen_net_xmit,
	.start = xen_net_start,
	.stop = xen_net_stop,
	.set_macaddr = xen_net_setmac,
};

static int xen_net_init(void) {
	int res;
	struct net_device *nic;
	struct netfront_dev *nic_priv;
	
	nic = etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &xen_net_drv_ops;

	nic_priv = netdev_priv(nic, struct netfront_dev);
	memset(nic_priv, 0, sizeof(*nic_priv));

	res = netfront_priv_init(nic_priv);
	if (res != 0) {
		return res;
	}

	res = inetdev_register_dev(nic);
	if (res < 0) {
		printk("inetdev_register_dev error: %i\n", res);
	}
	return res;
}
