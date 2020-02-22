/**
 * @file 
 * @brief Frontend initialization
 *
 * @date 
 * @author 
 */

#ifndef NETFRONT_H_
#define NETFRONT_H_

#include <stdint.h>

#include <xen/xen.h>
#include <xen/io/ring.h>
#include <xen/io/netif.h>
#include <xen/io/xenbus.h>
#include <xen/grant_table.h>
#include <xen/event_channel.h>

#define CONFIG_PARAVIRT
extern unsigned long *phys_to_machine_mapping;
#ifdef CONFIG_PARAVIRT

#define pfn_to_mfn(_pfn) (phys_to_machine_mapping[(_pfn)])
#define mfn_to_pfn(_mfn) (machine_to_phys_mapping[(_mfn)])

#else //ifNdef CONFIG_PARAVIRT
#define pfn_to_mfn(_pfn) ((unsigned long)(_pfn))
#define mfn_to_pfn(_mfn) ((unsigned long)(_mfn))


#endif

#define L1_PAGETABLE_SHIFT      12
#define L2_PAGETABLE_SHIFT      21
#define L3_PAGETABLE_SHIFT      30

#define L1_PAGETABLE_ENTRIES    512
#define L2_PAGETABLE_ENTRIES    512
#define L3_PAGETABLE_ENTRIES    4

/* Given a virtual address, get an entry offset into a page table. */
#define l1_table_offset(_a) \
  (((_a) >> L1_PAGETABLE_SHIFT) & (L1_PAGETABLE_ENTRIES - 1))
#define l2_table_offset(_a) \
  (((_a) >> L2_PAGETABLE_SHIFT) & (L2_PAGETABLE_ENTRIES - 1))
#define l3_table_offset(_a) \
  (((_a) >> L3_PAGETABLE_SHIFT) & (L3_PAGETABLE_ENTRIES - 1))

#define VIRT_START                 ((unsigned long)&_text)
#define to_phys(x)                 ((unsigned long)(x)-VIRT_START)
#define PFN_DOWN(x)	((x) >> L1_PAGETABLE_SHIFT)
#define virt_to_pfn(_virt)         (PFN_DOWN(to_phys(_virt)))

#define virt_to_mfn(_virt)         (pfn_to_mfn(virt_to_pfn(_virt)))

#define CONST(x) x ## ULL
#define _PAGE_PRESENT  CONST(0x001)
#define _PAGE_RW       CONST(0x002)
#define _PAGE_ACCESSED CONST(0x020)
#define _PAGE_PSE      CONST(0x080)
#define L1_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED)
#define L1_MASK  ((1UL << L2_PAGETABLE_SHIFT) - 1)
#define L1_FRAME                1
#define L2_FRAME                2
typedef uint64_t pgentry_t;

#define PAGE_SHIFT      12
#define PAGE_SIZE 				(1ULL << PAGE_SHIFT) // or 1??
#define PAGE_MASK       (~(PAGE_SIZE-1))
#define PADDR_BITS              44
#define PADDR_MASK              ((1ULL << PADDR_BITS)-1)
#define to_virt(x)                 ((void *)((unsigned long)(x)+VIRT_START))

#define pte_to_mfn(_pte)           (((_pte) & (PADDR_MASK&PAGE_MASK)) >> L1_PAGETABLE_SHIFT)
#define mfn_to_virt(_mfn)          (to_virt(mfn_to_pfn(_mfn) << PAGE_SHIFT))

typedef unsigned long long paddr_t;
typedef unsigned long long maddr_t;
static __inline__ maddr_t phys_to_machine(paddr_t phys)
{
	maddr_t machine = pfn_to_mfn(phys >> PAGE_SHIFT);
	machine = (machine << PAGE_SHIFT) | (phys & ~PAGE_MASK);
	return machine;
}
#define virt_to_mach(_virt)        (phys_to_machine(to_phys(_virt)))


#if 0
#define PAGE_SIZE 				4096
#define L1_PAGETABLE_SHIFT 		12
#define VIRT_START 				((unsigned long)&_text)
#define PFN_DOWN(x) 			((x) >> L1_PAGETABLE_SHIFT)
#define to_phys(x) 				((unsigned long)(x)-VIRT_START)
#define pfn_to_mfn(_pfn) 		((unsigned long)(_pfn))
#define virt_to_pfn(_virt) 		(PFN_DOWN(to_phys(_virt)))
#define virt_to_mfn(_virt) 		(pfn_to_mfn(virt_to_pfn(_virt)))
#endif

#define NET_TX_RING_SIZE __CONST_RING_SIZE(netif_tx, PAGE_SIZE)
#define NET_RX_RING_SIZE __CONST_RING_SIZE(netif_rx, PAGE_SIZE)

// #define wmb() __asm__("dsb":::"memory");

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
#if 0
    evtchn_port_t evtchn;
#else
    evtchn_port_t evtchn_tx;
    evtchn_port_t evtchn_rx;
#endif
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

extern struct netfront_dev *init_netfront(
	char *_nodename,
	void (*thenetif_rx)(unsigned char* data,
			int len, void* arg),
	unsigned char rawmac[6],
	char **ip
);
void network_rx(struct netfront_dev *dev);
grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame,
		int readonly);
#endif /* NETFRONT_H_ */
