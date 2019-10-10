/**
 * @file
 *
 * @brief
 *
 * @date 28.06.2011
 * @author Anton Bondarev
 */

#ifndef PCI_UTILS_H_
#define PCI_UTILS_H_

#include <stdint.h>

#define PCIUTILS_SUCCESS 0
#define PCIUTILS_BUSBUSY 1
#define PCIUTILS_NODATA  2
#define PCIUTILS_INVALID 3
#define PCIUTILS_TIMEOUT 4

extern int pci_check_type(void);

extern uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value);

extern uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value);

extern uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value);

extern uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value);

extern uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value);

extern uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t value);

/* PCI IRQ numbering depends on platform, this function calculates
 * platform-specific IRQ number */
struct pci_slot_dev;
extern unsigned int pci_irq_number(struct pci_slot_dev *dev);

#endif /* PCI_UTILS_H_ */
