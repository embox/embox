/**
 * @file
 *
 * @date Feb 10, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_PCI_MSI_PCI_MSI_H_
#define SRC_DRIVERS_PCI_MSI_PCI_MSI_H_

#include <stdint.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_regs.h>

static inline void pci_msi_set_enable(struct pci_slot_dev *dev, int enable) {
	uint16_t control;

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &control);
	control &= ~PCI_MSI_FLAGS_ENABLE;
	if (enable) {
		control |= PCI_MSI_FLAGS_ENABLE;
	}
	pci_write_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, control);
}

#endif /* SRC_DRIVERS_PCI_MSI_PCI_MSI_H_ */
