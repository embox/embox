/**
 * @file
 * @brief Protocol independent device support routines.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdlib.h>
#include <string.h>
#include <hal/ipl.h>
#include <linux/list.h>
#include <util/list.h>
#include <util/hashtable.h>
#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(netdev_init);

POOL_DEF(netdev_pool, struct net_device, OPTION_GET(NUMBER, netdev_quantity));
struct hashtable *netdevs_table;

struct net_device * netdev_alloc(const char *name,
		void (*setup)(struct net_device *)) {
	struct net_device *dev;

	assert((name != NULL) && (setup != NULL));

	dev = (struct net_device *)pool_alloc(&netdev_pool);
	if (dev == NULL) {
		return NULL;
	}

	(*setup)(dev);

	list_link_init(&dev->rx_lnk);
	skb_queue_init(&dev->dev_queue);
	skb_queue_init(&dev->tx_dev_queue);
	skb_queue_init(&dev->txing_queue);

	strncpy(dev->name, name, sizeof dev->name);

	return dev;
}

void netdev_free(struct net_device *dev) {
	assert(dev != NULL);
	pool_free(&netdev_pool, dev);
}

int netdev_register(struct net_device *dev) {
	assert((dev != NULL) && (dev->name != NULL));
	return hashtable_put(netdevs_table, (void *)dev->name, (void *)dev);
}

int netdev_unregister(struct net_device *dev) {
	assert((dev != NULL) && (dev->name != NULL));
	return hashtable_del(netdevs_table, (void *)dev->name);
}

struct net_device * netdev_get_by_name(const char *name) {
	assert(name != NULL);
	return hashtable_get(netdevs_table, (void *)name);
}

int netdev_open(struct net_device *dev) {
	int ret;

	assert(dev != NULL);

	if (dev->flags & IFF_UP) {
		return 0;
	}

	if (dev->netdev_ops->ndo_open != NULL) {
		ret = dev->netdev_ops->ndo_open(dev);
		if (ret != 0) {
			return ret;
		}
	}

	/* TODO IFF_RUNNING sets not here*/
	dev->flags |= IFF_UP | IFF_RUNNING;

	return 0;
}

int netdev_close(struct net_device *dev) {
	int ret;

	assert(dev != NULL);

	if (!(dev->flags & IFF_UP)) {
		return 0;
	}

	if (dev->netdev_ops->ndo_stop != NULL) {
		ret = dev->netdev_ops->ndo_stop(dev);
		if (ret != 0) {
			return ret;
		}
	}

	/* TODO IFF_RUNNING sets not here*/
	dev->flags &= ~(IFF_UP | IFF_RUNNING);

	return 0;
}

int netdev_flag_up(struct net_device *dev, unsigned int flag) {
	int ret;

	if (dev->flags & flag) {
		return 0;
	}

	switch (flag) {
	case IFF_UP:
		ret = netdev_open(dev);
		if (ret != 0) {
			return ret;
		}
		break;
	}

	dev->flags |= flag;

	return 0;
}

int netdev_flag_down(struct net_device *dev, unsigned int flag) {
	int ret;

	if (!(dev->flags & flag)) {
		return 0;
	}

	switch (flag) {
	case IFF_UP:
		ret = netdev_close(dev);
		if (ret != 0) {
			return ret;
		}
		break;
	}

	dev->flags &= ~flag;

	return 0;
}

int netdev_set_macaddr(struct net_device *dev,
		const void *mac_addr) {
	if ((dev == NULL) || (mac_addr == NULL)) {
		return -EINVAL;
	}

	assert(dev->netdev_ops != NULL);
	if (dev->netdev_ops->ndo_set_mac_address == NULL) {
		return -ENOSUPP;
	}

	return dev->netdev_ops->ndo_set_mac_address(dev, mac_addr);
}

int netdev_set_irq(struct net_device *dev, int irq_num) {
	assert(dev != NULL);
	dev->irq = irq_num;
	return 0;
}

int netdev_set_baseaddr(struct net_device *dev, unsigned long base_addr) {
	assert(dev != NULL);
	dev->base_addr = base_addr;
	return 0;
}

int netdev_set_txqueuelen(struct net_device *dev, unsigned long new_len) {
	assert(dev != NULL);
	dev->tx_queue_len = new_len;
	return 0;
}

int netdev_set_bcastaddr(struct net_device *dev, const void *bcast_addr) {
	assert(dev != NULL);
	assert(bcast_addr != NULL);
	memcpy(&dev->broadcast[0], bcast_addr, dev->addr_len);
	return 0;
}

static size_t netdev_hash(const char *name) {
	size_t hash;
	hash = 0;
	while (*name != '\0') {
		hash ^= *name++;
	}
	return hash;
}

static int netdev_init(void) {
	netdevs_table = hashtable_create(OPTION_GET(NUMBER,netdev_table_sz),
			(get_hash_ft)&netdev_hash, (ht_cmp_ft)&strcmp);
	if (netdevs_table == NULL) {
		return -ENOMEM;
	}
	return 0;
}
