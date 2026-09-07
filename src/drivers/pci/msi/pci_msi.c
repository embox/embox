/**
 * @file
 *
 * @date Feb 9, 2022
 * @author Anton Bondarev
 */

#include <compiler.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_msi.h>
#include <drivers/pci/pci_regs.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/irq/irq_msi.h>
#include <mem/vmem_device_memory.h>
#include <util/log.h>

#define writel(val, addr) REG32_STORE(addr, val)
#define readl(addr)       REG32_LOAD(addr)

#define msi_desc_to_pci_dev(desc) (desc->dev)
#define dev_to_msi_list(dev)      (&dev->msi_list)

#define msix_table_size(flags) ((flags & PCI_MSIX_FLAGS_QSIZE) + 1)

#ifdef CONFIG_PCI_MSI_IRQ_DOMAIN
static int pci_msi_setup_msi_irqs(struct pci_dev *dev, int nvec, int type) {
	struct irq_domain *domain;

	domain = dev_get_msi_domain(&dev->dev);
	if (domain && irq_domain_is_hierarchy(domain))
		return msi_domain_alloc_irqs(domain, &dev->dev, nvec);

	return arch_setup_msi_irqs(dev, nvec, type);
}

static void pci_msi_teardown_msi_irqs(struct pci_dev *dev) {
	struct irq_domain *domain;

	domain = dev_get_msi_domain(&dev->dev);
	if (domain && irq_domain_is_hierarchy(domain))
		msi_domain_free_irqs(domain, &dev->dev);
	else
		arch_teardown_msi_irqs(dev);
}
#else
#define pci_msi_setup_msi_irqs    arch_setup_msi_irqs
#define pci_msi_teardown_msi_irqs arch_teardown_msi_irqs
#endif

/* Arch hooks */

int __weak arch_setup_msi_irq(struct pci_slot_dev *dev, struct msi_desc *desc) {
#if 0
	struct msi_controller *chip = dev->bus->msi;
	int err;

	if (!chip || !chip->setup_irq)
		return -EINVAL;

	err = chip->setup_irq(chip, dev, desc);
	if (err < 0)
		return err;

	irq_set_chip_data(desc->irq, chip);
#endif
	return 0;
}

void __weak arch_teardown_msi_irq(unsigned int irq) {
#if 0
	struct msi_controller *chip = irq_get_chip_data(irq);

	if (!chip || !chip->teardown_irq)
		return;

	chip->teardown_irq(chip, irq);
#endif
}

int __weak arch_setup_msi_irqs(struct pci_slot_dev *dev, int nvec, int type) {
#if 0
	struct msi_controller *chip = dev->bus->msi;
	struct msi_desc *entry;
	int ret;

	if (chip && chip->setup_irqs)
		return chip->setup_irqs(chip, dev, nvec, type);
	/*
	 * If an architecture wants to support multiple MSI, it needs to
	 * override arch_setup_msi_irqs()
	 */
	if (type == PCI_CAP_ID_MSI && nvec > 1)
		return 1;

	for_each_pci_msi_entry(entry, dev) {
		ret = arch_setup_msi_irq(dev, entry);
		if (ret < 0)
			return ret;
		if (ret > 0)
			return -ENOSPC;
	}
#endif
	return 0;
}

/*
 * We have a default implementation available as a separate non-weak
 * function, as it is used by the Xen x86 PCI code
 */
void default_teardown_msi_irqs(struct pci_slot_dev *dev) {
#if 0
	int i;
	struct msi_desc *entry;

	for_each_pci_msi_entry(entry, dev)
		if (entry->irq)
			for (i = 0; i < entry->nvec_used; i++)
				arch_teardown_msi_irq(entry->irq + i);
#endif
}

static void default_restore_msi_irq(struct pci_slot_dev *dev, int irq) {
#if 0
	struct msi_desc *entry;

	entry = NULL;
	if (dev->msix_enabled) {
		for_each_pci_msi_entry(entry, dev) {
			if (irq == entry->irq)
				break;
		}
	} else if (dev->msi_enabled)  {
		entry = irq_get_msi_desc(irq);
	}

	if (entry)
		__pci_write_msi_msg(entry, &entry->msg);
#endif
}

void default_restore_msi_irqs(struct pci_slot_dev *dev) {
	struct msi_desc *entry;

	for_each_pci_msi_entry(entry, dev) {
		default_restore_msi_irq(dev, entry->irq);
	}
}

void __weak arch_teardown_msi_irqs(struct pci_slot_dev *dev) {
	return default_teardown_msi_irqs(dev);
}

void __weak arch_restore_msi_irqs(struct pci_slot_dev *dev) {
	return default_restore_msi_irqs(dev);
}

static void free_msi_irqs(struct pci_slot_dev *dev) {
	struct msi_desc *entry;
	struct dlist_head *l;

	pci_msi_teardown_msi_irqs(dev);

	while ((l = dlist_first_or_null(dev_to_msi_list(dev))) != NULL) {
		entry = dlist_entry(l, struct msi_desc, list);
		dlist_del_init(l);
		msi_entry_free(entry);
	}
}

static void *pci_msix_desc_addr(struct msi_desc *desc) {
	if (desc->msi_attrib.is_virtual) {
		return NULL;
	}

	return desc->mask_base + desc->msi_attrib.entry_nr * PCI_MSIX_ENTRY_SIZE;
}

uint32_t __pci_msix_desc_mask_irq(struct msi_desc *desc, uint32_t flag) {
	uint32_t mask_bits = desc->masked;
	void *desc_addr;

	desc_addr = pci_msix_desc_addr(desc);
	if (!desc_addr) {
		return 0;
	}

	mask_bits &= ~PCI_MSIX_ENTRY_CTRL_MASKBIT;
	if (flag & PCI_MSIX_ENTRY_CTRL_MASKBIT) {
		mask_bits |= PCI_MSIX_ENTRY_CTRL_MASKBIT;
	}

	writel(mask_bits, desc_addr + PCI_MSIX_ENTRY_VECTOR_CTRL);

	return mask_bits;
}

static inline __attribute_const__ uint32_t msi_mask(unsigned x) {
	/* Don't shift by >= width of type */
	if (x >= 5) {
		return 0xffffffff;
	}
	return (1 << (1 << x)) - 1;
}

/*
 * PCI 2.3 does not specify mask bits for each MSI interrupt.  Attempting to
 * mask all MSI interrupts by clearing the MSI enable bit does not work
 * reliably as devices without an INTx disable bit will then generate a
 * level IRQ which will never be cleared.
 */
static uint32_t __pci_msi_desc_mask_irq(struct msi_desc *desc, uint32_t mask,
    uint32_t flag) {
	uint32_t mask_bits = desc->masked;

	mask_bits &= ~mask;
	mask_bits |= flag;
	pci_write_config_dword(msi_desc_to_pci_dev(desc), desc->mask_pos, mask_bits);

	return mask_bits;
}

static void msi_mask_irq(struct msi_desc *desc, uint32_t mask, uint32_t flag) {
	desc->masked = __pci_msi_desc_mask_irq(desc, mask, flag);
}

static void msix_mask_irq(struct msi_desc *desc, uint32_t flag) {
	desc->masked = __pci_msix_desc_mask_irq(desc, flag);
}

/*
 * Deliver a message to the device. For MSI-X the vector stays masked
 * while the address/data pair is rewritten and keeps whatever mask
 * state it had; for MSI the capability registers are updated
 * directly.
 */
void __pci_write_msi_msg(struct msi_desc *entry, struct msi_msg *msg) {
	struct pci_slot_dev *dev = msi_desc_to_pci_dev(entry);

	if (entry->msi_attrib.is_msix) {
		void *desc_addr = pci_msix_desc_addr(entry);
		uint32_t ctrl;

		if (desc_addr == NULL) {
			return;
		}
		/* The order and the barriers follow the proven bare-metal
		 * setup (PCIe spec 7.7.2.1): mask the entry, observe the
		 * mask, then the whole address/data triple, and only then
		 * unmask. A posted message write that the endpoint sees
		 * while the vector is unmasked may arm a half-written
		 * entry. */
		ctrl = readl(desc_addr + PCI_MSIX_ENTRY_VECTOR_CTRL);
		writel(ctrl | PCI_MSIX_ENTRY_CTRL_MASKBIT,
		    desc_addr + PCI_MSIX_ENTRY_VECTOR_CTRL);
		dsb(st);

		writel(msg->address_lo, desc_addr + PCI_MSIX_ENTRY_LOWER_ADDR);
		writel(msg->address_hi, desc_addr + PCI_MSIX_ENTRY_UPPER_ADDR);
		writel(msg->data, desc_addr + PCI_MSIX_ENTRY_DATA);
		dsb(st);

		writel(ctrl, desc_addr + PCI_MSIX_ENTRY_VECTOR_CTRL);
		dsb(st);
	}
	else {
		pci_write_config_dword(dev, dev->msi_cap + PCI_MSI_ADDRESS_LO,
		    msg->address_lo);
		if (entry->msi_attrib.is_64) {
			pci_write_config_dword(dev, dev->msi_cap + PCI_MSI_ADDRESS_HI,
			    msg->address_hi);
			pci_write_config_dword(dev, dev->msi_cap + PCI_MSI_DATA_64, msg->data);
		}
		else {
			pci_write_config_dword(dev, dev->msi_cap + PCI_MSI_DATA_32, msg->data);
		}
		dsb(st);
	}
}

static int pci_msi_supported(struct pci_slot_dev *dev, int nvec) {
	return 1;
}

static void pci_intx_for_msi(struct pci_slot_dev *dev, int enable) {
	pci_intx(dev, enable);
}

int pci_msi_vec_count(struct pci_slot_dev *dev) {
	int ret;
	uint16_t msgctl;

	if (!dev->msi_cap) {
		return -EINVAL;
	}

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &msgctl);
	ret = 1 << ((msgctl & PCI_MSI_FLAGS_QMASK) >> 1);

	return ret;
}

static struct msi_desc *msi_setup_entry(struct pci_slot_dev *dev, int nvec) {
	struct msi_desc *entry;
	uint16_t control;

	/* MSI Entry Initialization */
	entry = msi_entry_alloc(dev, nvec);
	if (!entry) {
		goto out;
	}

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &control);

	entry->msi_attrib.is_msix = 0;
	entry->msi_attrib.is_64 = !!(control & PCI_MSI_FLAGS_64BIT);
	entry->msi_attrib.is_virtual = 0;
	entry->msi_attrib.entry_nr = 0;
	entry->msi_attrib.maskbit = !!(control & PCI_MSI_FLAGS_MASKBIT);
	entry->msi_attrib.default_irq = dev->irq; /* Save IOAPIC IRQ */
	entry->msi_attrib.multi_cap = (control & PCI_MSI_FLAGS_QMASK) >> 1;
	entry->msi_attrib.multiple = 0; //ilog2(__roundup_pow_of_two(nvec));

	if (control & PCI_MSI_FLAGS_64BIT) {
		entry->mask_pos = dev->msi_cap + PCI_MSI_MASK_64;
	}
	else {
		entry->mask_pos = dev->msi_cap + PCI_MSI_MASK_32;
	}

	/* Save the initial mask status */
	if (entry->msi_attrib.maskbit) {
		pci_read_config_dword(dev, entry->mask_pos, &entry->masked);
	}

out:

	return entry;
}

static int msi_verify_entries(struct pci_slot_dev *dev) {
	struct msi_desc *entry;

	for_each_pci_msi_entry(entry, dev) {
#if 0
		if (!dev->no_64bit_msi || !entry->msg.address_hi) {
			continue;
		}
		log_error(dev, "Device has broken 64-bit MSI but arch"
			" tried to assign one above 4G");
		return -EIO;
#endif
	}
	return 0;
}

static int msi_capability_init(struct pci_slot_dev *dev, int nvec) {
	struct msi_desc *entry;
	int ret;
	unsigned mask;

	pci_msi_set_enable(dev, 0); /* Disable MSI during set up */

	entry = msi_setup_entry(dev, nvec);
	if (!entry) {
		return -ENOMEM;
	}

	/* All MSIs are unmasked by default; mask them all */
	mask = msi_mask(entry->msi_attrib.multi_cap);
	msi_mask_irq(entry, mask, mask);

	dlist_add_next(&entry->list, dev_to_msi_list(dev));

	/* Configure MSI capability structure */
	ret = pci_msi_setup_msi_irqs(dev, nvec, PCI_CAP_ID_MSI);
	if (ret) {
		msi_mask_irq(entry, mask, ~mask);
		free_msi_irqs(dev);
		return ret;
	}

	ret = msi_verify_entries(dev);
	if (ret) {
		msi_mask_irq(entry, mask, ~mask);
		free_msi_irqs(dev);
		return ret;
	}

	/* Unmask before enabling: the message must be fully armed by
	 * the time the endpoint sees MSI enabled. */
	msi_mask_irq(entry, mask, 0);
	dsb(st);

	/* Set MSI enabled bits	*/
	pci_intx_for_msi(dev, 0);
	pci_msi_set_enable(dev, 1);
	dev->msi_enabled = 1;
	dsb(st);

	dev->irq = entry->irq;

	return 0;
}

static int __pci_enable_msi_range(struct pci_slot_dev *dev, int minvec, int maxvec) {
	int nvec;
	int rc;

	if (!pci_msi_supported(dev, minvec)) {
		return -EINVAL;
	}

	if (maxvec < minvec) {
		return -ERANGE;
	}

	if (dev->msi_enabled) {
		return -EINVAL;
	}

	nvec = pci_msi_vec_count(dev);
	if (nvec < 0) {
		return nvec;
	}
	if (nvec < minvec) {
		return -ENOSPC;
	}

	if (nvec > maxvec) {
		nvec = maxvec;
	}

	for (;;) {
		rc = msi_capability_init(dev, nvec);
		if (rc == 0) {
			return nvec;
		}

		if (rc < 0) {
			return rc;
		}

		if (rc < minvec) {
			return -ENOSPC;
		}

		nvec = rc;
	}
}

static void *msix_map_region(struct pci_slot_dev *dev, unsigned nr_entries) {
	void *base;
	size_t phys_addr;
	uint32_t table_offset;
	uint8_t bir;

	pci_read_config_dword(dev, dev->msix_cap + PCI_MSIX_TABLE, &table_offset);
	bir = (uint8_t)(table_offset & PCI_MSIX_TABLE_BIR);

	table_offset &= PCI_MSIX_TABLE_OFFSET;
	phys_addr = pci_resource_start(dev, bir) + table_offset;

	/* The boards run identity mapped and the PCI memory windows are
	 * covered by the platform mapping already: when the dedicated
	 * mapping is refused because the region is mapped earlier, fall
	 * back to the physical address itself. */
	base = mmap_device_memory((void *)((uintptr_t)phys_addr),
	    nr_entries * PCI_MSIX_ENTRY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
	    (uintptr_t)phys_addr);
	if (base == NULL) {
		base = (void *)phys_addr;
	}

	return base;
}

int pci_msix_vec_count(struct pci_slot_dev *dev) {
	uint16_t control;

	if (!dev->msix_cap) {
		return -EINVAL;
	}

	pci_read_config_word(dev, dev->msix_cap + PCI_MSIX_FLAGS, &control);

	return msix_table_size(control);
}

static int msix_setup_entries(struct pci_slot_dev *dev, void *base,
    struct msix_entry *entries, int nvec) {
	struct msi_desc *entry;
	int ret, i;
	int vec_count = pci_msix_vec_count(dev);

	for (i = 0; i < nvec; i++) {
		entry = msi_entry_alloc(dev, 1);
		if (!entry) {
#if 0
			if (!i) {
				iounmap(base);
			} else {
				msi_irqs_free(dev);
			}
#endif
			/* No enough memory. Don't try again */
			log_error("No enough memory for nvec(%d)", nvec);
			ret = -ENOMEM;
			goto out;
		}

		entry->msi_attrib.is_msix = 1;
		entry->msi_attrib.is_64 = 1;
		if (entries)
			entry->msi_attrib.entry_nr = entries[i].entry;
		else
			entry->msi_attrib.entry_nr = i;

		entry->msi_attrib.is_virtual = entry->msi_attrib.entry_nr >= vec_count;

		entry->msi_attrib.default_irq = dev->irq;
		entry->mask_base = base;

		/* Tail insertion: the list order must match the table
		 * entry order, pci_irq_vector() indexes the list. */
		dlist_add_prev(&entry->list, dev_to_msi_list(dev));
	}
	ret = 0;
out:
	return ret;
}

static void msix_program_entries(struct pci_slot_dev *dev,
    struct msix_entry *entries) {
	struct msi_desc *entry;
	int i = 0;
	void *desc_addr;

	for_each_pci_msi_entry(entry, dev) {
		if (entries) {
			entries[i++].vector = entry->irq;
		}

		desc_addr = pci_msix_desc_addr(entry);
		if (desc_addr) {
			entry->masked = readl(desc_addr + PCI_MSIX_ENTRY_VECTOR_CTRL);
		}
		else {
			entry->masked = 0;
		}

		/* Some endpoints come up with every vector masked; the
		 * function-level MASKALL is dropped right after this loop,
		 * so arm the vectors here or no interrupt will ever
		 * surface. */
		msix_mask_irq(entry, 0);
	}
}

static int msix_capability_init(struct pci_slot_dev *dev,
    struct msix_entry *entries, int nvec) {
	int ret;
	uint16_t control;
	void *base;

	/* Ensure MSI-X is disabled while it is set up */
	pci_msix_clear_and_set_ctrl(dev, PCI_MSIX_FLAGS_ENABLE, 0);

	pci_read_config_word(dev, dev->msix_cap + PCI_MSIX_FLAGS, &control);
	/* Request & Map MSI-X table region */
	base = msix_map_region(dev, msix_table_size(control));
	if (!base) {
		return -ENOMEM;
	}

	ret = msix_setup_entries(dev, base, entries, nvec);
	if (ret) {
		return ret;
	}

	ret = pci_msi_setup_msi_irqs(dev, nvec, PCI_CAP_ID_MSIX);
	if (ret) {
		goto out_avail;
	}

	/* Check if all MSI entries honor device restrictions */
	ret = msi_verify_entries(dev);
	if (ret) {
		goto out_free;
	}

	/*
	 * Arm the function the way the proven bare-metal setup does:
	 * mask the function while the entries are written, keep ENABLE
	 * down until every entry is programmed and unmasked, then raise
	 * it with a single config write. Endpoints sample the table
	 * state as ENABLE rises; one that captures a masked state at
	 * that moment never re-samples, and no interrupt is ever sent.
	 */
	pci_msix_clear_and_set_ctrl(dev, 0, PCI_MSIX_FLAGS_MASKALL);

	msix_program_entries(dev, entries);
	dsb(st);

	/* Set MSI-X enabled bits and unmask the function */
	pci_intx_for_msi(dev, 0);
	dev->msix_enabled = 1;
	pci_msix_clear_and_set_ctrl(dev, PCI_MSIX_FLAGS_MASKALL,
	    PCI_MSIX_FLAGS_ENABLE);
	dsb(st);

	return 0;

out_avail:
	if (ret < 0) {
		/*
		 * If we had some success, report the number of IRQs
		 * we succeeded in setting up.
		 */
		struct msi_desc *entry;
		int avail = 0;

		for_each_pci_msi_entry(entry, dev) {
			if (entry->irq != 0)
				avail++;
		}
		if (avail != 0)
			ret = avail;
	}

out_free:
	free_msi_irqs(dev);

	return ret;
}

static int __pci_enable_msix(struct pci_slot_dev *dev,
    struct msix_entry *entries, int nvec, int flags) {
	int nr_entries;
	int i, j;

	if (!pci_msi_supported(dev, nvec)) {
		return -EINVAL;
	}

	nr_entries = pci_msix_vec_count(dev);
	if (nr_entries < 0) {
		return nr_entries;
	}
	if (nvec > nr_entries && !(flags & PCI_IRQ_VIRTUAL)) {
		return nr_entries;
	}

	if (entries) {
		/* Check for any invalid entries */
		for (i = 0; i < nvec; i++) {
			if (entries[i].entry >= nr_entries) {
				return -EINVAL; /* invalid entry */
			}
			for (j = i + 1; j < nvec; j++) {
				if (entries[i].entry == entries[j].entry) {
					return -EINVAL; /* duplicate entry */
				}
			}
		}
	}

	/* Check whether driver already requested for MSI IRQ */
	if (dev->msi_enabled) {
		log_info("can't enable MSI-X (MSI IRQ already assigned)");
		return -EINVAL;
	}
	return msix_capability_init(dev, entries, nvec);
}

static int __pci_enable_msix_range(struct pci_slot_dev *dev,
    struct msix_entry *entries, int minvec, int maxvec, int flags) {
	int rc, nvec = maxvec;

	if (maxvec < minvec) {
		return -ERANGE;
	}

	for (;;) {
		rc = __pci_enable_msix(dev, entries, nvec, flags);
		if (rc == 0) {
			return nvec;
		}

		if (rc < 0) {
			return rc;
		}
		if (rc < minvec) {
			return -ENOSPC;
		}

		nvec = rc;
	}
}

int pci_alloc_irq_vectors(struct pci_slot_dev *dev, unsigned int min_vecs,
    unsigned int max_vecs, unsigned int flags) {
	int msix_vecs = -ENOSPC;
	int msi_vecs = -ENOSPC;

	if (flags & PCI_IRQ_MSIX) {
		msix_vecs = __pci_enable_msix_range(dev, NULL, min_vecs, max_vecs, flags);
		if (msix_vecs > 0) {
			return msix_vecs;
		}
	}

	if (flags & PCI_IRQ_MSI) {
		msi_vecs = __pci_enable_msi_range(dev, min_vecs, max_vecs);
		if (msi_vecs > 0) {
			return msi_vecs;
		}
	}

	/* use legacy IRQ if allowed */
	if (flags & PCI_IRQ_LEGACY) {
		if (min_vecs == 1 && dev->irq) {
			pci_intx(dev, 1);
			return 1;
		}
	}

	if (msix_vecs == -ENOSPC) {
		return -ENOSPC;
	}

	return msi_vecs;
}

int pci_irq_vector(struct pci_slot_dev *dev, unsigned int nr) {
	if (dev->msix_enabled) {
		struct msi_desc *entry;
		int i = 0;

		for_each_pci_msi_entry(entry, dev) {
			if (i == nr) {
				return entry->irq;
			}
			i++;
		}

		return -EINVAL;
	}

	return dev->irq + nr;
}
