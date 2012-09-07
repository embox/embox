/**
 * @file
 * @brief Peripheral Component Interconnect (PCI) bus module
 *
 * @details This module scans PCI bus when system starting, and loads every
 *          available PCI drivers. When scan active the module collects slots
 *          information in a single linked list and then required drivers can
 *          be loaded manually.
 *
 * @date 20.01.11
 * @author Nikolay Korotky
 */

#include <util/slist.h>
#include <util/array.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>

#include <drivers/pci.h>

EMBOX_UNIT_INIT(pci_init);

typedef struct pci_slot {
	uint8_t bus;
	uint8_t slot;
	uint8_t func;
	uint16_t ven;
	uint16_t dev;
	uint8_t base_clase;
	uint8_t subclass;
	uint8_t rev;
	uint8_t irq;
	uint32_t bar[6];
} pci_slot_t;

POOL_DEF(devs_pool, struct pci_slot_dev, OPTION_GET(NUMBER,dev_quantity));

/* repository */
struct slist __pci_devs_list = SLIST_INIT(&__pci_devs_list);

/* check whether correct pci device in the slot bus */
static uint32_t pci_get_vendor_id(uint32_t bus, uint32_t devfn) {
	uint32_t vendor;
	pci_read_config32(bus, devfn, PCI_VENDOR_ID, &vendor);
	if ((vendor == PCI_VENDOR_NONE) || (vendor == PCI_VENDOR_WRONG)) {
		return (uint32_t)-1;
	}
	return vendor;
}

/* receive information about single slot on the pci bus */
static int pci_get_slot_info(struct pci_slot_dev *dev) {
	int bar_num;
	uint32_t devfn = dev->func;

	pci_read_config8(dev->busn, devfn, PCI_BASECLASS_CODE, &dev->baseclass);
	pci_read_config8(dev->busn, devfn, PCI_SUBCLASS_CODE, &dev->subclass);
	pci_read_config8(dev->busn, devfn, PCI_REVISION_ID, &dev->rev);
	pci_read_config8(dev->busn, devfn, PCI_INTERRUPT_LINE, &dev->irq);

	for(bar_num = 0; bar_num < ARRAY_SIZE(dev->bar); bar_num ++) {
		pci_read_config32(dev->busn, devfn, PCI_BASE_ADDR_REG_0 + (bar_num << 2),
						&dev->bar[bar_num]);
	}
	dev->func = devfn & 0x07;
	dev->slot = (devfn >> 3) & 0x1f;

	return 0;
}
/* global quantity founded pci devices */
static int dev_cnt = 0;

/* insert information about pci device into the repository */
static inline int pci_add_dev(struct pci_slot_dev *dev) {
	slist_link_init(&dev->lst);
	slist_add_first(dev, &__pci_devs_list, lst);
	dev_cnt ++;
	return 0;
}

/* collecting information about available device on the pci bus */
static int pci_scan_start(void) {
	uint32_t bus, devfn;
	struct pci_slot_dev *new_dev;

	if (!slist_empty(&__pci_devs_list)) {
		return dev_cnt;
	}

	for (bus = 0; bus < PCI_BUS_QUANTITY; ++bus) {
		for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
			uint32_t vendor_reg;
			if (-1 == (vendor_reg = pci_get_vendor_id(bus, devfn))) {
				continue;
			}

			/*add bus and device to list*/
			new_dev = pool_alloc(&devs_pool);
			new_dev->busn = (uint8_t)bus;
			new_dev->func = (uint8_t)devfn;

			new_dev->vendor = (uint16_t)vendor_reg & 0xffff;
			new_dev->device = (uint16_t)(vendor_reg >> 16) & 0xffff;

			pci_get_slot_info(new_dev);

			pci_add_dev(new_dev);
		}
	}
	return dev_cnt;
}

/* load every available pci driver */
static void pci_load(void) {
	struct pci_slot_dev *dev;
	pci_foreach_dev(dev) {
		pci_driver_load(dev);
	}
}

static int pci_init(void) {
	if(-1 == pci_is_supported()) {
		return 0;
	}
	/* scan bus */
	pci_scan_start();

	/* load all available pci drivers */
	pci_load();

	return 0;
}
