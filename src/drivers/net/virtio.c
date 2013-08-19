/**
 * @file
 * @brief Virtual High Performance Ethernet card
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <drivers/ethernet/virtio.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_id.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/printk.h>

PCI_DRIVER("virtio", virtio_init, PCI_VENDOR_ID_VIRTIO, PCI_DEV_ID_VIRTIO_NET);

/* TODO device private data */
static struct vring ring[2]; /* For RX and TX */

/* TODO malloc better :) */
#define RING_SIZE 12288
static char ring_data_0[RING_SIZE] __attribute__((aligned(VRING_ALIGN_BOUND)));
static char ring_data_1[RING_SIZE] __attribute__((aligned(VRING_ALIGN_BOUND)));

static void vr_init(int id, struct net_device *dev) {
	uint16_t queue_sz;
	uint32_t ring_sz;
	void *ring_mem;

	virtio_select_queue(id, dev);

	queue_sz = virtio_get_queue_size(dev);
	ring_sz = vring_size(queue_sz);

	/* TODO alloc memory */
	ring_mem = id ? ring_data_0 : ring_data_1;
	assert(ring_sz == RING_SIZE);
	memset(ring_mem, 0, ring_sz);

	vring_init(&ring[id], queue_sz, ring_mem);
	virtio_set_queue_addr(ring_mem, dev);
}

static void vr_fini(int id) {
#if 0
	free(ring[id].desc);
#endif
}

static int virtio_xmit(struct net_device *dev, struct sk_buff *skb) {
	const int id = VIRTIO_NET_QUEUE_TX;
	int ret;
	struct virtio_net_hdr *pkt_hdr;
	static char _b[1514 + 96];

	pkt_hdr = (struct virtio_net_hdr *)&_b[0];
	pkt_hdr->flags = 0;
	pkt_hdr->gso_type = VIRTIO_NET_HDR_GSO_NONE;

	memcpy(&_b[sizeof *pkt_hdr], skb->mac.raw, skb->len);

	ret = vring_push_buff(&_b[0], sizeof *pkt_hdr + skb->len, 0, &ring[id]);
	if (ret != 0) {
		return ret;
	}

	virtio_notify_queue(id, dev);

	skb_free(skb);
	return 0;
}

static irq_return_t virtio_interrupt(unsigned int irq_num, void *dev_id) {
	static uint16_t last_rx, last_tx;
	int id;
	struct vring_used_elem *used_elem;
	struct virtio_net_hdr *pkt_hdr;
	struct sk_buff *skb;
	struct vring_desc *desc;
	unsigned int len;

	if (!(virtio_load8(VIRTIO_REG_ISR_S, dev_id) & 1)) {
		return IRQ_NONE;
	}
//	printk("!");

	id = VIRTIO_NET_QUEUE_TX;
	while (last_tx != ring[id].used->idx) {
		used_elem = &ring[id].used->ring[last_tx % ring[id].num];
		ring[id].desc[used_elem->id].addr = 0;
		++last_tx;
	}

	id = VIRTIO_NET_QUEUE_RX;
	while (last_rx != ring[id].used->idx) {
//		printk("+");
		used_elem = &ring[id].used->ring[last_rx % ring[id].num];
		desc = &ring[id].desc[used_elem->id];
		pkt_hdr = (struct virtio_net_hdr *)(uintptr_t)desc->addr;
		len = used_elem->len - sizeof *pkt_hdr;

		if ((skb = skb_alloc(len))) {
			memcpy(skb->mac.raw, pkt_hdr + 1, len);
			skb->dev = (struct net_device *)dev_id;
			netif_rx(skb);
		}

		++last_rx;
		vring_push_desc(used_elem->id, &ring[id]);
		virtio_notify_queue(id, dev_id);
	}

	return IRQ_HANDLED;
}

static int virtio_open(struct net_device *dev) {
	printk("dev_f%#x\n", virtio_load32(VIRTIO_REG_DEVICE_F, dev));
	printk("guest_f%#x\n", virtio_load32(VIRTIO_REG_GUEST_F, dev));
	printk("net_s%#x\n", virtio_load16(VIRTIO_REG_NET_STATUS, dev));
	printk("dev_s%#hhx\n", virtio_load8(VIRTIO_REG_DEVICE_S, dev));

	vr_init(VIRTIO_NET_QUEUE_RX, dev);
	vr_init(VIRTIO_NET_QUEUE_TX, dev);

	{
		const int id = VIRTIO_NET_QUEUE_RX;
		static char _b[1514 + 96];
		int ret;

		ret = vring_push_buff(&_b[0], sizeof _b, 1, &ring[id]);
		if (ret != 0) {
			return ret;
		}

		virtio_notify_queue(id, dev);
	}

	return 0;
}

static int virtio_stop(struct net_device *dev) {
	vr_fini(VIRTIO_NET_QUEUE_RX);
	vr_fini(VIRTIO_NET_QUEUE_TX);
	return 0;
}

static int virtio_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver virtio_drv_ops = {
	.xmit = virtio_xmit,
	.start = virtio_open,
	.stop = virtio_stop,
	.set_macaddr = virtio_set_macaddr
};

static void virtio_config(struct net_device *dev) {
	/* known device */
	virtio_store8(VIRTIO_CONFIG_S_ACKNOWLEDGE | VIRTIO_CONFIG_S_DRIVER,
			VIRTIO_REG_DEVICE_S, dev);

	/* load device mac */
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_MAC) {
		for (int i = 0; i < dev->addr_len; ++i) {
			dev->dev_addr[i] = virtio_load8(VIRTIO_REG_NET_MAC(i), dev);
		}
	}

	/* device is ready */
	virtio_orin8(VIRTIO_CONFIG_S_DRIVER_OK, VIRTIO_REG_DEVICE_S, dev);
}

static int virtio_init(struct pci_slot_dev *pci_dev) {
	int ret;
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &virtio_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	ret = irq_attach(nic->irq, virtio_interrupt, IF_SHARESUP, nic, "virtio");
	if (ret != 0) {
		return ret;
	}

	virtio_config(nic);

	return inetdev_register_dev(nic);
}
