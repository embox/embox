/**
 * @file
 * @brief Xen PV net driver
 *
 * @author
 * @date
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <embox/unit.h>
#include <xenstore.h>
#include <kernel/printk.h>

#include <kernel/irq.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <arpa/inet.h>
#include <net/l2/ethernet.h>

#include <xen/io/ring.h>
#include <xen/xen.h>
#include <xen/io/netif.h>
#include <xen/grant_table.h>
#include <xen/io/xenbus.h>

#define XS_MAX_KEY_LENGTH 		256
#define XS_MAX_VALUE_LENGTH		1024
#define XS_MAX_NAME_LENGTH		16
#define XS_MAX_DOMID_LENGTH		4

#define XS_LS_NOT_RECURSIVE		0
#define XS_LS_RECURSIVE			1

char _text;

// #define wmb() __asm__("dsb":::"memory");
typedef uint32_t grant_ref_t;

#define PAGE_SIZE				4096
#define L1_PAGETABLE_SHIFT      12
#define VIRT_START                 ((unsigned long)&_text)
#define PFN_DOWN(x)				   ((x) >> L1_PAGETABLE_SHIFT)
#define to_phys(x)                 ((unsigned long)(x)-VIRT_START)
#define pfn_to_mfn(_pfn) 		   ((unsigned long)(_pfn))
#define virt_to_pfn(_virt)         (PFN_DOWN(to_phys(_virt)))
#define virt_to_mfn(_virt)         (pfn_to_mfn(virt_to_pfn(_virt)))

#define NET_TX_RING_SIZE __CONST_RING_SIZE(netif_tx, PAGE_SIZE)
#define NET_RX_RING_SIZE __CONST_RING_SIZE(netif_rx, PAGE_SIZE)

EMBOX_UNIT_INIT(xen_net_init);

static grant_entry_v1_t gnttab_table[16];
grant_ref_t ref = 0; // it's ok

char memory_pages[2][PAGE_SIZE];

struct net_buffer {
    void* page;
    grant_ref_t gref;
};

struct netfront_dev {
    domid_t dom;

    unsigned short tx_freelist[NET_TX_RING_SIZE + 1];
    // struct semaphore tx_sem;

    struct net_buffer rx_buffers[NET_RX_RING_SIZE];
    struct net_buffer tx_buffers[NET_TX_RING_SIZE];

    struct netif_tx_front_ring tx;
    struct netif_rx_front_ring rx;
    grant_ref_t tx_ring_ref;
    grant_ref_t rx_ring_ref;
    evtchn_port_t evtchn;

    char *nodename;
    char backend[64];
    char mac[64];

    // xenbus_event_queue events;

    void (*netif_rx)(unsigned char* data, int len, void* arg);
    void *netif_rx_arg;
};

struct netfront_dev_list {
    struct netfront_dev *dev;
    unsigned char rawmac[6];
    char *ip;

    int refcount;

    struct netfront_dev_list *next;
};

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

static void xenstore_interaction(struct netfront_dev *dev) {
	char xs_key[XS_MAX_KEY_LENGTH], xs_value[XS_MAX_KEY_LENGTH];
	int err;

	printk("Start transaction\n");

	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/tx-ring-ref", dev->nodename);

	memset(xs_value, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_value, "%u", dev->tx_ring_ref);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write tx ring-ref\n");
		return;
	}

	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/rx-ring-ref", dev->nodename);
	memset(xs_value, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_value, "%u", dev->rx_ring_ref);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write rx ring-ref\n");
		return;
	}

#if 0
	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/event-channel", dev->nodename);
	memset(xs_value, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_value, "%u", dev->evtchn);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return;
	}
#endif

	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/request-rx-copy", dev->nodename);
	memset(xs_value, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_value, "%u", 1);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write request-rx-copy\n");
		return;
	}


	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/state", dev->nodename);

	memset(xs_value, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_value, "%u", XenbusStateConnected);

#if 0
	sprintf(path, sizeof(path), "%s/state", dev->nodename);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] can not switch state\n");
		return;
	}
#endif

	printk("End transaction\n");

	printk("backend %p %d %d\n", dev->backend, sizeof(dev->backend), sizeof(dev->nodename));
	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/backend", dev->nodename);
	xenstore_read(xs_key, dev->backend, sizeof(dev->backend));

	memset(xs_key, 0, XS_MAX_KEY_LENGTH);
	sprintf(xs_key, "%s/mac", dev->nodename);
	xenstore_read(xs_key, dev->mac, XS_MAX_KEY_LENGTH);

	if ((dev->backend == NULL) || (dev->mac == NULL)) {
		printk("[PANIC!] backend/mac failed\n");
		return;
	}

	printk("backend at %s\n", dev->backend);
	printk("mac is %s\n", dev->mac);

	{
		XenbusState state;
		int count = 0;

		memset(xs_key, 0, XS_MAX_KEY_LENGTH);
		sprintf(xs_key, "%s/state", dev->backend);

		// xenbus_watch_path_token(XBT_NIL, path, path, &dev->events);

		memset(xs_value, 0, XS_MAX_KEY_LENGTH);
		xenstore_read(xs_key, xs_value, XS_MAX_KEY_LENGTH);
		state = atoi(xs_value);
		while (count < 10 && state < XenbusStateConnected) {
			memset(xs_value, 0, XS_MAX_KEY_LENGTH);
			xenstore_read(xs_key, xs_value, XS_MAX_KEY_LENGTH);
			state = atoi(xs_value);
			sleep(1);
			++count;
		}
		if (state != XenbusStateConnected) {
			printk("[PANIC!] backend not avalable, state=%d\n", state);
			// xenbus_unwatch_path_token(XBT_NIL, path, path);
			return;
		}
	}

	printk("**************************\n");
	// unmask_evtchn(dev->evtchn);

	return;
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

grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame, int readonly)
{
    gnttab_table[ref].frame = frame;
    gnttab_table[ref].domid = domid;
    // wmb();
    readonly *= GTF_readonly;
    gnttab_table[ref].flags = GTF_permit_access | readonly;

    return ref++;
}

static struct netfront_dev *init_netfront(char *_nodename,
		void (*thenetif_rx)(unsigned char* data,
			int len, void* arg),
		unsigned char rawmac[6],
		char **ip)
{

	struct netif_tx_sring *txs;
	struct netif_rx_sring *rxs;

	txs = (struct netif_tx_sring *) memory_pages[0];
	rxs = (struct netif_rx_sring *) memory_pages[1];
	memset(txs, 0, PAGE_SIZE);
	memset(rxs, 0, PAGE_SIZE);

	SHARED_RING_INIT(txs);
	SHARED_RING_INIT(rxs);

	char nodename[256];
	static int netfrontends = 0;

	snprintf(nodename, sizeof(nodename), "device/vif/%d", netfrontends);

	struct netfront_dev *dev = NULL;
	dev = malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));
	dev->nodename = strdup(nodename);

	FRONT_RING_INIT(&dev->tx, txs, PAGE_SIZE);
	FRONT_RING_INIT(&dev->rx, rxs, PAGE_SIZE);

	dev->tx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(txs), 0);
	dev->rx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(rxs), 0);

	return dev;
}

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

	char nodename[] = "device/vif/0";
	struct netfront_dev *dev = init_netfront(nodename, NULL, NULL, NULL);

	xenstore_interaction(dev);
	
	printk("nodename: %s\n"
		   "backend: %s\n"
		   "mac %s\n",
		   dev->nodename,
		   dev->backend,
		   dev->mac);
		   
	return 0;
	// return inetdev_register_dev(nic);
}
