/**
 * @file
 * @brief Peripheral Component Interconnect
 *
 * @date 20.01.11
 * @author Nikolay Korotky
 */

#ifndef PCI_H_
#define PCI_H_

#include <stdint.h>

#include <drivers/pci/pci_chip/pci_utils.h>
#include <drivers/pci/pci_id.h>
#include <drivers/pci/pci_regs.h>
#include <util/dlist.h>

/**
 * Allowed up to 256 buses, each with up to 32 devices,
 * each supporting 8 functions
 */
#define PCI_DEV_QUANTITY             32
#define MIN_DEVFN                    0x00
#define MAX_DEVFN                    0xff

/**
 * The slot/func address of each device is encoded in a single byte as follows:
 *   7:3 = slot, 2:0 = function
 */
#define PCI_SLOT(devfn)              (((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)              ((devfn)&0x07)

#define PCI_DEVFN(slot, func)        ((((slot)&0x1f) << 3) | ((func)&0x07))

#define PCI_IRQ_LEGACY               (1 << 0) /* allow legacy interrupts */
#define PCI_IRQ_MSI                  (1 << 1) /* allow MSI interrupts */
#define PCI_IRQ_MSIX                 (1 << 2) /* allow MSI-X interrupts */
#define PCI_IRQ_AFFINITY             (1 << 3) /* auto-assign affinity */

/*
 * Virtual interrupts allow for more interrupts to be allocated
 * than the device has interrupts for. These are not programmed
 * into the device's MSI-X table and must be handled by some
 * other driver means.
 */
#define PCI_IRQ_VIRTUAL              (1 << 4)

#define PCI_IRQ_ALL_TYPES            (PCI_IRQ_LEGACY | PCI_IRQ_MSI | PCI_IRQ_MSIX)

/** Device classes and subclasses */

#define PCI_CLASS_NOT_DEFINED        0x00
#define PCI_CLASS_NOT_DEFINED_VGA    0x0001

#define PCI_BASE_CLASS_STORAGE       0x01
#define PCI_CLASS_STORAGE_SCSI       0x0000
#define PCI_CLASS_STORAGE_IDE        0x0001
#define PCI_CLASS_STORAGE_FLOPPY     0x0002
#define PCI_CLASS_STORAGE_OTHER      0x0080

#define PCI_BASE_CLASS_NETWORK       0x02
#define PCI_CLASS_NETWORK_ETHERNET   0x0000
#define PCI_CLASS_NETWORK_OTHER      0x0080

#define PCI_BASE_CLASS_DISPLAY       0x03
#define PCI_CLASS_DISPLAY_VGA        0x0000
#define PCI_CLASS_DISPLAY_XGA        0x0001
#define PCI_CLASS_DISPLAY_OTHER      0x0080

#define PCI_BASE_CLASS_MULTIMEDIA    0x04

#define PCI_BASE_CLASS_MEMORY        0x05
#define PCI_CLASS_MEMORY_RAM         0x0000
#define PCI_CLASS_MEMORY_FLASH       0x0001
#define PCI_CLASS_MEMORY_OTHER       0x0080

#define PCI_BASE_CLASS_BRIDGE        0x06
#define PCI_CLASS_BRIDGE_HOST        0x0000
#define PCI_CLASS_BRIDGE_ISA         0x0001
#define PCI_CLASS_BRIDGE_EISA        0x0002
#define PCI_CLASS_BRIDGE_MC          0x0003
#define PCI_CLASS_BRIDGE_PCI         0x0004
#define PCI_CLASS_BRIDGE_PCMCIA      0x0005
#define PCI_CLASS_BRIDGE_NUBUS       0x0006
#define PCI_CLASS_BRIDGE_CARDBUS     0x0007
#define PCI_CLASS_BRIDGE_OTHER       0x0080

#define PCI_BASE_CLASS_COMMUNICATION 0x07
#define PCI_BASE_CLASS_SYSTEM        0x08
#define PCI_BASE_CLASS_INPUT         0x09
#define PCI_BASE_CLASS_DOCKING       0x0a
#define PCI_BASE_CLASS_PROCESSOR     0x0b

#define PCI_BASE_CLASS_SERIAL        0x0c
#define PCI_CLASS_SERIAL_FIREWIRE    0x0000
#define PCI_CLASS_SERIAL_ACCESS      0x0001
#define PCI_CLASS_SERIAL_SSA         0x0002
#define PCI_CLASS_SERIAL_USB         0x0003

#define PCI_BASE_CLASS_WIRELESS      0x0d
#define PCI_BASE_CLASS_INTELLIGENT   0x0e
#define PCI_BASE_CLASS_SATELLITE     0x0f
#define PCI_BASE_CLASS_CRYPT         0x10
#define PCI_BASE_CLASS_SIGNAL_PROC   0x11
#define PCI_CLASS_OTHERS             0xff

struct pci_slot_dev {
	struct dlist_head lst;
	const struct pci_driver *pci_drv;

	uint32_t busn;
	uint8_t slot;
	uint8_t func;
	uint8_t rev;
	uint8_t is_bridge;
	uint8_t is_multi;
	uint16_t vendor;
	uint16_t device;
	uint8_t baseclass;
	uint8_t subclass;
	unsigned int irq; /* IRQ number suitable for passing to irq_attach() */
	uint8_t irq_pin;  /* IRQ pin from PCI configuration space */
	uint8_t irq_line; /* IRQ line from PCI configuration space */
	uint32_t bar[6];
	uint8_t primary;
	uint8_t secondary;
	uint8_t subordinate;
	uint32_t membaselimit;

	int msix_enabled;
	int msi_enabled;
	uint8_t msi_cap;  /* MSI capability offset */
	uint8_t msix_cap; /* MSI-X capability offset */
	struct dlist_head msi_list;

	int is_busmaster;
};

#define PCI_BAR_BASE(bar) (bar & 0xFFFFFFF0)

#define pci_foreach_dev(pci_dev)                  \
	dlist_foreach_entry(pci_dev, __extension__({  \
		extern struct dlist_head __pci_devs_list; \
		&__pci_devs_list;                         \
	}),                                           \
	    lst)

struct pci_slot_dev *pci_insert_dev(char configured, uint32_t bus,
    uint32_t devfn, uint32_t vendor_reg);

extern void pci_set_master(struct pci_slot_dev *slot_dev);
extern void pci_intx(struct pci_slot_dev *pdev, int enable);

extern int pci_read_config_byte(struct pci_slot_dev *dev, int where,
    uint8_t *val);
extern int pci_read_config_word(struct pci_slot_dev *dev, int where,
    uint16_t *val);
extern int pci_read_config_dword(struct pci_slot_dev *dev, int where,
    uint32_t *val);

extern int pci_write_config_byte(struct pci_slot_dev *dev, int where,
    uint8_t val);
extern int pci_write_config_word(struct pci_slot_dev *dev, int where,
    uint16_t val);
extern int pci_write_config_dword(struct pci_slot_dev *dev, int where,
    uint32_t val);

extern int pci_alloc_irq_vectors(struct pci_slot_dev *dev,
    unsigned int min_vecs, unsigned int max_vecs, unsigned int flags);

extern int pci_irq_vector(struct pci_slot_dev *dev, unsigned int nr);

#define pci_resource_start(d, b) (d->bar[(b)] & ~0xF)

struct msix_entry {
	uint32_t vector; /* kernel uses to write allocated vector */
	uint16_t entry;  /* driver uses to specify entry, OS writes */
};

#define for_each_pci_msi_entry(entry, dev) \
	dlist_foreach_entry(entry, &dev->msi_list, list)

#endif /* PCI_H_ */
