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
#include <drivers/pci/pci_id.h>
#include <util/dlist.h>
#include <drivers/pci/pci_chip/pci_utils.h>

/**
 * Allowed up to 256 buses, each with up to 32 devices,
 * each supporting 8 functions
 */
#define PCI_DEV_QUANTITY        32
#define MIN_DEVFN               0x00
#define MAX_DEVFN               0xff

/**
 * The slot/func address of each device is encoded in a single byte as follows:
 *   7:3 = slot, 2:0 = function
 */
#define PCI_SLOT(devfn)         (((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)         ((devfn) & 0x07)

#define PCI_DEVFN(slot, func)   ((((slot) & 0x1f) << 3) | ((func) & 0x07))

/**
 * PCI configuration space
 * (Each device on the bus has a 256 bytes configuration space,
 * the first 64 bytes are standardized)
 * (according to PCI Local Bus Specification 2.2
 * DeviceID, VendorID, Status, Command, Class Code,
 * Revision ID, Header Type are required)
 */
#define PCI_VENDOR_ID           0x00   /* 16 bits */
#define PCI_DEVICE_ID           0x02   /* 16 bits */
#define PCI_COMMAND             0x04   /* 16 bits */
#define   PCI_COMMAND_IO         0x1   /* Enable response in I/O space */
#define   PCI_COMMAND_MEMORY     0x2   /* Enable response in Memory space */
#define   PCI_COMMAND_MASTER     0x4   /* Enable bus mastering */
#define PCI_STATUS              0x06   /* 16 bits */
#define PCI_REVISION_ID         0x08   /* 8 bits  */
#define PCI_PROG_IFACE          0x09   /* 8 bits  */
#define PCI_SUBCLASS_CODE       0x0a   /* 8 bits  */
#define PCI_BASECLASS_CODE      0x0b   /* 8 bits  */
#define PCI_CACHE_LINE_SIZE     0x0C   /* 8 bits  */
#define PCI_LATENCY_TIMER       0x0D   /* 8 bits  */
#define PCI_HEADER_TYPE         0x0e   /* 8 bits  */
#define PCI_BIST                0x0f   /* 8 bits  */

#define PCI_VENDOR_WRONG        0xFFFFFFFF /* device is not found in the slot */
#define PCI_VENDOR_NONE         0x00000000 /* device is not found in the slot */

/**
 * +------------+------------+---------+-----------+
 * |31         4|           3|2       1|          0|
 * +------------+------------+---------+-----------+
 * |Base Address|Prefetchable|Locatable|region type|
 * +------------+------------+---------+-----------+
 * \___________________________________/
 *           For Memory BARs
 * +--------------------------+--------+-----------+
 * |31                       2|       1|          0|
 * +--------------------------+--------+-----------+
 * |Base Address              |Reserved|region type|
 * +--------------------------+--------+-----------+
 * \___________________________________/
 *           For I/O BARs (Deprecated)
 * region type:  0 = Memory, 1 = I/O (deprecated)
 * Locatable:    0 = any 32-bit, 1 = < 1MiB, 2 = any 64-bit
 * Prefetchable: 0 = no, 1 = yes
 * Base Address: 16-byte aligned
 */
#define PCI_BASE_ADDR_REG_0     0x10   /* 32 bits */
#define PCI_BASE_ADDR_REG_1     0x14   /* 32 bits */
#define PCI_BASE_ADDR_REG_2     0x18   /* 32 bits */
#define PCI_BASE_ADDR_REG_3     0x1C   /* 32 bits */
#define PCI_BASE_ADDR_REG_4     0x20   /* 32 bits */
#define PCI_BASE_ADDR_REG_5     0x24   /* 32 bits */
#define   PCI_BASE_ADDR_IO_MASK (~0x03)
#define PCI_CARDBUS_CIS_POINTER 0x28   /* 32 bits */
#define PCI_SUBSYSTEM_VENDOR_ID 0x2C   /* 16 bits */
#define PCI_SUBSYSTEM_ID        0x2E   /* 16 bits */
#define PCI_EXP_ROM_BASE_ADDR   0x30   /* 32 bits */
#define PCI_CAPAB_POINTER       0x34   /* 8 bits  */
#define PCI_INTERRUPT_LINE      0x3C   /* 8 bits  */
#define PCI_INTERRUPT_PIN       0x3D   /* 8 bits  */
#define PCI_MIN_GNT             0x3E   /* 8 bits  */
#define PCI_MAX_LAT             0x3F   /* 8 bits  */

#define PCI_PRIMARY_BUS         0x18
#define PCI_SECONDARY_BUS       0x19
#define PCI_SUBORDINATE_BUS     0x1a

/** Device classes and subclasses */

#define PCI_CLASS_NOT_DEFINED           0x00
#define PCI_CLASS_NOT_DEFINED_VGA       0x0001

#define PCI_BASE_CLASS_STORAGE          0x01
#define PCI_CLASS_STORAGE_SCSI          0x0000
#define PCI_CLASS_STORAGE_IDE           0x0001
#define PCI_CLASS_STORAGE_FLOPPY        0x0002
#define PCI_CLASS_STORAGE_OTHER         0x0080

#define PCI_BASE_CLASS_NETWORK          0x02
#define PCI_CLASS_NETWORK_ETHERNET      0x0000
#define PCI_CLASS_NETWORK_OTHER         0x0080

#define PCI_BASE_CLASS_DISPLAY          0x03
#define PCI_CLASS_DISPLAY_VGA           0x0000
#define PCI_CLASS_DISPLAY_XGA           0x0001
#define PCI_CLASS_DISPLAY_OTHER         0x0080

#define PCI_BASE_CLASS_MULTIMEDIA       0x04

#define PCI_BASE_CLASS_MEMORY           0x05
#define PCI_CLASS_MEMORY_RAM            0x0000
#define PCI_CLASS_MEMORY_FLASH          0x0001
#define PCI_CLASS_MEMORY_OTHER          0x0080

#define PCI_BASE_CLASS_BRIDGE           0x06
#define PCI_CLASS_BRIDGE_HOST           0x0000
#define PCI_CLASS_BRIDGE_ISA            0x0001
#define PCI_CLASS_BRIDGE_EISA           0x0002
#define PCI_CLASS_BRIDGE_MC             0x0003
#define PCI_CLASS_BRIDGE_PCI            0x0004
#define PCI_CLASS_BRIDGE_PCMCIA         0x0005
#define PCI_CLASS_BRIDGE_NUBUS          0x0006
#define PCI_CLASS_BRIDGE_CARDBUS        0x0007
#define PCI_CLASS_BRIDGE_OTHER          0x0080

#define PCI_BASE_CLASS_COMMUNICATION    0x07
#define PCI_BASE_CLASS_SYSTEM           0x08
#define PCI_BASE_CLASS_INPUT            0x09
#define PCI_BASE_CLASS_DOCKING          0x0a
#define PCI_BASE_CLASS_PROCESSOR        0x0b

#define PCI_BASE_CLASS_SERIAL           0x0c
#define PCI_CLASS_SERIAL_FIREWIRE       0x0000
#define PCI_CLASS_SERIAL_ACCESS         0x0001
#define PCI_CLASS_SERIAL_SSA            0x0002
#define PCI_CLASS_SERIAL_USB            0x0003

#define PCI_BASE_CLASS_WIRELESS         0x0d
#define PCI_BASE_CLASS_INTELLIGENT      0x0e
#define PCI_BASE_CLASS_SATELLITE        0x0f
#define PCI_BASE_CLASS_CRYPT            0x10
#define PCI_BASE_CLASS_SIGNAL_PROC      0x11
#define PCI_CLASS_OTHERS                0xff

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
	uint8_t irq;
	uint32_t bar[6];
	uint8_t primary;
	uint8_t secondary;
	uint8_t subordinate;
	uint32_t membaselimit;
};

#define PCI_BAR_BASE(bar)   (bar & 0xFFFFFFF0)

#define pci_foreach_dev(pci_dev) \
	dlist_foreach_entry(pci_dev, __extension__ ({   \
		extern struct dlist_head __pci_devs_list; &__pci_devs_list; \
	}), lst)

struct pci_slot_dev *pci_insert_dev(char configured, uint32_t bus,
		uint32_t devfn, uint32_t vendor_reg);

extern void pci_set_master(struct pci_slot_dev * slot_dev);

#endif /* PCI_H_ */
