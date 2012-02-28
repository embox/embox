/**
 * @file
 * @brief IP device support routines.
 * @details realizing interface if_device struct (interface device)
 *
 * @date 18.07.09
 * @author Anton Bondarev
 */

#include <embox/unit.h>
#include <string.h>
#include <net/inetdevice.h>
#include <linux/init.h>
#include <util/array.h>
#include <err.h>
#include <errno.h>
#include <util/member.h>
#include <mem/misc/pool.h>
#include <lib/list.h>

EMBOX_UNIT_INIT(devinet_init);

struct callback_info {
	struct list_head lnk;
	unsigned short type;
	ETH_LISTEN_CALLBACK func;
};

struct inetdev_info {
	struct list_head lnk;
	struct in_device in_dev;
	struct list_head cb_info_list;
};

POOL_DEF(indev_info_pool, struct inetdev_info, CONFIG_NET_INTERFACES_QUANTITY);
POOL_DEF(callback_info_pool, struct callback_info, CONFIG_NET_CALLBACK_QUANTITY);
static struct list_head indev_info_list;

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
				ETH_LISTEN_CALLBACK callback) {
	struct inetdev_info *indev_info;
	struct callback_info *cb_info;

	assert(in_dev != NULL);

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

/* static */int free_callback(struct in_device *in_dev, ETH_LISTEN_CALLBACK callback) {
	struct inetdev_info *indev_info;
	struct callback_info *cb_info;
	struct list_head *tmp;

	assert(in_dev != NULL);

	indev_info = find_indev_info_entry(in_dev);
	if (indev_info == NULL) {
		return -ENOENT;
	}

	list_for_each(tmp, &indev_info->cb_info_list) {
		cb_info = member_cast_out(tmp, struct callback_info, lnk);
		if (cb_info->func == callback) {
			list_del(&cb_info->lnk);
			pool_free(&callback_info_pool, cb_info);
			return ENOERR;
		}
	}

	return -ENOENT;
}

struct in_device * in_dev_get(struct net_device *dev) {
	assert(dev != NULL);
	return inet_dev_find_by_name(dev->name);
}

int inet_dev_listen(struct in_device *in_dev, unsigned short type,
			ETH_LISTEN_CALLBACK callback) {
	if ((in_dev == NULL) || (callback == NULL)) {
		return -EINVAL;
 	}
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

struct in_device * inet_dev_find_by_name(const char *if_name) {
	struct inetdev_info *indev_info;
	struct list_head *tmp;

	if (if_name == NULL) {
		return NULL;
	}

	list_for_each(tmp, &indev_info_list) {
		indev_info = member_cast_out(tmp, struct inetdev_info, lnk);
		if (strncmp(if_name, indev_info->in_dev.dev->name,
				ARRAY_SIZE(indev_info->in_dev.dev->name)) == 0) {
			return &indev_info->in_dev;
		}
	}

	return NULL;
}

struct net_device *inet_get_loopback_dev(void){
	return inet_dev_find_by_name("lo")->dev;
}

int inet_dev_set_interface(const char *if_name, in_addr_t ipaddr,
		in_addr_t mask, const unsigned char *macaddr) {
	int res;
	struct inetdev_info *indev_info;
	struct list_head *tmp;

	if ((if_name == NULL) || (macaddr == NULL)) {
		return -EINVAL;
	}

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
	if (in_dev == NULL) {
		return -EINVAL;
	}

	in_dev->ifa_address = ipaddr;

	return ENOERR;
}

int inet_dev_set_mask(struct in_device *in_dev, in_addr_t mask) {
	if (in_dev == NULL) {
		return -EINVAL;
	}

	in_dev->ifa_mask = mask;
	in_dev->ifa_broadcast = in_dev->ifa_address | ~in_dev->ifa_mask;

	return ENOERR;
}

int inet_dev_set_macaddr(struct in_device *in_dev, const unsigned char *macaddr) {
	struct net_device *dev;

	if ((in_dev == NULL) || (macaddr ==  NULL)) {
		return -EINVAL;
	}

	dev = in_dev->dev;

	assert(dev != NULL);

	if (dev->netdev_ops->ndo_set_mac_address == NULL) {
		/* driver doesn't support setting mac address */
		return -ENOSUPP;
	}

	return dev->netdev_ops->ndo_set_mac_address(dev, (void *)macaddr);
}

in_addr_t inet_dev_get_ipaddr(struct in_device *in_dev) {
	return (in_dev == NULL) ? 0 : in_dev->ifa_address;
}

int inet_dev_add_dev(struct net_device *dev) {
	int res;
	struct inetdev_info *indev_info;

	indev_info = (struct inetdev_info *)pool_alloc(&indev_info_pool);
	if (indev_info == NULL) {
		LOG_ERROR("ifdev up: can't find find free handler\n");
		return -ENOMEM;
	}

	res = dev_open(dev);
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

	indev_info = member_cast_out(in_dev, struct inetdev_info, in_dev);

	list_del(&indev_info->lnk);

	assert(indev_info->in_dev.dev != NULL);

	res = dev_close(indev_info->in_dev.dev);
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

#if 0
/**
 * this function is called by device layer from function "netif_rx"
 * before proto parse
 * socket must call "ifdev_listen" if it wants ifdev to have to
 * receive raw socket (for tcpdump for example)
 */
void ifdev_rx_callback(sk_buff_t *pack) {
	size_t i;
	/* if there are some callback handlers for packet's protocol */
	in_device_t *in_dev = (in_device_t *) pack->ifdev;
	if (NULL == dev)
		return;

	INETDEV_INFO *ifdev_info = find_ifdev_info_entry(in_dev);

	for (i = 0; i < ARRAY_SIZE(ifdev_info->cb_info); i++) {
		if (NULL != ifdev_info->cb_info[i].func) {
			if ((NET_TYPE_ALL_PROTOCOL == ifdev_info->cb_info[i].type)
					|| (ifdev_info->cb_info[i].type == pack->protocol)) {
				/* todo may be copy pack for different protocols*/
				ifdev_info->cb_info[i].func(pack);
			}
		}
	}
}
/**
 * this function is called by device layer from function "eth_send"
 * socket must call "ifdev_listen" if it wants ifdev to have to
 * receive raw socket (for tcpdump for example)
 */
void ifdev_tx_callback(sk_buff_t *pack) {
}
#endif

struct in_device * inet_dev_get_fist_used(void) {
	struct inetdev_info *indev_info;

	if (list_empty(&indev_info_list)) {
		return NULL;
	}

	indev_info = member_cast_out(indev_info_list.next, struct inetdev_info, lnk);

	return &indev_info->in_dev;
}

struct in_device * inet_dev_get_next_used(struct in_device *in_dev) {
	struct inetdev_info *indev_info;

	indev_info = member_cast_out(in_dev, struct inetdev_info, in_dev);

	indev_info = member_cast_out(indev_info->lnk.next, struct inetdev_info, lnk);

	return (&indev_info->lnk == &indev_info_list) ? NULL : &indev_info->in_dev;
}

static int devinet_init(void) {
	INIT_LIST_HEAD(&indev_info_list);
	return ENOERR;
}
