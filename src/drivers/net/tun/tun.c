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
#include <fs/file_desc.h>
#include <fs/inode.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched/waitq.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/thread_sched_wait.h>
#include <mem/misc/pool.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <util/err.h>
#include <util/math.h>

#define TUN_N 1

POOL_DEF(cdev_tun_pool, struct dev_module, TUN_N);

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
	struct tun *tun = netdev_priv(dev);
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

static int tun_setup(struct net_device *dev) {
	dev->mtu = (16 * 1024) + 20 + 20 + 12;
	dev->hdr_len = ETH_HEADER_SIZE;
	dev->addr_len = ETH_ALEN;
	dev->type = ARP_HRD_LOOPBACK;
	dev->flags = IFF_NOARP | IFF_RUNNING;
	dev->drv_ops = &tun_ops;
	dev->ops = &ethernet_ops;
	return 0;
}

static inline struct net_device *tun_netdev_by_name(const char *name) {
	int i;
	for (i = 0; i < TUN_N; i++) {
		if (0 == strcmp(tun_g_array[i]->name, name)) {
			return tun_g_array[i];
		}
	}
	return NULL;
}

static inline int tun_netdev_by_idesc(struct idesc *idesc,
    struct net_device **netdev, struct tun **tun) {
	struct dev_module *cdev;

	cdev = idesc_to_dev_module(idesc);

	*netdev = cdev->dev_priv;
	if (!*netdev) {
		return -ENOENT;
	}

	*tun = netdev_priv(*netdev);
	if (!*tun) {
		return -ENOENT;
	}

	return 0;
}

static struct idesc *tun_dev_open(struct dev_module *cdev, void *priv) {
	struct net_device *netdev;
	struct tun *tun;

	netdev = (struct net_device *)cdev->dev_priv;
	if (!netdev) {
		return err_ptr(ENOENT);
	}

	tun = netdev_priv(netdev);
	if (!tun) {
		return err_ptr(ENOENT);
	}

	tun_user_lock(tun);

	waitq_init(&tun->wq);

	tun_krnl_lock(tun);
	{ skb_queue_init(&tun->rx_q); }
	tun_krnl_unlock(tun);

	return char_dev_idesc_create(cdev);
}

static void tun_dev_close(struct idesc *idesc) {
	struct net_device *netdev;
	struct tun *tun;
	int err;

	err = tun_netdev_by_idesc(idesc, &netdev, &tun);
	if (err) {
		return;
	}

	tun_user_unlock(tun);
}

static ssize_t tun_dev_read(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	struct waitq_link *wql = &thread_self()->schedee.waitq_link;
	struct net_device *netdev;
	struct tun *tun;
	struct sk_buff *skb;
	int err, ret;

	err = tun_netdev_by_idesc(idesc, &netdev, &tun);
	if (err) {
		return err;
	}

	assert(iov);
	if (cnt == 0) {
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
			int len = skb->len;
			unsigned char *raw = skb->mac.raw;
			ret = 0;

			for (int i = 0; i < cnt && len > 0; ++i) {
				int min_len = (len < iov[i].iov_len) ? len : iov[i].iov_len;
				memcpy(iov[i].iov_base, raw, min_len);
				len -= min_len;
				raw += min_len;
				ret += min_len;
			}
			break;
		}
		else {
			ret = sched_wait_timeout(SCHED_TIMEOUT_INFINITE, NULL);
		}
	} while (ret == 0);
	waitq_wait_cleanup(&tun->wq, wql);
	return ret;
}

static ssize_t tun_dev_write(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	struct net_device *netdev;
	struct tun *tun;
	struct sk_buff *skb;
	int err;
	int ret = 0;
	size_t size = 0;
	unsigned char *raw;
	struct ethhdr *ethh;

	err = tun_netdev_by_idesc(idesc, &netdev, &tun);
	if (err) {
		return err;
	}

	assert(iov);
	if (cnt == 0) {
		return -EINVAL;
	}

	for (int i = 0; i < cnt; ++i) {
		size += iov[i].iov_len;
	}
	skb = skb_alloc(size + ETH_HLEN);
	if (!skb) {
		return -ENOMEM;
	}

	ethh = eth_hdr(skb);

	ethh->h_proto = htons(ETH_P_IP);
	memcpy(ethh->h_dest, netdev->dev_addr, ETH_ALEN);
	memset(ethh->h_source, 0, ETH_ALEN);

	raw = skb->mac.raw + ETH_HLEN;
	for (int i = 0; i < cnt; ++i) {
		memcpy(raw, iov[i].iov_base, iov[i].iov_len);
		raw += iov[i].iov_len;
		ret += iov[i].iov_len;
	}

	skb->dev = netdev;
	netif_rx(skb);

	return ret;
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
	static const struct idesc_ops tun_idesc_ops;

	int err;
	struct net_device *tdev;
	char tun_name[16];
	int i;

	for (i = 0; i < TUN_N; i++) {
		struct tun *tun;
		struct dev_module *cdev;

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

		tun = netdev_priv(tdev);
		mutex_init(&tun->mtx_use);

		tun_g_array[i] = tdev;

		cdev = pool_alloc(&cdev_tun_pool);
		if (!cdev) {
			return -ENOMEM;
		}

		memset(cdev, 0, sizeof(*cdev));
		memcpy(cdev->name, tun_name, sizeof(cdev->name));

		cdev->dev_open = tun_dev_open;
		cdev->dev_iops = &tun_idesc_ops;
		cdev->dev_priv = tdev;

		err = char_dev_register(cdev);
		if (err != 0) {
			goto err_inetdev_deregister;
		}
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

static const struct idesc_ops tun_idesc_ops = {
    .id_readv = tun_dev_read,
    .id_writev = tun_dev_write,
    .close = tun_dev_close,
};
