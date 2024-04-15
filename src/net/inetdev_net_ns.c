/**
 * @file
 * @brief
 *
 * @date 18.07.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <stddef.h>
#include <string.h>
#include <lib/libds/dlist.h>
#include <net/net_namespace.h>

#define MODOPS_AMOUNT_INTERFACE OPTION_GET(NUMBER, amount_interface)

POOL_DEF(inetdev_pool, struct in_device, MODOPS_AMOUNT_INTERFACE);
static DLIST_DEFINE(inetdev_list);

int inetdev_register_dev(struct net_device *dev) {
	int ret;
	struct in_device *in_dev;

	if (dev == NULL) {
		return -EINVAL;
	}

	in_dev = (struct in_device *)pool_alloc(&inetdev_pool);
	if (in_dev == NULL) {
		return -ENOMEM;
	}

	ret = netdev_register(dev);
	if (ret != 0) {
		pool_free(&inetdev_pool, in_dev);
		return ret;
	}

	memset(in_dev, 0, sizeof *in_dev);
	dlist_head_init(&in_dev->lnk);
	in_dev->dev = dev;

	dlist_add_prev_entry(in_dev, &inetdev_list, lnk);

	return 0;
}

int inetdev_unregister_dev(struct net_device *dev) {
	int ret;
	struct in_device *in_dev;

	if (dev == NULL) {
		return -EINVAL;
	}

	in_dev = inetdev_get_by_dev(dev);
	if (in_dev == NULL) {
		return -ESRCH;
	}

	ret = netdev_unregister(dev);
	if (ret != 0) {
		pool_free(&inetdev_pool, in_dev);
		return ret;
	}

	dlist_del_init_entry(in_dev, lnk);
	pool_free(&inetdev_pool, in_dev);

	return 0;
}

#include <util/log.h>
struct in_device * inetdev_get_by_name(const char *name) {
	return inetdev_get_by_name_netns(name, get_net_ns());
}

struct in_device * inetdev_get_by_name_netns(const char *name,
					net_namespace_p net_ns) {
	struct in_device *in_dev;

	if (name == NULL) {
		return NULL; /* error: invalid name or net namespace */
	}

	dlist_foreach_entry(in_dev, &inetdev_list, lnk) {
		assert(in_dev->dev != NULL);
		//if ((net_ns == in_dev->dev->net_ns) &&
		if (cmp_net_ns(net_ns, in_dev->dev->net_ns) &&
			(strcmp(name, &in_dev->dev->name[0]) == 0)) {
			return in_dev;
		}
	}

	return NULL; /* error: no such device */
}

struct in_device * inetdev_get_by_dev(struct net_device *dev) {
	if (dev == NULL) {
		return NULL; /* error: invalid argument */
	}

	return inetdev_get_by_name_netns(&dev->name[0], dev->net_ns);
}

struct in_device * inetdev_get_by_addr(in_addr_t addr) {
	struct in_device *in_dev;

	dlist_foreach_entry(in_dev, &inetdev_list, lnk) {
		if (in_dev->ifa_address == addr) {
			return in_dev;
		}
	}

	return NULL; /* error: no such device */
}

struct in_device *inetdev_get_loopback_dev_netns(net_namespace_p net_ns) {
	return inetdev_get_by_name_netns("lo", net_ns);
}

struct in_device * inetdev_get_loopback_dev(void) {
	return inetdev_get_by_name_netns("lo", get_net_ns());
}

int inetdev_set_addr(struct in_device *in_dev, in_addr_t addr) {
	if (in_dev == NULL) {
		return -EINVAL;
	}

	in_dev->ifa_address = addr;

	return 0;
}

int inetdev_set_mask(struct in_device *in_dev, in_addr_t mask) {
	if (in_dev == NULL) {
		return -EINVAL;
	}

	in_dev->ifa_mask = mask;
	in_dev->ifa_broadcast = in_dev->ifa_address | ~in_dev->ifa_mask;

	return 0;
}

int inetdev_set_bcast(struct in_device *in_dev, in_addr_t bcast) {
	if (in_dev == NULL) {
		return -EINVAL;
	}

	in_dev->ifa_broadcast = bcast;

	return 0;
}

in_addr_t inetdev_get_addr(struct in_device *in_dev) {
	if (in_dev == NULL) {
		return 0; /* error: invalid argument */
	}

	return in_dev->ifa_address;
}

struct in_device * inetdev_get_next_net_ns(struct in_device *in_dev,
					   net_namespace_p netns) {
	if (in_dev == NULL) {
		return NULL; /* error: invalid argument */
	}

	while (1) {
		if (in_dev == dlist_last_entry(&inetdev_list,
					struct in_device, lnk)) {
			return NULL; /* error: there are no more devices */
		}
		in_dev = dlist_entry(in_dev->lnk.next, struct in_device, lnk);
		if (!cmp_net_ns(in_dev->dev->net_ns, netns)) {
			continue;
		} else {
			return in_dev;
		}
	}

	return NULL;
}

struct in_device * inetdev_get_first_net_ns(net_namespace_p netns) {
	struct in_device * in_dev;
	in_dev = dlist_next_entry_or_null(&inetdev_list, struct in_device, lnk);

	if (!cmp_net_ns(in_dev->dev->net_ns, netns)) {
		return inetdev_get_next_net_ns(in_dev, netns);
	}

	return in_dev;
}

struct in_device * inetdev_get_first(void) {
	return inetdev_get_first_net_ns(get_net_ns());
}

struct in_device * inetdev_get_next(struct in_device *in_dev) {
	return inetdev_get_next_net_ns(in_dev, get_net_ns());
}

struct in_device * inetdev_get_first_all(void) {
	return dlist_next_entry_or_null(&inetdev_list, struct in_device, lnk);
}

struct in_device * inetdev_get_next_all(struct in_device *in_dev) {
	if (in_dev == NULL) {
		return NULL; /* error: invalid argument */
	}

	if (in_dev == dlist_last_entry(&inetdev_list,
				struct in_device, lnk)) {
		return NULL; /* error: there are no more devices */
	}

	return dlist_entry(in_dev->lnk.next, struct in_device, lnk);
}

#include <linux/in.h>
int ip_is_local(in_addr_t addr, int opts) {
	struct in_device *in_dev;

	if (ipv4_is_loopback(addr)) {
		return true;
	}

	if ((opts & IP_LOCAL_BROADCAST) && ipv4_is_broadcast(addr)) {
		return true;
	}

	if ((opts & IP_LOCAL_MULTICAST) && ipv4_is_multicast(addr)) {
		return true;
	}

	dlist_foreach_entry(in_dev, &inetdev_list, lnk) {
		if (in_dev->ifa_address == addr) {
			return true;
		}
		if ((opts & IP_LOCAL_BROADCAST) && (in_dev->ifa_broadcast == addr)) {
			return true;
		}
	}

	return false;
}

int ip_is_local_net_ns(in_addr_t addr, int opts,
		net_namespace_p net_ns) {
	struct in_device *in_dev;

	if (ipv4_is_loopback(addr)) {
		return true;
	}

	if ((opts & IP_LOCAL_BROADCAST) && ipv4_is_broadcast(addr)) {
		return true;
	}

	if ((opts & IP_LOCAL_MULTICAST) && ipv4_is_multicast(addr)) {
		return true;
	}

	dlist_foreach_entry(in_dev, &inetdev_list, lnk) {
		if (!cmp_net_ns(in_dev->dev->net_ns, net_ns))
			continue;
		if (in_dev->ifa_address == addr) {
			return true;
		}
		if ((opts & IP_LOCAL_BROADCAST) && (in_dev->ifa_broadcast == addr)) {
			return true;
		}
	}

	return false;
}
