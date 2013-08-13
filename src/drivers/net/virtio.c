/**
 * @file
 * @brief Virtual High Performance Ethernet card
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#include <asm/io.h>
#include <errno.h>
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

static int virtio_open(struct net_device *dev) {
	printk("%lx %#hhx\n", dev->base_addr, in8(dev->base_addr + 0x12));
	out8(1, dev->base_addr + 0x12);
	printk("%#hhx\n", in8(dev->base_addr + 0x12));
	out8(2, dev->base_addr + 0x12);
	printk("%#hhx\n", in8(dev->base_addr + 0x12));
	out8(4, dev->base_addr + 0x12);
	printk("%#hhx\n", in8(dev->base_addr + 0x12));
	return 0;
}

static int virtio_stop(struct net_device *dev) {
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

	return inetdev_register_dev(nic);
}
