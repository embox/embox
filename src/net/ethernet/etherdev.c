/**
 * @file
 * @brief Ethernet-type device handling.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <linux/etherdevice.h>
#include <net/etherdevice.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <stdio.h>
#include <string.h>
#include <util/array.h>

int etherdev_set_macaddr(struct net_device *dev, const void *mac_addr) {
	assert(dev != NULL);
	assert(mac_addr != NULL);

	if (!is_valid_ether_addr(mac_addr)) {
		return -EINVAL;
	}

	return netdev_set_macaddr(dev, mac_addr);
}

int etherdev_set_mtu(struct net_device *dev, int new_mtu) {
	assert(dev != NULL);

	if ((new_mtu < ETH_ZLEN) || (new_mtu > ETH_FRAME_LEN)) {
		return -EINVAL;
	}

	dev->mtu = new_mtu;

	return 0;
}

static void etherdev_setup(struct net_device *dev) {
	assert(dev != NULL);

	dev->header_ops    = eth_get_header_ops();
	dev->type          = ARPG_HRD_ETHERNET;
	dev->mtu           = ETH_FRAME_LEN;
	dev->addr_len      = ETH_ALEN;
	dev->flags         = IFF_BROADCAST | IFF_MULTICAST;
	dev->tx_queue_len  = 1000;
	memset(dev->broadcast, 0xFF, ETH_ALEN);
}

struct net_device * etherdev_alloc(void) {
	static unsigned char etherdev_idx = 0;
	char buff[IFNAMSIZ];

	snprintf(buff, ARRAY_SIZE(buff), "eth%u", etherdev_idx++);

	return netdev_alloc(buff, &etherdev_setup);
}

void etherdev_free(struct net_device *dev) {
	netdev_free(dev);
}
