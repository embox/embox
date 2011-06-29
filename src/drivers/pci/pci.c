/**
 * @file
 * @brief Peripheral Component Interconnect
 *
 * @date 20.01.11
 * @author Nikolay Korotky
 */

#include <util/slist.h>
#include <util/array.h>
#include <util/pool.h>
#include <embox/unit.h>

#include <drivers/pci.h>

//TODO separate common and architecture pci's part
#include <asm/io.h>

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

POOL_DEF(devs_pool, struct pci_dev, 0x10);

struct slist __pci_devs_list = SLIST_INIT(&__pci_devs_list);

static int pci_init(void) {
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

static inline int pci_get_slot_info(struct pci_dev *dev) {
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

static int dev_cnt = 0;

static inline int pci_add_dev(struct pci_dev *dev) {
	slist_link_init(&dev->lst);
	slist_add_first(dev, &__pci_devs_list, lst);
	dev_cnt ++;
	return 0;
}

int pci_scan_start(void) {
	uint32_t bus, devfn;
	struct pci_dev *new_dev;

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

struct pci_dev *pci_find_dev(uint16_t ven_id, uint16_t dev_id) {
	struct pci_dev *dev;
	pci_foreach_dev(dev) {
		if(ven_id == dev->vendor && dev_id == dev->device) {
			return dev;
		}
	}
	return NULL;
}

