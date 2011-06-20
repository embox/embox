/**
 * @file
 * @brief Pseudo-driver for the loopback interface.
 *
 * @date 29.12.09
 * @author Nikolay Korotky
 * @author Dmitry Zubarevich
 */

#include <string.h>

#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/if_ether.h>
#include <net/if_arp.h>
#include <net/etherdevice.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <linux/init.h>

EMBOX_UNIT_INIT(unit_init);

static net_device_stats_t *lb_stats;

static int loopback_xmit(sk_buff_t *skb, net_device_t *dev) {
	int len;

	if (NULL == skb || NULL == dev) {
		return -1;
	}

	lb_stats = &(dev->stats);

	len = skb->len;
#if 0
	skb->protocol = eth_type_trans(rx_skb, dev);
	skb->dev = skb->dev;
#endif
	if (netif_rx(skb) == NET_RX_SUCCESS) {
		lb_stats->tx_packets++;
		lb_stats->tx_bytes += len;
	} else {
		lb_stats->tx_err++;
	}
	return 0;
}

static net_device_stats_t *loopback_get_stats(net_device_t *dev) {
	if (NULL == dev) {
		return NULL;
	}
	return &(dev->stats);
}

static const struct net_device_ops loopback_ops = {
	.ndo_start_xmit      = loopback_xmit,
	.ndo_get_stats       = loopback_get_stats,
};

/**
 * The loopback device is special. There is only one instance
 * per network namespace.
 */
static void loopback_setup(net_device_t *dev) {
	dev->mtu                = (16 * 1024) + 20 + 20 + 12;
	dev->addr_len           = ETH_ALEN;
	dev->tx_queue_len       = 0;
	dev->type               = ARPHRD_LOOPBACK;
	dev->flags              = IFF_LOOPBACK;
	dev->netdev_ops         = &loopback_ops;
	dev->header_ops         = get_eth_header_ops();
}

/**
 * The initialization of loopback device
 */
static int __init unit_init(void) {
	//net_device_t *net_device;
	printk("alloc lo interface\n");
	if (NULL == alloc_netdev(0, "lo", loopback_setup)) {
		LOG_ERROR("Can't allocate net device\n");
		return -1;
	}
	return 0;
}
