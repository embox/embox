/**
 * @file
 * @brief Virtual High Performance Ethernet card
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/ethernet/virtio.h>
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
#include <util/sys_log.h>

PCI_DRIVER("virtio", virtio_init, PCI_VENDOR_ID_VIRTIO, PCI_DEV_ID_VIRTIO_NET);

#define MODOPS_PREP_BUFF_CNT OPTION_GET(NUMBER, prep_buff_cnt)

struct virtio_priv {
	struct virtqueue rq;
	struct virtqueue tq;
};

static int vq_init(struct virtqueue *vq, int id, struct net_device *dev) {
	uint16_t queue_sz;
	uint32_t ring_sz;
	void *ring_data;

	assert(vq != NULL);

	virtio_select_queue(id, dev);

	queue_sz = virtio_get_queue_size(dev);
	ring_sz = vring_size(queue_sz);

	ring_data = memalign(VRING_ALIGN_BOUND, ring_sz);
	if (ring_data == NULL) {
		return -ENOMEM;
	}
	memset(ring_data, 0, ring_sz);

	vring_init(&vq->ring, queue_sz, ring_data);
	vq->ring_data = ring_data;
	vq->last_seen_used = vq->next_free_desc = 0;

	virtio_set_queue_addr(ring_data, dev);

	return 0;
}

static void vq_fini(struct virtqueue *vq) {
	assert(vq != NULL);
	free(vq->ring_data);
}

#include <kernel/printk.h>
static int virtio_xmit(struct net_device *dev, struct sk_buff *skb) {
	int desc_id;
	struct virtio_net_hdr *pkt_hdr;
	static char _b[sizeof *pkt_hdr + ETH_FRAME_LEN];
	struct virtqueue *vq;

//	printk("xmit[%u]",skb->len);
	pkt_hdr = (struct virtio_net_hdr *)&_b[0];
	pkt_hdr->flags = 0;
	pkt_hdr->csum_start = pkt_hdr->csum_offset = 0;
	pkt_hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;
	pkt_hdr->hdr_len = pkt_hdr->gso_size = 0;
	pkt_hdr->num_buffers = 0;

	vq = &netdev_priv(dev, struct virtio_priv)->tq;
	desc_id = vq->next_free_desc;

	vq->next_free_desc = (vq->next_free_desc + 1) % vq->ring.num;
	assert(vq->ring.desc[desc_id].addr == 0); /* overflow */
	vring_desc_init(&vq->ring.desc[desc_id], pkt_hdr, sizeof *pkt_hdr,
			VRING_DESC_F_NEXT);
	vq->ring.desc[desc_id].next = desc_id + 1;

	vq->next_free_desc = (vq->next_free_desc + 1) % vq->ring.num;
	vring_desc_init(&vq->ring.desc[desc_id + 1], skb->mac.raw, skb->len, 0);

	vring_push_desc(desc_id, &vq->ring);

	virtio_notify_queue(VIRTIO_NET_QUEUE_TX, dev);

	/* wait while txing */
	while (netdev_priv(dev, struct virtio_priv)->tq.ring.desc[desc_id].addr);
//	desc_id = desc_id;

	vq->ring.desc[desc_id + 1].addr = 0;
	skb_free(skb);

	return 0;
}
/* Debugging routines */
static inline void show_packet(uint8_t *raw, int size, char *title) {
	int i;

	printk("\nPACKET(%d) %s:", size, title);
	for (i = 0; i < size; i++) {
		if (!(i % 16)) {
			printk("\n");
		}
		printk(" %02hhX", *(raw + i));
	}
	printk("\n.\n");
}
static irq_return_t virtio_interrupt(unsigned int irq_num, void *dev_id) {
	struct net_device *dev;
	struct virtqueue *vq;
	struct vring_used_elem *used_elem;
	struct sk_buff *skb;
	struct sk_buff_data *new_data;
	struct vring_desc *desc;

	dev = dev_id;

	/* it is really? */
	if (!(virtio_load8(VIRTIO_REG_ISR_S, dev) & 1)) {
		return IRQ_NONE;
	}

//	printk("!");
	/* release outgoing packets */
	vq = &netdev_priv(dev, struct virtio_priv)->tq;
	while (vq->last_seen_used != vq->ring.used->idx) {
//printk("*");
		used_elem = &vq->ring.used->ring[vq->last_seen_used % vq->ring.num];
		desc = &vq->ring.desc[used_elem->id];
//		skb_data_free((struct sk_buff_data *)(uintptr_t)desc->addr);
		desc->addr = 0;
		++vq->last_seen_used;
	}

	/* receive incoming packets */
	vq = &netdev_priv(dev, struct virtio_priv)->rq;
	while (vq->last_seen_used != vq->ring.used->idx) {
		used_elem = &vq->ring.used->ring[vq->last_seen_used % vq->ring.num];
		desc = &vq->ring.desc[used_elem->id];
		skb = skb_wrap(used_elem->len,
					(struct sk_buff_data *)(uintptr_t)desc->addr);
		if (skb == NULL) {
			LOG_ERROR("virtio_interrupt", "skb_wrap return NULL");
			break;
		}
		skb->dev = dev;
		skb->mac.raw = skb->p_data + sizeof(struct virtio_net_hdr);
		skb->len -= sizeof(struct virtio_net_hdr);
		netif_rx(skb);

		++vq->last_seen_used;
		new_data = skb_data_alloc();
		if (new_data == NULL) {
			desc->addr = 0;
			LOG_ERROR("virtio_interrupt", "skb_data_alloc return NULL");
			break;
		}
		desc->addr = (uintptr_t)new_data;
		vring_push_desc(used_elem->id, &vq->ring);
		virtio_notify_queue(VIRTIO_NET_QUEUE_RX, dev);
	}

	return IRQ_HANDLED;
}

#include <kernel/irq_lock.h>
#define TEST_F(f) \
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & f) { \
		printk(#f "is set\n"); \
	}
static int virtio_open(struct net_device *dev) {
	/* device is ready */
	virtio_orin8(VIRTIO_CONFIG_S_DRIVER_OK, VIRTIO_REG_DEVICE_S, dev);

#if 0
irq_lock();
   printk("dev_f%#x\n", virtio_load32(VIRTIO_REG_DEVICE_F, dev));
   printk("guest_f%#x\n", virtio_load32(VIRTIO_REG_GUEST_F, dev));
   printk("net_s%#x\n", virtio_load16(VIRTIO_REG_NET_STATUS, dev));
   printk("dev_s%#hhx\n", virtio_load8(VIRTIO_REG_DEVICE_S, dev));

TEST_F(VIRTIO_NET_F_CSUM)
TEST_F(VIRTIO_NET_F_GUEST_CSUM)
TEST_F(VIRTIO_NET_F_MAC)
TEST_F(VIRTIO_NET_F_GSO)
TEST_F(VIRTIO_NET_F_GUEST_TSO4)
TEST_F(VIRTIO_NET_F_GUEST_TSO6)
TEST_F(VIRTIO_NET_F_GUEST_ECN)
TEST_F(VIRTIO_NET_F_GUEST_UFO)
TEST_F(VIRTIO_NET_F_HOST_TSO4)
TEST_F(VIRTIO_NET_F_HOST_TSO6)
TEST_F(VIRTIO_NET_F_HOST_ECN)
TEST_F(VIRTIO_NET_F_HOST_UFO)
TEST_F(VIRTIO_NET_F_MRG_RXBUF)
TEST_F(VIRTIO_NET_F_STATUS)
TEST_F(VIRTIO_NET_F_CTRL_VQ)
TEST_F(VIRTIO_NET_F_CTRL_RX)
TEST_F(VIRTIO_NET_F_CTRL_VLAN)
TEST_F(VIRTIO_NET_F_CTRL_RX_EXTRA)
TEST_F(VIRTIO_NET_F_GUEST_ANNOUNCE)
TEST_F(VIRTIO_NET_F_MQ)
TEST_F(VIRTIO_NET_F_CTRL_MAC_ADDR)
irq_unlock();
#endif
	return 0;
}

static int virtio_stop(struct net_device *dev) {
	/* device is not ready */
	virtio_andin8(~VIRTIO_CONFIG_S_DRIVER_OK, VIRTIO_REG_DEVICE_S, dev);
	return 0;
}

static int virtio_set_macaddr(struct net_device *dev, const void *addr) {
	unsigned char i;

	/* setup mac */
	for (i = 0; i < dev->addr_len; ++i) {
		virtio_store8(dev->dev_addr[i], VIRTIO_REG_NET_MAC(i), dev);
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

	/* it's known device */
	virtio_store8(VIRTIO_CONFIG_S_ACKNOWLEDGE | VIRTIO_CONFIG_S_DRIVER,
			VIRTIO_REG_DEVICE_S, dev);

	/* load device mac */
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_MAC) {
		for (i = 0; i < dev->addr_len; ++i) {
			dev->dev_addr[i] = virtio_load8(VIRTIO_REG_NET_MAC(i), dev);
		}
	}

#if 0
	/* negotiate CSUM bit */
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_CSUM) {
		virtio_store32(VIRTIO_NET_F_CSUM, VIRTIO_REG_GUEST_F, dev);
		virtio_store32(VIRTIO_NET_F_HOST_TSO4, VIRTIO_REG_GUEST_F, dev);
		virtio_store32(VIRTIO_NET_F_HOST_TSO6, VIRTIO_REG_GUEST_F, dev);
		virtio_store32(VIRTIO_NET_F_HOST_ECN, VIRTIO_REG_GUEST_F, dev);
		virtio_store32(VIRTIO_NET_F_HOST_UFO, VIRTIO_REG_GUEST_F, dev);
	}

	/* negotiate MAC bit */
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_MAC) {
		virtio_store32(VIRTIO_NET_F_MAC, VIRTIO_REG_GUEST_F, dev);
	}
#endif

	/* negotiate MSG_RXBUF bit */
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_MRG_RXBUF) {
		virtio_store32(VIRTIO_NET_F_MRG_RXBUF, VIRTIO_REG_GUEST_F, dev);
	}
	else {
		assert(0);
	}
}

static void virtio_priv_fini(struct virtio_priv *dev_priv) {
	int i;
	struct vring_desc *desc;

	/* free receive buffers */
	for (i = 0, desc = &dev_priv->rq.ring.desc[0];
			i < dev_priv->rq.ring.num;
			++i, ++desc) {
		if (desc->addr != 0) {
			skb_data_free((struct sk_buff_data *)(uintptr_t)desc->addr);
		}
	}

	/* free virtqueues */
	vq_fini(&dev_priv->rq);
	vq_fini(&dev_priv->tq);
}

static int virtio_priv_init(struct virtio_priv *dev_priv,
		struct net_device *dev) {
	int ret, i;
	struct sk_buff_data *skb_data;

	/* init receive queue */
	ret = vq_init(&dev_priv->rq, VIRTIO_NET_QUEUE_RX, dev);
	if (ret != 0) {
		return ret;
	}

	/* init transmit queue */
	ret = vq_init(&dev_priv->tq,
			VIRTIO_NET_QUEUE_TX, dev);
	if (ret != 0) {
		vq_fini(&dev_priv->rq);
		return ret;
	}

	/* add receive buffer */
	for (i = 0; i < MODOPS_PREP_BUFF_CNT; ++i) {
		skb_data = skb_data_alloc();
		if (skb_data == NULL) {
			virtio_priv_fini(dev_priv);
			return -ENOMEM;
		}
		ret = virtqueue_push_buff(skb_data, skb_max_size(), 1,
				&dev_priv->rq);
		if (ret != 0) {
			virtio_priv_fini(dev_priv);
			return ret;
		}
	}
	virtio_notify_queue(VIRTIO_NET_QUEUE_RX, dev);

	return 0;
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
		virtio_priv_fini(nic_priv);
		return ret;
	}

	return inetdev_register_dev(nic);
}
