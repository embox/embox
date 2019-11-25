/**
 * @file 
 * @brief Frontend initialization
 *
 * @date 
 * @author 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <kernel/printk.h>
#include <defines/null.h>

#include <xenstore.h>
#include <barrier.h>

#include "netfront.h"

#define XS_MSG_LEN 256

#define ASSERT(x)                           \
do {                                        \
	if (!(x)) {                          	\
		printk("ASSERTION FAILED!");       	\
	}                                       \
} while(0)

#define BUG_ON(x) ASSERT(!(x))

static char _text;

static char memory_pages[16][PAGE_SIZE];

static grant_entry_v1_t gnttab_table[16];
static grant_ref_t ref = 0; // it's ok

static grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame,
		int readonly)
{
    gnttab_table[ref].frame = frame;
    gnttab_table[ref].domid = domid;
    // wmb();
    readonly *= GTF_readonly;
    gnttab_table[ref].flags = GTF_permit_access | readonly;

    return ref++;
}


//int alloc_evtchn(struct xenbus_device *dev, int *port)
int alloc_evtchn(evtchn_port_t *port)
{
	struct evtchn_alloc_unbound alloc_unbound;
	int err;

	alloc_unbound.dom = DOMID_SELF;
	//alloc_unbound.remote_dom = dev->otherend_id;
	alloc_unbound.remote_dom = 0;

	err = HYPERVISOR_event_channel_op(EVTCHNOP_alloc_unbound,
					  &alloc_unbound);
	if (err)
		printk("ERROR IN alloc_evtchn");
		//xenbus_dev_fatal(dev, err, "allocating event channel");
	else
		*port = alloc_unbound.port;

	return err;
}

static void xenstore_interaction(struct netfront_dev *dev, char **ip) {
	char xs_key[XS_MSG_LEN], xs_value[XS_MSG_LEN];
	int err;

	printk(">>>>>xenstore_interaction: Start transaction\n");

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/tx-ring-ref", dev->nodename);

	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->tx_ring_ref);
	printk("tx_ring_ref=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write tx ring-ref\n");
		return;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/rx-ring-ref", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->rx_ring_ref);
	printk("rx-ring-ref=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write rx ring-ref\n");
		return;
	}

#if 0
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return;
	}
#else
	alloc_evtchn(&dev->evtchn_tx);
	alloc_evtchn(&dev->evtchn_rx);
	/*DEPRECATED
	register_event(dev->evtchn_tx, xen_net_irq);
	register_event(dev->evtchn_rx, xen_net_irq);
	*/

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel-tx", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn_tx);
	printk("event-channel-tx=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel-rx", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn_rx);
	printk("event-channel-rx=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return;
	}
#endif

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/request-rx-copy", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", 1);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write request-rx-copy\n");
		return;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/state", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", XenbusStateConnected);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] can not switch state\n");
		return;
	}

	printk(">>>>>xenstore_interaction: End transaction\n");

	printk("backend %p %d %d\n", dev->backend, sizeof(dev->backend),
			sizeof(dev->nodename));
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/backend", dev->nodename);
	xenstore_read(xs_key, dev->backend, sizeof(dev->backend));

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/mac", dev->nodename);
	xenstore_read(xs_key, dev->mac, XS_MSG_LEN);

	if ((dev->backend == NULL) || (dev->mac == NULL)) {
		printk("[PANIC!] backend/mac failed\n");
		return;
	}

	printk("backend at %s\n", dev->backend);
	printk("mac is %s\n", dev->mac);

	{
		XenbusState state;
		int count = 0;

		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/state", dev->backend);

		// xenbus_watch_path_token(XBT_NIL, path, path, &dev->events);

		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		state = atoi(xs_value);
		while (count < 10 && state < XenbusStateConnected) {
			memset(xs_value, 0, XS_MSG_LEN);
			xenstore_read(xs_key, xs_value, XS_MSG_LEN);
			state = atoi(xs_value);
			sleep(1);
			++count;
		}

		if (state != XenbusStateConnected) {
			printk("[PANIC!] backend not avalable, state=%d\n", state);
			// xenbus_unwatch_path_token(XBT_NIL, path, path);
			return;
		}
		//read ip from xenstore? o_O?
#if 0 
		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/ip", dev->backend);
		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		strcpy(*ip, xs_value);
#endif
	}

	printk(">>>>>xenstore_interaction: finish xenstore_interaction\n");
	unmask_evtchn(dev->evtchn_rx);

	return;
}
#if 1
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
		printk("NOTIFY!\n");
}
#endif

static inline int notify_remote_via_evtchn(evtchn_port_t port)
{
    evtchn_send_t op;
    op.port = port;
    return HYPERVISOR_event_channel_op(EVTCHNOP_send, &op);
}

void init_rx_buffers(struct netfront_dev *dev)
{
    int i, requeue_idx;
    netif_rx_request_t *req;
    int notify;

    /* Rebuild the RX buffer freelist and the RX ring itself. */
    for (requeue_idx = 0, i = 0; i < NET_RX_RING_SIZE; i++) 
    {
        struct net_buffer* buf = &dev->rx_buffers[requeue_idx];
        req = RING_GET_REQUEST(&dev->rx, requeue_idx);

        buf->gref = req->gref = 
            gnttab_grant_access(dev->dom,virt_to_mfn(buf->page),0);

        req->id = requeue_idx;

        requeue_idx++;
    }

    dev->rx.req_prod_pvt = requeue_idx;

    RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);

    if (notify) 
		notify_remote_via_evtchn(dev->evtchn_rx);

    dev->rx.sring->rsp_event = dev->rx.rsp_cons + 1;
}

struct netfront_dev *init_netfront(
	char *_nodename,
	void (*thenetif_rx)(unsigned char* data,
		int len, void* arg),
	unsigned char rawmac[6],
	char **ip
) {
	printk(">>>>>init_netfront\n");
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

    init_rx_buffers(dev);

    dev->netif_rx = thenetif_rx;

    //NEED IT??
	//dev->events = NULL;

	xenstore_interaction(dev, ip);

    if (rawmac)
        sscanf(dev->mac,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			   &rawmac[0],
			   &rawmac[1],
			   &rawmac[2],
			   &rawmac[3],
			   &rawmac[4],
			   &rawmac[5]);
	

	printk("nodename: %s\n"
		   "backend: %s\n"
		   "mac: %s\n"
		   "ip[hardcoded]: %s\n",
		   dev->nodename,
		   dev->backend,
		   dev->mac,
		   *ip);
#if 0
	while(1) {
		network_rx(dev);
	}
#endif
	return dev;
}
