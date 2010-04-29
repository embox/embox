/**
 * @file
 * @brief Pseudo-driver for the loopback interface.
 *
 * @date 29.12.09
 * @author Nikolay Korotky
 */

#include <string.h>

#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/if_ether.h>
#include <net/if_arp.h>
#include <net/etherdevice.h>
#include <net/net_pack_manager.h>
#include <kernel/irq.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

#if 0
static int loopback_xmit(sk_buff_t *skb, net_device_t *dev) {
	//TODO:
	return 0;
}

static net_device_stats_t *loopback_get_stats(net_device_t *dev) {
	//TODO:
	return NULL;
}

static const struct net_device_ops loopback_ops = {
	.ndo_start_xmit= loopback_xmit,
	.ndo_get_stats = loopback_get_stats,
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
}
#endif
static int __init unit_init() {
	//TODO:
	return 0;
}
