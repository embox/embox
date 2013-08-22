/**
 * @file
 * @brief Protocol independent device support routines.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/unit.h>
#include <errno.h>
#include <stdlib.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <string.h>
#include <util/hashtable.h>
#include <util/list.h>

#define MODOPS_NETDEV_QUANTITY OPTION_GET(NUMBER, netdev_quantity)
#define MODOPS_NETDEV_TABLE_SZ OPTION_GET(NUMBER, netdev_table_sz)

EMBOX_UNIT_INIT(netdev_unit_init);

POOL_DEF(netdev_pool, struct net_device, MODOPS_NETDEV_QUANTITY);
struct hashtable *netdevs_table = NULL;

static int netdev_init(struct net_device *dev, const char *name,
		int (*setup)(struct net_device *), size_t priv_size) {
	assert(dev != NULL);
	assert(name != NULL);
	assert(setup != NULL);

	list_link_init(&dev->rx_lnk);
	strcpy(&dev->name[0], name);
	memset(&dev->stats, 0, sizeof dev->stats);
	skb_queue_init(&dev->dev_queue);
	skb_queue_init(&dev->tx_dev_queue);

	if (priv_size != 0) {
		dev->priv = malloc(priv_size);
		if (dev->priv == NULL) {
			return -ENOMEM;
		}
	}
	else {
		dev->priv = NULL;
	}

	return (*setup)(dev);
}

struct net_device * netdev_alloc(const char *name,
		int (*setup)(struct net_device *), size_t priv_size) {
	int ret;
	struct net_device *dev;

	if ((name == NULL) || (setup == NULL)) {
		return NULL; /* error: invalid args */
	}

	if (strlen(name) >= ARRAY_SIZE(dev->name)) {
		return NULL; /* error: name too big */
	}

	dev = (struct net_device *)pool_alloc(&netdev_pool);
	if (dev == NULL) {
		return NULL; /* error: no memory */
	}

	ret = netdev_init(dev, name, setup, priv_size);
	if (ret != 0) {
		netdev_free(dev);
		return NULL; /* error: see return code */
	}

	return dev;
}

void netdev_free(struct net_device *dev) {
	if (dev != NULL) {
		list_unlink_link(&dev->rx_lnk);
		skb_queue_purge(&dev->dev_queue);
		skb_queue_purge(&dev->tx_dev_queue);
		free(dev->priv);
		pool_free(&netdev_pool, dev);
	}
}

int netdev_register(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	return hashtable_put(netdevs_table, (void *)&dev->name[0],
			(void *)dev);
}

int netdev_unregister(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	return hashtable_del(netdevs_table, (void *)&dev->name[0]);
}

struct net_device * netdev_get_by_name(const char *name) {
	if (name == NULL) {
		return NULL; /* error: invalid name */
	}

	return hashtable_get(netdevs_table, (void *)name);
}

int netdev_open(struct net_device *dev) {
	int ret;

	if (dev == NULL) {
		return -EINVAL;
	}

	if (dev->flags & IFF_UP) {
		return 0;
	}

	if (dev->drv_ops->start != NULL) {
		ret = dev->drv_ops->start(dev);
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

	if (dev == NULL) {
		return -EINVAL;
	}

	if (!(dev->flags & IFF_UP)) {
		return 0;
	}

	if (dev->drv_ops->stop != NULL) {
		ret = dev->drv_ops->stop(dev);
		if (ret != 0) {
			return ret;
		}
	}

	/* TODO IFF_RUNNING sets not here*/
	dev->flags &= ~(IFF_UP | IFF_RUNNING);

	return 0;
}

int netdev_set_macaddr(struct net_device *dev, const void *addr) {
	int ret;

	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	assert(dev->ops != NULL);
	if (dev->ops->check_addr != NULL) {
		ret = dev->ops->check_addr(addr);
		if (ret != 0) {
			return ret;
		}
	}

	assert(dev->drv_ops != NULL);
	if (dev->drv_ops->set_macaddr != NULL) {
		ret = dev->drv_ops->set_macaddr(dev, addr);
		if (ret != 0) {
			return ret;
		}
	}

	memcpy(&dev->dev_addr[0], addr, dev->addr_len);

	return 0;
}

int netdev_set_bcastaddr(struct net_device *dev, const void *bcast_addr) {
	if ((dev == NULL) || (bcast_addr == NULL)) {
		return -EINVAL;
	}

	memcpy(&dev->broadcast[0], bcast_addr, dev->addr_len);

	return 0;
}

int netdev_flag_up(struct net_device *dev, unsigned int flag) {
	int ret;

	if (dev == NULL) {
		return -EINVAL;
	}

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

	if (dev == NULL) {
		return -EINVAL;
	}

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

int netdev_set_mtu(struct net_device *dev, int mtu) {
	int ret;

	if (dev == NULL) {
		return -EINVAL;
	}

	assert(dev->ops != NULL);
	if (dev->ops->check_mtu != NULL) {
		ret = dev->ops->check_mtu(mtu);
		if (ret != 0) {
			return ret;
		}
	}

	dev->mtu = mtu;

	return 0;
}

int netdev_set_txqueuelen(struct net_device *dev, unsigned long new_len) {
	if (dev == NULL) {
		return -EINVAL;
	}

	dev->tx_queue_len = new_len;

	return 0;
}

int netdev_set_baseaddr(struct net_device *dev, unsigned long base_addr) {
	if (dev == NULL) {
		return -EINVAL;
	}

	dev->base_addr = base_addr;

	return 0;
}

int netdev_set_irq(struct net_device *dev, int irq_num) {
	if (dev == NULL) {
		return -EINVAL;
	}

	dev->irq = irq_num;

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

static int netdev_unit_init(void) {
	netdevs_table = hashtable_create(MODOPS_NETDEV_TABLE_SZ,
			(get_hash_ft)&netdev_hash, (ht_cmp_ft)&strcmp);
	if (netdevs_table == NULL) {
		return -ENOMEM;
	}

	return 0;
}
