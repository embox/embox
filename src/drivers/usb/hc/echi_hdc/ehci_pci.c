/*
 * @file
 *
 * @date Aug 13, 2019
 * @author Anton Bondarev
 */
#include <sys/mman.h>

#include <kernel/irq.h>

#include <mem/misc/pool.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/usb/usb.h>

#include "ehci.h"

static int ehci_pci_verify(struct ehci_caps *base) {
	return 1;
}

static int ehci_pci_init(struct pci_slot_dev *pci_dev) {
	struct ehci_caps *base = (struct ehci_caps *) pci_dev->bar[0];

	base = (struct ehci_caps *)mmap_device_memory(base, 0x1000,
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			(uintptr_t)base);

	if (!ehci_pci_verify(base)) {
		return -ENOTSUP;
	}

	return ehci_hcd_register(base, pci_dev->irq);
}

PCI_DRIVER("USB2 EHCI Controller", ehci_pci_init, PCI_VENDOR_ID_INTEL,
		PCI_DEV_ID_INTEL_82801DB_USB_EHCI);
