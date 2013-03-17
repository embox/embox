/**
 * @file
 * @brief IP device support routines.
 * @details realizing interface if_device struct (interface device)
 *
 * @date 18.07.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <mem/misc/pool.h>
#include <framework/mod/options.h>
#include <util/list.h>

#define MODOPS_AMOUNT_INTERFACE OPTION_GET(NUMBER, amount_interface)

POOL_DEF(inetdev_pool, struct in_device, MODOPS_AMOUNT_INTERFACE);
static LIST_DEF(inetdev_list);

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
	list_link_init(&in_dev->lnk);
	in_dev->dev = dev;

	list_add_last_element(in_dev, &inetdev_list, lnk);

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

	list_unlink_element(in_dev, lnk);
	pool_free(&inetdev_pool, in_dev);

	return 0;
}

struct in_device * inetdev_get_by_name(const char *name) {
	struct in_device *in_dev;

	if (name == NULL) {
		return NULL;
	}

	list_foreach(in_dev, &inetdev_list, lnk) {
		if (strcmp(name, &in_dev->dev->name[0]) == 0) {
			return in_dev;
		}
	}

	return NULL;
}

struct in_device * inetdev_get_by_dev(struct net_device *dev) {
	if (dev == NULL) {
		return NULL;
	}

	return inetdev_get_by_name(&dev->name[0]);
}

struct in_device * inetdev_get_by_addr(in_addr_t addr) {
	struct in_device *in_dev;

	list_foreach(in_dev, &inetdev_list, lnk) {
		if (in_dev->ifa_address == addr) {
			return in_dev;
		}
	}

	return NULL;
}

struct in_device * inetdev_get_loopback_dev(void) {
	return inetdev_get_by_name("lo");
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

int inetdev_set_macaddr(struct in_device *in_dev, const unsigned char *macaddr) {
	struct net_device *dev;

	if ((in_dev == NULL) || (macaddr == NULL)) {
		return -EINVAL;
	}

	dev = in_dev->dev;
	assert(dev != NULL);

	if (dev->netdev_ops->ndo_set_mac_address == NULL) {
		return -ENOSUPP;
	}

	return dev->netdev_ops->ndo_set_mac_address(dev, (void *)macaddr);
}

in_addr_t inetdev_get_addr(struct in_device *in_dev) {
	return in_dev != NULL ? in_dev->ifa_address : 0;
}

struct in_device * inetdev_get_first(void) {
	return list_first_element(&inetdev_list, struct in_device, lnk);
}

struct in_device * inetdev_get_next(struct in_device *in_dev) {
	if (in_dev == NULL) {
		return NULL;
	}

	if (in_dev == list_last_element(&inetdev_list,
				struct in_device, lnk)) {
		return NULL;
	}

	return list_element(in_dev->lnk.next, struct in_device, lnk);
}



#include <arpa/inet.h>
#include <linux/in.h>
bool ip_is_local(in_addr_t addr, bool check_broadcast, bool check_multicast) {
	struct in_device *in_dev;
	in_addr_t indev_anycast, indev_subnet, indev_broadcast;

	if (check_broadcast) {
		if (addr == htonl(INADDR_BROADCAST)) {
			return true;
		}

#if 0	/* Obsoleted broadcast */
		/* RFC 919/922 Section 7 - deprecated
		 * RFC 1122. Section 3.3.6 - obsoleted
		 * Please, don't confuse external and internal conventions!
		 */
		if (addr == htonl(INADDR_ANY))
			return true;
#endif
	}

	if (ipv4_is_multicast(addr) && check_multicast) {
		return true;
	}

	if (ipv4_is_loopback(addr)) {
		return true;
	}

	list_foreach(in_dev, &inetdev_list, lnk) {
		if (in_dev->ifa_address == addr) {
			return true;
		}
		if (check_broadcast) {
			indev_anycast = in_dev->ifa_anycast; /* It MUST be init to INADDR_BROADCAST by default */
			indev_subnet = in_dev->ifa_address & in_dev->ifa_mask; /* Obsoleted broadcast, not used any more */
			indev_broadcast = in_dev->ifa_broadcast; /* If someone inited it to something strange */
			if ((addr == indev_anycast) || (addr == indev_subnet) || (addr == indev_broadcast)) {
				return true;
			}
		}
	}

	return false;
}
