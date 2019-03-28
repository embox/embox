/**
 * @file
 * @brief Xen PV net driver
 *
 * @author
 * @date
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <embox/unit.h>
#include <xenstore.h>
#include <kernel/printk.h>

#include <kernel/irq.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <arpa/inet.h>
#include <net/l2/ethernet.h>

#define XS_MAX_KEY_LENGTH 		256
#define XS_MAX_VALUE_LENGTH		1024
#define XS_MAX_NAME_LENGTH		16
#define XS_MAX_DOMID_LENGTH		4

#define XS_LS_NOT_RECURSIVE		0
#define XS_LS_RECURSIVE			1

EMBOX_UNIT_INIT(xen_net_init);

int xenstore_ls_val(char *ls_key, int flag) {
	char ls_value[XS_MAX_VALUE_LENGTH];
	char key[XS_MAX_KEY_LENGTH], value[XS_MAX_VALUE_LENGTH];
	int r;
	char *output;

	memset(&ls_value, 0, XS_MAX_VALUE_LENGTH);
	r = xenstore_ls(ls_key, ls_value, XS_MAX_VALUE_LENGTH);

	if (r <= 0)
		return r;

	output = ls_value;
	while(*output != 0) {
		int s;
		memset(&key, 0, XS_MAX_KEY_LENGTH);
		memset(&value, 0, XS_MAX_VALUE_LENGTH);
		sprintf(key, "%s/%s", ls_key, output);
		s = xenstore_read(key, value, XS_MAX_VALUE_LENGTH);

		if (s < 0) {
			printk("Can't read %s. Error %i\n", key, r);
			output += strlen(output) + 1;
			continue;
		}

		printk("%s = \"%s\"\n", key, value);

		if (flag == XS_LS_RECURSIVE)
			xenstore_ls_val(key, XS_LS_RECURSIVE);

		output += strlen(output) + 1;
	}
	return r;
}

static void xenstore_info() {
	char key[XS_MAX_KEY_LENGTH];
	char name[XS_MAX_NAME_LENGTH], domid[XS_MAX_DOMID_LENGTH];
	int r;

	printk("\n --- XenStore Info Begin ---\n");

	memset(&name, 0, XS_MAX_NAME_LENGTH);
	r = xenstore_read("name", name, XS_MAX_NAME_LENGTH);

	if (r < 0)
		printk("Can't retrieve name. Error %i\n", r);

	printk("\nname = %s\n", name);

	memset(&domid, 0, XS_MAX_DOMID_LENGTH);
	r = xenstore_read("domid", domid, XS_MAX_DOMID_LENGTH);

	if (r < 0)
		printk("Can't retrieve domid. Error %i\n", r);

	printk("\ndomid = %s\n", domid);

	printk("\nDomain Home Path contents:\n");
	memset(&key, 0, XS_MAX_KEY_LENGTH);
	sprintf(key, "/local/domain/%s", domid);
	r = xenstore_ls_val(key, XS_LS_RECURSIVE);

	if (r < 0)
		printk("Can't retrieve /local/domain/%s contents. Error %i\n", domid, r);

	printk("\n --- XenStore Info End ---\n");
}

struct host_net_adp {
	int fd;
};
enum host_net_op {
	HOST_NET_INIT,
	HOST_NET_START,
	HOST_NET_STOP,
};
void host_net_tx(struct host_net_adp *hnet, const void *buf, int len) {

}
int host_net_cfg(struct host_net_adp *hnet, enum host_net_op op) {
	return 0;
}
int host_net_rx_count(struct host_net_adp *hnet) {
	return 0;
}
#define SCRATCH_LEN 1514
static char host_net_scratch_rx[SCRATCH_LEN];
int host_net_rx(struct host_net_adp *hnet, void *buf, int len) {
	memcpy(buf, host_net_scratch_rx, len);

	return len;
}
#define HOST_NET_IRQ 29
struct net_device * xen_net_dev_alloc(size_t priv_size) {

	return NULL;
}
void xen_net_dev_free(struct net_device *dev) {

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
	// struct net_device *dev = (struct net_device *) dev_id;
	// struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);
	// struct sk_buff *skb;
	// int len;

	// while ((len = host_net_rx_count(hnet))) {
	// 	if (!(skb = skb_alloc(len))) {
	// 		return IRQ_NONE;
	// 	}

	// 	host_net_rx(hnet, skb->mac.raw, len);
	// 	skb->dev = dev;

	// 	netif_rx(skb);
	// }

	return IRQ_NONE;
}


static const struct net_driver xen_net_drv_ops = {
	.xmit = xen_net_xmit,
	.start = xen_net_start,
	.stop = xen_net_stop,
	.set_macaddr = xen_net_setmac,
};
static int xen_net_init(void) {
	xenstore_info();

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

	return inetdev_register_dev(nic);
}
