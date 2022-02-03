/**
 * @file
 *
 * @date Feb 3, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_CMDS_HARDWARE_LSPCI_LSPCI_H_
#define SRC_CMDS_HARDWARE_LSPCI_LSPCI_H_

#include <stdint.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>


/* Message Signaled Interrupts registers */
#if 0
#define PCI_MSI_FLAGS		2	/* Various flags */
#define  PCI_MSI_FLAGS_MASK_BIT	0x100	/* interrupt masking & reporting supported */
#define  PCI_MSI_FLAGS_64BIT	0x080	/* 64-bit addresses allowed */
#define  PCI_MSI_FLAGS_QSIZE	0x070	/* Message queue size configured */
#define  PCI_MSI_FLAGS_QMASK	0x00e	/* Maximum queue size available */
#define  PCI_MSI_FLAGS_ENABLE	0x001	/* MSI feature enabled */
#define PCI_MSI_RFU		3	/* Rest of capability flags */
#define PCI_MSI_ADDRESS_LO	4	/* Lower 32 bits */
#define PCI_MSI_ADDRESS_HI	8	/* Upper 32 bits (if PCI_MSI_FLAGS_64BIT set) */
#define PCI_MSI_DATA_32		8	/* 16 bits of data for 32-bit devices */
#define PCI_MSI_DATA_64		12	/* 16 bits of data for 64-bit devices */
#define PCI_MSI_MASK_BIT_32	12	/* per-vector masking for 32-bit devices */
#define PCI_MSI_MASK_BIT_64	16	/* per-vector masking for 64-bit devices */
#define PCI_MSI_PENDING_32	16	/* per-vector interrupt pending for 32-bit devices */
#define PCI_MSI_PENDING_64	20	/* per-vector interrupt pending for 64-bit devices */
#endif
/* MSI-X */
#define  PCI_MSIX_ENABLE	0x8000
#define  PCI_MSIX_MASK		0x4000
#define  PCI_MSIX_TABSIZE	0x07ff
#define PCI_MSIX_TABLE		4
#define PCI_MSIX_PBA		8
#define  PCI_MSIX_BIR		0x7


#define FLAG(x,y) ((x & y) ? '+' : '-')

static inline uint32_t get_conf_long(struct pci_slot_dev *pci_dev, uint32_t pos) {
	uint32_t val32;

	pci_read_config32(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			pos, &val32);
	return val32;
}

static inline uint16_t get_conf_word(struct pci_slot_dev *pci_dev, uint32_t pos) {
	uint16_t val16;

	pci_read_config16(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			pos, &val16);

	return val16;
}

static inline uint16_t get_conf_byte(struct pci_slot_dev *pci_dev, uint32_t pos) {
	uint8_t val8;

	pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			pos, &val8);

	return val8;
}

#endif /* SRC_CMDS_HARDWARE_LSPCI_LSPCI_H_ */
