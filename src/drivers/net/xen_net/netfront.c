/**
 * @file 
 * @brief Frontend initialization
 *
 * @date 
 * @author 
 */

#include <barrier.h>
#include <grant_table.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xenstore.h>

#include <kernel/printk.h>
#include <kernel/sched/sched_lock.h>
#include <net/l0/net_entry.h>
#include <xen/event.h>
#include <xen/io/xenbus.h>
#include <xen/memory.h>

#include "netfront.h"

#define XS_MSG_LEN 256
#define ASSERT(x)                        \
	do {                                 \
		if (!(x)) {                      \
			printk("ASSERTION FAILED!"); \
		}                                \
	} while (0)

#define BUG_ON(x)           ASSERT(!(x))
#define NET_FRONTEND_NUMBER 0

static inline void add_id_to_freelist(unsigned int id,
    unsigned short *freelist) {
	freelist[id + 1] = freelist[0];
	freelist[0] = id;
}

static inline unsigned short get_id_from_freelist(unsigned short *freelist) {
	unsigned int id = freelist[0];
	freelist[0] = freelist[id + 1];
	return id;
}

int alloc_evtchn(evtchn_port_t *port) {
	struct evtchn_alloc_unbound alloc_unbound;
	int err;

	alloc_unbound.dom = DOMID_SELF;
	alloc_unbound.remote_dom = 0;

	err = HYPERVISOR_event_channel_op(EVTCHNOP_alloc_unbound, &alloc_unbound);
	if (err)
		printk("ERROR IN alloc_evtchn");
	else
		*port = alloc_unbound.port;

	return err;
}

int setup_netfront(struct netfront_dev *dev) {
	// Rings
	struct netif_tx_sring *txs;
	struct netif_rx_sring *rxs;

	txs = (struct netif_tx_sring *)xen_mem_alloc(1);
	rxs = (struct netif_rx_sring *)xen_mem_alloc(1);

	memset(txs, 0, PAGE_SIZE());
	memset(rxs, 0, PAGE_SIZE());

	SHARED_RING_INIT(txs);
	SHARED_RING_INIT(rxs);

	FRONT_RING_INIT(&dev->tx, txs, PAGE_SIZE());
	FRONT_RING_INIT(&dev->rx, rxs, PAGE_SIZE());

	dev->tx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(txs), 0);
	dev->rx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(rxs), 0);

	// Alloc pages for buffer rings
	int i;
	for (i = 0; i < NET_TX_RING_SIZE; i++) {
		struct net_buffer *buf = &dev->tx_buffers[i];
		buf->page = (void *)xen_mem_alloc(1);
		buf->gref = gnttab_grant_access(dev->dom, virt_to_mfn(buf->page), 1);
	}

	for (i = 0; i < NET_RX_RING_SIZE; i++) {
		struct net_buffer *buf = &dev->rx_buffers[i];
		buf->page = (void *)xen_mem_alloc(1);
		buf->gref = gnttab_grant_access(dev->dom, virt_to_mfn(buf->page), 1);
	}

	// Event channel
#ifdef FEATURE_SPLIT_CHANNELS
	alloc_evtchn(&dev->evtchn_rx);
	alloc_evtchn(&dev->evtchn_tx);
#else
	alloc_evtchn(&dev->evtchn);
#endif
	return 0;
}

static int xenstore_interaction(struct netfront_dev *dev) {
	char xs_key[XS_MSG_LEN], xs_value[XS_MSG_LEN];
	int err;

	// Set backend node and mac
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/backend", dev->nodename);
	xenstore_read(xs_key, dev->backend, sizeof(dev->backend));

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/mac", dev->nodename);
	xenstore_read(xs_key, dev->default_mac, XS_MSG_LEN);

	if ((dev->backend == NULL) || (dev->default_mac == NULL)) {
		printk("[PANIC!] backend/mac failed\n");
		return -1;
	}

	// Transmit ring
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/tx-ring-ref", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->tx_ring_ref);
	printk("tx_ring_ref=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write tx ring-ref\n");
		return err;
	}

	// Recieve ring
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/rx-ring-ref", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->rx_ring_ref);
	printk("rx-ring-ref=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write rx ring-ref\n");
		return err;
	}

	// Event channel
#ifdef FEATURE_SPLIT_CHANNELS
	alloc_evtchn(&dev->evtchn_tx);
	alloc_evtchn(&dev->evtchn_rx);

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel-tx", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn_tx);
	printk("event-channel-tx=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return err;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel-rx", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn_rx);
	printk("event-channel-rx=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return err;
	}
#else

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn);
	printk("event-channel=%s\n", xs_value);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return err;
	}
#endif

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/request-rx-copy", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", 1);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write request-rx-copy\n");
		return err;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/feature-rx-copy", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", 1);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write request-rx-copy\n");
		return err;
	}

	return 0;
}

int change_state_connected(struct netfront_dev *dev) {
	int ret;
	char xs_key[XS_MSG_LEN], xs_value[XS_MSG_LEN];

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/state", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", XenbusStateConnected);
	ret = xenstore_write(xs_key, xs_value);
	if (ret) {
		printk("%d [PANIC!] can not switch state\n", ret);
		return ret;
	}

	//wait for backend
	XenbusState state = XenbusStateUnknown;

	int count = 0;
	while (count < 10000 && state != XenbusStateConnected) {
		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/state", dev->backend);
		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		printk(">>>State is:%s\n", xs_value);
		state = atoi(xs_value);
		//sleep(5);
		++count;
	}

	if (state != XenbusStateConnected) {
		printk("[PANIC!] backend not avalable, state=%d\n", state);
		return -state;
	}
	return 0;
}

static inline int notify_remote_via_evtchn(evtchn_port_t port) {
	evtchn_send_t op;
	op.port = port;
	return HYPERVISOR_event_channel_op(EVTCHNOP_send, &op);
}

static inline int xennet_rxidx(RING_IDX idx) {
	return idx & (NET_RX_RING_SIZE - 1);
}

void init_rx_buffers(struct netfront_dev *dev) {
	int i, requeue_idx;
	netif_rx_request_t *req;
	int notify;

	for (requeue_idx = 0, i = 0; i < 256; i++) {
		struct net_buffer *buf = &dev->rx_buffers[requeue_idx];

		req = RING_GET_REQUEST(&dev->rx, requeue_idx);

		req->gref = gnttab_regrant_access(buf->gref, 0);
		req->id = xennet_rxidx(requeue_idx);

		requeue_idx++;
	}

	dev->rx.req_prod_pvt = requeue_idx;

	wmb();

	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);

	if (notify) {
#ifdef FEATURE_SPLIT_CHANNELS
		notify_remote_via_evtchn(dev->evtchn_rx);
#else
		notify_remote_via_evtchn(dev->evtchn);
#endif
	}

	dev->rx.sring->rsp_event = dev->rx.rsp_cons + 1;
}

int netfront_priv_init(struct netfront_dev *dev) {
	char nodename[256];
	int ret;

	snprintf(nodename, sizeof(nodename), "device/vif/%d", NET_FRONTEND_NUMBER);
	dev->nodename = strdup(nodename);

	ret = setup_netfront(dev);
	if (ret) {
		printk("%d Setup netfront failed\n", ret);
		return ret;
	}

	ret = xenstore_interaction(dev);
	if (ret) {
		printk("%d XenStore interaction failed\n", ret);
		return ret;
	}

	ret = change_state_connected(dev);
	if (ret) {
		printk("%d Change state failed\n", ret);
		return ret;
	}
#ifdef FEATURE_SPLIT_CHANNELS
	notify_remote_via_evtchn(dev->evtchn_tx);
#else
	notify_remote_via_evtchn(dev->evtchn);
#endif
	init_rx_buffers(dev);

	return 0;
}

void network_rx(struct netfront_dev *dev, struct net_device *embox_dev) {
	RING_IDX rp, cons, req_prod;
	int nr_consumed, i;
	int more, notify;
	int dobreak;

	nr_consumed = 0;
moretodo:
	rp = dev->rx.sring->rsp_prod;
	rmb(); /* Ensure we see queued responses up to 'rp'. */

	dobreak = 0;
	for (cons = dev->rx.rsp_cons; cons != rp && !dobreak;
	     nr_consumed++, cons++) {
		struct net_buffer *buf;
		unsigned char *page;
		int id;

		struct netif_rx_response *rx = RING_GET_RESPONSE(&dev->rx, cons);

		id = rx->id;
		BUG_ON(id >= NET_RX_RING_SIZE);

		buf = &dev->rx_buffers[id];
		page = (unsigned char *)buf->page;

		gnttab_end_access(buf->gref);

		if (rx->status > NETIF_RSP_NULL) {
			struct sk_buff *skb;
			if (!(skb = skb_alloc(rx->status))) {
				return;
			}
			memcpy(skb->mac.raw, page + rx->offset, skb->len);
			skb->dev = embox_dev;
			netif_rx(skb);
		}
	}

	dev->rx.rsp_cons = cons;

	RING_FINAL_CHECK_FOR_RESPONSES(&dev->rx, more);
	if (more && !dobreak)
		goto moretodo;

	req_prod = dev->rx.req_prod_pvt;

	for (i = 0; i < nr_consumed; i++) {
		int id = xennet_rxidx(req_prod + i);
		netif_rx_request_t *req = RING_GET_REQUEST(&dev->rx, req_prod + i);

		struct net_buffer *buf = &dev->rx_buffers[id];
		req->gref = gnttab_regrant_access(buf->gref, 0);
		req->id = id;
	}

	wmb();

	dev->rx.req_prod_pvt = req_prod + i;
	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);
	if (notify) {
#ifdef FEATURE_SPLIT_CHANNELS
		notify_remote_via_evtchn(dev->evtchn_rx);
#else
		notify_remote_via_evtchn(dev->evtchn);
#endif
	}
}

static inline int xennet_txidx(RING_IDX idx) {
	return idx & (NET_TX_RING_SIZE - 1);
}

void network_tx_buf_gc(struct netfront_dev *dev) {
	RING_IDX cons, prod;
	unsigned short id;

	do {
		prod = dev->tx.sring->rsp_prod;
		rmb(); /* Ensure we see responses up to 'rp'. */

		for (cons = dev->tx.rsp_cons; cons != prod; cons++) {
			struct netif_tx_response *txrsp;
			struct net_buffer *buf;

			txrsp = RING_GET_RESPONSE(&dev->tx, cons);
			if (txrsp->status == NETIF_RSP_NULL)
				continue;

			if (txrsp->status == NETIF_RSP_ERROR)
				printk("packet error\n");

			id = txrsp->id;
			BUG_ON(id >= NET_TX_RING_SIZE);
			buf = &dev->tx_buffers[id];
			gnttab_end_access(buf->gref);
		}

		dev->tx.rsp_cons = prod;

		dev->tx.sring->rsp_event = prod
		                           + ((dev->tx.sring->req_prod - prod) >> 1)
		                           + 1;
		mb();
	} while ((cons == prod) && (prod != dev->tx.sring->rsp_prod));
}

void netfront_xmit(struct netfront_dev *dev, unsigned char *data, int len) {
	struct netif_tx_request *tx;
	RING_IDX i, id;
	int notify;
	struct net_buffer *buf;

	BUG_ON(len > PAGE_SIZE());

	sched_lock();
	id = xennet_txidx(dev->tx.sring->req_prod);
	if (xennet_txidx(id + 1) == dev->tx.sring->rsp_prod) {
		printk("CATCH TAIL!!!\n\n");
		return;
	}

	sched_unlock();

	buf = &dev->tx_buffers[id];
	i = dev->tx.req_prod_pvt;

	tx = RING_GET_REQUEST(&dev->tx, i);

	memcpy(buf->page, data, len);

	tx->gref = gnttab_regrant_access(buf->gref, 0);

	tx->offset = 0;
	tx->size = len;
	tx->flags = 0;
	tx->id = id;
	dev->tx.req_prod_pvt = i + 1;

	wmb();

	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->tx, notify);
#ifdef FEATURE_SPLIT_CHANNELS
	if (notify)
		notify_remote_via_evtchn(dev->evtchn_tx);
#else
	if (notify)
		notify_remote_via_evtchn(dev->evtchn);
#endif

	sched_lock();
	network_tx_buf_gc(dev);
	sched_unlock();
}
