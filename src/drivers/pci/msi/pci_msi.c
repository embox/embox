/**
 * @file
 *
 * @date Feb 9, 2022
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <errno.h>

#include <compiler.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_msi.h>
#include <kernel/irq/irq_msi.h>

#define msi_desc_to_pci_dev(desc) (desc->dev)

#define msix_table_size(flags)	((flags & PCI_MSIX_FLAGS_QSIZE) + 1)

#ifdef CONFIG_PCI_MSI_IRQ_DOMAIN
static int pci_msi_setup_msi_irqs(struct pci_dev *dev, int nvec, int type)
{
	struct irq_domain *domain;

	domain = dev_get_msi_domain(&dev->dev);
	if (domain && irq_domain_is_hierarchy(domain))
		return msi_domain_alloc_irqs(domain, &dev->dev, nvec);

	return arch_setup_msi_irqs(dev, nvec, type);
}

static void pci_msi_teardown_msi_irqs(struct pci_dev *dev)
{
	struct irq_domain *domain;

	domain = dev_get_msi_domain(&dev->dev);
	if (domain && irq_domain_is_hierarchy(domain))
		msi_domain_free_irqs(domain, &dev->dev);
	else
		arch_teardown_msi_irqs(dev);
}
#else
#define pci_msi_setup_msi_irqs		arch_setup_msi_irqs
#define pci_msi_teardown_msi_irqs	arch_teardown_msi_irqs
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
#if 0
	struct list_head *msi_list = dev_to_msi_list(&dev->dev);
	struct msi_desc *entry, *tmp;
	struct attribute **msi_attrs;
	struct device_attribute *dev_attr;
	int i, count = 0;

	for_each_pci_msi_entry(entry, dev)
		if (entry->irq)
			for (i = 0; i < entry->nvec_used; i++)
				BUG_ON(irq_has_action(entry->irq + i));

	pci_msi_teardown_msi_irqs(dev);

	list_for_each_entry_safe(entry, tmp, msi_list, list) {
		if (entry->msi_attrib.is_msix) {
			if (list_is_last(&entry->list, msi_list))
				iounmap(entry->mask_base);
		}

		list_del(&entry->list);
		free_msi_entry(entry);
	}

	if (dev->msi_irq_groups) {
		sysfs_remove_groups(&dev->dev.kobj, dev->msi_irq_groups);
		msi_attrs = dev->msi_irq_groups[0]->attrs;
		while (msi_attrs[count]) {
			dev_attr = container_of(msi_attrs[count],
						struct device_attribute, attr);
			kfree(dev_attr->attr.name);
			kfree(dev_attr);
			++count;
		}
		kfree(msi_attrs);
		kfree(dev->msi_irq_groups[0]);
		kfree(dev->msi_irq_groups);
		dev->msi_irq_groups = NULL;
	}
#endif
}

static inline __attribute_const__ uint32_t msi_mask(unsigned x)
{
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
static uint32_t __pci_msi_desc_mask_irq(struct msi_desc *desc, uint32_t mask, uint32_t flag)
{
	uint32_t mask_bits = desc->masked;

	mask_bits &= ~mask;
	mask_bits |= flag;
	pci_write_config_dword(msi_desc_to_pci_dev(desc), desc->mask_pos, mask_bits);

	return mask_bits;
}

static void msi_mask_irq(struct msi_desc *desc, uint32_t mask, uint32_t flag)
{
	desc->masked = __pci_msi_desc_mask_irq(desc, mask, flag);
}

static int pci_msi_supported(struct pci_slot_dev *dev, int nvec) {
	return 1;
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

static struct msi_desc *
msi_setup_entry(struct pci_slot_dev *dev, int nvec) {
	struct msi_desc *entry;
	uint16_t control;

	/* MSI Entry Initialization */
	entry = msi_entry_alloc(dev, nvec);
	if (!entry) {
		goto out;
	}

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &control);

	entry->msi_attrib.is_msix	= 0;
	entry->msi_attrib.is_64		= !!(control & PCI_MSI_FLAGS_64BIT);
	entry->msi_attrib.is_virtual    = 0;
	entry->msi_attrib.entry_nr	= 0;
	entry->msi_attrib.maskbit	= !!(control & PCI_MSI_FLAGS_MASKBIT);
	entry->msi_attrib.default_irq	= dev->irq;	/* Save IOAPIC IRQ */
	entry->msi_attrib.multi_cap	= (control & PCI_MSI_FLAGS_QMASK) >> 1;
	entry->msi_attrib.multiple	= 0;//ilog2(__roundup_pow_of_two(nvec));

	if (control & PCI_MSI_FLAGS_64BIT) {
		entry->mask_pos = dev->msi_cap + PCI_MSI_MASK_64;
	} else {
		entry->mask_pos = dev->msi_cap + PCI_MSI_MASK_32;
	}

	/* Save the initial mask status */
	if (entry->msi_attrib.maskbit) {
		pci_read_config_dword(dev, entry->mask_pos, &entry->masked);
	}

out:

	return entry;
}


static int msi_capability_init(struct pci_slot_dev *dev, int nvec) {
	struct msi_desc *entry;
	int ret;
	unsigned mask;

	pci_msi_set_enable(dev, 0);	/* Disable MSI during set up */

	entry = msi_setup_entry(dev, nvec);
	if (!entry) {
		return -ENOMEM;
	}

	/* All MSIs are unmasked by default; mask them all */
	mask = msi_mask(entry->msi_attrib.multi_cap);
	msi_mask_irq(entry, mask, mask);

	dlist_add_next(&entry->list, &dev->msi_list);

	/* Configure MSI capability structure */
	ret = pci_msi_setup_msi_irqs(dev, nvec, PCI_CAP_ID_MSI);
	if (ret) {
		msi_mask_irq(entry, mask, ~mask);
		free_msi_irqs(dev);
		return ret;
	}
#if 0
	ret = msi_verify_entries(dev);
	if (ret) {
		msi_mask_irq(entry, mask, ~mask);
		free_msi_irqs(dev);
		return ret;
	}

	ret = populate_msi_sysfs(dev);
	if (ret) {
		msi_mask_irq(entry, mask, ~mask);
		free_msi_irqs(dev);
		return ret;
	}

	/* Set MSI enabled bits	*/
	pci_intx_for_msi(dev, 0);
	pci_msi_set_enable(dev, 1);
	dev->msi_enabled = 1;

	pcibios_free_irq(dev);
	dev->irq = entry->irq;
#endif
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

int pci_alloc_irq_vectors(struct pci_slot_dev *dev, unsigned int min_vecs, unsigned int max_vecs, unsigned int flags) {
	int msix_vecs = -ENOSPC;
	int msi_vecs = -ENOSPC;
#if 0
	if (flags & PCI_IRQ_MSIX) {
		msix_vecs = __pci_enable_msix_range(dev, NULL, min_vecs, max_vecs, flags);
		if (msix_vecs > 0) {
			return msix_vecs;
		}
	}
#endif
	if (flags & PCI_IRQ_MSI) {
		msi_vecs = __pci_enable_msi_range(dev, min_vecs, max_vecs);
		if (msi_vecs > 0) {
			return msi_vecs;
		}
	}

#if 0
	/* use legacy IRQ if allowed */
	if (flags & PCI_IRQ_LEGACY) {
		if (min_vecs == 1 && dev->irq) {
			pci_intx(dev, 1);
			return 1;
		}
	}
#endif

	if (msix_vecs == -ENOSPC) {
		return -ENOSPC;
	}

	return msi_vecs;
}
