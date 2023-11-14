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

#include <util/log.h>

#include <errno.h>
#include <string.h>

#include <util/dlist.h>
#include <util/array.h>
#include <embox/unit.h>

#include <mem/misc/pool.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/pci/pci_regs.h>

#define PCI_BUS_N_TO_SCAN OPTION_GET(NUMBER,bus_n_to_scan)
#define PCI_IRQ_BASE      OPTION_GET(NUMBER,irq_base)

EMBOX_UNIT_INIT(pci_init);

POOL_DEF(devs_pool, struct pci_slot_dev, OPTION_GET(NUMBER,dev_quantity));

/* repository */
DLIST_DEFINE(__pci_devs_list);

/* check whether correct pci device in the slot bus */
uint32_t pci_get_vendor_id(uint32_t bus, uint32_t devfn) {
	uint32_t vendor;
	pci_read_config32(bus, devfn, PCI_VENDOR_ID, &vendor);
	if ((vendor == PCI_VENDOR_NONE) || (vendor == PCI_VENDOR_WRONG)) {
		return (uint32_t) -1;
	}
	return vendor;
}

static inline int get_cap_id(struct pci_slot_dev *pci_dev, uint32_t where, uint8_t *val8) {
	int ret;

	ret = pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			where + PCI_CAP_LIST_ID, val8);
	if (PCIUTILS_SUCCESS == ret) {
		return 0;
	}
	return -1;
}

static inline int get_cap_next(struct pci_slot_dev *pci_dev, uint32_t where, uint8_t *val8) {
	int ret;

	ret = pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			where + PCI_CAP_LIST_NEXT, val8);
	if (PCIUTILS_SUCCESS == ret) {
		*val8 &= ~0x3;
		return 0;
	}
	return -1;
}

static inline int get_cap_flags(struct pci_slot_dev *pci_dev, uint32_t where, uint16_t *val16) {
	int ret;

	ret = pci_read_config16(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			where + PCI_CAP_FLAGS, val16);
	if (PCIUTILS_SUCCESS == ret) {
		return 0;
	}
	return -1;
}


static void __pci_dev_fill_caps(struct pci_slot_dev *pci_dev) {
	uint32_t where;
	uint8_t val8;


	pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			PCI_CAPABILITY_LIST, &val8);

	where = val8;

	while (where) {
		uint8_t id, next;
		uint16_t cap;

		get_cap_id(pci_dev, where, &id);
		get_cap_next(pci_dev, where, &next);
		get_cap_flags(pci_dev, where, &cap);

		if (id == 0xff) {
			break;
		}
		switch (id) {
		case PCI_CAP_ID_MSI:
			pci_dev->msi_cap = where;
			dlist_init(&pci_dev->msi_list);
			break;
		case PCI_CAP_ID_MSIX:
			pci_dev->msix_cap = where;
			dlist_init(&pci_dev->msi_list);
			break;
		default:
			break;
		}
		where = next;
	}
}

/* receive information about single slot on the pci bus */
static int pci_get_slot_info(struct pci_slot_dev *dev) {
	int bar_num;
	uint32_t devfn = dev->func;
	uint16_t status;

	pci_read_config8(dev->busn, devfn, PCI_BASECLASS_CODE, &dev->baseclass);
	pci_read_config8(dev->busn, devfn, PCI_SUBCLASS_CODE, &dev->subclass);
	pci_read_config8(dev->busn, devfn, PCI_REVISION_ID, &dev->rev);
	pci_read_config8(dev->busn, devfn, PCI_INTERRUPT_LINE, &dev->irq_line);
	pci_read_config8(dev->busn, devfn, PCI_INTERRUPT_PIN, &dev->irq_pin);

	for (bar_num = 0; bar_num < ARRAY_SIZE(dev->bar); bar_num ++) {
		pci_read_config32(dev->busn, devfn,
			PCI_BASE_ADDR_REG_0 + (bar_num << 2), &dev->bar[bar_num]);
	}
	dev->func = PCI_FUNC(devfn);
	dev->slot = PCI_SLOT(devfn);
	dev->irq = pci_irq_number(dev);

	pci_read_config16(dev->busn, devfn, PCI_STATUS, &status);
	if (status & PCI_STATUS_CAP_LIST) {
		__pci_dev_fill_caps(dev);
	}

	return 0;
}

/* global quantity founded pci devices */
static size_t dev_cnt = 0;

/* insert information about pci device into the repository */
static inline int pci_add_dev(struct pci_slot_dev *dev) {
	dlist_head_init(&dev->lst);
	dlist_add_prev(&dev->lst, &__pci_devs_list);
	dev_cnt ++;
	return 0;
}

struct pci_slot_dev *pci_insert_dev(char configured,
			uint32_t bus, uint32_t devfn, uint32_t vendor_reg) {
	struct pci_slot_dev *new_dev;

	if(NULL == (new_dev = pool_alloc(&devs_pool))) {
		log_debug("pci dev pool overflow");
		return NULL;
	}

	memset(new_dev, 0, sizeof(*new_dev));

	new_dev->busn = (uint8_t) bus;
	new_dev->func = (uint8_t) devfn;

	new_dev->vendor = (uint16_t) vendor_reg & 0xffff;
	new_dev->device = (uint16_t) (vendor_reg >> 16) & 0xffff;
	if (configured) {
		pci_get_slot_info(new_dev);
	}
	pci_add_dev(new_dev);
	log_debug("Add pci >> bc %d, sc %d, rev %d, irq %d",
			new_dev->baseclass, new_dev->subclass, new_dev->rev, new_dev->irq);
	for (int bar_num = 0; bar_num < ARRAY_SIZE(new_dev->bar); bar_num ++) {
		log_debug("bar[%d] 0x%X ", bar_num, new_dev->bar[bar_num]);
	}
	log_debug("\n fu %d, slot %d \n", new_dev->func, new_dev->slot);
	return new_dev;
}

/* collecting information about available device on the pci bus */
static int pci_scan_start(void) {
	uint32_t bus, devfn;
	uint32_t vendor_reg;
	uint8_t hdr_type, is_multi = 0;

	/*
	 * TODO need alternative list for static pci empty slot
	 * if (!dlist_empty(&__pci_devs_list)) {
	 *     return dev_cnt;
	 * }
	 */

	for (bus = 0; bus < PCI_BUS_N_TO_SCAN; ++bus) {
		for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {

			/* Devices are required to implement function 0, so if
			 * it's missing then there is no device here. */
			if (PCI_FUNC(devfn) && !is_multi) {
				/* Not a multiple function device */
				continue;
			}

			pci_read_config8(bus, devfn, PCI_HEADER_TYPE, &hdr_type);
			if (!PCI_FUNC(devfn)) {
				/* If bit 7 of this register is set, the device
				 * has multiple functions;
				 * otherwise, it is a single function device */
				is_multi = hdr_type & (1 << 7);
			}

			if (-1 == (vendor_reg = pci_get_vendor_id(bus, devfn))) {
				is_multi = 0;
				continue;
			}

			/* add bus and device to list*/
			pci_insert_dev(1, bus, devfn, vendor_reg);
		}
	}
	return dev_cnt;
}

static int pci_init(void) {
#if 0 /* TODO */
	if (0 == pci_check_type()) {
		return 0;
	}
#endif
	/* scan bus */
	pci_scan_start();

	return 0;

}

void pci_set_master(struct pci_slot_dev * slot_dev) {
	uint16_t cmd;
	uint8_t lat;
	uint16_t devfn = PCI_DEVFN(slot_dev->slot, slot_dev->func);

	pci_read_config16(slot_dev->busn, devfn, PCI_COMMAND, &cmd);
	if (!(cmd & PCI_COMMAND_MASTER)) {
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config16(slot_dev->busn, devfn, PCI_COMMAND, cmd);
	}
	pci_read_config8(slot_dev->busn, devfn, PCI_LATENCY_TIMER, &lat);
	if (lat < 16) {
		log_info("Increasing latency timer of device %02x:%02x to 64\n",
				slot_dev->busn, devfn);
		pci_write_config8(slot_dev->busn, devfn, PCI_LATENCY_TIMER, 64);
	}
}

void pci_intx(struct pci_slot_dev *pdev, int enable) {
	uint16_t pci_command, new;

	pci_read_config_word(pdev, PCI_COMMAND, &pci_command);

	if (enable) {
		new = pci_command & ~PCI_COMMAND_INTX_DISABLE;
	} else {
		new = pci_command | PCI_COMMAND_INTX_DISABLE;
	}

	if (new != pci_command) {
		pci_write_config_word(pdev, PCI_COMMAND, new);
	}
}

