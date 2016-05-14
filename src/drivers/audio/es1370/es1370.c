/**
 * @file
 *
 * @date 14.05.2016
 * @author Anton Bondarev
 */

#include <drivers/pci/pci_driver.h>

static int es1370_init(struct pci_slot_dev *pci_dev) {
	return 0;
}

PCI_DRIVER("es1370 Audio Controller", es1370_init, 0x1274, 0x5000);
