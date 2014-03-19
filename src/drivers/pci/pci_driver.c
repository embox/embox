/**
 * @file
 *
 * @date 06.09.12
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <util/array.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/api.h>
#include <kernel/printk.h>

#define PCI_INFO_LABEL "\tpci: "

static int pci_mod_enable(const struct mod *self) {
	const struct pci_driver *pci_drv = (const struct pci_driver *) self;
	struct pci_slot_dev *dev, *nxt_pci_dev;

	printk(PCI_INFO_LABEL "%s driver inserted\n", pci_drv->name);

	pci_foreach_dev(dev, nxt_pci_dev) {
		if (!dev->pci_drv) {
			if (dev->vendor == pci_drv->ven_id &&
					dev->device == pci_drv->dev_id &&
					!pci_drv->init(dev)) {

				printk(PCI_INFO_LABEL "%s handles %04x:%04x bus %d slot %d func %d\n",
						pci_drv->name, dev->vendor, dev->device,
						dev->busn, dev->slot, dev->func);

				dev->pci_drv = pci_drv;
			}
		}
	}

	return 0;
}

static int pci_mod_disable(const struct mod *self) {
	const struct pci_driver *pci_drv = (const struct pci_driver *) self;
	struct pci_slot_dev *dev, *nxt_pci_dev;

	pci_foreach_dev(dev, nxt_pci_dev) {
		if (dev->pci_drv == pci_drv) {
			/* deinit */
		}
	}

	return 0;
}

const struct mod_ops __pci_mod_ops = {
	.enable = pci_mod_enable,
	.disable = pci_mod_disable,
};
