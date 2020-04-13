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
#include <xen_memory.h>


//net
#include <net/netdevice.h>

#include <net/inetdevice.h>
#include <arpa/inet.h>
#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>


#define NET_TX_RING_SIZE __CONST_RING_SIZE(netif_tx, PAGE_SIZE())
#define NET_RX_RING_SIZE __CONST_RING_SIZE(netif_rx, PAGE_SIZE())

// #define wmb() __asm__("dsb":::"memory");

#define NR_GRANT_FRAMES 4
#define NR_GRANT_ENTRIES (NR_GRANT_FRAMES * PAGE_SIZE() / sizeof(grant_entry_v1_t))


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
#ifdef FEATURE_SPLIT_CHANNELS //false
    evtchn_port_t evtchn_tx;
    evtchn_port_t evtchn_rx;
#else
    evtchn_port_t evtchn;
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

/*extern struct netfront_dev *init_netfront(
	char *_nodename,
	void (*thenetif_rx)(unsigned char* data,
			int len, void* arg),
	unsigned char rawmac[6],
	char **ip
);*/
int netfront_priv_init(struct netfront_dev *dev);
void network_rx(struct netfront_dev *dev, struct net_device *embox_dev);
grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame,
		int readonly);

#endif /* NETFRONT_H_ */
