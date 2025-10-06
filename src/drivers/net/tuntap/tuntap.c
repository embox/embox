/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @author  Maxim Kamendov
 * @date    06.10.2025
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
#include <net/if_tun.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/l3/ipv4/ip_options.h>
#include <net/l3/route.h>
#include <net/netdevice.h>
#include <util/err.h>
#include <util/math.h>

struct tun_dev {
	struct char_dev cdev;
	struct net_device *netdev;
	struct mutex mtx_use;
	struct sk_buff_head rx_q;
	struct waitq wq;
	unsigned int flags;
	bool is_registered;
};

struct tun_dev *tun_devices[MAX_TUNTAP_DEVICES];
static const struct char_dev_ops tun_dev_ops;

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
	if (tun->flags & IFF_TAP) {
		struct ethhdr *ethh;
		/* we don't build headers for dev with NOARP flag */
		ethh = eth_hdr(skb);
		memcpy(ethh->h_source, skb->dev->dev_addr, ETH_ALEN);
		memset(ethh->h_dest, 0, ETH_ALEN);
	}

	tun_krnl_lock(tun);
	{
		skb_queue_push(&tun->rx_q, skb);
		waitq_wakeup(&tun->wq, 1);
	}
	tun_krnl_unlock(tun);

	return 0;
}

static const struct net_driver tun_netdrv_ops = {
    .xmit = tun_xmit,
    .start = tun_open,
    .set_macaddr = tun_set_mac,
};

static int tun_setup(struct net_device *dev) {
	dev->mtu = (16 * 1024) + 20 + 20 + 12;
	dev->drv_ops = &tun_netdrv_ops;
	dev->ops = &ethernet_ops;

	return 0;
}

static void tun_dev_close(struct char_dev *cdev) {
	/* All cleanup is handled in tun_instance_close(),
	 * this is just a dummy function */
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
				if (tun->flags & IFF_TUN) {
					memcpy(buf, skb->nh.raw, min_len);
				}
				else {
					memcpy(buf, skb->mac.raw, min_len);
				}
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

int tun_dev_ioctl(struct char_dev *dev, int cmd, void *data) {
	if (dev == NULL) {
		return -ENODEV;
	}

	if (dev == NULL) {
		return -EINVAL;
	}

	int err = 0;
	struct tun_dev *tun = (struct tun_dev *)dev;
	struct ifreq *ifr = (struct ifreq *)(data);

	tun_user_lock(tun);
	switch (cmd) {
	case TUNSETIFF: {
		if (ifr->ifr_flags & ~(IFF_TUN | IFF_TAP | IFF_NOARP)) {
			err = -EINVAL;
			break;
		}

		if (tun->is_registered) {
			inetdev_unregister_dev(tun->netdev);
			tun->is_registered = false;
		}

		strncpy(tun->netdev->name, ifr->ifr_name, IFNAMSIZ - 1);
		tun->netdev->name[IFNAMSIZ - 1] = '\0';

		tun->flags = ifr->ifr_flags;

		if ((err = inetdev_register_dev(tun->netdev))) {
			break;
		}
		tun->is_registered = true;

		if (tun->flags & IFF_TUN) {
			tun->netdev->hdr_len = 0;
			tun->netdev->type = ARP_HRD_NONE;
		}
		else if (tun->flags & IFF_TAP) {
			tun->netdev->hdr_len = ETH_HEADER_SIZE;
			tun->netdev->type = ARP_HRD_ETHERNET;
			tun->netdev->addr_len = ETH_ALEN;
		}
		tun->netdev->flags = IFF_RUNNING | IFF_UP;

		break;
	}
	case TUNGETIFF: {
		if (tun->is_registered) {
			inetdev_unregister_dev(tun->netdev);
			tun->is_registered = false;
		}

		strncpy(ifr->ifr_name, tun->netdev->name, IFNAMSIZ);
		ifr->ifr_flags = tun->flags;
		break;

		if ((err = inetdev_register_dev(tun->netdev))) {
			break;
		}
		tun->is_registered = true;
	}

	default:
		err = -ENOSYS;
	}
	tun_user_unlock(tun);

	return err;
}

static ssize_t tun_dev_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	struct net_device *netdev;
	struct tun_dev *tun;
	struct sk_buff *skb;

	assert(cdev);

	tun = (struct tun_dev *)cdev;
	netdev = tun->netdev;

	skb = skb_alloc(nbyte);

	if (!skb) {
		return -ENOMEM;
	}

	if (tun->flags & IFF_TAP) {
		struct ethhdr *ethh;
		ethh = eth_hdr(skb);
		skb->mac.raw = (unsigned char *)skb->data + 8;
		skb->nh.raw = skb->mac.raw + ETH_HLEN;
		memcpy(ethh->h_dest, netdev->dev_addr, ETH_ALEN);
		memset(ethh->h_source, 0, ETH_ALEN);
		memcpy(skb->mac.raw, buf, nbyte);
	}
	else {
		skb->nh.raw = (unsigned char *)skb->data + 8;
		memcpy(((void *)skb->nh.raw), buf, nbyte);
	}

	skb->dev = netdev;
	netif_rx(skb);

	return nbyte;
}

static struct char_dev tun_cloning_dev = CHAR_DEV_INIT(tun_cloning_dev, "tun0",
    &tun_dev_ops);

static int tun_dev_init(void) {
	memset(tun_devices, 0, sizeof(tun_devices));
	return char_dev_register(&tun_cloning_dev);

	return 0;
}

static ssize_t tun_instance_readv(struct idesc *idesc, const struct iovec *iov,
    int iovcnt) {
	struct tun_dev *tun = idesc->priv;
	ssize_t nbyte = 0;
	ssize_t res;
	int i;

	if (!tun)
		return -ENODEV;

	for (i = 0; i < iovcnt; i++) {
		res = tun_dev_read(&tun->cdev, iov[i].iov_base, iov[i].iov_len);
		if (res < 0) {
			return res;
		}
		nbyte += res;
	}
	return nbyte;
}

static ssize_t tun_instance_writev(struct idesc *idesc, const struct iovec *iov,
    int iovcnt) {
	struct tun_dev *tun = idesc->priv;
	ssize_t nbyte = 0;
	ssize_t res;
	int i;

	if (!tun)
		return -ENODEV;

	for (i = 0; i < iovcnt; i++) {
		res = tun_dev_write(&tun->cdev, iov[i].iov_base, iov[i].iov_len);
		if (res < 0) {
			return res;
		}
		nbyte += res;
	}
	return nbyte;
}

static int tun_instance_ioctl(struct idesc *idesc, int request, void *data) {
	struct tun_dev *tun = idesc->priv;
	if (!tun)
		return -ENODEV;
	return tun_dev_ioctl(&tun->cdev, request, data);
}

static void tun_instance_close(struct idesc *idesc) {
	struct tun_dev *tun = idesc->priv;
	if (tun) {
		inetdev_unregister_dev(tun->netdev);
		netdev_free(tun->netdev);
		skb_queue_purge(&tun->rx_q);

		for (int i = 0; i < MAX_TUNTAP_DEVICES; i++) {
			if (tun_devices[i] == tun) {
				rt_del_route_if(tun->netdev);
				tun_devices[i] = NULL;
				break;
			}
		}

		free(tun);
	}
}

static int tun_instance_fstat(struct idesc *idesc, struct stat *stat) {
	memset(stat, 0, sizeof(struct stat));
	stat->st_mode = S_IFCHR;
	return 0;
}

static int tun_instance_status(struct idesc *idesc, int mask) {
	return 0;
}

static const struct idesc_ops tun_instance_idesc_ops = {
    .id_readv = tun_instance_readv,
    .id_writev = tun_instance_writev,
    .close = tun_instance_close,
    .ioctl = tun_instance_ioctl,
    .fstat = tun_instance_fstat,
    .status = tun_instance_status,
    .idesc_mmap = NULL,
};

static int tun_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct tun_dev *tun;
	struct net_device *netdev;
	int idx;
	cdev->usage_count = 0;

	for (idx = 0; idx < MAX_TUNTAP_DEVICES; idx++) {
		if (!tun_devices[idx])
			break;
	}
	if (idx == MAX_TUNTAP_DEVICES)
		return -ENFILE;

	tun = malloc(sizeof(*tun));
	if (!tun)
		return -ENOMEM;
	memset(tun, 0, sizeof(*tun));

	char name[IFNAMSIZ];
	snprintf(name, IFNAMSIZ, "tun%d", idx);
	netdev = netdev_alloc(name, &tun_setup, 0);
	if (!netdev) {
		free(tun);
		return -ENOMEM;
	}

	mutex_init(&tun->mtx_use);
	waitq_init(&tun->wq);
	skb_queue_init(&tun->rx_q);

	tun->netdev = netdev;
	netdev->priv = tun;
	tun_devices[idx] = tun;

	idesc->priv = tun;

	idesc->idesc_ops = &tun_instance_idesc_ops;

	return 0;
}

static const struct char_dev_ops tun_dev_ops = {
    .read = tun_dev_read,
    .write = tun_dev_write,
    .open = tun_dev_open,
    .close = tun_dev_close,
    .ioctl = tun_dev_ioctl,
};
