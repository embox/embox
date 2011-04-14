/**
 * @file
 * @brief Peripheral Component Interconnect
 *
 * @date 20.01.11
 * @author Nikolay Korotky
 */

#include <drivers/pci.h>
#include <lib/list.h>
#include <embox/kernel.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <asm/io.h>

EMBOX_UNIT_INIT(pci_init);

static LIST_HEAD(root_bus);

//static pci_bus_t bus_pool[PCI_BUS_QUANTITY];
//static pci_dev_t dev_pool[PCI_BUS_QUANTITY * PCI_DEV_QUANTITY];

//TODO: move to lspci cmd

static pci_vendor_t const pci_vendors[] = {
	#include <pci_vendors_table.inc>
};

static pci_device_t const pci_devices[] = {
	#include <pci_devices_table.inc>
};

static pci_baseclass_t const pci_baseclasses[] = {
	#include <pci_baseclass_table.inc>
};

static pci_subclass_t const pci_subclasses[] = {
	#include <pci_subclass_table.inc>
};

#define CONFIG_CMD(bus, dev_fn, where) \
		(0x80000000 | (bus << 16) | (dev_fn << 8) | (where & ~3))

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
				uint32_t where, uint32_t value) {
	out32(CONFIG_CMD(bus, dev_fn, where), PCI_CONFIG_ADDRESS);
	out32(value, PCI_CONFIG_DATA);
	return 0;
}

const char *find_vendor_name(uint16_t ven_id) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_vendors); i++) {
		if (pci_vendors[i].ven_id == ven_id) {
			return pci_vendors[i].ven_name;
		}
	}
	return NULL;
}

const char *find_device_name(uint16_t dev_id) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_devices); i++) {
		if (pci_devices[i].dev_id == dev_id) {
			return pci_devices[i].dev_name;
		}
	}
	return NULL;
}

const char *find_class_name(uint8_t base, uint8_t sub) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_subclasses); i++) {
		if (pci_subclasses[i].baseclass == base &&
		    pci_subclasses[i].subclass == sub) {
			return pci_subclasses[i].name;
		}
	}
	for (i = 0; i < ARRAY_SIZE(pci_baseclasses); i++) {
		if (pci_baseclasses[i].baseclass == base) {
			return pci_baseclasses[i].name;
		}
	}
	return NULL;
}

static int __init pci_init(void) {
	out32(PCI_CONFIG_ADDRESS, 0);
	out32(PCI_CONFIG_ADDRESS + 0x2, 0);
	if (in32(PCI_CONFIG_ADDRESS) == 0 && in32(PCI_CONFIG_ADDRESS + 0x2) == 0) {
		LOG_ERROR("PCI is not supported\n");
		return -1;
	}
	return 0;
}
