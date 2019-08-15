/**
 * @file
 *
 * @date Mar 21, 2018
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>

#include <drivers/pci/pci.h>
#include <asm/io.h>

/* Using for search PCI configuration space start position address */
#define PCI_REG_ADDR(bus, physdev, fun, where) \
	(((where) & ~3) | ((fun) << 12) | ((physdev) << 15) | ((bus) << 20))

static inline uint8_t uint32_to_uint8(uint32_t val, int offset) {
	return (uint8_t) ((val >> (offset * 8)) & 0xFF);
}

static inline uint16_t uint32_to_uint16(uint32_t val, int offset) {
	return (uint16_t) ((val >> (offset * 8)) & 0xFFFF);
}

static inline uint32_t e2k_pci_config_read(uint32_t bus, uint32_t dev_fn,
				uint32_t where, int size, void *ptr) {
	uint32_t tmp;

	/* TODO Get pmc 0xd64100 */
	/* pci_mem_core_t *ppmc = (pci_mem_core_t *) (pMACHINE->pPMC); */
	/* TODO Get SIC_RT_PCICFG_BASE */
	unsigned long pci_conf_base = 0x200000;
	uintptr_t dev_addr = PCI_REG_ADDR(bus, dev_fn >> 3, dev_fn & 0x7, where);

	pci_conf_base <<= 12;

	tmp = e2k_read32((dev_addr + pci_conf_base));

	if (tmp == 0xFFFFFFFF) {
		* (uint32_t *) ptr = PCI_VENDOR_WRONG;
		return PCIUTILS_NODATA;
	}

	if (size == 1) {
		* (uint8_t *) ptr = uint32_to_uint8(tmp, where & 3);
	} else if (size == 2) {
		* (uint16_t *) ptr = uint32_to_uint16(tmp, where & 3);
	} else {
		* (uint32_t *) ptr = tmp;
	}

	log_debug("bus %d def_fn %d where %d result 0x%X", bus, dev_fn, where, tmp);

	return PCIUTILS_SUCCESS;
}

static inline uint32_t e2k_pci_config_write(uint32_t bus, uint32_t dev_fn,
				uint32_t where, int size, uint32_t value) {
	/* TODO Get pmc 0xd64100 */
	/* pci_mem_core_t *ppmc = (pci_mem_core_t *) (pMACHINE->pPMC); */
	/* TODO Get SIC_RT_PCICFG_BASE */
	unsigned long pci_conf_base = 0x200000;
	uintptr_t dev_addr = PCI_REG_ADDR(bus, dev_fn >> 3, dev_fn & 0x7, where);

	log_error("*******************");

	e2k_write32(value, dev_addr + pci_conf_base);
/*
	ptr = (void *) (pci_conf_base + where);
	if (size == 1) {
		* (volatile uint8_t *) ptr = value;
	} else if (size == 2) {
		* (volatile uint16_t *) ptr = value;
	} else {
		* (volatile uint32_t *) ptr = value;
	}
*/
	return PCIUTILS_SUCCESS;
}
#if 0
/**
 The PCI specification defines 2 formats for the PCI Configuration addresses:
 Type 1 (for endpoints) and Type 2 (for bridges)
*/
int pci_check_type(void) {
	return 0;
}
#endif

uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value) {
	return e2k_pci_config_read(bus, dev_fn, where, 1, value);
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value) {
	return e2k_pci_config_read(bus, dev_fn, where, 2, value);
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value) {
	return e2k_pci_config_read(bus, dev_fn, where, 4, value);
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value) {
	return e2k_pci_config_write(bus, dev_fn, where, 1, value);
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value) {
	return e2k_pci_config_write(bus, dev_fn, where, 2, value);
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
		uint32_t where,	uint32_t value) {
	return e2k_pci_config_write(bus, dev_fn, where, 4, value);
}

/* NOTE: this may be inaccurate! */
unsigned int pci_irq_number(struct pci_slot_dev *dev) {
	return (unsigned int) dev->irq_line;
}
