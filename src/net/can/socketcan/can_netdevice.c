/**
 * @file
 *
 * @date 23.04.26
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <net/l2/ethernet.h>
#include <net/netdevice.h>

#include "can_netdevice.h"

static struct net_device *can_netdev;

static int can_netdev_setup(struct net_device *dev) {
	static unsigned int can_id = 0;

	int ret;
	char name_fmt[IFNAMSIZ];

	assert(dev);

	strcpy(name_fmt, dev->name);
	ret = snprintf(dev->name, IFNAMSIZ, name_fmt, can_id);
	if (ret < 0) {
		return -EIO;
	}
	if (ret >= IFNAMSIZ) {
		return -ENOMEM;
	}

	++can_id;

	return 0;
}

struct net_device *can_netdev_alloc(void) {
	return netdev_alloc("can%u", &can_netdev_setup, 0);
}

void can_netdev_free(struct net_device *dev) {
	netdev_free(dev);
}

int can_netdev_register(struct net_device *netdev) {
	can_netdev = netdev;
	return 0;
}

struct net_device *can_netdev_get(const char *name, int idx) {
	return can_netdev;
}
