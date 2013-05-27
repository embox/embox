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
#include <net/l2/ethernet.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

EMBOX_UNIT_INIT(loopback_init);

static int loopback_xmit(struct net_device *dev, struct sk_buff *skb) {
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

static const struct net_driver loopback_ops = {
	.xmit = loopback_xmit
};

/**
 * The loopback device is special. There is only one instance
 * per network namespace.
 */
static int loopback_setup(struct net_device *dev) {
	dev->mtu                = (16 * 1024) + 20 + 20 + 12;
	dev->addr_len           = ETH_ALEN;
	dev->tx_queue_len       = 0;
	dev->type               = ARPG_HRD_LOOPBACK;
	dev->flags              = IFF_LOOPBACK;
	dev->drv_ops         = &loopback_ops;
	dev->ops         = &ethernet_ops;
	return 0;
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
