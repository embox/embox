/**
 * \file net_device.c
 *
 * \date Mar 4, 2009
 * \author Anton Bondarev
 */
#include "types.h"
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
    net_device_stats_t *stats;

    if ((NULL == pack) || (NULL == pack->netdev))
        return -1;

    stats = pack->netdev->get_stats(pack->netdev);

    if (ETH_P_ARP != pack->protocol) {
        if (-1 == dev->net_dev->rebuild_header(pack)) {
            kfree_skb(pack);
            stats->tx_err += 1;
            return -1;
        }
    }
//TODO delete this because we will can manager ifdev debug mod in future
    //packet_dump(pack);
    if (-1 == pack->netdev->hard_start_xmit(pack, pack->netdev)) {
    	kfree_skb(pack);
    	stats->tx_err += 1;
    	return -1;
    }
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

#if 0
    //now we have not ifdev field in skb
    if (NULL == (pack->ifdev = inet_dev_find_by_name(pack->netdev->name))){
        LOG_ERROR("wrong interface name during receiving packet\n");
        kfree_skb(pack);
        return -1;
    }
#endif
    if (ETH_P_ARP == pack->protocol) {
        arp_received_packet(pack);
    }
    if (ETH_P_IP == pack->protocol) {
        ip_received_packet(pack);
    }
#if 0
    /* if there are some callback handlers for packet's protocol */
    dev = (IF_DEVICE *) pack->ifdev;
    for (i = 0; i < array_len(dev->cb_info); i++) {
        if (1 == dev->cb_info[i].is_busy) {
            if ((NET_TYPE_ALL_PROTOCOL == dev->cb_info[i].type)
                    || (dev->cb_info[i].type == pack->protocol)) {
                //may be copy pack for different protocols
                dev->cb_info[i].func(pack);
            }
        }
    }
#endif

    //free packet
    kfree_skb(pack);
    return 0;

}

void dev_add_pack(struct packet_type *pt){

}

void dev_remove_pack(struct packet_type *pt){

}
