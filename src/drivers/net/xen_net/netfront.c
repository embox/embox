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
#include <xen/memory.h>
#include "netfront.h"
//add this for unmask
#include <xen/event.h>

#define XS_MSG_LEN 256

#define ASSERT(x)                           \
do {                                        \
	if (!(x)) {                          	\
		printk("ASSERTION FAILED!");       	\
	}                                       \
} while(0)

#define BUG_ON(x) ASSERT(!(x))

static grant_ref_t ref = 10; // first 8 entries are reserved 

grant_entry_v1_t *grant_table;

grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame, int readonly)
{
	//printk("grant access for ref:%d on addr:%p\n", ref, &grant_table[ref]);
    
    grant_table[ref].frame = frame;
    grant_table[ref].domid = domid;
    wmb();
    readonly *= GTF_readonly;
    grant_table[ref].flags = GTF_permit_access | readonly;

	//printk("frame setuped\n");
    return ref++;
}
#if 0 //сохраним для истории
static grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame,
		int readonly)
{
	//TODO update this after grant table implementation
/*
    gnttab_table[ref].frame = frame;
    gnttab_table[ref].domid = domid;
    wmb();
    readonly *= GTF_readonly;
    gnttab_table[ref].flags = GTF_permit_access | readonly;
*/
    return ref++;
}
#endif

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
	
	// set backend node and mac
	printk(">>>>>xenstore_interaction: Start transaction\n");
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

	// tx
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

	// rx
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

	//eventch
#ifdef FEATURE_SPLIT_CHANNELS //false
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
#else
	
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn);
	printk("event-channel=%s\n", xs_value);
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


		//read ip from xenstore? o_O?
#if 0 
		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/ip", dev->backend);
		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		strcpy(*ip, xs_value);
#endif

	printk(">>>>>xenstore_interaction: finish xenstore_interaction\n");
	//unmask_evtchn(dev->evtchn_rx);

	return;
}
static int rx_buffers_mfn[NET_RX_RING_SIZE];

static inline int notify_remote_via_evtchn(evtchn_port_t port)
{
    evtchn_send_t op;
    op.port = port;
    return HYPERVISOR_event_channel_op(EVTCHNOP_send, &op);
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
//TODO
        //gnttab_end_access(buf->gref);

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
#if 0 //TODO gnttab_grant_access
        struct net_buffer* buf = &dev->rx_buffers[id];
        //void* page = buf->page;
//TODO virt->mfn

        /* We are sure to have free gnttab entries since they got released above */
        buf->gref = req->gref = gnttab_grant_access(dev->dom,
                                                    rx_buffers_mfn[id],
                                                    0);
#endif												
	    req->id = id;
    }

    wmb();

    dev->rx.req_prod_pvt = req_prod + i;
    RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);
	if (notify) 
	{
#ifdef FEATURE_SPLIT_CHANNELS //false
	notify_remote_via_evtchn(dev->evtchn_rx);
#else
	notify_remote_via_evtchn(dev->evtchn);
#endif
	}
}
#endif


void init_rx_buffers(struct netfront_dev *dev)
{
	printk(">>>>>init_rx_buffers\n");
	//printk(">>>>>NET_RX_RING_SIZE=%li\n", NET_RX_RING_SIZE);
	//printk(">>>>>NET_RX_RING_SIZE_2=%d\n", RING_SIZE(&dev->rx));
	
    int i, requeue_idx;
    netif_rx_request_t *req;
    int notify;

    /* Rebuild the RX buffer freelist and the RX ring itself. */
    for (requeue_idx = 0, i = 0; i < 256; i++) 
    {
        struct net_buffer* buf = &dev->rx_buffers[requeue_idx];
		//printk("dev->rx.req_cons=%u, RING_SIZE(_r)=%d, finally=%d\n",dev->rx.req_cons, RING_SIZE(&dev->rx), ((dev->rx.req_cons) & (RING_SIZE(&dev->rx) - 1)));

		req = RING_GET_REQUEST(&dev->rx, requeue_idx);

		printk("addr for %d: %p\n", requeue_idx, req);
        buf->gref = req->gref = 
            gnttab_grant_access(dev->dom,rx_buffers_mfn[requeue_idx],0);

        req->id = xennet_rxidx(requeue_idx);
		printk("request: id=%d, gref=%u\n", req->id, req->gref);

        requeue_idx++;
    }

    dev->rx.req_prod_pvt = requeue_idx; //256

	wmb();

    RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);
/*	
	printk(">>>\n");
	req = RING_GET_REQUEST(&dev->rx, 0);
	printk("request: id=%d, gref=%u\n", req->id, req->gref);
	req = RING_GET_REQUEST(&dev->rx, 1);
	printk("request: id=%d, gref=%u\n", req->id, req->gref);
	req = RING_GET_REQUEST(&dev->rx, 2);
	printk("request: id=%d, gref=%u\n", req->id, req->gref);
	req = RING_GET_REQUEST(&dev->rx, 3);
	printk("request: id=%d, gref=%u\n", req->id, req->gref);
*/
	printk(">>>>>notify=%i\n", notify);
    
	if (notify) 
	{
#ifdef FEATURE_SPLIT_CHANNELS //false
	notify_remote_via_evtchn(dev->evtchn_rx);
#else
	notify_remote_via_evtchn(dev->evtchn);
#endif
	}

    dev->rx.sring->rsp_event = dev->rx.rsp_cons + 1;
}


extern char memory_pages[600][PAGE_SIZE()];
static int alloc_page_counter = 0;

int alloc_page(unsigned long *va, unsigned long *mfn)
{
	printk("alloc page #%d\n", alloc_page_counter);
	if(alloc_page_counter == 600)
	{
		printk("KRITICAL ERROR!!!: ENOMEM\n");
		return -ENOMEM;
	}
//ask for mem (for rings txs rxs)
    struct xen_memory_reservation reservation;
    reservation.nr_extents = 1;
    reservation.extent_order = 3;
    reservation.address_bits= 0;
    reservation.domid = DOMID_SELF;
    unsigned long frame_list[1];
    set_xen_guest_handle(reservation.extent_start, frame_list);
   	int rc;
    rc = HYPERVISOR_memory_op(XENMEM_increase_reservation, &reservation);
    printk("XENMEM_increase_reservation=%d\n", rc);
    printk("frame_list=%lu\n", frame_list[0]);
	
//map it
	printk("addr:%p\n", &memory_pages[alloc_page_counter]);
	rc = HYPERVISOR_update_va_mapping((unsigned long) &memory_pages[alloc_page_counter],
			__pte((frame_list[0]<< PAGE_SHIFT) | 7),
			UVMF_INVLPG);
	printk("HYPERVISOR_update_va_mapping:%d\n", rc);

	*mfn = frame_list[0];
	*va = (unsigned long)&memory_pages[alloc_page_counter];
	alloc_page_counter++;
//done!
	return 0;
}
void setup_netfront(struct netfront_dev *dev, 
					void (*thenetif_rx)(unsigned char* data, int len, void* arg))
{
	struct netif_tx_sring *txs;
	struct netif_rx_sring *rxs;
	unsigned long txs_va = (unsigned long)xen_mem_alloc(1);
	unsigned long txs_mfn = virt_to_mfn(txs_va);
	//printk("alloc_page rc=%d\n", alloc_page(&txs_va, &txs_mfn));
	unsigned long rxs_va = (unsigned long)xen_mem_alloc(1);
	unsigned long rxs_mfn = virt_to_mfn(rxs_va);
	//printk("alloc_page rc=%d\n", alloc_page(&rxs_va, &rxs_mfn));
	
	//alloc pages for buffer rings
	int i;
	for(i=0;i<NET_TX_RING_SIZE;i++)
    {
	    //add_id_to_freelist(i,dev->tx_freelist);
        dev->tx_buffers[i].page = NULL;
    }

    for(i=0;i<NET_RX_RING_SIZE;i++)
    {
	/* TODO: that's a lot of memory */
		#ifdef XEN_MEM_RESERVATION //false
		unsigned long va;
		unsigned long mfn;
		printk("alloc_page rc=%d\n", alloc_page(&va, &mfn));
		rx_buffers_mfn[i] = mfn;
        dev->rx_buffers[i].page = (char*)va;
		#endif
		char *va = xen_mem_alloc(1);
		dev->rx_buffers[i].page = va;
		rx_buffers_mfn[i] = virt_to_mfn(va);
    }

	txs = (struct netif_tx_sring *) txs_va;
	rxs = (struct netif_rx_sring *) rxs_va;

	memset(txs, 0, PAGE_SIZE());
	memset(rxs, 0, PAGE_SIZE());

	SHARED_RING_INIT(txs);
	SHARED_RING_INIT(rxs);

	FRONT_RING_INIT(&dev->tx, txs, PAGE_SIZE());
	FRONT_RING_INIT(&dev->rx, rxs, PAGE_SIZE());
	
	dev->tx_ring_ref = gnttab_grant_access(dev->dom, txs_mfn, 0);
	dev->rx_ring_ref = gnttab_grant_access(dev->dom, rxs_mfn, 0);

	printk(">>>>>>>>>>after grant\n");

    dev->netif_rx = thenetif_rx;

    //NEED IT??
	//dev->events = NULL;
	
	//experiment
	alloc_evtchn(&dev->evtchn);
}

//why it is here?
struct xen_netif_rx_request {
	uint16_t id;		/* Echoed in response message.        */
	uint16_t pad;
	grant_ref_t gref;
};

void print_info() 
{
	long ret;
    domid_t domid = DOMID_SELF;
	ret = HYPERVISOR_memory_op(XENMEM_maximum_reservation, &domid);
	unsigned long nr_max_pages;
	nr_max_pages = ret;
    printk("XENMEM_maximum_reservation=%ld\n", nr_max_pages);

/*	int rc;
//useless: адреса за пределами видимости domU, а доступ можно получить просто так -- del this
	struct xen_machphys_mapping mpm;
    rc = HYPERVISOR_memory_op(XENMEM_machphys_mapping, &mpm);
	printk("XENMEM_machphys_mapping=%p, %d\n", (void*)rc, rc);
	unsigned long *mp = (unsigned long *)mpm.v_start;
	printk("v_start = %ld,%ld,%ld,%ld\n", mp[0],mp[1],mp[2],mp[3]);
// ничего не происходит -- del this
	struct xen_machphys_mfn_list mpl;
	unsigned long frames[100];
	set_xen_guest_handle(mpl.extent_start, frames);
	mpl.nr_extents = 100;
    rc = HYPERVISOR_memory_op(XENMEM_machphys_mfn_list, &mpl);

	printk("XENMEM_machphys_mfn_list=%p, %d\n", (void*)rc, rc);
*/
/* also returns only 32 frames -- del this
    struct xen_memory_reservation reservation = {
        .domid        = DOMID_SELF
    };
	static unsigned long balloon_frames[256];
    set_xen_guest_handle(reservation.extent_start, balloon_frames);
	reservation.extent_order = 3;
    reservation.nr_extents = 256;
    int rc;
	rc = HYPERVISOR_memory_op(XENMEM_populate_physmap, &reservation);
	printk("XENMEM_populate_physmap=%d\n", rc);
*/
}

struct netfront_dev *init_netfront(
	char *_nodename,
	void (*thenetif_rx)(unsigned char* data,
		int len, void* arg),
	unsigned char rawmac[6],
	char **ip
) {
	printk(">>>>>init_netfront\n");
	print_info();
	char nodename[256];
	static int netfrontends = 0;

	snprintf(nodename, sizeof(nodename), "device/vif/%d", netfrontends);

	struct netfront_dev *dev = NULL;
	dev = malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));
	dev->nodename = strdup(nodename);
	printk(">>>>>>>>>>dev->dom=%d\n",dev->dom);

	setup_netfront(dev, thenetif_rx);

	xenstore_interaction(dev, ip);

	
#if 1
	//init_rx_buffers(dev);

	char xs_key[XS_MSG_LEN], xs_value[XS_MSG_LEN];
	int err;
	// state 	
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/state", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", XenbusStateConnected);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] can not switch state\n");
		//return;
	}
	
	//wait for backend
	XenbusState state = XenbusStateUnknown;
	int count = 0;

//are we reading our state(not backend)?
	while (count < 10 && state != XenbusStateConnected) {
		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/state", dev->backend);
		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		printk(">>>State is:%s\n",xs_value);
		state = atoi(xs_value);
		sleep(5);
		++count;
	}
	printk(">>>>>Backend state is:%i\n>>>>>Tries:%i\n", state, count);
	
	if (state != XenbusStateConnected) {
		printk("[PANIC!] backend not avalable, state=%d\n", state);
		// xenbus_unwatch_path_token(XBT_NIL, path, path);
		//return;
	}

	printk(">>>>>xenstore_interaction: End transaction\n");
	notify_remote_via_evtchn(dev->evtchn);
	init_rx_buffers(dev);
	printk("backend %p %d %d\n", dev->backend, sizeof(dev->backend),
			sizeof(dev->nodename));
#else
	xennet_alloc_rx_buffers(dev);
#endif

#if 0 //DEBUG
	for (i = 0; i < NET_RX_RING_SIZE; i++) 
    {
		printk("addr for %d: %p\n", i, RING_GET_REQUEST(&dev->rx, i));
	}
#endif

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

	while(1) {
		network_rx(dev);
	}

	return dev;
}
