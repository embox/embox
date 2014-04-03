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
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_entry.h>

EMBOX_UNIT_INIT(loopback_init);

static int loopback_xmit(struct net_device *dev,
		struct sk_buff *skb) {
	struct net_device_stats *lb_stats;
	size_t skb_len;

	if ((skb == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	skb_len = skb->len;

	lb_stats = &dev->stats;

	if (netif_rx(skb) == NET_RX_SUCCESS) {
		lb_stats->tx_packets++;
		lb_stats->rx_packets++;
		lb_stats->tx_bytes += skb_len;
		lb_stats->rx_bytes += skb_len;
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
	dev->mtu      = (16 * 1024) + 20 + 20 + 12;
	dev->hdr_len  = ETH_HEADER_SIZE;
	dev->addr_len = ETH_ALEN;
	dev->type     = ARP_HRD_LOOPBACK;
	dev->flags    = IFF_LOOPBACK | IFF_RUNNING;
	dev->drv_ops  = &loopback_ops;
	dev->ops      = &ethernet_ops;
	return 0;
}

/**
 * The initialization of loopback device
 */
static int loopback_init(void) {
	int ret;
	struct net_device *loopback_dev;

	loopback_dev = netdev_alloc("lo", &loopback_setup, 0);
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
