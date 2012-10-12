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
#include <lib/list.h>
#include <util/hashtable.h>
#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(netdev_init);

POOL_DEF(netdev_pool, struct net_device, OPTION_GET(NUMBER, netdev_quantity));
struct hashtable *netdevs_table;

static int netdev_process_backlog(struct net_device *dev) {
	struct sk_buff *skb;

	while ((skb = skb_queue_pop(&(dev->dev_queue))) != NULL) {
		netif_receive_skb(skb);
	}

	return ENOERR;
}

struct net_device * netdev_alloc(const char *name,
		void (*setup)(struct net_device *)) {
	struct net_device *dev;

	assert((name != NULL) && (setup != NULL));

	dev = (struct net_device *)pool_alloc(&netdev_pool);
	if (dev == NULL) {
		return NULL;
	}

	(*setup)(dev);

	skb_queue_init(&dev->dev_queue);
	skb_queue_init(&dev->tx_dev_queue);
	skb_queue_init(&dev->txing_queue);
	dev->poll = &netdev_process_backlog;

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
	int res;
	const struct net_device_ops *ops;

	/* Is it already up? */
	if (dev->flags & IFF_UP) {
		return ENOERR;
	}

	res = ENOERR;
	ops = dev->netdev_ops;
	dev->state |= __LINK_STATE_START;

	if (ops->ndo_open) {
		res = ops->ndo_open(dev);
	}
	if (res != ENOERR) {
		dev->state &= ~__LINK_STATE_START;
	} else {
		/* Set the flags. */
		/*TODO: IFF_RUNNING sets not here*/
		dev->flags |= (IFF_UP | IFF_RUNNING);
	}

	return res;
}

int netdev_close(struct net_device *dev) {
	int res;
	const struct net_device_ops *ops;

	if (!(dev->flags & IFF_UP)) {
		return ENOERR;
	}

	res = ENOERR;
	ops = dev->netdev_ops;
	dev->state &= ~__LINK_STATE_START;

	if (ops->ndo_stop) {
		res = ops->ndo_stop(dev);
	}
	if (res != ENOERR) {
		dev->state |= __LINK_STATE_START;
	} else {
		/* Device is now down. */
		/* TODO: IFF_RUNNING sets not here*/
		dev->flags &= ~(IFF_UP | IFF_RUNNING);
	}

	return res;
}

unsigned int netdev_get_flags(const struct net_device *dev) {
	return dev->flags;
}

int netdev_set_flags(struct net_device *dev, unsigned int flags) {
	int res, old_flags;
	int (*func)(struct net_device *);

	res = ENOERR;
	old_flags = dev->flags;
	if ((old_flags ^ flags) & IFF_UP) { /* i.e. IFF_UP bit was inverted */
		func = (flags & IFF_UP) ? &netdev_open : &netdev_close;
		res = (*func)(dev);
	}

	dev->flags = flags;

	return res;
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
	return ENOERR;
}

