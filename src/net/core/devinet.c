/**
 * @file devinet.c
 *
 * @brief IP device support routines.
 * @details realizing interface if_device struct (interface device)
 * @date 18.07.2009
 * @author Anton Bondarev
 */
#include "string.h"
#include "common.h"
#include "net/net.h"
#include "net/inetdevice.h"
#include "net/netdevice.h"
#include "net/skbuff.h"

#define IFDEV_CBINFO_QUANTITY 8

typedef struct _CALLBACK_INFO {
	unsigned short      type;
	ETH_LISTEN_CALLBACK func;
} CALLBACK_INFO;

typedef struct _INETDEV_INFO {
	in_device_t   dev;
	int           is_busy;
	CALLBACK_INFO cb_info[IFDEV_CBINFO_QUANTITY];
} INETDEV_INFO;

static INETDEV_INFO ifs_info[NET_INTERFACES_QUANTITY];

INETDEV_INFO *find_ifdev_info_entry(in_device_t *in_dev) {
	int i;
	for (i = 0; i < array_len(ifs_info); i ++) {
		if (&ifs_info[i].dev == in_dev) {
			return &ifs_info[i];
		}
	}
	return NULL;
}

static in_device_t *find_free_handler() {
	int i;
	for(i = 0; i < NET_INTERFACES_QUANTITY; i ++) {
    		if (0 == ifs_info[i].is_busy) {
        		ifs_info[i].is_busy = 1;
        		return &ifs_info[i].dev;
    		}
	}
	return NULL;
}

static int free_handler(in_device_t * handler) {
	int i;
	for(i = 0; i < NET_INTERFACES_QUANTITY; i ++) {
    		if ((1 == ifs_info[i].is_busy) &&
    		    (&ifs_info[i].dev == handler)) {
        		ifs_info[i].is_busy = 0;
        		return 0;
    		}
	}
	return -1;
}

static int alloc_callback(in_device_t *in_dev, unsigned int type,
                          ETH_LISTEN_CALLBACK callback) {
	int i;
	INETDEV_INFO *ifdev_info = find_ifdev_info_entry(in_dev);
	for (i = 0; i < array_len(ifdev_info->cb_info); i++) {
    		if (0 == ifdev_info->cb_info[i].func) {
        		ifdev_info->cb_info[i].type    = type;
        		ifdev_info->cb_info[i].func    = callback;
        		return i;
    		}
	}
	return -1;
}

static int free_callback(in_device_t *in_dev, ETH_LISTEN_CALLBACK callback) {
	int i;
	INETDEV_INFO *ifdev_info = find_ifdev_info_entry(in_dev);
	for (i = 0; i < array_len(ifdev_info->cb_info); i++) {
    		if (callback == ifdev_info->cb_info[i].func) {
        		ifdev_info->cb_info[i].func    = NULL;
        		return i;
    		}
	}
	return -1;
}

int inet_dev_init() {
	return 0;
}

struct net_device *inet_dev_get_netdevice(void *handler) {
	in_device_t *dev = (in_device_t *) handler;
	if (NULL == dev) {
    		LOG_ERROR("handler is NULL\n");
    		return NULL;
	}
        return dev->net_dev;
}

int inet_dev_listen(void *handler, unsigned short type,
                 ETH_LISTEN_CALLBACK callback) {
	in_device_t *dev = (in_device_t *) handler;
	if (NULL == dev) {
    		return -1;
	}
	return alloc_callback(dev, type, callback);
}

struct net_device *ip_dev_find(in_addr_t addr) {
	int i;
	for (i = 0; i < NET_INTERFACES_QUANTITY; i++) {
    		if (ifs_info[i].dev.ifa_address == addr) {
        		return ifs_info[i].dev.net_dev;
    		}
	}
	return NULL;
}

void *inet_dev_find_by_name(const char *if_name) {
	int i;
	for (i = 0; i < NET_INTERFACES_QUANTITY; i++) {
    		LOG_DEBUG("ifname %s, net_dev 0x%X\n", if_name,
    					    ifs_info[i].dev.net_dev);
    		if (0 == strncmp(if_name, ifs_info[i].dev.net_dev->name,
            	    sizeof(ifs_info[i].dev.net_dev->name))) {
        		return &ifs_info[i].dev;
    		}
	}
	return NULL;
}

int inet_dev_set_interface(char *name, in_addr_t ipaddr, in_addr_t mask,
							unsigned char *macaddr) {
	int i;
	for (i = 0; i < NET_INTERFACES_QUANTITY; i++) {
    		if (0 == strncmp(name, ifs_info[i].dev.net_dev->name,
    		    array_len(ifs_info[i].dev.net_dev->name))) {
        		if((-1 == inet_dev_set_ipaddr(&ifs_info[i].dev, ipaddr))||
        		   (-1 == inet_dev_set_mask(&ifs_info[i].dev, mask)) ||
            		   (-1 == inet_dev_set_macaddr(&ifs_info[i].dev, macaddr))) {
        			return -1;
    			}
        		return i;
    		}
	}
	return -1;
}

int inet_dev_set_ipaddr(void *ifdev, const in_addr_t ipaddr) {
	if (NULL == ifdev) {
    		return -1;
	}
	in_device_t *dev = (in_device_t *) ifdev;
	dev->ifa_address = ipaddr;
	return 0;
}

int inet_dev_set_mask(void *ifdev, const in_addr_t mask) {
	if (NULL == ifdev) {
        	return -1;
	}
	in_device_t *dev = (in_device_t *) ifdev;
	dev->ifa_mask = mask;
	dev->ifa_broadcast = dev->ifa_address | ~dev->ifa_mask;
	return 0;
}

int inet_dev_set_macaddr(void *ifdev, const unsigned char *macaddr) {
	if (NULL == ifdev || NULL == macaddr) {
    		return -1;
    	}
	net_device_t *dev = ((in_device_t*)ifdev)->net_dev;
	if (NULL == dev) {
    		return -1;
    	}
	return dev->netdev_ops->ndo_set_mac_address(dev, (void*)macaddr);
}

in_addr_t inet_dev_get_ipaddr(void *handler) {
	in_device_t *dev = (in_device_t *) handler;
	if (NULL == dev) {
    		return 0;
        }
	return dev->ifa_address;
}
#if 0
/**
 * this function is called by device layer from function "netif_rx"
 * before proto parse
 * socket must call "ifdev_listen" if it wants ifdev to have to
 * receive raw socket (for tcpdump for example)
 */
void ifdev_rx_callback(sk_buff_t *pack){
	int i;
	/* if there are some callback handlers for packet's protocol */
	in_device_t *in_dev = (in_device_t *) pack->ifdev;
	if (NULL == dev)
		return;

	INETDEV_INFO *ifdev_info = find_ifdev_info_entry(in_dev);

	for (i = 0; i < array_len(ifdev_info->cb_info); i++) {
		if (NULL != ifdev_info->cb_info[i].func) {
			if ((NET_TYPE_ALL_PROTOCOL == ifdev_info->cb_info[i].type)
			   || (ifdev_info->cb_info[i].type == pack->protocol)) {
				//may be copy pack for different protocols
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
void ifdev_tx_callback(sk_buff_t *pack){

}
#endif
/* iterator functions */
static int iterator_cnt;

in_device_t * inet_dev_get_fist_used() {
	for(iterator_cnt = 0; iterator_cnt < NET_INTERFACES_QUANTITY;
						iterator_cnt++) {
    		if (1 == ifs_info[iterator_cnt].is_busy) {
        		iterator_cnt++;
        		return &ifs_info[iterator_cnt - 1].dev;
    		}
	}
	return NULL;
}

in_device_t * inet_dev_get_next_used() {
	for(; iterator_cnt < NET_INTERFACES_QUANTITY; iterator_cnt++) {
    		if (1 == ifs_info[iterator_cnt].is_busy) {
        		iterator_cnt++;
        		return &ifs_info[iterator_cnt - 1].dev;
    		}
	}
	return NULL;
}

//TODO follow functions either have different interface or move to another place
int ifdev_up(const char *iname){
    in_device_t *ifhandler;
    if (NULL == (ifhandler = find_free_handler ())){
        LOG_ERROR("ifdev up: can't find find free handler\n");
        return -1;
    }
    if (NULL == (ifhandler->net_dev = netdev_get_by_name(iname))){
        LOG_ERROR("ifdev up: can't find net_device with name\n", iname);
        return -1;
    }
    return dev_open(ifhandler->net_dev);
}

int ifdev_down(const char *iname){
    in_device_t *ifhandler;
    if (NULL == (ifhandler = inet_dev_find_by_name(iname))){
            LOG_ERROR("ifdev down: can't find ifdev with name\n", iname);
            return -1;
        }
    if (NULL == (ifhandler->net_dev)){
            LOG_ERROR("ifdev down: can't find net_device with name\n", iname);
            return -1;
        }
    return dev_close(ifhandler->net_dev);
}

int ifdev_set_debug_mode(const char *iname, unsigned int type_filter){
	return 0;
}

int ifdev_clear_debug_mode(const char *iname){
	return 0;
}
