/*
 * @file
 *
 * @date Dec 19, 2012
 * @author: Anton Bondarev
 */


//CIRRUS_LOGIC_GD_5446

//http://www.faqs.org/faqs/pc-hardware-faq/supervga-programming/

#include <drivers/pci/pci_driver.h>
#include <drivers/pci/pci_id.h>

#include <drivers/video/vga.h>
#include <drivers/video/vesa.h>



static int cirrus_init(struct pci_slot_dev *pci_dev) {
	int res = 0;
	uint32_t nic_base;
	struct net_device *nic;

	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;


	nic->netdev_ops = &_netdev_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = nic_base;

	res = irq_attach(pci_dev->irq, e1000_interrupt, 0, nic, "e1000");
	if (res < 0) {
		return res;
	}

	return netdev_register(nic);
}


VGA_MODE_DEFINE(0x114,NULL,NULL,NULL,NULL,800,600);
VGA_MODE_DEFINE(0x115,NULL,NULL,NULL,NULL,800,600);


PCI_DRIVER("cl_gd5446", cirrus_init, PCI_VENDOR_ID_CIRRUS, PCI_DEV_ID_CIRRUS_5446);
