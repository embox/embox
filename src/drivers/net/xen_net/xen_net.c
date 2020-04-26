/**
 * @file 
 * @brief Xen PV net driver initialization
 *
 * @date 
 * @author 
 */
#include <embox/unit.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <kernel/printk.h>
#include <defines/null.h>

#include <kernel/irq.h>

#include "xen_net.h"
#include "xs_info.h"
#include "netfront.h"

EMBOX_UNIT_INIT(xen_net_init);
#include <kernel/sched/sched_lock.h>
static int xen_net_xmit(struct net_device *dev, struct sk_buff *skb) {
//TODO: add lock!!!
	sched_lock();
	netfront_xmit(netdev_priv(dev, struct netfront_dev), skb->mac.raw, skb->len );
	//printk("!!!ALARM!!! xen_net_xmit is called, you should check the code!\n");
	sched_unlock();
	return ENOERR;
}

static int xen_net_start(struct net_device *dev) {
	//printk("!!!ALARM!!! xen_net_start is called, you should check the code!\n");
//TODO
	return ENOERR;
}

static int xen_net_stop(struct net_device *dev) {
	printk("!!!ALARM!!! xen_net_stop is called, you should check the code!\n");
	return ENOERR;
}

static int xen_net_setmac(struct net_device *dev, const void *addr) {
	memcpy(dev->dev_addr, addr, ETH_ALEN);
	/*
	struct netfront_dev *nic_priv;
	nic_priv = netdev_priv(dev, struct netfront_dev);

	unsigned char rawmac[6];

	sscanf(nic_priv->mac,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
		&rawmac[0],
		&rawmac[1],
		&rawmac[2],
		&rawmac[3],
		&rawmac[4],
		&rawmac[5]);
	*/
	//printk("!!!ALARM!!! xen_net_setmac is called, you should check the code!\n");
	
	return ENOERR;

}

static irq_return_t xen_net_irq(unsigned int irq_num, void *dev_id) {
	//printk("======>IRQ:%u\n",irq_num);
//TODO: Implement handler
//TODO: need save flags?
sched_lock();
	struct net_device *dev;
	struct netfront_dev *nic_priv;

	dev = dev_id;
	nic_priv = netdev_priv(dev, struct netfront_dev);

	network_rx(nic_priv, dev);
//TODO: clean tx
	//printk("======>IRQ_NONE\n");
	sched_unlock();
	return IRQ_NONE;
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
	//struct netfront_dev *dev;

	
	nic = etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &xen_net_drv_ops;
	//nic->base_addr = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;
	nic_priv = netdev_priv(nic, struct netfront_dev);

	//virtio_config(nic);

	res = netfront_priv_init(nic_priv);
	if (res != 0) {
		return res;
	}

	nic->irq = nic_priv->evtchn;
	res = irq_attach(nic_priv->evtchn, xen_net_irq, IF_SHARESUP, nic,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
//move it to start?
	res = inetdev_register_dev(nic);
	if (res < 0) {
		printk("inetdev_register_dev error: %i\n", res);
	}
	return res;



#if 0 //old stuff
	int res = 0;
	struct net_device *nic;

	nic = etherdev_alloc(sizeof(struct host_net_adp));
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &xen_net_drv_ops;

	//xenstore_info();

	char nodename[] = "device/vif/0";
	unsigned char rawmac[6];
	char *ip = (char *) malloc(16);
	ip="192.168.2.2";
	dev = init_netfront(nodename, print_packet, rawmac, &ip);
	printk(">>>>>afterinit_netfront\n");
	
#ifdef FEATURE_SPLIT_CHANNELS //false
    //Danger! hardcode of dev->evtchn_rx = 9
	nic->irq = 9;
	res = irq_attach(9, xen_net_irq, IF_SHARESUP, nic,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
#else
	nic->irq = dev->evtchn;
	res = irq_attach(dev->evtchn, xen_net_irq, IF_SHARESUP, nic,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
#endif
/*
	printk("%s\n", ip);
	printk("nodename: %s\n"
		   "backend: %s\n"
		   "mac: %s\n"
		   "ip: %s\n",
		   dev->nodename,
		   dev->backend,
		   dev->mac,
		   ip);
*/
	//free(ip);
	return inetdev_register_dev(nic);
#endif
}
