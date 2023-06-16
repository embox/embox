/**
 * @file 
 * @brief Frontend initialization
 *
 * @date 
 * @author 
 */

#ifndef NETFRONT_H_
#define NETFRONT_H_

#include <xen/grant_table.h>
#include <xen/io/netif.h>
#include <xen/event_channel.h>
#include <xen_memory.h>
#include <net/netdevice.h>

#define NET_TX_RING_SIZE __CONST_RING_SIZE(netif_tx, PAGE_SIZE())
#define NET_RX_RING_SIZE __CONST_RING_SIZE(netif_rx, PAGE_SIZE())

struct net_buffer {
	void* page;
	grant_ref_t gref;
};

struct netfront_dev {
	domid_t dom;

	//unsigned short tx_freelist[NET_TX_RING_SIZE + 1];
	// struct semaphore tx_sem;

	struct net_buffer rx_buffers[NET_RX_RING_SIZE];
	struct net_buffer tx_buffers[NET_TX_RING_SIZE];

	struct netif_tx_front_ring tx;
	struct netif_rx_front_ring rx;
	grant_ref_t tx_ring_ref;
	grant_ref_t rx_ring_ref;
	#define FEATURE_SPLIT_CHANNELS
#ifdef FEATURE_SPLIT_CHANNELS //false
	evtchn_port_t evtchn_tx;
	evtchn_port_t evtchn_rx;
#else
	evtchn_port_t evtchn;
#endif
	char *nodename;
	char backend[64];
	//char mac[64];
	char default_mac[64];

	//void (*netif_rx)(unsigned char* data, int len, void* arg);
	//void *netif_rx_arg;
};

int netfront_priv_init(struct netfront_dev *dev);
void network_rx(struct netfront_dev *dev, struct net_device *embox_dev);
void netfront_xmit(struct netfront_dev *dev, unsigned char* data,int len);
void network_tx_buf_gc(struct netfront_dev *dev);
#endif /* NETFRONT_H_ */
