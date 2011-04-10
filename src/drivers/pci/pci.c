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

static inline const char *find_vendor_name(uint16_t ven_id) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_vendors); i++) {
		if (pci_vendors[i].ven_id == ven_id) {
			return pci_vendors[i].ven_name;
		}
	}
	return NULL;
}

static inline const char *find_device_name(uint16_t dev_id) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_devices); i++) {
		if (pci_devices[i].dev_id == dev_id) {
			return pci_devices[i].dev_name;
		}
	}
	return NULL;
}

static inline const char *find_class_name(uint8_t base, uint8_t sub) {
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

static void scan_bus(void) {
	uint32_t devfn, l, bus, slot, func;
	uint16_t vendor, device;
	uint8_t hdr_type = 0, baseclass, subclass, rev;

	TRACE("\n");
	for (bus = 0; bus < PCI_BUS_QUANTITY; ++bus) {
		for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
			func = devfn & 0x07;
			slot = (devfn >> 3) & 0x1f;
			if (func == 0) {
				pci_read_config8(bus, devfn, PCI_HEADER_TYPE, &hdr_type);
			} else if (!(hdr_type & 0x80)) {
				continue;
			}
			pci_read_config32(bus, devfn, PCI_VENDOR_ID, &l);
			if (l == 0xffffffff || l == 0x00000000) {
				hdr_type = 0;
				continue;
			}
			vendor = l & 0xffff;
			device = (l >> 16) & 0xffff;

			pci_read_config8(bus, devfn, PCI_BASECLASS_CODE, &baseclass);
			pci_read_config8(bus, devfn, PCI_SUBCLASS_CODE, &subclass);
			pci_read_config8(bus, devfn, PCI_REVISION_ID, &rev);

			TRACE("%02d:%02x.%d %s: %s %s (rev %02d)\n",
				bus, slot, func,
				find_class_name(baseclass, subclass),
				find_vendor_name(vendor),
				find_device_name(device), rev);
			//TODO:
		}
	}
}

static int __init pci_init(void) {
	out32(PCI_CONFIG_ADDRESS, 0);
	out32(PCI_CONFIG_ADDRESS + 0x2, 0);
	if (in32(PCI_CONFIG_ADDRESS) == 0 && in32(PCI_CONFIG_ADDRESS + 0x2) == 0) {
		LOG_ERROR("PCI is not supported\n");
		return -1;
	}
	scan_bus();
	return 0;
}
