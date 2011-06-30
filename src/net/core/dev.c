/**
 * @file
 * @brief Protocol independent device support routines.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 */

#include <types.h>
#include <string.h>
#include <stdio.h>

#include <net/arp.h>
#include <net/inetdevice.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <linux/list.h>
#include <kernel/mm/misc/pool.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <embox/unit.h>
#include <stdlib.h>

POOL_DEF(netdev_pool, struct net_device, CONFIG_NET_DEVICES_QUANTITY);

static struct net_device *opened_netdevs[CONFIG_NET_DEVICES_QUANTITY];

//static struct net_device *rx_netdevs;
//static struct net_device *tx_netdevs;

int dev_is_busy(int num) {
	return opened_netdevs[num] ? 1 : 0;
}

net_device_t *get_dev_by_idx(int num) {
	return opened_netdevs[num];
}

int dev_alloc_name(struct net_device *dev, const char *name) {
	strcpy(dev->name, name);
	return 0;
}
static inline int netdev_get_free_idx(void) {
	return 0;
}
static int process_backlog(net_device_t *dev) {
	struct sk_buff *skb;
	//TODO:
	while (NULL != (skb = skb_dequeue(&(dev->dev_queue)))) {
		netif_receive_skb(skb);
	}
	return 0;
}

static inline int netdev_get_hash_idx(const char *if_name) {
	long int idx;
	char *endptr;

	if(0 == strncmp(if_name, "eth", 3)) {
		idx = strtol(if_name + 3, &endptr, 10);
		return idx + 1;
	} else { /*lo interface*/
		return 0;
	}
}

net_device_t *alloc_netdev(int sizeof_priv, const char *name, void(*setup)(
		net_device_t *)) {
	struct net_device *dev;
	char buff[IFNAMSIZ];
	int hash_idx;


	if(NULL == (dev = (struct net_device *)pool_alloc(&netdev_pool))) {
		return NULL;
	}

	/*add to hash table*/
	if (-1 == (hash_idx = netdev_get_hash_idx(name))) {
		return NULL;
	}

	if(hash_idx == 0) {
		strncpy(dev->name, name, sizeof(dev->name) - 1);
	} else {
		sprintf(buff, name, hash_idx - 1);
		strncpy(dev->name, buff, sizeof(dev->name) - 1);
	}

	opened_netdevs[hash_idx] = dev;

	setup(dev);

	dev->dev_queue.next = (sk_buff_t *) (&(dev->dev_queue));
	dev->dev_queue.prev = (sk_buff_t *) (&(dev->dev_queue));
	dev->dev_queue.qlen = 0;
	dev->dev_queue.lock = 0;
	dev->poll = process_backlog;

	return dev;
}

void free_netdev(net_device_t *dev) {
	pool_free(&netdev_pool, dev);
}

int register_netdev(struct net_device *dev) {
	/*FIXME we must create queue for each device*/
	return 0;
}

void unregister_netdev(struct net_device *dev) {

}

net_device_t *netdev_get_by_name(const char *name) {
	int idx;

	if (-1 == (idx = netdev_get_hash_idx(name))) {
		return 0;
	}

	return opened_netdevs[idx];
}

net_device_t *dev_getbyhwaddr(unsigned short type, char *ha) {
	struct net_device *dev;
	size_t i;

	for (i = 1; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		dev = opened_netdevs[i];
		if (dev_is_busy(i) && !memcmp(ha, dev->dev_addr, dev->addr_len)) {
			return dev;
		}
	}

	return NULL;
}

int dev_open(struct net_device *dev) {
	int ret = 0;
	const struct net_device_ops *ops;

	/* Is it already up? */
	if (dev->flags & IFF_UP) {
		return 0;
	}
	ops = dev->netdev_ops;
	dev->state |= __LINK_STATE_START;

	if (ops->ndo_open) {
		ret = ops->ndo_open(dev);
	}
	if (ret) {
		dev->state &= ~__LINK_STATE_START;
	} else {
		/* Set the flags. */
		/*TODO: IFF_RUNNING sets not here*/
		dev->flags |= (IFF_UP | IFF_RUNNING);
	}

	return ret;
}

int dev_close(struct net_device *dev) {
	const struct net_device_ops *ops;

	if (!(dev->flags & IFF_UP)) {
		return 0;
	}
	ops = dev->netdev_ops;
	dev->state &= ~__LINK_STATE_START;

	if (ops->ndo_stop) {
		ops->ndo_stop(dev);
	}
	/* Device is now down. */
	/*TODO: IFF_RUNNING sets not here*/
	dev->flags &= ~(IFF_UP | IFF_RUNNING);

	return 0;
}

unsigned dev_get_flags(const struct net_device *dev) {
	/*TODO: ...*/
	return dev->flags;
}

int dev_change_flags(struct net_device *dev, unsigned flags) {
	int ret = 0;
	int old_flags = dev->flags;

	if ((old_flags ^ flags) & IFF_UP) {
		ret = ((old_flags & IFF_UP) ? dev_close : dev_open)(dev);
	}
	/*TODO: ...*/
	dev->flags = dev->flags & flags;

	return ret;
}

