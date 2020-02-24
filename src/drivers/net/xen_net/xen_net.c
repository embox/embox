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

///////////////////////////////////////////////////////
// Here we are going to realize grant table mechanism//
///////////////////////////////////////////////////////

/*
 * Reserve an area of virtual address space for mappings and Heap
 */
//static unsigned long demand_map_area_start;
//static unsigned long demand_map_area_end;
//Need it?

#include <xen_hypercall-x86_32.h>
#if 0
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


//TODO!!!!!!!
void arch_init_demand_mapping_area(void)
{
    demand_map_area_start = VIRT_DEMAND_AREA;
    demand_map_area_end = demand_map_area_start + DEMAND_MAP_PAGES * PAGE_SIZE;
    printk("Demand map pfns at %lx-%lx.\n", demand_map_area_start,
           demand_map_area_end);

#ifdef HAVE_LIBC
    heap_mapped = brk = heap = VIRT_HEAP_AREA;
    heap_end = heap_mapped + HEAP_PAGES * PAGE_SIZE;
    printk("Heap resides at %lx-%lx.\n", brk, heap_end);
#endif
}

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



/* map f[i*stride]+i*increment for i in 0..n-1, aligned on alignment pages */
/*
 * Map an array of MFNs contiguous into virtual address space. Virtual
 * addresses are allocated from the on demand area.
 */
void *map_frames_ex(const unsigned long *mfns, 
                    unsigned long n, //4
                    unsigned long stride, //1
                    unsigned long incr, //0
                    unsigned long alignment, //1
                    domid_t id, int *err, //DOMID_SELF 32752, NULL
                    unsigned long prot) //L1_PROT 35
{
    unsigned long va = allocate_ondemand(n, alignment);

    if ( !va )
        return NULL;

    if ( do_map_frames(va, mfns, n, stride, incr, id, err, prot) )
        return NULL;

    return (void *)va;
}


#define map_frames(f, n) map_frames_ex(f, n, 1, 0, 1, DOMID_SELF, NULL, L1_PROT)

void init_gnttab(void)
{

/*TODO detection
    int i;
    for (i = NR_RESERVED_ENTRIES; i < NR_GRANT_ENTRIES; i++)
        put_free_entry(i);
*/	
	struct gnttab_setup_table setup;
    unsigned long frames[NR_GRANT_FRAMES];

    setup.dom = DOMID_SELF;
    setup.nr_frames = NR_GRANT_FRAMES;
    set_xen_guest_handle(setup.frame_list, frames);

    HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup, 1);
    
	gnttab_table = map_frames(frames, NR_GRANT_FRAMES);
	
    printk("gnttab_table mapped at %p.\n", gnttab_table);
}
#endif
extern char _text;

extern start_info_t my_start_info;
unsigned long *phys_to_machine_mapping;
/*
 * get the PTE for virtual address va if it exists. Otherwise NULL.
 */
 /*
 * Lookup the page table entry for a virtual address in a specific pgd.
 * Return a pointer to the entry and the level of the mapping.
 */
 //arch/x86/mm/pageattr.c:353
static pgentry_t *get_pgt(unsigned long va)
{
    printk(">>>>>start get_pgt\n");

    unsigned long mfn;
    pgentry_t *tab;
    unsigned offset;
    pgentry_t *pt_base;
    
    
    pt_base = (pgentry_t *)my_start_info.pt_base;
    phys_to_machine_mapping = (unsigned long *)my_start_info.mfn_list;
    printk("pt_base=%llu\n", *pt_base);


    tab = pt_base;
    mfn = virt_to_mfn(pt_base);
printk("DEBUGG\n");

    offset = l3_table_offset(va);
    printk("DEBUGG2\n");
 
    
    printk("DEBUGG\n");
    if ( !(tab[offset] & _PAGE_PRESENT) )
    {
        printk("DEBUGG_null\n");
        return NULL;
    }
        
    printk("DEBUGG3\n");
    mfn = pte_to_mfn(tab[offset]);
    printk("DEBUGG4\n");
    tab = mfn_to_virt(mfn);
    printk("DEBUGG5\n");
    offset = l2_table_offset(va);
    printk("DEBUGG6-1\n");
    printk("offset=%lln\n", tab);
    printk("offset=%llu\n", (tab[offset]& _PAGE_PRESENT));
    printk("DEBUGG6,2\n");
    if ( !(tab[offset] & _PAGE_PRESENT) )
    {
        printk("DEBUGG_null\n");
        
        return NULL;
    }
    
    printk("DEBUGG7\n");
    if ( tab[offset] & _PAGE_PSE )
    {
        printk("DEBUGG_pse\n");
        return &tab[offset];
    }
    printk("DEBUGG8\n");
    mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(mfn);
    printk("DEBUGG9\n");
    offset = l1_table_offset(va);
    
    printk("end of get_pgt \n");
    printk("offset=%u\n", offset);
    printk("offset=%p\n", &tab);
    printk("offset=%p\n", &tab[offset]);
    pgentry_t *asd = &tab[offset];
    printk("end of get_pgt \n");
    return asd;
}

/*
 * return a valid PTE for a given virtual address. If PTE does not exist,
 * allocate page-table pages.
 */
pgentry_t *need_pgt(unsigned long va)
{
    printk("start_need_pgt\n");
    unsigned long pt_mfn;
    pgentry_t *tab;
    //unsigned long pt_pfn;
    unsigned offset;

    //вынести это отдельно
    pgentry_t *pt_base;
    pt_base = (pgentry_t *)my_start_info.pt_base;
    
    tab = pt_base;
    pt_mfn = virt_to_mfn(pt_base);

    offset = l3_table_offset(va);
    if ( !(tab[offset] & _PAGE_PRESENT) ) 
    {
        printk("!(tab[offset] & _PAGE_PRESENT) true \n");
        /*pt_pfn = virt_to_pfn(alloc_page());
        if ( !pt_pfn )
            return NULL;
        new_pt_frame(&pt_pfn, pt_mfn, offset, L2_FRAME);
        */
    }
    if(!(tab[offset] & _PAGE_PRESENT))
    {
        printk("That's not good\n");
    }
    pt_mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(pt_mfn);
    offset = l2_table_offset(va);
    if ( !(tab[offset] & _PAGE_PRESENT) )
    {
        printk("2!(tab[offset] & _PAGE_PRESENT) true \n");
        /*
        pt_pfn = virt_to_pfn(alloc_page());
        if ( !pt_pfn )
            return NULL;
        new_pt_frame(&pt_pfn, pt_mfn, offset, L1_FRAME);
        */
    }
    if(tab[offset] & _PAGE_PRESENT)
    {
        printk("That's not good\n");
    }

    if ( tab[offset] & _PAGE_PSE )
        return &tab[offset];

    pt_mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(pt_mfn);

    offset = l1_table_offset(va);
    printk("end_need_pgt\n");
    return &tab[offset];
}

unsigned long allocate_ondemand(unsigned long n)
{
    printk(">>>>>start allocate_ondemand\n");
    unsigned long x;
    unsigned long y = 0;
    unsigned long alignment = 1;
#define DEMAND_MAP_PAGES        CONST(0x10000000) //somewhere out of embox memory

    unsigned long demand_map_area_start = DEMAND_MAP_PAGES;

    /* Find a properly aligned run of n contiguous frames */
    for ( x = 0;
          x <= DEMAND_MAP_PAGES - n; 
          x = (x + y + 1 + alignment - 1) & ~(alignment - 1) ) //it can be easy
    {
        printk("x=%lu\n", x);
    
        unsigned long addr = demand_map_area_start + x * PAGE_SIZE;
        printk("addr=%lu\n", addr);

        pgentry_t *pgt = get_pgt(addr);
        
        
        for ( y = 0; y < n; y++, addr += PAGE_SIZE ) 
        {
            printk("y=%lu\n", y);
            if ( !(addr & L1_MASK) )
            {
                pgt = get_pgt(addr);
                printk("debug1\n");
            }
            printk("debug2\n");
            if ( pgt )
            {
                printk("debug3");
                //printk("debug3; *pgt=%llu\n",*pgt);
                if ( *pgt & _PAGE_PRESENT )
                {
                    printk("debug4\n");
                    break;
                }
                printk("debug5\n");
                pgt++;
            }
            printk("debug6\n");
        }
        if ( y == n )
        {
            printk("debug7\n");
            break;
        }
           printk("debug8\n");
    }
    if ( y != n )
    {
        printk("Failed to find %ld frames!\n", n);
        return 0;
    }
    printk("debug9\n");
    printk("x=%ld\n",x);
    
    return demand_map_area_start + x * PAGE_SIZE;
}

void init_grant_table(int n)
{
    printk(">>>>>init_grant_table\n");

/*TODO detection
    int i;
    for (i = NR_RESERVED_ENTRIES; i < NR_GRANT_ENTRIES; i++)
        put_free_entry(i);
*/	
	struct gnttab_setup_table setup;
    unsigned long frames[n];

    setup.dom = DOMID_SELF;     //32752
    setup.nr_frames = n;
    set_xen_guest_handle(setup.frame_list, frames);

    int rc;
    rc = HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup, 1);
    printk("HYPERVISOR_grant_table_op returned:%d, status=%d\n", rc, setup.status);
    int count;

#if 0
    printk("grant table virtual:%p\n", gnttab_table);
    unsigned long va = allocate_ondemand(1); 
    
    pgentry_t *pgt = NULL;
    if ( !pgt || !(va & L1_MASK) )
    {
        printk("deal with it\n");

        pgt = need_pgt(va);
    }
                    
    printk(">>>>>%lu\n", va);

    //gnttab_table = (pgentry_t)((frames[0] << PAGE_SHIFT) | L1_PROT);
    //printk("grant table machine:%ld\n", virt_to_mfn(gnttab_table));
    
    ////printk("grant table flags:%d\n", gnttab_table[0].flags);
#elif 1 //HYPERVISOR_update_va_mapping

    for(count = 0; count < n; count++)
    {
        printk("entry %d mapped at %ld(mfn).\n",count, frames[count]);

        rc = HYPERVISOR_update_va_mapping((unsigned long) grant_table[count],
                __pte((frames[count]<< PAGE_SHIFT) | 7),
                UVMF_INVLPG);
        printk("HYPERVISOR_update_va_mapping:%d\n", rc);
    }
#else
    
    mmu_update_t mmu_updates[1];
    mmu_updates[0].ptr = virt_to_mach((pgentry_t)(void*)&grant_table) | MMU_NORMAL_PT_UPDATE;
    mmu_updates[0].val = ((pgentry_t)(frames[0]) << PAGE_SHIFT) | L1_PROT;
                            
    rc = HYPERVISOR_mmu_update(mmu_updates, 1, NULL, DOMID_SELF);
    printk("rc=%d\n",rc);
#endif
    printk(">>>>>END OF init_grant_table\n");
}

static int xen_net_init(void) {
	printk("\n");
	printk(">>>>>xen_net_init\n");
	init_grant_table(NR_GRANT_FRAMES);
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
#if 1 //try split channel
	nic->irq = dev->evtchn_rx;
	res = irq_attach(dev->evtchn_rx, xen_net_irq, IF_SHARESUP, nic,
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
