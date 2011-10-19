/**
 * @file
 * @brief Network socket
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <net/types.h>
#include <net/core.h>
#include <embox/unit.h>

#include <errno.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <net/netdevice.h>

POOL_DEF(netdev_pool, struct net_device, CONFIG_NET_DEVICES_QUANTITY);
static struct net_device *opened_netdevs[CONFIG_NET_DEVICES_QUANTITY] = {0}; /* We must clear array */

struct net_device * alloc_netdev(int sizeof_priv, const char *name,
		void (*setup)(struct net_device *)) {
	struct net_device *dev;

	if ((name == NULL) || (setup == NULL)) {
		return NULL; /* Invalid args */
	}

	dev = (struct net_device *)pool_alloc(&netdev_pool);
	if (dev == NULL) {
		return NULL;
	}

	setup(dev);

	strncpy(dev->name, name, IFNAMSIZ);

	return dev;
}

struct net_device * get_dev_by_idx(int num) {
	if ((num < 0) || (num >= CONFIG_NET_DEVICES_QUANTITY)) {
		return NULL;
	}

	return opened_netdevs[num];
}


void free_netdev(struct net_device *dev) {
	if (dev == NULL) {
		return;
	}
	pool_free(&netdev_pool, dev);
}


int register_netdev(struct net_device *dev) {
	size_t idx;

	if (dev == NULL) {
		return -EINVAL;
	}

	for (idx = 0; idx < CONFIG_NET_DEVICES_QUANTITY; ++idx) {
		if (opened_netdevs[idx] == NULL) {
			opened_netdevs[idx] = dev;
			return ENOERR;
		}
	}

	return -ENOMEM;
}

void unregister_netdev(struct net_device *dev) {
	size_t i;

	if (dev == NULL) {
		return;
	}

	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; ++i) {
		if (opened_netdevs[i] == dev) {
			opened_netdevs[i] = NULL;
			return;
		}
	}
}


struct net_device * netdev_get_by_name(const char *name) {
	size_t i;
	struct net_device *dev;

	if (name == NULL) {
		return NULL;
	}

	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; ++i) {
		dev = opened_netdevs[i];
		if (strncmp(name, dev->name, IFNAMSIZ) == 0) {
			return dev;
		}
	}

	return NULL;
}

struct net_device * dev_getbyhwaddr(unsigned short type, char *hw_addr) {
	size_t i;
	struct net_device *dev;

	for (i = 1; i < CONFIG_NET_DEVICES_QUANTITY; ++i) {
		dev = opened_netdevs[i];
		if ((dev != NULL) && (memcmp(hw_addr, dev->dev_addr, dev->addr_len) == 0)) {
			return dev;
		}
	}

	return NULL;
}

EMBOX_UNIT_INIT(net_dev_init);

static int tx_hnd(net_node_t this, void *data) {
//	struct net_device *dev = opened_netdevs[this->node_addr];
	return 0;
}

net_proto_t dev_proto;

static int net_dev_init(void) {
	net_proto_init(dev_proto, 0, NULL, tx_hnd);
	return 0;
}
