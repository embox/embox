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

#define PAGE_SIZE 				4096
#define L1_PAGETABLE_SHIFT 		12
#define VIRT_START 				((unsigned long)&_text)
#define PFN_DOWN(x) 			((x) >> L1_PAGETABLE_SHIFT)
#define to_phys(x) 				((unsigned long)(x)-VIRT_START)
#define pfn_to_mfn(_pfn) 		((unsigned long)(_pfn))
#define virt_to_pfn(_virt) 		(PFN_DOWN(to_phys(_virt)))
#define virt_to_mfn(_virt) 		(pfn_to_mfn(virt_to_pfn(_virt)))

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

#endif /* NETFRONT_H_ */
