/**
 * @file dev.c
 *
 * @brief Protocol independent device support routines.
 * @date 04.03.2009
 * @author Anton Bondarev
 */
#include <types.h>
#include <string.h>
#include <common.h>
#include <kernel/module.h>
#include <net/skbuff.h>
#include <net/net.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>

static LIST_HEAD(ptype_base);
static LIST_HEAD(ptype_all);

typedef struct _NET_DEV_INFO {
        net_device_t     dev;
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

int dev_alloc_name(struct net_device *dev, const char *name) {
	return 0;
}

net_device_t *alloc_netdev(int sizeof_priv, const char *name,
                           void (*setup)(net_device_t *)) {
        struct net_device *dev;
        int i;
        for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
                if (!dev_is_busy(i)) {
                        dev = dev_lock(i);
                        setup(dev);
                        char buff[IFNAMSIZ];
                        sprintf(buff, name, i);
                        strcpy(dev->name, buff);
                        return dev;
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

int register_netdev(struct net_device *dev) {
	return 0;
}

void unregister_netdev(struct net_device *dev)
{

}

net_device_t *netdev_get_by_name(const char *name) {
        int i;
        for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
                if (dev_is_busy(i) &&
                            !strncmp(name, net_devices[i].dev.name, IFNAMSIZ)) {
                        return &net_devices[i].dev;
                }
        }
        return NULL;
}

int dev_queue_xmit(struct sk_buff *pack) {
        if(NULL == pack) {
                return -1;
        }
        net_device_t *dev = pack->dev;
        if (NULL == dev) {
                return -1;
        }
        const struct net_device_ops *ops = dev->netdev_ops;
        net_device_stats_t *stats = ops->ndo_get_stats(dev);

        if (dev->flags & IFF_UP) {
                if (ETH_P_ARP != pack->protocol) {
                        if (-1 == dev->header_ops->rebuild(pack)) {
                                kfree_skb(pack);
                                stats->tx_err += 1;
                                return -1;
                        }
                }

                if (-1 == ops->ndo_start_xmit(pack, dev)) {
                        kfree_skb(pack);
                        stats->tx_err += 1;
                        return -1;
                }
                /* update statistic */
                stats->tx_packets += 1;
                stats->tx_bytes   += pack->len;
        }
        kfree_skb(pack);
        return 0;
}

int netif_rx(struct sk_buff *pack) {
        if(NULL == pack) {
                return NET_RX_DROP;
        }
        net_device_t *dev = pack->dev;
        if(NULL == dev) {
                return NET_RX_DROP;
        }
        pack->nh.raw = (void *) pack->data + ETH_HEADER_SIZE;

#if 0
        //now we have not ifdev field in skb
        if (NULL == (pack->ifdev = inet_dev_find_by_name(pack->dev->name))) {
                LOG_ERROR("wrong interface name during receiving packet\n");
                kfree_skb(pack);
                return NET_RX_DROP;
        }
#endif
        struct list_head *head;
        struct packet_type *q;
        list_for_each_entry(q, &ptype_base, list) {
                if(q->type == pack->protocol) {
                        q->func(pack, NULL, NULL, NULL);
                }
        }
#if 0
        if (ETH_P_ARP == pack->protocol) {
                arp_rcv(pack);
        }
        if (ETH_P_IP == pack->protocol) {
                ip_rcv(pack);
        }
#endif
#if 0
        /* if there are some callback handlers for packet's protocol */
        dev = (IF_DEVICE *) pack->ifdev;
        int i;
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
        return NET_RX_SUCCESS;
}

void dev_add_pack(struct packet_type *pt) {
        int hash;
        if (pt->type == htons(ETH_P_ALL)) {
                list_add(&pt->list, &ptype_all);
        } else {
                list_add(&pt->list, &ptype_base);
        }
}

void dev_remove_pack(struct packet_type *pt) {
        struct list_head *head;
        struct packet_type *pt1;

        if (pt->type == htons(ETH_P_ALL)) {
                head = &ptype_all;
        } else {
                head = &ptype_base;
        }
        list_for_each_entry(pt1, head, list) {
                if (pt == pt1) {
                        list_del(&pt->list);
                        return;
                }
        }
}

int dev_open(struct net_device *dev) {
        int ret = 0;
        /* Is it already up? */
        if (dev->flags & IFF_UP) {
                return 0;
        }
        const struct net_device_ops *ops = dev->netdev_ops;
        dev->state |= __LINK_STATE_START;

        if (ops->ndo_open) {
                ret = ops->ndo_open(dev);
        } else {
                LOG_ERROR("can't find open function in net_device\n");
        }

        if (ret) {
                dev->state &= ~__LINK_STATE_START;
        } else {
                /* Set the flags. */
                //TODO: IFF_RUNNING sets not here
                dev->flags |= IFF_UP|IFF_RUNNING;
        }
        return ret;
}

int dev_close(struct net_device *dev) {
        if (!(dev->flags & IFF_UP)) {
                return 0;
        }
        const struct net_device_ops *ops = dev->netdev_ops;
        dev->state &= ~__LINK_STATE_START;

        if (ops->ndo_stop) {
                ops->ndo_stop(dev);
        } else {
                LOG_ERROR("ifdev down: can't find stop function in net_device with name\n");
        }
        /* Device is now down. */
        //TODO: IFF_RUNNING sets not here
        dev->flags &= ~IFF_UP|IFF_RUNNING;
        return 0;
}

unsigned dev_get_flags(const struct net_device *dev) {
        //TODO: ...
        return dev->flags;
}

int dev_change_flags(struct net_device *dev, unsigned flags) {
        int ret;
        int old_flags = dev->flags;
        if ((old_flags ^ flags) & IFF_UP) {
                ret = ((old_flags & IFF_UP) ? dev_close : dev_open)(dev);
        }
        //TODO: ...
        dev->flags = dev->flags & flags;
        return ret;
}
