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
#if 0
	/* x86 version */
	uint16_t control;

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &control);
	control &= ~PCI_MSI_FLAGS_ENABLE;
	if (enable) {
		control |= PCI_MSI_FLAGS_ENABLE;
	}
	pci_write_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, control);
#else
	/* e2k version */
	uint32_t control;

	pci_read_config_dword(dev, dev->msi_cap, &control);
	control &= ~(PCI_MSI_FLAGS_ENABLE << 16);
	if (enable) {
		control |= (PCI_MSI_FLAGS_ENABLE << 16);
	}
	pci_write_config_dword(dev, dev->msi_cap, control);
#endif

}

static inline void pci_msix_clear_and_set_ctrl(struct pci_slot_dev *dev,
		uint16_t clear, uint16_t set) {
#if 0 /* x86 version */
	uint16_t ctrl;

	pci_read_config_word(dev, dev->msix_cap + PCI_MSIX_FLAGS, &ctrl);
	ctrl &= ~clear;
	ctrl |= set;
	pci_write_config_word(dev, dev->msix_cap + PCI_MSIX_FLAGS, ctrl);
#else
	/* e2k version */
	uint32_t ctrl;

	pci_read_config_dword(dev, dev->msix_cap, &ctrl);
	ctrl &= ~clear;
	ctrl |= ((uint32_t)set << 16);
	pci_write_config_dword(dev, dev->msix_cap, ctrl);
}
#endif

#endif /* SRC_DRIVERS_PCI_MSI_PCI_MSI_H_ */
