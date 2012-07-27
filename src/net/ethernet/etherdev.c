/**
 * @file
 * @brief Ethernet-type device handling.
 *
 * @date 4.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <err.h>
#include <errno.h>
#include <net/arp.h>
#include <net/etherdevice.h>
#include <net/if.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <string.h>
#include <assert.h>

static unsigned char etherdev_idx = 0;

int etherdev_mac_addr(struct net_device *dev, struct sockaddr *addr) {
	assert(dev != NULL);
	assert(addr != NULL);

	if (!is_valid_ether_addr((const uint8_t *) addr->sa_data)) {
		return -EADDRNOTAVAIL;
	}

	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);

	return ENOERR;
}

int etherdev_change_mtu(struct net_device *dev, int new_mtu) {
	assert(dev != NULL);

	if ((new_mtu < 68) || (new_mtu > ETH_FRAME_LEN)) {
		return -EINVAL;
	}

	dev->mtu = new_mtu;

	return ENOERR;
}

int etherdev_flag_up(struct net_device *dev, int flag_type) {
	assert(dev != NULL);
	dev->flags |= flag_type;
	return ENOERR;
}

int etherdev_flag_down(struct net_device *dev, int flag_type) {
	assert(dev != NULL);
	dev->flags &= ~flag_type;
	return ENOERR;
}

int etherdev_set_irq(struct net_device *dev, int irq_num) {
	assert(dev != NULL);
	dev->irq = irq_num;
	return ENOERR;
}

int etherdev_set_baseaddr(struct net_device *dev, unsigned long base_addr) {
	assert(dev != NULL);
	dev->base_addr = base_addr;
	return ENOERR;
}

int etherdev_set_txqueuelen(struct net_device *dev, unsigned long new_len) {
	assert(dev != NULL);
	dev->tx_queue_len = new_len;
	return ENOERR;
}

int etherdev_set_broadcast_addr(struct net_device *dev, unsigned char *broadcast_addr) {
	assert(dev != NULL);
	assert(broadcast_addr != NULL);

	memcpy((void *)dev->broadcast, (void *)broadcast_addr, ETH_ALEN);

	return ENOERR;
}

static void etherdev_setup(struct net_device *dev) {
	assert(dev != NULL);

	dev->header_ops    = eth_get_header_ops();
	dev->type          = ARPHRD_ETHER;
	dev->mtu           = ETH_FRAME_LEN;
	dev->addr_len      = ETH_ALEN;
	dev->flags         = IFF_BROADCAST | IFF_MULTICAST;
	dev->tx_queue_len  = 1000;
	memset(dev->broadcast, 0xFF, ETH_ALEN);
}

struct net_device * etherdev_alloc(void) {
	char buff[IFNAMSIZ];

	sprintf(buff, "eth%u", etherdev_idx++); /* TODO use snprintf instead */

	return netdev_alloc(buff, &etherdev_setup);
}

void etherdev_free(struct net_device *dev) {
	netdev_free(dev);
}

