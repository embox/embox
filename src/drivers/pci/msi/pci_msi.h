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

struct msi_desc;
struct msi_msg;

static inline void pci_msi_set_enable(struct pci_slot_dev *dev, int enable) {
	uint16_t control;

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &control);
	control &= ~PCI_MSI_FLAGS_ENABLE;
	if (enable) {
		control |= PCI_MSI_FLAGS_ENABLE;
	}
	pci_write_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, control);
}

static inline void pci_msix_clear_and_set_ctrl(struct pci_slot_dev *dev,
    uint16_t clear, uint16_t set) {
	uint16_t ctrl;

	pci_read_config_word(dev, dev->msix_cap + PCI_MSIX_FLAGS, &ctrl);
	ctrl &= ~clear;
	ctrl |= set;
	pci_write_config_word(dev, dev->msix_cap + PCI_MSIX_FLAGS, ctrl);
}

/*
 * Deliver a message to the device: the MSI capability registers for
 * legacy MSI, or the MSI-X table entry otherwise.
 */
extern void __pci_write_msi_msg(struct msi_desc *entry, struct msi_msg *msg);

#endif /* SRC_DRIVERS_PCI_MSI_PCI_MSI_H_ */
