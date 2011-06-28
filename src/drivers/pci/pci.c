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
#include <util/pool.h>

//TODO separate common and architecture pci's part
#include <asm/io.h>

EMBOX_UNIT_INIT(pci_init);

#if 0
static LIST_HEAD(root_bus);
#endif

POOL_DEF(devs_pool, struct pci_dev, 0x10);
POOL_DEF(bus_pool, struct pci_bus, 0x10);

static struct pci_bus *buses_list = NULL;


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

static int __init pci_init(void) {
	out32(PCI_CONFIG_ADDRESS, 0);
	out32(PCI_CONFIG_ADDRESS + 0x2, 0);
	if (in32(PCI_CONFIG_ADDRESS) == 0 && in32(PCI_CONFIG_ADDRESS + 0x2) == 0) {
		LOG_ERROR("PCI is not supported\n");
		return -1;
	}

	/*scan bus*/
	pci_scan_start();
	return 0;
}

static inline uint32_t pci_get_vendor_id(uint32_t bus, uint32_t devfn) {
	uint32_t vendor;
	pci_read_config32(bus, devfn, PCI_VENDOR_ID, &vendor);
	if (vendor == 0xffffffff || vendor == 0x00000000) {
		return (uint32_t)-1;
	}
	return vendor;
}

typedef struct pci_slot {
	uint8_t bus;
	uint8_t func;
	uint8_t slot;
	uint16_t ven;
	uint16_t dev;
	uint8_t base_clase;
	uint8_t subclass;
	uint8_t rev;
} pci_slot_t;

static inline int pci_get_slot_info(struct pci_slot *slot) {
	uint32_t vendor_reg;
	uint32_t devfn = slot->func;

	if (-1 == (vendor_reg = pci_get_vendor_id(slot->bus, slot->func))) {
		return -1;
	}

	slot->ven = (uint16_t)vendor_reg & 0xffff;
	slot->dev = (uint16_t)(vendor_reg >> 16) & 0xffff;

	pci_read_config8(slot->bus, devfn, PCI_BASECLASS_CODE, &slot->base_clase);
	pci_read_config8(slot->bus, devfn, PCI_SUBCLASS_CODE, &slot->subclass);
	pci_read_config8(slot->bus, devfn, PCI_REVISION_ID, &slot->rev);

	slot->func = devfn & 0x07;
	slot->slot = (devfn >> 3) & 0x1f;

	return -1;
}

static inline int pci_add_dev(struct pci_dev *pci_dev) {
	if (NULL == buses_list) {
		buses_list = pool_alloc(&bus_pool);
	}
	return 0;
}

int pci_scan_start(void) {
	int dev_cnt;
	struct pci_slot slot;
	uint32_t bus, devfn;
	struct pci_dev *new_dev;

	for (bus = 0; bus < PCI_BUS_QUANTITY; ++bus) {
		for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
			slot.bus = (uint8_t)bus;
			slot.func = (uint8_t)devfn;

			if (-1 == pci_get_slot_info(&slot)) {
				continue;
			}
			dev_cnt ++;

			/*add bus and device to list*/
			new_dev = pool_alloc(&devs_pool);
			new_dev->busn = slot.bus;
			pci_add_dev(new_dev);
		}
	}
	return 0;
}

struct pci_bus *pci_get_buses_list(void) {
	return buses_list;
}

