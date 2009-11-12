/**
 * \file net_device.c
 *
 * \date Mar 4, 2009
 * \author Anton Bondarev
 */
#include "asm/types.h"
#include "conio.h"
#include "string.h"
#include "common.h"

#include "net/skbuff.h"
#include "net/net.h"
#include "net/net_device.h"
#include "net/if_device.h"

typedef struct _NET_DEV_INFO {
	net_device_t       dev;
	net_device_stats_t stats;
	int              is_busy;
} NET_DEV_INFO;

static NET_DEV_INFO net_devices[NET_DEVICES_QUANTITY];

static inline int dev_is_busy(int num) {
	return net_devices[num].is_busy;
}

static inline net_device_t *dev_lock(int num) {
	net_devices[num].is_busy = 1;
	return &net_devices[num].dev;
}

static inline void dev_unlock(int num) {
	net_devices[num].is_busy = 0;
}

net_device_t *alloc_netdev() {
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (!dev_is_busy(i)) {
			return dev_lock(i);
		}
	}
	return NULL;
}

void free_netdev(net_device_t *dev) {
        int i;
	for(i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (dev == &net_devices[i].dev) {
    			dev_unlock(i);
    		}
	}
}

net_device_t *netdev_get_by_name(const char *name) {
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (dev_is_busy(i) &&
		   (0 == strncmp(name, net_devices[i].dev.name, sizeof (net_devices[i].dev.name)))) {
			return &net_devices[i].dev;
		}
	}
	return NULL;
}

int dev_queue_xmit(struct sk_buff *pack) {
    inet_device_t *dev;
    net_device_stats_t *stats = pack->netdev->get_stats(pack->netdev);

    if ((NULL == pack) || (NULL == pack->ifdev))
        return -1;

    dev = (inet_device_t *) pack->ifdev;
    if (ETH_P_ARP != pack->protocol) {
        if (-1 == dev->net_dev->rebuild_header(pack)) {
            kfree_skb(pack);
            stats->tx_err += 1;
            return -1;
        }
    }
//TODO delete this because we will can manager ifdev debug mod in future
    //packet_dump(pack);
    if (-1 == dev->net_dev->hard_start_xmit(pack, pack->netdev)) {
    	kfree_skb(pack);
    	stats->tx_err += 1;
    	return -1;
    }
#if 0
    ifdev_tx_callback(pack);
#endif
    /* update statistic */
    stats->tx_packets += 1;
    stats->tx_bytes   += pack->len;
    kfree_skb(pack);
    return 0;
}

int netif_rx(struct sk_buff *pack) {
    int i;
    inet_device_t *dev;
    if ((NULL == pack) || (NULL == pack->netdev)) {
        return -1;
    }
    pack->nh.raw = (void *) pack->data + ETH_HEADER_SIZE;
    if (NULL == (pack->ifdev = inet_dev_find_by_name(pack->netdev->name))){
        LOG_ERROR("wrong interface name during receiving packet\n");
        kfree_skb(pack);
        return -1;
    }
}

void dev_add_pack(struct packet_type *pt){

}

void dev_remove_pack(struct packet_type *pt){

}
