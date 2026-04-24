/**
 * @file
 *
 * @date 23.04.2012
 * @author Anton Bondarev
 */

#include <net/netdevice.h>
#include <net/can_netdevice.h>

static struct net_device *can_netdev;

int cannetdev_register(struct net_device *netdev) {
	can_netdev = netdev;
	return 0;
}

struct net_device *cannetdev_get(const char *name, int idx) {
	return can_netdev;
}
