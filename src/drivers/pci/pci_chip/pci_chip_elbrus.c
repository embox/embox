/**
 * @file
 *
 * @date Mar 21, 2018
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <drivers/pci/pci.h>


/**
 The PCI specification defines 2 formats for the PCI Configuration addresses:
 Type 1 (for endpoints) and Type 2 (for bridges)
*/
int pci_check_type(void) {
	return 0;
}

uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value) {
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value) {
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value) {
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value) {
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value) {
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
		uint32_t where,	uint32_t value) {
	return PCIUTILS_SUCCESS;
}
