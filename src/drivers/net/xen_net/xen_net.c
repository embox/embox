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
	printk("!!!ALARM!!! xen_net_xmit is called, you should check the code!");
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	host_net_tx(hnet, skb->mac.raw, skb->len);

	return 0;
}

static int xen_net_start(struct net_device *dev) {
	printk("!!!ALARM!!! xen_net_start is called, you should check the code!");
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_START);
}

static int xen_net_stop(struct net_device *dev) {
	printk("!!!ALARM!!! xen_net_stop is called, you should check the code!");
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_STOP);
}

static int xen_net_setmac(struct net_device *dev, const void *addr) {
	printk("!!!ALARM!!! xen_net_setmac is called, you should check the code!");
	return ENOERR;
}

static irq_return_t xen_net_irq(unsigned int irq_num, void *dev_id) {
	printk("======>IRQ:%u\n",irq_num);
//TODO: Implement handler
//TODO: need save flags?
	struct net_device *dev;
	struct netfront_dev *nic_priv;

	dev = dev_id;
	nic_priv = netdev_priv(dev, struct netfront_dev);

	network_rx(nic_priv, dev);
	printk("======>IRQ_NONE\n");
	return IRQ_NONE;
}

static const struct net_driver xen_net_drv_ops = {
	.xmit = xen_net_xmit,
	.start = xen_net_start,
	.stop = xen_net_stop,
	.set_macaddr = xen_net_setmac,
};

#include <xen_hypercall-x86_32.h>

int get_max_nr_grant_frames() {
    struct gnttab_query_size query;
	
    int rc;
	query.dom = DOMID_SELF;
	rc = HYPERVISOR_grant_table_op(GNTTABOP_query_size, &query, 1);
	if ((rc < 0) || (query.status != GNTST_okay))
        return 4; /* Legacy max supported number of frames */
    return query.max_nr_frames;
}


#include <xen/xen.h>
#include <xen/version.h>
#include <xen/features.h>
int is_auto_translated_physmap(void) {
    unsigned char xen_features[32]; //__read_mostly
	struct xen_feature_info fi;

	int j;
    fi.submap_idx = 0;
    if (HYPERVISOR_xen_version(XENVER_get_features, &fi) < 0) 
    {
        printk("error while feature getting!");
    }
    for (j = 0; j < 32; j++)
    {
        xen_features[j] = !!(fi.submap & 1<<j);
    }

    return xen_features[XENFEAT_auto_translated_physmap];
}


static int xen_net_init(void) {
	/*
	printk("\n");
	printk(">>>>>xen_net_init\n");
    printk("max number of grant FRAMES:%d\n", get_max_nr_grant_frames()); //32
    printk("XENFEAT_auto_translated_physmap=%d\n", is_auto_translated_physmap()); //0
    printk("PAGE_SIZE=%d\n",PAGE_SIZE());
  */  
	
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
		printk("PANIC1");
		return res;
	}

	nic->irq = nic_priv->evtchn;
	res = irq_attach(nic_priv->evtchn, xen_net_irq, IF_SHARESUP, nic,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}

	res = inetdev_register_dev(nic);
	if (res < 0) {
		printk("inetdev_register_dev error: %i\n", res);
	}
	return res;



#if 0
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
