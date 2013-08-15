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
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/printk.h>

PCI_DRIVER("virtio", virtio_init, PCI_VENDOR_ID_VIRTIO, PCI_DEV_ID_VIRTIO_NET);

#define VQS_SZ 2
static struct {
	struct vring vr;
	void *vr_storage;
} vqs[VQS_SZ];

/* FIXME */
#define QUEUE_SIZE 256
#define RING_SIZE 12288
static char vqs_storage_0[RING_SIZE] __attribute__((aligned(ALIGN_BOUND)));
static char vqs_storage_1[RING_SIZE] __attribute__((aligned(ALIGN_BOUND)));
static char vr_desc_buff[VQS_SZ][1514];

static void vq_init(int id, struct net_device *dev) {
	uint32_t queue_sz, ring_sz;
	void *ring_mem;

	printk("vq %d:\n", id);
	assert(id < VQS_SZ);
	assert(vqs[id].vr_storage ==  NULL);

	virtio_store16(id, VIRTIO_REG_QUEUE_SL, dev);
	queue_sz = virtio_load16(VIRTIO_REG_QUEUE_SZ, dev);
	ring_sz = vring_size(queue_sz);

	assert(queue_sz == QUEUE_SIZE);
	assert(ring_sz == RING_SIZE);

#if 0
	ring_mem = malloc(vring_size(queue_sz));
#else
	ring_mem = id ? vqs_storage_1 : vqs_storage_0;
#endif
	assert(ring_mem != NULL); /* FIXME */
	memset(ring_mem, 0, ring_sz);

	printk("\t ring_mem %p\n", ring_mem);

	virtio_store32((uintptr_t)ring_mem / ALIGN_BOUND, VIRTIO_REG_QUEUE_A, dev);

	vring_init(&vqs[id].vr, queue_sz, ring_mem);
	vqs[id].vr_storage = ring_mem;
}

static void vq_fini(int id) {
	assert(id < VQS_SZ);
	assert(vqs[id].vr_storage != NULL);
#if 0
	free(vqs[id].vr_storage);
#endif
	vqs[id].vr_storage = NULL;
}

#include <linux/compiler.h>
static void vq_add_buff(int id, struct net_device *dev) {
	assert(id < VQS_SZ);
	vring_desc_init(&vqs[id].vr.desc[0], &vr_desc_buff[id][0], 1514);
	vqs[id].vr.avail->ring[vqs[id].vr.avail->idx] = 0;
	__barrier();
	vqs[id].vr.avail->idx++;
	__barrier();
	virtio_store16(id, VIRTIO_REG_QUEUE_N, dev);
}

static int virtio_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static irq_return_t virtio_interrupt(unsigned int irq_num, void *dev_id) {
	printk("!");
	return IRQ_HANDLED;
}

static int virtio_open(struct net_device *dev) {
	printk("%#x\n", virtio_load32(VIRTIO_REG_DEVICE_F, dev));
	printk("%#x\n", virtio_load32(VIRTIO_REG_GUEST_F, dev));
	printk("%#x\n", virtio_load16(VIRTIO_REG_NET_STATUS, dev));
	printk("%#hhx\n", virtio_load8(VIRTIO_REG_DEVICE_S, dev));

	vq_init(VIRTIO_NET_QUEUE_RX, dev);
	vq_init(VIRTIO_NET_QUEUE_TX, dev);

	printk("%#x\n", virtio_load8(VIRTIO_REG_ISR_S, dev));

	vq_add_buff(VIRTIO_NET_QUEUE_RX, dev);
	vq_add_buff(VIRTIO_NET_QUEUE_TX, dev);

	return 0;
}

static int virtio_stop(struct net_device *dev) {
	vq_fini(VIRTIO_NET_QUEUE_RX);
	vq_fini(VIRTIO_NET_QUEUE_TX);
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

	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_MAC)
		printk("VIRTIO_NET_F_MAC is set\n");
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_CTRL_VQ)
		printk("VIRTIO_NET_F_CTRL_VQ is set\n");
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_STATUS)
		printk("VIRTIO_NET_F_STATUS is set\n");
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_CSUM)
		printk("VIRTIO_NET_F_CSUM is set\n");
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_GUEST_CSUM)
		printk("VIRTIO_NET_F_GUEST_CSUM is set\n");
	if (virtio_load32(VIRTIO_REG_DEVICE_F, dev) & VIRTIO_NET_F_MRG_RXBUF) {
		printk("VIRTIO_NET_F_MRG_RXBUF is set\n");
		virtio_store32(VIRTIO_NET_F_MRG_RXBUF, VIRTIO_REG_GUEST_F, dev);
	}

	/* device is ready */
	virtio_orin8(VIRTIO_CONFIG_S_DRIVER_OK, VIRTIO_REG_DEVICE_S, dev);
}

static int virtio_init(struct pci_slot_dev *pci_dev) {
	int ret;
	struct net_device *nic;

	nic = etherdev_alloc();
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
