/**
 * \file ifdev.c
 * \date Jul 18, 2009
 * \author Anton Bondarev
 * \details realizing interface if_device struct (interface device)
 */
#include "conio.h"
#include "string.h"
#include "common.h"
#include "net/net.h"
#include "net/if_device.h"
#include "net/net_device.h"
#include "net/skbuff.h"


#define IFDEV_CBINFO_QUANTITY 8

typedef struct _CALLBACK_INFO {
    unsigned short      type;
    ETH_LISTEN_CALLBACK func;
} CALLBACK_INFO;

typedef struct _IFDEV_INFO {
    inet_device_t dev;
    int       is_busy;
    CALLBACK_INFO cb_info[IFDEV_CBINFO_QUANTITY];
} IFDEV_INFO;

static IFDEV_INFO ifs_info[NET_INTERFACES_QUANTITY];
IFDEV_INFO *find_ifdev_info_entry(inet_device_t *ifdev){
	int i;
	for (i = 0; i < array_len(ifs_info); i ++){
		if (&ifs_info[i].dev == ifdev){
			return &ifs_info[i];
		}
	}
	return NULL;
}
static inet_device_t * find_free_handler(){
    int i;
    for(i = 0; i < NET_INTERFACES_QUANTITY; i ++){
        if (0 == ifs_info[i].is_busy){
            ifs_info[i].is_busy = 1;
            return &ifs_info[i].dev;
        }
    }
    return NULL;
}

static int free_handler(inet_device_t * handler){
    int i;
    for(i = 0; i < NET_INTERFACES_QUANTITY; i ++){
        if ((1 == ifs_info[i].is_busy) && (&ifs_info[i].dev == handler)){
            ifs_info[i].is_busy = 0;
            return 0;
        }
    }
    return -1;
}

static int alloc_callback(inet_device_t *dev, unsigned int type,
                          ETH_LISTEN_CALLBACK callback) {
    int i;
    IFDEV_INFO *ifdev_info = find_ifdev_info_entry(dev);
    for (i = 0; i < array_len(ifdev_info->cb_info); i++) {
        if (0 == ifdev_info->cb_info[i].func) {
        	ifdev_info->cb_info[i].type    = type;
        	ifdev_info->cb_info[i].func    = callback;
            return i;
        }
    }
    return -1;
}

static int free_callback(inet_device_t *dev, ETH_LISTEN_CALLBACK callback) {
    int i;
    IFDEV_INFO *ifdev_info = find_ifdev_info_entry(dev);
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
    inet_device_t *dev = (inet_device_t *) handler;
    if (NULL == dev) {
        LOG_ERROR("handler is NULL\n");
        return NULL;
    }
    return dev->net_dev;
}

int inet_dev_listen(void *handler, unsigned short type,
                 ETH_LISTEN_CALLBACK callback) {
    inet_device_t *dev = (inet_device_t *) handler;
    if (NULL == dev) {
        return -1;
    }
    return alloc_callback(dev, type, callback);
}

int inet_dev_find_by_ip(const unsigned char *ipaddr) {
    int i;
    for (i = 0; i < NET_INTERFACES_QUANTITY; i++) {
        if (0 == memcmp(ifs_info[i].dev.ipv4_addr, ipaddr, 4)) {
            return 0;
        }
    }
    return -1;
}

void *inet_dev_find_by_name(const char *if_name) {
    int i;
    for (i = 0; i < NET_INTERFACES_QUANTITY; i++) {
        LOG_DEBUG("ifname %s, net_dev 0x%X\n", if_name, ifs_info[i].dev.net_dev);
        if (0 == strncmp(if_name, ifs_info[i].dev.net_dev->name,
                sizeof(ifs_info[i].dev.net_dev->name))) {
            return &ifs_info[i].dev;
        }
    }
    return NULL;
}

int inet_dev_set_interface(char *name, char *ipaddr, char *macaddr) {
    int i;
    for (i = 0; i < NET_INTERFACES_QUANTITY; i++) {
        if (0 == strncmp(name, ifs_info[i].dev.net_dev->name, array_len(ifs_info[i].dev.net_dev->name))) {
            if( (-1 == inet_dev_set_ipaddr(&ifs_info[i].dev, ipaddr)) ||
                (-1 == inet_dev_set_macaddr(&ifs_info[i].dev, macaddr)) ) {
        	return -1;
    	    }
            return i;
        }
    }
    return -1;
}

int inet_dev_set_ipaddr(void *ifdev, const unsigned char *ipaddr) {
    if (NULL == ifdev)
        return -1;
    inet_device_t *dev = (inet_device_t *) ifdev;
    memcpy(dev->ipv4_addr, ipaddr, sizeof(dev->ipv4_addr));
    return 0;
}

int inet_dev_set_macaddr(void *ifdev, const unsigned char *macaddr) {
    if (NULL == ifdev || NULL == macaddr)
        return -1;
    net_device_t *dev = ((inet_device_t*)ifdev)->net_dev;
    if (NULL == dev)
        return -1;
    return dev->set_mac_address(dev, (void*)macaddr);
}

unsigned char *inet_dev_get_ipaddr(void *handler) {
    inet_device_t *dev = (inet_device_t *) handler;
    if (NULL == dev)
        return NULL;
    return dev->ipv4_addr;
}

/**
 * this function is called by device layer from function "netif_rx"
 * before proto parse
 * socket must call "ifdev_listen" if it wants ifdev to have to
 * receive raw socket (for tcpdump for example)
 */
void ifdev_rx_callback(sk_buff_type *pack){
	int i;
	/* if there are some callback handlers for packet's protocol */
	inet_device_t *dev = (inet_device_t *) pack->ifdev;
	if (NULL == dev)
		return;

	IFDEV_INFO *ifdev_info = find_ifdev_info_entry(dev);

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
void ifdev_tx_callback(sk_buff_type *pack){

}

/* iterator functions */
static int iterator_cnt;
inet_device_t * inet_dev_get_fist_used(){
    for(iterator_cnt = 0; iterator_cnt < NET_INTERFACES_QUANTITY; iterator_cnt++){
        if (1 == ifs_info[iterator_cnt].is_busy){
            iterator_cnt++;
            return &ifs_info[iterator_cnt - 1].dev;
        }
    }
    return NULL;
}

inet_device_t * inet_dev_get_next_used(){
    for(; iterator_cnt < NET_INTERFACES_QUANTITY; iterator_cnt++){
        if (1 == ifs_info[iterator_cnt].is_busy){
            iterator_cnt++;
            return &ifs_info[iterator_cnt - 1].dev;
        }
    }
    return NULL;
}

//TODO follow functions either have different interface or move to another place
int ifdev_up(const char *iname){
    inet_device_t *ifhandler;
    if (NULL == (ifhandler = find_free_handler ())){
        LOG_ERROR("ifdev up: can't find find free handler\n");
        return -1;
    }
    if (NULL == (ifhandler->net_dev = netdev_get_by_name(iname))){
        LOG_ERROR("ifdev up: can't find net_device with name\n", iname);
        return -1;
    }
    if (NULL == ifhandler->net_dev->open) {
        LOG_ERROR("ifdev up: can't find open function in net_device with name\n", iname);
        return -1;
    }
    ifhandler->net_dev->flags |= (IFF_UP|IFF_RUNNING);
    return ifhandler->net_dev->open(ifhandler->net_dev);
}

int ifdev_down(const char *iname){
    inet_device_t *ifhandler;
    if (NULL == (ifhandler = inet_dev_find_by_name(iname))){
            LOG_ERROR("ifdev down: can't find ifdev with name\n", iname);
            return -1;
        }
    if (NULL == (ifhandler->net_dev)){
            LOG_ERROR("ifdev down: can't find net_device with name\n", iname);
            return -1;
        }
    if (NULL == ifhandler->net_dev->stop) {
        LOG_ERROR("ifdev down: can't find stop function in net_device with name\n", iname);
        return -1;
    }
    free_handler(ifhandler);
    ifhandler->net_dev->flags &= ~(IFF_UP|IFF_RUNNING);
    return ifhandler->net_dev->stop(ifhandler->net_dev);
}

int ifdev_set_debug_mode(const char *iname, unsigned int type_filter){
	return 0;
}

int ifdev_clear_debug_mode(const char *iname){
	return 0;
}
