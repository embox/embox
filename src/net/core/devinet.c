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

EMBOX_UNIT_INIT(devinet_init);

#define IFDEV_CBINFO_QUANTITY 8

typedef struct callback_info {
	unsigned short      type;
	ETH_LISTEN_CALLBACK func;
} callback_info_t;

typedef struct inetdev_info {
	in_device_t   dev;
	int is_busy;
	callback_info_t cb_info[IFDEV_CBINFO_QUANTITY];
} inetdev_info_t;

static inetdev_info_t ifs_info[CONFIG_NET_INTERFACES_QUANTITY];

static inetdev_info_t * find_ifdev_info_entry(in_device_t *in_dev) {
	size_t i;

	for (i = 0; i < ARRAY_SIZE(ifs_info); ++i) {
		if (ifs_info[i].is_busy && (&ifs_info[i].dev == in_dev)) {
			return &ifs_info[i];
		}
	}

	return NULL;
}

static in_device_t * get_free_handler(void) {
	size_t i;

	for (i = 0; i < ARRAY_SIZE(ifs_info); i++) {
		if (!ifs_info[i].is_busy) {
			ifs_info[i].is_busy = 1;
			return &ifs_info[i].dev;
		}
	}

	return NULL;
}

static void free_handler(in_device_t *if_handler) {
	member_cast_out(if_handler, inetdev_info_t, dev)->is_busy = 0;
}


#if 0
static int free_handler(in_device_t * handler) {
	size_t i;
	for (i = 0; i < CONFIG_NET_INTERFACES_QUANTITY; i++) {
		if ((1 == ifs_info[i].is_busy) &&
			(&ifs_info[i].dev == handler)) {
			ifs_info[i].is_busy = 0;
			return 0;
		}
	}
	return -1;
}
#endif

static int alloc_callback(in_device_t *in_dev, unsigned int type,
				ETH_LISTEN_CALLBACK callback) {
	size_t i;
	inetdev_info_t *ifdev_info;

	ifdev_info = find_ifdev_info_entry(in_dev);
	for (i = 0; i < ARRAY_SIZE(ifdev_info->cb_info); i++) {
		if (ifdev_info->cb_info[i].func == NULL) {
			ifdev_info->cb_info[i].type = type;
			ifdev_info->cb_info[i].func = callback;
			return ENOERR;
		}
	}

	return -ENOMEM;
}
#if 0
static int free_callback(in_device_t *in_dev, ETH_LISTEN_CALLBACK callback) {
	size_t i;
	INETDEV_INFO *ifdev_info = find_ifdev_info_entry(in_dev);
	for (i = 0; i < ARRAY_SIZE(ifdev_info->cb_info); i++) {
		if (callback == ifdev_info->cb_info[i].func) {
			ifdev_info->cb_info[i].func    = NULL;
			return i;
		}
	}
	return -1;
}
#endif

in_device_t * in_dev_get(const net_device_t *dev) {
	return inet_dev_find_by_name(dev->name);
}

int inet_dev_listen(in_device_t *dev, unsigned short type,
			ETH_LISTEN_CALLBACK callback) {
	if ((dev == NULL) || (callback == NULL)) {
		return -EINVAL;
 	}
	return alloc_callback(dev, type, callback);
}

net_device_t *ip_dev_find(in_addr_t addr) {
	size_t i;

	for (i = 0; i < CONFIG_NET_INTERFACES_QUANTITY; i++) {
		if (ifs_info[i].dev.ifa_address == addr) {
			return ifs_info[i].dev.dev;
		}
	}
	return NULL;
}

in_device_t * inet_dev_find_by_name(const char *if_name) {
	size_t i;

	for (i = 0; i < ARRAY_SIZE(ifs_info); i++) {
		if (0 == strncmp(if_name, ifs_info[i].dev.dev->name, IFNAMSIZ)) {
			return &ifs_info[i].dev;
		}
	}

	return NULL;
}

int inet_dev_set_interface(const char *name, in_addr_t ipaddr, in_addr_t mask,
				const unsigned char *macaddr) {
	size_t i;

	for (i = 0; i < ARRAY_SIZE(ifs_info); i++) {
		if (0 != strncmp(name, ifs_info[i].dev.dev->name,
				ARRAY_SIZE(ifs_info[i].dev.dev->name))) {
			continue;
		}
		if ((-1 == inet_dev_set_ipaddr(&ifs_info[i].dev, ipaddr)) ||
		    (-1 == inet_dev_set_mask(&ifs_info[i].dev, mask)) ||
		    (-1 == inet_dev_set_macaddr(&ifs_info[i].dev, macaddr))) {
			return -EINVAL;
		}
		return ENOERR;
	}
	return -ENOMEM;
}

int inet_dev_set_ipaddr(in_device_t *in_dev, const in_addr_t ipaddr) {
	if (in_dev == NULL) {
		return -EINVAL;
	}

	in_dev->ifa_address = ipaddr;

	return ENOERR;
}

int inet_dev_set_mask(in_device_t *in_dev, const in_addr_t mask) {
	if (NULL == in_dev) {
		return -EINVAL;
	}

	in_dev->ifa_mask = mask;
	in_dev->ifa_broadcast = in_dev->ifa_address | ~in_dev->ifa_mask;

	return ENOERR;
}

int inet_dev_set_macaddr(in_device_t *in_dev, const unsigned char *macaddr) {
	net_device_t *dev;

	if ((in_dev == NULL) || (macaddr ==  NULL)) {
		return -EINVAL;
	}

	dev = in_dev->dev;
	if (NULL == dev) {
		return -EINVAL;
	}

	if (dev->netdev_ops->ndo_set_mac_address == NULL) {
		/* driver doesn't support setting mac address */
		return -ENOSUPP;
	}

	return dev->netdev_ops->ndo_set_mac_address(dev, (void *) macaddr);
}

in_addr_t inet_dev_get_ipaddr(in_device_t *in_dev) {
	return (in_dev == NULL) ? 0 : in_dev->ifa_address;
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

in_device_t * inet_dev_get_fist_used(size_t *iter) {
	for (*iter = 0; *iter < ARRAY_SIZE(ifs_info); ++(*iter)) {
		if (ifs_info[*iter].is_busy) {
			return &ifs_info[*iter].dev;
		}
	}

	return NULL;
}

in_device_t * inet_dev_get_next_used(size_t *iter) {
	while (++(*iter) < ARRAY_SIZE(ifs_info)) {
 		if (ifs_info[*iter].is_busy) {
			return &ifs_info[*iter].dev;
		}
	}

	return NULL;
}

/*TODO follow functions either have different interface or move to another place
     move into ifconfig -- sikmir*/
int ifdev_up(const char *iname) {
	in_device_t *ifhandler;

	ifhandler = get_free_handler();
	if (ifhandler == NULL) {
		LOG_ERROR("ifdev up: can't find find free handler\n");
		return -ENOMEM;
	}
	ifhandler->dev = netdev_get_by_name(iname);
	if (ifhandler->dev == NULL) {
		LOG_ERROR("ifdev up: can't find net_device with name %s\n", iname);
		free_handler(ifhandler);
		return -ENOENT;
	}
	return dev_open(ifhandler->dev);
}

int ifdev_down(const char *iname) {
	in_device_t *in_dev;

	in_dev = inet_dev_find_by_name(iname);
	if (in_dev == NULL) {
		LOG_ERROR("ifdev down: can't find ifdev with name %s\n", iname);
		return -ENOENT;
	}
	if (in_dev->dev == NULL) {
		LOG_ERROR("ifdev down: can't find net_device with name %s\n", iname);
		return -ENOENT;
	}
	return dev_close(in_dev->dev);
}

int ifdev_set_debug_mode(const char *iname, unsigned int type_filter) {
	return 0;
}

int ifdev_clear_debug_mode(const char *iname) {
	return 0;
}

static int devinet_init(void) {
	int i;

	for (i = 0; i < CONFIG_NET_INTERFACES_QUANTITY; ++i) {;
		ifs_info[i].is_busy = 0;
	}

	return ENOERR;
}
