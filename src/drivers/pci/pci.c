/**
 * @file
 * @brief Peripheral Component Interconnect
 *
 * @date 20.01.11
 * @author Nikolay Korotky
 */

#include <lib/list.h>
#include <util/array.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <util/pool.h>
#include <drivers/pci.h>

//TODO separate common and architecture pci's part
#include <asm/io.h>

EMBOX_UNIT_INIT(pci_init);

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

POOL_DEF(devs_pool, struct pci_dev, 0x10);
POOL_DEF(bus_pool, struct pci_bus, 0x10);

static struct pci_bus *buses_list = NULL;

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
	static int dev_cnt = 0;
	struct pci_slot slot;
	uint32_t bus, devfn;
	struct pci_dev *new_dev;

	if (NULL != buses_list) {
		return dev_cnt;
	}

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
	return dev_cnt;
}

struct pci_bus *pci_get_buses_list(void) {
	return buses_list;
}

