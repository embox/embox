/**
 * @file
 * @brief Virtual High Performance Ethernet card
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/virtio/virtio.h>
#include <drivers/virtio/virtio_ring.h>
#include <drivers/virtio/virtio_queue.h>
#include <drivers/net/virtio_net.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_id.h>
#include <drivers/pci/pci_driver.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <stdlib.h>
#include <string.h>
#include <util/log.h>
#include <kernel/sched/sched_lock.h>

PCI_DRIVER("virtio", virtio_init, PCI_VENDOR_ID_VIRTIO, PCI_DEV_ID_VIRTIO_NET);

#define MODOPS_PREP_BUFF_CNT OPTION_GET(NUMBER, prep_buff_cnt)

struct virtio_priv {
	struct virtqueue rq;
	struct virtqueue tq;
};

static int virtio_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct sk_buff_extra *skb_extra;
	struct sk_buff_data *skb_data;
	struct virtqueue *vq;
	struct virtio_net_hdr *hdr;
	uint32_t desc_id;
	struct vring_desc *desc;

	assert(dev != NULL);
	assert(skb != NULL);

	skb_extra = skb_extra_alloc();
	if (skb_extra == NULL) {
		return -ENOMEM;
	}

	skb_data = skb_data_clone(skb->data);
	if (skb_data == NULL) {
		skb_extra_free(skb_extra);
		return -ENOMEM;
	}

	vq = &netdev_priv(dev, struct virtio_priv)->tq;

	hdr = skb_extra_cast_in(skb_extra);
	hdr->flags = 0;
	hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;

	sched_lock();
	{
		desc_id = vq->next_free_desc;
		do { desc = virtqueue_alloc_desc(vq); } while (desc == NULL);
		vring_desc_init(desc, hdr, sizeof *hdr, VRING_DESC_F_NEXT);
		desc->next = vq->next_free_desc;

		do { desc = virtqueue_alloc_desc(vq); } while (desc == NULL);
		vring_desc_init(desc, skb_data_cast_in(skb_data), skb->len, 0);

		vring_push_desc(desc_id, &vq->ring);
	}
	sched_unlock();

	skb_free(skb);

	virtio_net_notify_queue(VIRTIO_NET_QUEUE_TX, dev);

	return 0;
}

static irq_return_t virtio_interrupt(unsigned int irq_num,
		void *dev_id) {
	struct net_device *dev;
	struct virtqueue *vq;
	struct vring_used_elem *used_elem;
	struct sk_buff *skb;
	struct sk_buff_data *new_data;
	struct vring_desc *desc, *next;

	dev = dev_id;

	/* it is really? */
	if (~virtio_net_get_isr_status(dev) & 1) {
		return IRQ_NONE;
	}

	/* release outgoing packets */
	vq = &netdev_priv(dev, struct virtio_priv)->tq;
	while (vq->last_seen_used != vq->ring.used->idx) {
		used_elem = &vq->ring.used->ring[vq->last_seen_used % vq->ring.num];

		desc = &vq->ring.desc[used_elem->id];
		skb_extra_free(skb_extra_cast_out((void *)(uintptr_t)desc->addr));
		desc->addr = 0;
		assert(desc->flags & VRING_DESC_F_NEXT);

		next = &vq->ring.desc[desc->next];
		skb_data_free(skb_data_cast_out((void *)(uintptr_t)next->addr));
		next->addr = 0;
		assert(~next->flags & VRING_DESC_F_NEXT);

		++vq->last_seen_used;
	}

	/* receive incoming packets */
	vq = &netdev_priv(dev, struct virtio_priv)->rq;
	while (vq->last_seen_used != vq->ring.used->idx) {
		used_elem = &vq->ring.used->ring[vq->last_seen_used % vq->ring.num];

		desc = &vq->ring.desc[used_elem->id];
		assert(desc->flags & VRING_DESC_F_NEXT);

		next = &vq->ring.desc[desc->next];
		assert(~next->flags & VRING_DESC_F_NEXT);

		skb = skb_wrap(used_elem->len - sizeof(struct virtio_net_hdr),
				skb_data_cast_out((void *)(uintptr_t)next->addr));
		if (skb == NULL) {
			log_error("skb_wrap return NULL");
			break;
		}
		skb->dev = dev;
		netif_rx(skb);

		++vq->last_seen_used;

		new_data = skb_data_alloc(skb_max_size());
		if (new_data == NULL) {
			skb_extra_free(skb_extra_cast_out((void *)(uintptr_t)desc->addr));
			desc->addr = next->addr = 0;
			log_error("skb_data_alloc return NULL");
			break;
		}

		/* desc->addr = desc->addr; -- the same */
		next->addr = (uintptr_t)skb_data_cast_in(new_data);

		vring_push_desc(used_elem->id, &vq->ring);
		virtio_net_notify_queue(VIRTIO_NET_QUEUE_RX, dev);
	}

	return IRQ_HANDLED;
}

static int virtio_open(struct net_device *dev) {
	/* device is ready */
	virtio_net_add_status(VIRTIO_CONFIG_S_DRIVER_OK, dev);
	return 0;
}

static int virtio_stop(struct net_device *dev) {
	/* device is not ready */
	virtio_net_del_status(VIRTIO_CONFIG_S_DRIVER_OK, dev);
	return 0;
}

static int virtio_set_macaddr(struct net_device *dev, const void *addr) {
	unsigned char i;

	/* setup MAC-address */
	for (i = 0; i < dev->addr_len; ++i) {
		virtio_net_set_mac(dev->dev_addr[i], i, dev);
	}

	return 0;
}

static const struct net_driver virtio_drv_ops = {
	.xmit = virtio_xmit,
	.start = virtio_open,
	.stop = virtio_stop,
	.set_macaddr = virtio_set_macaddr
};

static void virtio_config(struct net_device *dev) {
	unsigned char i;
	uint32_t guest_features;

	/* check extra header size */
	assert(skb_extra_max_size() >= sizeof(struct virtio_net_hdr));

	/* reset device */
	virtio_net_reset(dev);

	/* it's known device */
	virtio_net_add_status(VIRTIO_CONFIG_S_ACKNOWLEDGE
			| VIRTIO_CONFIG_S_DRIVER, dev);

	guest_features = 0;

	/* load device MAC-address and negotiate MAC bit */
	if (virtio_net_has_feature(VIRTIO_NET_F_MAC, dev)) {
		for (i = 0; i < dev->addr_len; ++i) {
			dev->dev_addr[i] = virtio_net_get_mac(i, dev);
		}
		guest_features |= VIRTIO_NET_F_MAC;
	}

	/* negotiate STATUS bit */
	if (virtio_net_has_feature(VIRTIO_NET_F_STATUS, dev)) {
		if (virtio_net_get_status(dev) & VIRTIO_NET_S_LINK_UP) {
			netdev_flag_up(dev, IFF_RUNNING);
		}
		guest_features |= VIRTIO_NET_F_STATUS;
	}

	/* finalize guest features bits */
	virtio_net_set_feature(guest_features, dev);
}

static void virtio_priv_fini(struct virtio_priv *dev_priv,
		struct net_device *dev) {
	struct virtqueue *vq;
	struct vring_desc *desc;

	/* free transmit queue */
	vq = &dev_priv->tq;
	for (desc = &vq->ring.desc[0];
			desc < &vq->ring.desc[vq->ring.num]; ++desc) {
		if (desc->addr != 0) {
			skb_extra_free(skb_extra_cast_out((void *)(uintptr_t)desc->addr));
			desc->addr = 0;
			assert(desc->flags & VRING_DESC_F_NEXT);

			assert(desc + 1 == &vq->ring.desc[desc->next]);
			++desc;
			skb_data_free(skb_data_cast_out((void *)(uintptr_t)desc->addr));
			desc->addr = 0;
			assert(~desc->flags & VRING_DESC_F_NEXT);
		}
	}
	virtqueue_net_destroy(vq, dev);

	/* free receive queue */
	vq = &dev_priv->rq;
	for (desc = &vq->ring.desc[0];
			desc < &vq->ring.desc[vq->ring.num]; ++desc) {
		if (desc->addr != 0) {
			skb_extra_free(skb_extra_cast_out((void *)(uintptr_t)desc->addr));
			desc->addr = 0;
			assert(desc->flags & VRING_DESC_F_NEXT);

			assert(desc + 1 == &vq->ring.desc[desc->next]);
			++desc;
			skb_data_free(skb_data_cast_out((void *)(uintptr_t)desc->addr));
			desc->addr = 0;
			assert(~desc->flags & VRING_DESC_F_NEXT);
		}
	}
	virtqueue_net_destroy(vq, dev);
}

static int virtio_priv_init(struct virtio_priv *dev_priv,
		struct net_device *dev) {
	int ret, i;
	struct sk_buff_extra *skb_extra;
	struct sk_buff_data *skb_data;
	struct virtqueue *vq;
	uint32_t desc_id;
	struct vring_desc *desc;

	/* init receive queue */
	ret = virtqueue_net_create(&dev_priv->rq, VIRTIO_NET_QUEUE_RX, dev);
	if (ret != 0) {
		return ret;
	}

	/* init transmit queue */
	ret = virtqueue_net_create(&dev_priv->tq,
			VIRTIO_NET_QUEUE_TX, dev);
	if (ret != 0) {
		virtqueue_net_destroy(&dev_priv->rq, dev);
		return ret;
	}

	/* add receive buffer */
	vq = &dev_priv->rq;
	if (MODOPS_PREP_BUFF_CNT * 2 > vq->ring.num) goto out_nomem;

	for (i = 0; i < MODOPS_PREP_BUFF_CNT; ++i) {
		desc_id = vq->next_free_desc;
		desc = virtqueue_alloc_desc(vq);
		if (desc == NULL) goto out_nomem;

		skb_extra = skb_extra_alloc();
		if (skb_extra == NULL) goto out_nomem;

		vring_desc_init(desc, skb_extra_cast_in(skb_extra),
				sizeof(struct virtio_net_hdr),
				VRING_DESC_F_WRITE | VRING_DESC_F_NEXT);
		desc->next = vq->next_free_desc;

		desc = virtqueue_alloc_desc(vq);
		if (desc == NULL) goto out_nomem;

		skb_data = skb_data_alloc(skb_max_size());
		if (skb_data == NULL) goto out_nomem;

		vring_desc_init(desc,
				skb_data_cast_in(skb_data), skb_max_size(),
				VRING_DESC_F_WRITE);

		vring_push_desc(desc_id, &vq->ring);
	}
	virtio_net_notify_queue(VIRTIO_NET_QUEUE_RX, dev);

	return 0;

out_nomem:
	virtio_priv_fini(dev_priv, dev);
	return -ENOMEM;
}

static int virtio_init(struct pci_slot_dev *pci_dev) {
	int ret;
	struct net_device *nic;
	struct virtio_priv *nic_priv;

	nic = etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &virtio_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;
	nic_priv = netdev_priv(nic, struct virtio_priv);

	virtio_config(nic);

	ret = virtio_priv_init(nic_priv, nic);
	if (ret != 0) {
		return ret;
	}

	ret = irq_attach(nic->irq, virtio_interrupt, IF_SHARESUP, nic, "virtio");
	if (ret != 0) {
		virtio_priv_fini(nic_priv, nic);
		return ret;
	}

	return inetdev_register_dev(nic);
}
