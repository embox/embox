/**
 * @file
 *
 * @brief
 *
 * @date 28.06.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <drivers/pci.h>

//TODO separate common and architecture pci's part
#include <asm/io.h>

#define CONFIG_CMD(bus, dev_fn, where) \
		(0x80000000 | (bus << 16) | (dev_fn << 8) | (where & ~3))

int pci_is_supported(void) {
	out32(PCI_CONFIG_ADDRESS, 0);
	out32(PCI_CONFIG_ADDRESS + 0x2, 0);
	if (in32(PCI_CONFIG_ADDRESS) == 0 && in32(PCI_CONFIG_ADDRESS + 0x2) == 0) {
		LOG_ERROR("PCI is not supported\n");
		return -1;
	}
	return 0;
}
uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	*value = in8(PCI_CONFIG_DATA + (where & 3));
	return 0;
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	*value = in16(PCI_CONFIG_DATA + (where & 1));  /* Change the selection bits in a double word from 2nd to 1st */
	return 0;
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	*value = in32(PCI_CONFIG_DATA);
	return 0;
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	out8(value, PCI_CONFIG_DATA + (where & 3));
	return 0;
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	out16(value, PCI_CONFIG_DATA + (where & 1)); /* Change the selection bits in a double word from 2nd to 1st */
	return 0;
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
		uint32_t where,	uint32_t value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	out32(value, PCI_CONFIG_DATA);
	return 0;
}
