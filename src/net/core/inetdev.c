/**
 * @file
 * @brief IP device support routines.
 * @details realizing interface if_device struct (interface device)
 *
 * @date 18.07.09
 * @author Anton Bondarev
 */
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <util/array.h>
#include <util/member.h>
#include <mem/misc/pool.h>

#include <net/inetdevice.h>
#include <net/in.h>
#include <framework/mod/options.h>

#include <lib/list.h>

#include <err.h>

struct callback_info {
	struct list_head lnk;
	unsigned short type;
	devinet_callback_t func;
};

struct inetdev_info {
	struct list_head lnk;
	struct in_device in_dev;
	struct list_head cb_info_list;
};

POOL_DEF(indev_info_pool, struct inetdev_info, OPTION_GET(NUMBER,net_interfaces_quantity));
POOL_DEF(callback_info_pool, struct callback_info, OPTION_GET(NUMBER,net_callback_quantity));
static LIST_HEAD(indev_info_list);

static struct inetdev_info * find_indev_info_entry(struct in_device *in_dev) {
	struct inetdev_info *indev_info;
	struct list_head *tmp;

	list_for_each(tmp, &indev_info_list) {
		indev_info = member_cast_out(tmp, struct inetdev_info, lnk);
		if (&indev_info->in_dev == in_dev) {
			return indev_info;
		}
	}

	return NULL;
}

static int alloc_callback(struct in_device *in_dev, unsigned int type,
				devinet_callback_t callback) {
	struct inetdev_info *indev_info;
	struct callback_info *cb_info;

	assert(in_dev != NULL);
	assert(callback != NULL);

	indev_info = find_indev_info_entry(in_dev);
	if (indev_info == NULL) {
		return -ENOENT;
	}

	cb_info = (struct callback_info *)pool_alloc(&callback_info_pool);
	if (cb_info == NULL) {
		return -ENOMEM;
	}

	cb_info->type = type;
	cb_info->func = callback;

	list_add_tail(&cb_info->lnk, &indev_info->cb_info_list);

	return ENOERR;
}

struct in_device * in_dev_get(struct net_device *dev) {
	assert(dev != NULL);
	return inet_dev_find_by_name(dev->name);
}

int inet_dev_listen(struct in_device *in_dev, unsigned short type,
		devinet_callback_t callback) {
	assert(in_dev != NULL);
	assert(callback != NULL);
	return alloc_callback(in_dev, type, callback);
}

struct net_device * ip_dev_find(in_addr_t addr) {
	struct inetdev_info *indev_info;
	struct list_head *tmp;

	list_for_each(tmp, &indev_info_list) {
		indev_info = member_cast_out(tmp, struct inetdev_info, lnk);
		if (indev_info->in_dev.ifa_address == addr) {
			return indev_info->in_dev.dev;
		}
	}

	return NULL;
}

bool ip_is_local(in_addr_t addr, bool check_broadcast, bool check_multicast) {
	struct inetdev_info *indev_info;
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

	list_for_each_entry(indev_info, &indev_info_list, lnk) {
		if (indev_info->in_dev.ifa_address == addr) {
			return true;
		}
		if (check_broadcast) {
			in_dev = &indev_info->in_dev;
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

struct in_device * inet_dev_find_by_name(const char *if_name) {
	struct inetdev_info *indev_info;
	struct list_head *tmp;

	assert(if_name != NULL);

	list_for_each(tmp, &indev_info_list) {
		indev_info = member_cast_out(tmp, struct inetdev_info, lnk);
		assert(indev_info->in_dev.dev != NULL);
		if (strncmp(if_name, indev_info->in_dev.dev->name,
				ARRAY_SIZE(indev_info->in_dev.dev->name)) == 0) {
			return &indev_info->in_dev;
		}
	}

	return NULL;
}

struct net_device * inet_get_loopback_dev(void) {
	assert(inet_dev_find_by_name("lo") != NULL);
	return inet_dev_find_by_name("lo")->dev;
}

int inet_dev_set_interface(const char *if_name, in_addr_t ipaddr,
		in_addr_t mask, const unsigned char *macaddr) {
	int res;
	struct inetdev_info *indev_info;
	struct list_head *tmp;

	assert(if_name != NULL);
	assert(macaddr != NULL);

	list_for_each(tmp, &indev_info_list) {
		indev_info = member_cast_out(tmp, struct inetdev_info, lnk);
		if (strncmp(if_name, indev_info->in_dev.dev->name,
				ARRAY_SIZE(indev_info->in_dev.dev->name)) == 0) {
			res = inet_dev_set_ipaddr(&indev_info->in_dev, ipaddr);
			if (res < 0) {
				return res;
			}
			res = inet_dev_set_mask(&indev_info->in_dev, mask);
			if (res < 0) {
				return res;
			}
			res = inet_dev_set_macaddr(&indev_info->in_dev, macaddr);
			if (res < 0) {
				return res;
			}
			return ENOERR;
		}
	}

	return -ENOENT;
}

int inet_dev_set_ipaddr(struct in_device *in_dev, in_addr_t ipaddr) {
	assert(in_dev != NULL);

	in_dev->ifa_address = ipaddr;

	return ENOERR;
}

int inet_dev_set_mask(struct in_device *in_dev, in_addr_t mask) {
	assert(in_dev != NULL);

	in_dev->ifa_mask = mask;
	in_dev->ifa_broadcast = in_dev->ifa_address | ~in_dev->ifa_mask;

	return ENOERR;
}

int inet_dev_set_macaddr(struct in_device *in_dev, const unsigned char *macaddr) {
	struct net_device *dev;

	assert(in_dev != NULL);
	assert(macaddr != NULL);

	dev = in_dev->dev;
	assert(dev != NULL);

	if (dev->netdev_ops->ndo_set_mac_address == NULL) {
		/* driver doesn't support setting mac address */
		return -ENOSUPP;
	}

	return dev->netdev_ops->ndo_set_mac_address(dev, (void *)macaddr);
}

in_addr_t inet_dev_get_ipaddr(struct in_device *in_dev) {
	assert(in_dev != NULL);
	return in_dev->ifa_address;
}

int inet_dev_add_dev(struct net_device *dev) {
	int res;
	struct inetdev_info *indev_info;

	assert(dev != NULL);

	indev_info = (struct inetdev_info *)pool_alloc(&indev_info_pool);
	if (indev_info == NULL) {
		LOG_ERROR("ifdev up: can't find find free handler\n");
		return -ENOMEM;
	}

	res = netdev_open(dev);
	if (res < 0) {
		LOG_ERROR("ifdev up: can't open device with name %s\n", dev->name);
		pool_free(&indev_info_pool, indev_info);
		return res;
	}

	indev_info->in_dev.dev = dev;
	INIT_LIST_HEAD(&indev_info->cb_info_list);
	list_add_tail(&indev_info->lnk, &indev_info_list);

	return ENOERR;
}

int inet_dev_remove_dev(struct in_device *in_dev) {
	int res;
	struct callback_info *cb_info;
	struct inetdev_info *indev_info;
	struct list_head *tmp, *safe;

	assert(in_dev != NULL);

	indev_info = member_cast_out(in_dev, struct inetdev_info, in_dev);
	list_del(&indev_info->lnk);

	assert(indev_info->in_dev.dev != NULL);

	res = netdev_close(indev_info->in_dev.dev);
	if (res < 0) {
		LOG_ERROR("ifdev down: can't close device with name %s\n", in_dev->dev->name);
		list_add_tail(&indev_info->lnk, &indev_info_list);
		return res;
	}

	list_for_each_safe(tmp, safe, &indev_info->cb_info_list) {
		cb_info = member_cast_out(tmp, struct callback_info, lnk);
		list_del(&cb_info->lnk);
		pool_free(&callback_info_pool, cb_info);
	}

	pool_free(&indev_info_pool, indev_info);

	return ENOERR;
}


struct in_device * inet_dev_get_first_used(void) {
	struct inetdev_info *indev_info;

	if (list_empty(&indev_info_list)) {
		return NULL;
	}

	indev_info = member_cast_out(indev_info_list.next, struct inetdev_info, lnk);

	return &indev_info->in_dev;
}

struct in_device * inet_dev_get_next_used(struct in_device *in_dev) {
	struct inetdev_info *indev_info;

	assert(in_dev != NULL);

	indev_info = member_cast_out(in_dev, struct inetdev_info, in_dev);

	indev_info = member_cast_out(indev_info->lnk.next, struct inetdev_info, lnk);

	return (&indev_info->lnk == &indev_info_list) ? NULL : &indev_info->in_dev;
}
