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

static char _text;

static char memory_pagess[16][PAGE_SIZE];

static grant_ref_t ref = 0; // it's ok

grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame, int readonly)
{
	printk("gnttab_grant_access start for ref:%d\n", ref);
    
    grant_table[ref]->frame = frame;
	printk("frame setuped\n");
    grant_table[ref]->domid = domid;
    wmb();
    readonly *= GTF_readonly;
    grant_table[ref]->flags = GTF_permit_access | readonly;

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

#if 0 //wrong experiment with unmask_evtchn
//this is from os.h
#define ADDR (*(volatile long *) addr)
static  void synch_clear_bit(int nr, volatile void * addr)
{
    __asm__ __volatile__ (
        "lock btrl %1,%0"
        : "=m" (ADDR) : "Ir" (nr) : "memory" );
}

static  int synch_var_test_bit(int nr, volatile void * addr)
{
    int oldbit;
    __asm__ __volatile__ (
        "btl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit) : "m" (ADDR), "Ir" (nr) );
    return oldbit;
}

static  int synch_const_test_bit(int nr, const volatile void * addr)
{
    return ((1UL << (nr & 31)) & 
            (((const volatile unsigned int *) addr)[nr >> 5])) != 0;
}

#define synch_test_bit(nr,addr) \
(__builtin_constant_p(nr) ? \
 synch_const_test_bit((nr),(addr)) : \
 synch_var_test_bit((nr),(addr)))

static  int synch_test_and_set_bit(int nr, volatile void * addr)
{
    int oldbit;
    __asm__ __volatile__ (
        "lock btsl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" (ADDR) : "Ir" (nr) : "memory");
    return oldbit;
}

/* This is a barrier for the compiler only, NOT the processor! */
#define barrier() __asm__ __volatile__("": : :"memory")

void force_evtchn_callback(void)
{
	extern shared_info_t xen_shared_info;
	shared_info_t *HYPERVISOR_shared_info = &xen_shared_info;
#ifdef XEN_HAVE_PV_UPCALL_MASK
    int save;
#endif
    vcpu_info_t *vcpu;
    vcpu = &HYPERVISOR_shared_info->vcpu_info[0];
#ifdef XEN_HAVE_PV_UPCALL_MASK
    save = vcpu->evtchn_upcall_mask;
#endif

    while (vcpu->evtchn_upcall_pending) {
#ifdef XEN_HAVE_PV_UPCALL_MASK
        vcpu->evtchn_upcall_mask = 1;
#endif
        barrier();
        do_hypervisor_callback(NULL);
        barrier();
#ifdef XEN_HAVE_PV_UPCALL_MASK
        vcpu->evtchn_upcall_mask = save;
        barrier();
#endif
    };
}

void unmask_evtchn(uint32_t port)
{
	extern shared_info_t xen_shared_info;
    shared_info_t *s = &xen_shared_info;
    vcpu_info_t *vcpu_info = &s->vcpu_info[0];

    synch_clear_bit(port, &s->evtchn_mask[0]);

    /*
     * The following is basically the equivalent of 'hw_resend_irq'. Just like
     * a real IO-APIC we 'lose the interrupt edge' if the channel is masked.
     */
    if (  synch_test_bit        (port,    &s->evtchn_pending[0]) && 
         !synch_test_and_set_bit(port / (sizeof(unsigned long) * 8),
              &vcpu_info->evtchn_pending_sel) )
    {
        vcpu_info->evtchn_upcall_pending = 1;
#ifdef XEN_HAVE_PV_UPCALL_MASK
        if ( !vcpu_info->evtchn_upcall_mask )
#endif
            force_evtchn_callback();
    }
}
#endif

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
#if 0
	alloc_evtchn(&dev->evtchn);
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

	// state 	
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/state", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", XenbusStateConnected);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] can not switch state\n");
		return;
	}
	
	//wait for backend
	XenbusState state = XenbusStateUnknown;
	int count = 0;
	
	while (count < 10 && state != XenbusStateConnected) {
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
		return;
	}

	printk(">>>>>xenstore_interaction: End transaction\n");

	printk("backend %p %d %d\n", dev->backend, sizeof(dev->backend),
			sizeof(dev->nodename));

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
	printk(">>>>>init_rx_buffers\n");
	printk(">>>>>NET_RX_RING_SIZE=%lli\n", NET_RX_RING_SIZE);
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
	printk(">>>>>requeue_idx=%i\n", requeue_idx);
    dev->rx.req_prod_pvt = requeue_idx;

    RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&dev->rx, notify);
	
	printk(">>>>>notify=%i\n", notify);
    
	if (notify) 
		notify_remote_via_evtchn(dev->evtchn_rx);
		//notify_remote_via_evtchn(dev->evtchn);

    dev->rx.sring->rsp_event = dev->rx.rsp_cons + 1;
}

#if 0
typedef unsigned long pgentry_t;
#define L1_PAGETABLE_SHIFT      12
#define L2_PAGETABLE_SHIFT      21
#define L3_PAGETABLE_SHIFT      30
#define L4_PAGETABLE_SHIFT      39

#define L1_PAGETABLE_ENTRIES    512
#define L2_PAGETABLE_ENTRIES    512
#define L3_PAGETABLE_ENTRIES    512
#define L4_PAGETABLE_ENTRIES    512
/* Given a virtual address, get an entry offset into a page table. */
#define l1_table_offset(_a) \
  (((_a) >> L1_PAGETABLE_SHIFT) & (L1_PAGETABLE_ENTRIES - 1))
#define l2_table_offset(_a) \
  (((_a) >> L2_PAGETABLE_SHIFT) & (L2_PAGETABLE_ENTRIES - 1))
#define l3_table_offset(_a) \
  (((_a) >> L3_PAGETABLE_SHIFT) & (L3_PAGETABLE_ENTRIES - 1))
#define _PAGE_PRESENT  CONST(0x001)
#define pte_to_mfn(_pte)           (((_pte) & (PADDR_MASK&PAGE_MASK)) >> L1_PAGETABLE_SHIFT)

pgentry_t *pt_base;
/*
 * get the PTE for virtual address va if it exists. Otherwise NULL.
 */
static pgentry_t *get_pgt(unsigned long va)
{
    unsigned long mfn;
    pgentry_t *tab;
    unsigned offset;

    tab = pt_base;
    mfn = virt_to_mfn(pt_base);

    offset = l3_table_offset(va);
    if ( !(tab[offset] & _PAGE_PRESENT) )
        return NULL;
    mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(mfn);
    offset = l2_table_offset(va);
    if ( !(tab[offset] & _PAGE_PRESENT) )
        return NULL;
    if ( tab[offset] & _PAGE_PSE )
        return &tab[offset];
    mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(mfn);
    offset = l1_table_offset(va);
    return &tab[offset];
}

#define DEMAND_MAP_PAGES        CONST(0x8000000)
static unsigned long demand_map_area_start;
unsigned long allocate_ondemand(unsigned long n, unsigned long alignment)
{
    unsigned long x;
    unsigned long y = 0;

    /* Find a properly aligned run of n contiguous frames */
    for ( x = 0;
          x <= DEMAND_MAP_PAGES - n; 
          x = (x + y + 1 + alignment - 1) & ~(alignment - 1) )
    {
        unsigned long addr = demand_map_area_start + x * PAGE_SIZE;
        pgentry_t *pgt = get_pgt(addr);
        for ( y = 0; y < n; y++, addr += PAGE_SIZE ) 
        {
            if ( !(addr & L1_MASK) )
                pgt = get_pgt(addr);
            if ( pgt )
            {
                if ( *pgt & _PAGE_PRESENT )
                    break;
                pgt++;
            }
        }
        if ( y == n )
            break;
    }
    if ( y != n )
    {
        printk("Failed to find %ld frames!\n", n);
        return 0;
    }
    return demand_map_area_start + x * PAGE_SIZE;
}
/*
 * Map an array of MFNs contiguous into virtual address space. Virtual
 * addresses are allocated from the on demand area.
 */
void *map_frames_ex(const unsigned long *mfns, unsigned long n, 
                    unsigned long stride, unsigned long incr,
                    unsigned long alignment,
                    domid_t id, int *err, unsigned long prot)
{
    unsigned long va = allocate_ondemand(n, alignment);

    if ( !va )
        return NULL;

    if ( do_map_frames(va, mfns, n, stride, incr, id, err, prot) )
        return NULL;

    return (void *)va;
}
#define map_frames(f, n) map_frames_ex(f, n, 1, 0, 1, DOMID_SELF, NULL, L1_PROT)

grant_entry_v1_t *arch_init_gnttab(int nr_grant_frames)
{
    struct gnttab_setup_table setup;
    unsigned long frames[nr_grant_frames];

    setup.dom = DOMID_SELF;
    setup.nr_frames = nr_grant_frames;
    set_xen_guest_handle(setup.frame_list, frames);

    HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup, 1);
    return map_frames(frames, nr_grant_frames);
}
#endif
extern unsigned long my_debug_info;
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
//#define TSX_DEBUG
#ifdef TSX_DEBUG
	txs = (struct netif_tx_sring *) &my_debug_info;
	rxs = (struct netif_rx_sring *) memory_pagess[1];
#else
	txs = (struct netif_tx_sring *) memory_pagess[0];
	rxs = (struct netif_rx_sring *) &my_debug_info;
#endif
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
	
    printk(">>>>>>>>>>dev->dom=%d\n",dev->dom);
#ifdef TSX_DEBUG //it should work both!!!
	unsigned long a = virt_to_mfn(txs);
	printk(">debug\n");
	dev->tx_ring_ref = gnttab_grant_access(dev->dom,a, 0);
#else
	dev->rx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(rxs), 0);
#endif
	printk(">>>>>>>>>>after grant\n");

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
