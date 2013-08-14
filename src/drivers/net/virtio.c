/**
 * @file
 * @brief Virtual High Performance Ethernet card
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <drivers/ethernet/virtio.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_id.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>

PCI_DRIVER("virtio", virtio_init, PCI_VENDOR_ID_VIRTIO, PCI_DEV_ID_VIRTIO_NET);

static int virtio_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

#include <kernel/printk.h>
static irq_return_t virtio_interrupt(unsigned int irq_num, void *dev_id) {
	printk("!");
	return IRQ_HANDLED;
}

#include <stdlib.h>
#include <string.h>
static int virtio_open(struct net_device *dev) {
	uint32_t queue_sz;
	void *queue;

	printk("%#x\n", virtio_load32(VIRTIO_REG_DEVICE_F, dev));
	printk("%#x\n", virtio_load32(VIRTIO_REG_GUEST_F, dev));
	printk("%#x\n", virtio_load16(VIRTIO_REG_NET_STATUS, dev));
	printk("%#hhx\n", virtio_load8(VIRTIO_REG_DEVICE_S, dev));

	printk("vq 0:\n");
	virtio_store16(0, VIRTIO_REG_QUEUE_SL, dev);
	queue_sz = virtio_load16(VIRTIO_REG_QUEUE_SZ, dev);
	printk("%#x\n", queue_sz);
	printk("%d\n", vring_size(queue_sz));
	queue = malloc(vring_size(queue_sz));
	if (queue == NULL) {
		return -ENOMEM;
	}
	memset(queue, 0, vring_size(queue_sz));
	printk("%#x\n", virtio_load32(VIRTIO_REG_QUEUE_A, dev));
	virtio_store32(((uint32_t)queue) / 4096, VIRTIO_REG_QUEUE_A, dev);
	printk("%#x\n", virtio_load32(VIRTIO_REG_QUEUE_A, dev));

	printk("vq 1:\n");
	virtio_store16(1, VIRTIO_REG_QUEUE_SL, dev);
	queue_sz = virtio_load16(VIRTIO_REG_QUEUE_SZ, dev);
	printk("%#x\n", queue_sz);
	printk("%d\n", vring_size(queue_sz));
	queue = malloc(vring_size(queue_sz));
	if (queue == NULL) {
		return -ENOMEM;
	}
	memset(queue, 0, vring_size(queue_sz));
	printk("%#x\n", virtio_load32(VIRTIO_REG_QUEUE_A, dev));
	virtio_store32(((uint32_t)queue) / 4096, VIRTIO_REG_QUEUE_A, dev);
	printk("%#x\n", virtio_load32(VIRTIO_REG_QUEUE_A, dev));


	printk("%#x\n", virtio_load8(VIRTIO_REG_ISR_S, dev));

	return 0;
}

static int virtio_stop(struct net_device *dev) {
	virtio_store16(0, VIRTIO_REG_QUEUE_SL, dev);
	free((void *)(virtio_load32(VIRTIO_REG_QUEUE_A, dev) * 4096));
	virtio_store16(1, VIRTIO_REG_QUEUE_SL, dev);
	free((void *)(virtio_load32(VIRTIO_REG_QUEUE_A, dev) * 4096));
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
