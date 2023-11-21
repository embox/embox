/**
 * @file
 *
 * @date Feb 10, 2022
 * @author Anton Bondarev
 */

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>

int pci_read_config_byte(struct pci_slot_dev *dev, int where, uint8_t *val) {
	int ret;

	ret = pci_read_config8(dev->busn, (dev->slot << 3) | dev->func, where, val);

	return ret;
}

int pci_read_config_word(struct pci_slot_dev *dev, int where, uint16_t *val) {
	int ret;

	ret = pci_read_config16(dev->busn, (dev->slot << 3) | dev->func, where,
	    val);

	return ret;
}

int pci_read_config_dword(struct pci_slot_dev *dev, int where, uint32_t *val) {
	int ret;

	ret = pci_read_config32(dev->busn, (dev->slot << 3) | dev->func, where,
	    val);

	return ret;
}

int pci_write_config_byte(struct pci_slot_dev *dev, int where, uint8_t val) {
	int ret;
	const uint8_t devfn = PCI_DEVFN(dev->slot, dev->func);

	ret = pci_write_config8(dev->busn, devfn, where, val);

	return ret;
}

int pci_write_config_word(struct pci_slot_dev *dev, int where, uint16_t val) {
	int ret;
	const uint8_t devfn = PCI_DEVFN(dev->slot, dev->func);

	ret = pci_write_config16(dev->busn, devfn, where, val);

	return ret;
}

int pci_write_config_dword(struct pci_slot_dev *dev, int where, uint32_t val) {
	int ret;
	const uint8_t devfn = PCI_DEVFN(dev->slot, dev->func);

	ret = pci_write_config32(dev->busn, devfn, where, val);

	return ret;
}
