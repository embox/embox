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

#include <barrier.h>

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
	printk("xen_net_xmit is called, you should check the code!");
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	host_net_tx(hnet, skb->mac.raw, skb->len);

	return 0;
}

static int xen_net_start(struct net_device *dev) {
	printk("xen_net_start is called, you should check the code!");
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_START);
}

static int xen_net_stop(struct net_device *dev) {
	printk("xen_net_stop is called, you should check the code!");
	struct host_net_adp *hnet = netdev_priv(dev, struct host_net_adp);

	return host_net_cfg(hnet, HOST_NET_STOP);
}

static int xen_net_setmac(struct net_device *dev, const void *addr) {
	printk("xen_net_setmac is called, you should check the code!");
	return ENOERR;
}
#if 1
static irq_return_t xen_net_irq(unsigned int irq_num, void *dev_id) {
	printk("======>IRQ:%u\n",irq_num);
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
#endif

static const struct net_driver xen_net_drv_ops = {
	.xmit = xen_net_xmit,
	.start = xen_net_start,
	.stop = xen_net_stop,
	.set_macaddr = xen_net_setmac,
};

static void print_packet(unsigned char* data, int len, void* arg) {
	unsigned char *out = data;

	printk("Packet: [");
	while (len) {
		printk("%c ", *out++);
		--len;
	}
	printk("]\n");
}
#if 0
static inline int xennet_rxidx(RING_IDX idx)
{
    return idx & (NET_RX_RING_SIZE - 1);
}
void network_rx(struct netfront_dev *dev)
{
    RING_IDX rp,cons,req_prod;
    int nr_consumed, i;
	int more, notify;
    int dobreak;

    nr_consumed = 0;
moretodo:
    rp = dev->rx.sring->rsp_prod;
    rmb(); /* Ensure we see queued responses up to 'rp'. */

    dobreak = 0;
    for (cons = dev->rx.rsp_cons; cons != rp && !dobreak; nr_consumed++, cons++)
    {
		printk("IM IN IT");
        struct net_buffer* buf;
        unsigned char* page;
        int id;

        struct netif_rx_response *rx = RING_GET_RESPONSE(&dev->rx, cons);

        id = rx->id;
        BUG_ON(id >= NET_RX_RING_SIZE);

        buf = &dev->rx_buffers[id];
        page = (unsigned char*)buf->page;
        // gnttab_end_access(buf->gref);

        if (rx->status > NETIF_RSP_NULL) {
#ifdef HAVE_LIBC
            if (dev->netif_rx == NETIF_SELECT_RX) {
                int len = rx->status;
                ASSERT(current == main_thread);
                if (len > dev->len)
                    len = dev->len;
                memcpy(dev->data, page+rx->offset, len);
                dev->rlen = len;
                /* No need to receive the rest for now */
                dobreak = 1;
            } else
#endif
		        dev->netif_rx(page+rx->offset, rx->status, dev->netif_rx_arg);
        }
    }
    dev->rx.rsp_cons=cons;

    RING_FINAL_CHECK_FOR_RESPONSES(&dev->rx,more);
    if(more && !dobreak) goto moretodo;

    req_prod = dev->rx.req_prod_pvt;

    for (i = 0; i < nr_consumed; i++) {
        int id = xennet_rxidx(req_prod + i);
        netif_rx_request_t *req = RING_GET_REQUEST(&dev->rx, req_prod + i);
        struct net_buffer* buf = &dev->rx_buffers[id];
        void* page = buf->page;

        /* We are sure to have free gnttab entries since they got released above */
        buf->gref = req->gref = gnttab_grant_access(dev->dom,
                                                    virt_to_mfn(page),
                                                    0);
        req->id = id;
    }

    wmb();

    dev->rx.req_prod_pvt = req_prod + i;
    RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);
    if (notify)
	{
		printk("NOTIFY!\n");
	}
}
#endif

static int xen_net_init(void) {
	printk("\n");
	printk(">>>>>xen_net_init\n");
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
	struct netfront_dev *dev = init_netfront(nodename, print_packet, rawmac, &ip);
	printk(">>>>>afterinit_netfront\n");
	
	//Danger! hardcode of dev->evtchn_rx = 9
	nic->irq = dev->evtchn_rx;
	res = irq_attach(dev->evtchn_rx, xen_net_irq, IF_SHARESUP, nic,
			"xen_net");
	if (res < 0) {
		printk("irq_attach error: %i\n", res);
		return res;
	}
	
#if 0
	while(1) {
		network_rx(dev);
	}
#endif

	printk("%s\n", ip);
	printk("nodename: %s\n"
		   "backend: %s\n"
		   "mac: %s\n"
		   "ip: %s\n",
		   dev->nodename,
		   dev->backend,
		   dev->mac,
		   ip);

	//free(ip);
	
	return inetdev_register_dev(nic);
}
