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
#include <linux/init.h>
#include <linux/interrupt.h>
#include <embox/unit.h>



typedef struct _NET_DEV_INFO {
	net_device_t dev;
	int is_busy;
} NET_DEV_INFO;

static NET_DEV_INFO net_devices[CONFIG_NET_DEVICES_QUANTITY];

int dev_is_busy(int num) {
	return net_devices[num].is_busy;
}

net_device_t *get_dev_by_idx(int num) {
	return &net_devices[num].dev;
}

static inline net_device_t *dev_lock(int num) {
	net_devices[num].is_busy = 1;
	return &net_devices[num].dev;
}

static inline void dev_unlock(int num) {
	net_devices[num].is_busy = 0;
}

int dev_alloc_name(struct net_device *dev, const char *name) {
	strcpy(dev->name, name);
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

net_device_t *alloc_netdev(int sizeof_priv, const char *name, void(*setup)(
		net_device_t *)) {
	struct net_device *dev;
	char buff[IFNAMSIZ];
	size_t i;
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		if (!dev_is_busy(i)) {
			dev = dev_lock(i);
			setup(dev);
			sprintf(buff, name, i);
			strcpy(dev->name, buff);

			dev->dev_queue.next = (sk_buff_t *) (&(dev->dev_queue));
			dev->dev_queue.prev = (sk_buff_t *) (&(dev->dev_queue));
			dev->dev_queue.qlen = 0;
			dev->dev_queue.lock = 0;
			dev->poll = process_backlog;
			return dev;
		}
	}
	return NULL;
}

void free_netdev(net_device_t *dev) {
	size_t i;
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		if (dev == &net_devices[i].dev) {
			dev_unlock(i);
		}
	}
}

int register_netdev(struct net_device *dev) {
	/*FIXME we must create queue for each device*/
	return 0;
}

void unregister_netdev(struct net_device *dev) {

}

net_device_t *netdev_get_by_name(const char *name) {
	struct net_device *dev;
	size_t i;
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		dev = &net_devices[i].dev;
		if (dev_is_busy(i) && !strncmp(name, dev->name, IFNAMSIZ)) {
			return dev;
		}
	}
	return NULL;
}

net_device_t *dev_getbyhwaddr(unsigned short type, char *ha) {
	struct net_device *dev;
	size_t i;
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		dev = &net_devices[i].dev;
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
