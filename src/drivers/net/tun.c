/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.07.2014
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <util/math.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <embox/unit.h>

#include <kernel/thread/sync/mutex.h>
#include <kernel/sched/sched_lock.h>
#include <embox/device.h> //XXX
#include <fs/node.h>
#include <fs/file_desc.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/thread.h>

#define TUN_N 1

EMBOX_UNIT_INIT(tun_init);

struct tun {
	struct mutex mtx_use;
	struct sk_buff_head rx_q;
	struct waitq wq;
};

static struct net_device *tun_g_array[TUN_N];

static inline void tun_krnl_lock(struct tun *tun) {
	sched_lock();
}

static inline void tun_krnl_unlock(struct tun *tun) {
	sched_unlock();
}

static inline void tun_user_lock(struct tun *tun) {
	mutex_lock(&tun->mtx_use);
}

static inline void tun_user_unlock(struct tun *tun) {
	mutex_unlock(&tun->mtx_use);
}


static int tun_xmit(struct net_device *dev, struct sk_buff *skb);
static int tun_open(struct net_device *dev);
static int tun_set_mac(struct net_device *dev, const void *addr);
static const struct net_driver tun_ops = {
	.xmit = tun_xmit,
	.start = tun_open,
	.set_macaddr = tun_set_mac,
};

static int tun_open(struct net_device *dev) {
	return 0;
}

static int tun_set_mac(struct net_device *dev, const void *addr) {
	memcpy(dev->dev_addr, addr, ETH_ALEN);
	return ENOERR;
}

static int tun_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct tun *tun = netdev_priv(dev, struct tun);
	skb_queue_push(&tun->rx_q, skb);
	waitq_wakeup(&tun->wq, 1);
	return 0;
}

static int tun_setup(struct net_device *dev) {
	dev->mtu      = (16 * 1024) + 20 + 20 + 12;
	dev->hdr_len  = ETH_HEADER_SIZE;
	dev->addr_len = ETH_ALEN;
	dev->type     = ARP_HRD_LOOPBACK;
	dev->flags    = IFF_NOARP | IFF_RUNNING;
	dev->drv_ops  = &tun_ops;
	dev->ops      = &ethernet_ops;
	return 0;
}

static int    tun_dev_open(struct node *node, struct file_desc *file_desc, int flags);
static int    tun_dev_close(struct file_desc *desc);
static size_t tun_dev_read(struct file_desc *desc, void *buf, size_t size);
static size_t tun_dev_write(struct file_desc *desc, void *buf, size_t size);
static int    tun_dev_ioctl(struct file_desc *desc, int request, ...);
static const struct kfile_operations tun_dev_file_ops = {
	.open  = tun_dev_open,
	.ioctl = tun_dev_ioctl,
	.read  = tun_dev_read,
	.write = tun_dev_write,
	.close = tun_dev_close,
};

static inline struct net_device *tun_netdev_by_name(const char *name) {
	int i;
	for (i = 0; i < TUN_N; i++) {
		if (0 == strcmp(tun_g_array[i]->name, name)) {
			return tun_g_array[i];
		}
	}
	return NULL;
}

static inline struct net_device *tun_netdev_by_node(const struct node *node) {
	return tun_netdev_by_name(node->name);
}

static inline int tun_netdev_by_desc(const struct file_desc *fdesc,
		struct net_device **netdev, struct tun **tun) {
	*netdev = fdesc->file_info;
	if (!*netdev) {
		return -ENOENT;
	}
	*tun = netdev_priv(*netdev, struct tun);
	return 0;
}

static int tun_dev_open(struct node *node, struct file_desc *file_desc, int flags) {
	struct net_device *netdev;
	struct tun *tun;

	netdev = tun_netdev_by_node(node);
	if (!netdev) {
		return -ENOENT;
	}

	tun = netdev_priv(netdev, struct tun);
	tun_user_lock(tun);

	waitq_init(&tun->wq);

	tun_krnl_lock(tun);
	{
		skb_queue_init(&tun->rx_q);
	}
	tun_krnl_unlock(tun);

	file_desc->file_info = netdev;

	return 0;
}

static int tun_dev_close(struct file_desc *desc) {
	struct net_device *netdev;
	struct tun *tun;
	int err;

	err = tun_netdev_by_desc(desc, &netdev, &tun);
	if (err) {
		return err;
	}

	tun_user_unlock(tun);

	return 0;
}

static size_t tun_dev_read(struct file_desc *desc, void *buf, size_t size) {
	struct waitq_link *wql = &thread_self()->schedee.waitq_link;
	struct net_device *netdev;
	struct tun *tun;
	struct sk_buff *skb;
	int err, ret;

	err = tun_netdev_by_desc(desc, &netdev, &tun);
	if (err) {
		return err;
	}

	if (size == 0) {
		return -EINVAL;
	}

	waitq_link_init(wql);
	do {
		tun_krnl_lock(tun);
		{
			waitq_wait_prepare(&tun->wq, wql);
			skb = skb_queue_pop(&tun->rx_q);
		}
		tun_krnl_unlock(tun);

		if (skb) {
			int len = min(size, skb->len);
			memcpy(buf, skb->mac.raw, len);
			ret = len;
			break;
		} else {
			ret = sched_wait_timeout(SCHED_TIMEOUT_INFINITE, NULL);
		}
	} while (ret == 0);
	waitq_wait_cleanup(&tun->wq, wql);
	return ret;
}

static size_t tun_dev_write(struct file_desc *desc, void *buf, size_t size) {
	struct net_device *netdev;
	struct tun *tun;
	struct sk_buff *skb;
	int err;

	err = tun_netdev_by_desc(desc, &netdev, &tun);
	if (err) {
		return err;
	}

	skb = skb_alloc(size + ETH_HLEN);
	if (!skb) {
		return -ENOMEM;
	}

	ethhdr_build(skb->mac.ethh, netdev->dev_addr, NULL, ETH_P_IP);

	memcpy(skb->mac.raw + ETH_HLEN, buf, size);
	skb->dev = netdev;
	netif_rx(skb);

	return 0;
}

static int tun_dev_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static void tun_deinit(void) {
	int i;

	for (i = 0; i < TUN_N; i++) {
		if (!tun_g_array[i]) {
			break;
		}

		/* inetdev_deregister */
		netdev_free(tun_g_array[i]);
	}
}

static int tun_init(void) {
	int err;
	struct net_device *tdev;
	char tun_name[16];
	int i;

	for (i = 0; i < TUN_N; i++) {
		struct tun *tun;

		snprintf(tun_name, sizeof(tun_name), "tun%d", i);

		tdev = netdev_alloc(tun_name, &tun_setup, sizeof(struct tun));
		if (tdev == NULL) {
			err = -ENOMEM;
			goto err_deinit;
		}

		err = inetdev_register_dev(tdev);
		if (err != 0) {
			goto err_netdev_free;
		}

		err = char_dev_register(tun_name, &tun_dev_file_ops);
		if (err != 0) {
			goto err_inetdev_deregister;
		}

		tun = netdev_priv(tdev, struct tun);
		mutex_init(&tun->mtx_use);

		tun_g_array[i] = tdev;
	}

	return 0;

err_inetdev_deregister:
	/* inetdev_deregister */
err_netdev_free:
	netdev_free(tdev);
err_deinit:
	tun_deinit();
	return err;

}
