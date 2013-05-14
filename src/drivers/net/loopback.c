/**
 * @file
 * @brief Pseudo-driver for the loopback interface.
 *
 * @date 29.12.09
 * @author Nikolay Korotky
 * @author Dmitry Zubarevich
 */

#include <assert.h>
#include <embox/unit.h>
#include <errno.h>
#include <net/etherdevice.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

EMBOX_UNIT_INIT(loopback_init);

static int loopback_xmit(struct sk_buff *skb, struct net_device *dev) {
	struct net_device_stats *lb_stats;

	if ((skb == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	lb_stats = &dev->stats;

	if (netif_rx(skb) == NET_RX_SUCCESS) {
		lb_stats->tx_packets++;
		lb_stats->rx_packets++;
		lb_stats->tx_bytes += skb->len;
		lb_stats->rx_bytes += skb->len;
	} else {
		lb_stats->rx_err++;
		lb_stats->tx_err++;
	}

	return 0;
}

static const struct net_device_ops loopback_ops = {
	.ndo_start_xmit = loopback_xmit
};

/**
 * The loopback device is special. There is only one instance
 * per network namespace.
 */
static void loopback_setup(struct net_device *dev) {
	dev->mtu                = (16 * 1024) + 20 + 20 + 12;
	dev->addr_len           = ETH_ALEN;
	dev->tx_queue_len       = 0;
	dev->type               = ARPG_HRD_LOOPBACK;
	dev->flags              = IFF_LOOPBACK;
	dev->netdev_ops         = &loopback_ops;
	dev->header_ops         = eth_get_header_ops();
}

/**
 * The initialization of loopback device
 */
static int loopback_init(void) {
	int ret;
	struct net_device *loopback_dev;

	loopback_dev = netdev_alloc("lo", &loopback_setup);
	if (loopback_dev == NULL) {
		return -ENOMEM;
	}

	ret = inetdev_register_dev(loopback_dev);
	if (ret != 0) {
		netdev_free(loopback_dev);
		return ret;
	}

	return 0;
}
