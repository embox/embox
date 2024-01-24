/**
 * @file
 * @brief Protocol independent device support routines.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include <lib/libds/hashtable.h>
#include <lib/libds/dlist.h>
#include <lib/libds/indexator.h>
#include <lib/libds/array.h>

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/netlink.h>

#include <framework/mod/options.h>

#define MODOPS_NETDEV_QUANTITY OPTION_GET(NUMBER, netdev_quantity)
#define MODOPS_NETDEV_TABLE_SZ OPTION_GET(NUMBER, netdev_table_sz)

POOL_DEF(netdev_pool, struct net_device, MODOPS_NETDEV_QUANTITY);

INDEX_DEF(netdev_index, 1, MODOPS_NETDEV_QUANTITY);

static size_t netdev_hash(void *key) {
	size_t hash;
	const char *name = key;

	hash = 0;
	while (*name != '\0') {
		hash ^= *name++;
	}

	return hash;
}

HASHTABLE_DEF(nd_ht, MODOPS_NETDEV_TABLE_SZ, &netdev_hash, (ht_cmp_ft)&strcmp);
struct hashtable *netdevs_table = &nd_ht;

POOL_DEF(netdev_htitem_pool, struct hashtable_item, MODOPS_NETDEV_QUANTITY);

static int netdev_init(struct net_device *dev, const char *name,
		int (*setup)(struct net_device *), size_t priv_size) {
	assert(dev != NULL);
	assert(name != NULL);
	assert(setup != NULL);

	dlist_head_init(&dev->rx_lnk);
	dlist_head_init(&dev->tx_lnk);
	strcpy(&dev->name[0], name);
	memset(&dev->stats, 0, sizeof dev->stats);
	skb_queue_init(&dev->dev_queue);
	skb_queue_init(&dev->dev_queue_tx);

	if (priv_size != 0) {
		dev->priv = sysmalloc(priv_size);
		if (dev->priv == NULL) {
			return -ENOMEM;
		}
	}
	else {
		dev->priv = NULL;
	}

	return (*setup)(dev);
}

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
void dev_net_set(struct net_device *dev, net_namespace_p net_ns) {
	assign_net_ns(dev->net_ns, net_ns);
}
#endif

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

	dev->index = index_alloc(&netdev_index, INDEX_NEXT);

	ret = netdev_init(dev, name, setup, priv_size);
	if (ret != 0) {
		netdev_free(dev);
		return NULL; /* error: see return code */
	}

	return dev;
}

void netdev_free(struct net_device *dev) {
	if (dev != NULL) {
		dlist_del_init(&dev->rx_lnk);
		dlist_del_init(&dev->tx_lnk);
		skb_queue_purge(&dev->dev_queue);
		skb_queue_purge(&dev->dev_queue_tx);
		if (dev->priv) {
			sysfree(dev->priv);
		}
		index_free(&netdev_index, dev->index);
		pool_free(&netdev_pool, dev);
	}
}

int netdev_register(struct net_device *dev) {
	struct hashtable_item *ht_item;

	if (dev == NULL) {
		return -EINVAL;
	}

	ht_item = pool_alloc(&netdev_htitem_pool);
	ht_item = hashtable_item_init(ht_item, (void *)&dev->name[0], (void *)dev);

	return hashtable_put(netdevs_table, ht_item);
}

int netdev_unregister(struct net_device *dev) {
	struct hashtable_item *ht_item;

	if (dev == NULL) {
		return -EINVAL;
	}
	ht_item = hashtable_del(netdevs_table, (void *)&dev->name[0]);

	pool_free(&netdev_htitem_pool, ht_item);

	return 0;

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

	if (dev->drv_ops != NULL && dev->drv_ops->start != NULL) {
		ret = dev->drv_ops->start(dev);
		if (ret != 0) {
			return ret;
		}
	}

	dev->flags |= IFF_UP;

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

	dev->flags &= ~IFF_UP;

	return 0;
}

int netdev_set_macaddr(struct net_device *dev, const void *addr) {
	int ret;

	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	assert(dev->ops != NULL);
	if ((dev->ops->check_addr != NULL)
			&& !dev->ops->check_addr(addr)) {
		return -EINVAL; /* error: bad address */
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
	if (dev == NULL) {
		return -EINVAL;
	}

	assert(dev->ops != NULL);
	if ((dev->ops->check_mtu != NULL)
			&& !dev->ops->check_mtu(mtu)) {
		return -EINVAL; /* error: bad mtu value */
	}

	dev->mtu = mtu;

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

unsigned int if_nametoindex(const char *name) {
	struct net_device *dev;

	assert(name);

	dev = netdev_get_by_name(name);

	if (dev) {
		return dev->index;
	} else {
		errno = ENOENT;
		return 0;
	}
}

void netif_carrier_on(struct net_device *dev) {
	netlink_notify_newlink(dev);
}

void netif_carrier_off(struct net_device *dev) {
	netlink_notify_dellink(dev);
}

