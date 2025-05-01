/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.07.2014
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <drivers/char_dev.h>
#include <embox/unit.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched/waitq.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/thread_sched_wait.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <util/err.h>
#include <util/math.h>

struct tun_dev {
	struct char_dev cdev;
	struct net_device *netdev;
	struct mutex mtx_use;
	struct sk_buff_head rx_q;
	struct waitq wq;
};

EMBOX_UNIT_INIT(tun_dev_init);

static inline void tun_krnl_lock(struct tun_dev *tun) {
	sched_lock();
}

static inline void tun_krnl_unlock(struct tun_dev *tun) {
	sched_unlock();
}

static inline void tun_user_lock(struct tun_dev *tun) {
	mutex_lock(&tun->mtx_use);
}

static inline void tun_user_unlock(struct tun_dev *tun) {
	mutex_unlock(&tun->mtx_use);
}

static int tun_open(struct net_device *dev) {
	return 0;
}

static int tun_set_mac(struct net_device *dev, const void *addr) {
	memcpy(dev->dev_addr, addr, ETH_ALEN);
	return ENOERR;
}

static int tun_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct tun_dev *tun = netdev_priv(dev);
	struct ethhdr *ethh;

	/* we don't build headers for dev with NOARP flag */
	ethh = eth_hdr(skb);
	ethh->h_proto = htons(ETH_P_IP);
	memcpy(ethh->h_source, skb->dev->dev_addr, ETH_ALEN);
	memset(ethh->h_dest, 0, ETH_ALEN);

	skb_queue_push(&tun->rx_q, skb);
	waitq_wakeup(&tun->wq, 1);
	return 0;
}

static const struct net_driver tun_netdrv_ops = {
    .xmit = tun_xmit,
    .start = tun_open,
    .set_macaddr = tun_set_mac,
};

static int tun_setup(struct net_device *dev) {
	dev->mtu = (16 * 1024) + 20 + 20 + 12;
	dev->hdr_len = ETH_HEADER_SIZE;
	dev->addr_len = ETH_ALEN;
	dev->type = ARP_HRD_LOOPBACK;
	dev->flags = IFF_NOARP | IFF_RUNNING;
	dev->drv_ops = &tun_netdrv_ops;
	dev->ops = &ethernet_ops;
	return 0;
}

static int tun_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	assert(cdev);

	tun_user_lock((struct tun_dev *)cdev);

	return 0;
}

static void tun_dev_close(struct char_dev *cdev) {
	assert(cdev);

	tun_user_unlock((struct tun_dev *)cdev);
}

static ssize_t tun_dev_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct waitq_link *wql = &thread_self()->schedee.waitq_link;
	struct tun_dev *tun;
	struct sk_buff *skb;
	int ret = 0;
	int min_len;

	assert(cdev);

	tun = (struct tun_dev *)cdev;

	waitq_link_init(wql);
	do {
		tun_krnl_lock(tun);
		{
			waitq_wait_prepare(&tun->wq, wql);
			skb = skb_queue_pop(&tun->rx_q);
		}
		tun_krnl_unlock(tun);

		if (skb) {
			min_len = min(skb->len, nbyte);
			if (min_len > 0) {
				memcpy(buf, skb->mac.raw, min_len);
				ret = min_len;
			}
			break;
		}
		else {
			ret = sched_wait_timeout(SCHED_TIMEOUT_INFINITE, NULL);
		}
	} while (ret == 0);
	waitq_wait_cleanup(&tun->wq, wql);
	skb_free(skb);

	return ret;
}

static ssize_t tun_dev_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	struct net_device *netdev;
	struct tun_dev *tun;
	struct sk_buff *skb;
	unsigned char *raw;
	struct ethhdr *ethh;

	assert(cdev);

	tun = (struct tun_dev *)cdev;
	netdev = tun->netdev;

	skb = skb_alloc(nbyte + ETH_HLEN);
	if (!skb) {
		return -ENOMEM;
	}

	ethh = eth_hdr(skb);

	ethh->h_proto = htons(ETH_P_IP);
	memcpy(ethh->h_dest, netdev->dev_addr, ETH_ALEN);
	memset(ethh->h_source, 0, ETH_ALEN);

	raw = skb->mac.raw + ETH_HLEN;
	memcpy(raw, buf, nbyte);

	skb->dev = netdev;
	netif_rx(skb);

	return nbyte;
}

static const struct char_dev_ops tun_dev_ops = {
    .read = tun_dev_read,
    .write = tun_dev_write,
    .open = tun_dev_open,
    .close = tun_dev_close,
};

static struct tun_dev tun_dev = {
    .cdev = CHAR_DEV_INIT(tun_dev.cdev, "tun0", &tun_dev_ops),
};

CHAR_DEV_REGISTER((struct char_dev *)&tun_dev);

static int tun_dev_init(void) {
	struct net_device *netdev;
	int err;

	assert(strlen(tun_dev.cdev.name) < IFNAMSIZ);

	netdev = netdev_alloc(tun_dev.cdev.name, &tun_setup, 0);
	if (netdev == NULL) {
		return -ENOMEM;
	}

	if ((err = inetdev_register_dev(netdev))) {
		netdev_free(netdev);
		return err;
	}

	mutex_init(&tun_dev.mtx_use);
	waitq_init(&tun_dev.wq);
	skb_queue_init(&tun_dev.rx_q);

	tun_dev.netdev = netdev;
	netdev->priv = &tun_dev;

	return 0;
}
