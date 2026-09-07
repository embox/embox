/**
 * @file
 * @brief PCI MSI/MSI-X backend on top of the GICv3 ITS.
 *
 * Implements the architecture hooks of the PCI MSI framework: every
 * vector gets an LPI from the ITS, the message address points at
 * GITS_TRANSLATER and the payload is the MSI-X table entry number.
 * The PCIe requester id of the function serves as the ITS device id,
 * so every PCI function owns an independent interrupt translation.
 *
 * @author zhugengyu
 * @date 06.09.2026
 */

#include <errno.h>
#include <stdint.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_msi.h>
#include <kernel/irq/irq_msi.h>
#include <util/log.h>

#include "gic_lpi.h"
#include "gicv3_its.h"

static uint32_t msi_devid(const struct pci_slot_dev *dev) {
	return ((uint32_t)dev->busn << 8) | ((uint32_t)dev->slot << 3) | dev->func;
}

int arch_setup_msi_irqs(struct pci_slot_dev *dev, int nvec, int type) {
	struct msi_desc *entry;
	uint32_t devid = msi_devid(dev);
	uint64_t doorbell;
	int used = 0;

	if (type != PCI_CAP_ID_MSIX && type != PCI_CAP_ID_MSI) {
		return -EINVAL;
	}
	/* Multi-vector MSI needs per-vector payloads in the capability,
	 * which this driver does not manage; report back so the caller
	 * retries with a single vector. */
	if (type == PCI_CAP_ID_MSI && nvec > 1) {
		return 1;
	}

	if (gic_its_device_attach(devid) != 0) {
		return -ENOSPC;
	}

	doorbell = gic_its_trans_addr();
	for_each_pci_msi_entry(entry, dev) {
		int irq;

		if (used >= nvec) {
			break;
		}

		irq = gic_its_event_map(devid, used);
		if (irq < 0) {
			return irq;
		}

		entry->irq = irq;
		entry->msg.address_lo = (uint32_t)doorbell;
		entry->msg.address_hi = (uint32_t)(doorbell >> 32);
		entry->msg.data = used;
		__pci_write_msi_msg(entry, &entry->msg);
		used++;
	}

	log_info("its: %02x:%02x.%x got %d message irq(s)", dev->busn, dev->slot,
	    dev->func, used);

	return 0;
}

void arch_teardown_msi_irqs(struct pci_slot_dev *dev) {
	struct msi_desc *entry;
	uint32_t devid = msi_devid(dev);

	for_each_pci_msi_entry(entry, dev) {
		if (entry->irq != 0) {
			gic_its_event_unmap(devid, entry->msi_attrib.entry_nr);
			entry->irq = 0;
		}
	}
}
