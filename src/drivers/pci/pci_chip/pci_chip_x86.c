/*
 * @file
 *
 * @date Apr 2, 2013
 * @author: Anton Bondarev
 */


#include <stdint.h>
#include <drivers/pci/pci.h>


#include <asm/io.h>

/**
 * PCI access via x86 I/O address space
 */
enum {
	/**
	 * Address of the device's register
	 * +------+--------+-----+------+----+---------+-+-+
	 * |31    |30    24|23 16|15  11|10 8|7       2|1|0|
	 * +------+--------+-----+------+----+---------+-+-+
	 * |access|reserved|bus  |device|func|reg index|0|0|
	 * +------+--------+-----+------+----+---------+-+-+
	 */
	PCI_CONFIG_ADDRESS    = 0xCF8,
	/** Data that is supposed to be written to the device */
	PCI_CONFIG_DATA       = 0xCFC
};

#define PCI_CONFIG_CMD(bus, dev_fn, where) \
		(0x80000000 | (bus << 16) | (dev_fn << 8) | (where & ~3))

int pci_is_supported(void) {
	out32(PCI_CONFIG_ADDRESS, 0);
	out32(PCI_CONFIG_ADDRESS + 0x2, 0);
	if (in32(PCI_CONFIG_ADDRESS) == 0 && in32(PCI_CONFIG_ADDRESS + 0x2) == 0) {
		return -1; //TODO this is a bolean function
	}
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value) {
	out32(PCI_CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	*value = in8(PCI_CONFIG_DATA + (where & 3));
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value) {
	out32(PCI_CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	/* Change the selection bits in a double word from 2nd to 1st */
	*value = in16(PCI_CONFIG_DATA + (where & 1));
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value) {
	out32(PCI_CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	*value = in32(PCI_CONFIG_DATA);
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value) {
	out32(PCI_CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	out8(value, PCI_CONFIG_DATA + (where & 3));
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value) {
	out32(PCI_CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	/* Change the selection bits in a double word from 2nd to 1st */
	out16(value, PCI_CONFIG_DATA + (where & 1));
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
		uint32_t where,	uint32_t value) {
	out32(PCI_CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	out32(value, PCI_CONFIG_DATA);
	return PCIUTILS_SUCCESS;
}
