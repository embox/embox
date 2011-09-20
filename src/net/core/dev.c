/**
 * @file
 * @brief Protocol independent device support routines.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

POOL_DEF(netdev_pool, struct net_device, CONFIG_NET_DEVICES_QUANTITY);

static struct net_device * opened_netdevs[CONFIG_NET_DEVICES_QUANTITY];

#if 0
static struct net_device *rx_netdevs;
static struct net_device *tx_netdevs;
#endif

struct net_device * get_dev_by_idx(int num) {
	if ((num < 0) || (num >= CONFIG_NET_DEVICES_QUANTITY)) {
		return NULL;
	}
	return opened_netdevs[num];
}

int dev_alloc_name(struct net_device *dev, const char *name) {
	strcpy(dev->name, name);
	return ENOERR;
}

static int process_backlog(struct net_device *dev) {
	struct sk_buff *skb;

	skb = skb_dequeue(&(dev->dev_queue));
	while (skb != NULL) {
		netif_receive_skb(skb);
		skb = skb_dequeue(&(dev->dev_queue));
	}

	return ENOERR;
}

static int netdev_get_hash_idx(const char *if_name) {
	int idx;

	if (strcmp(if_name, "lo") == 0) { /* Loopback interface */
		return 0;
	}
	else if (strncmp(if_name, "eth", 3) == 0) { /* Ethernet */
		idx = (int)strtol(if_name + 3, NULL, 10) + 1;
		if (idx >= CONFIG_NET_DEVICES_QUANTITY) {
			return -ENOMEM;
		}
		return idx;
	}

	/* if there => unknown, so error */
	return -EINVAL;
}

struct net_device * alloc_netdev(int sizeof_priv, const char *name,
		void (*setup)(struct net_device *)) {
	struct net_device *dev;
	char buff[IFNAMSIZ];
	int hash_idx;

	if ((name == NULL) || (setup == NULL)) {
		return NULL; /* Invalid args */
	}

	dev = (struct net_device *)pool_alloc(&netdev_pool);
	if (dev == NULL) {
		return NULL;
	}

	/* Get id in hash table */
	hash_idx = netdev_get_hash_idx(name);
	if (hash_idx < 0) {
		pool_free(&netdev_pool, dev);
		return NULL;
	}

	if (hash_idx == 0) {
		strncpy(dev->name, name, sizeof(dev->name) - 1);
	} else {
		/* FIXME can happen array bounds exceeded. Need snprintf() function */
		sprintf(buff, "%s%d", name, hash_idx - 1);
		strncpy(dev->name, buff, sizeof(dev->name) - 1);
	}

	opened_netdevs[hash_idx] = dev;

	setup(dev);

	dev->dev_queue.next = (struct sk_buff *)(&(dev->dev_queue));
	dev->dev_queue.prev = (struct sk_buff *)(&(dev->dev_queue));
	dev->dev_queue.qlen = 0;
	dev->dev_queue.lock = 0;
	dev->poll = process_backlog;

	return dev;
}

void free_netdev(struct net_device *dev) {
	pool_free(&netdev_pool, dev);
}

#if 0
int register_netdev(struct net_device *dev) {
	/*FIXME we must create queue for each device*/
	return 0;
}

void unregister_netdev(struct net_device *dev) {

}
#endif

struct net_device * netdev_get_by_name(const char *name) {
	int idx;

	idx = netdev_get_hash_idx(name);
	if (idx < 0) {
		return NULL;
	}

	return opened_netdevs[idx];
}

struct net_device * dev_getbyhwaddr(unsigned short type, char *hw_addr) {
	size_t i;
	struct net_device *dev;

	for (i = 1; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		dev = opened_netdevs[i];
		if ((dev != NULL) && (memcmp(hw_addr, dev->dev_addr, dev->addr_len) == 0)) {
			return dev;
		}
	}

	return NULL;
}

int dev_open(struct net_device *dev) {
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

int dev_close(struct net_device *dev) {
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
	}
	else {
		/* Device is now down. */
		/*TODO: IFF_RUNNING sets not here*/
		dev->flags &= ~(IFF_UP | IFF_RUNNING);
	}

	return res;
}

unsigned int dev_get_flags(const struct net_device *dev) {
	return dev->flags;
}


int dev_set_flags(struct net_device *dev, unsigned int flags) {
	int res, old_flags;
	int (*func)(struct net_device *);

	res = ENOERR;
	old_flags = dev->flags;
	if ((old_flags ^ flags) & IFF_UP) { /* i.e. IFF_UP bit was inverted */
		func = (flags & IFF_UP) ? &dev_open : &dev_close;
		res = func(dev);
	}

	dev->flags = flags;

	return res;
}

