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

ARRAY_SPREAD_DEF(const struct pci_driver *, __pci_driver_registry);

static const struct pci_driver * pci_driver_find(uint16_t ven_id, uint16_t dev_id) {
	const struct pci_driver *pci_drv;
	array_foreach(pci_drv, __pci_driver_registry, ARRAY_SPREAD_SIZE(__pci_driver_registry)) {
		if (ven_id == pci_drv->ven_id && dev_id == pci_drv->dev_id) {
			return pci_drv;
		}
	}
	return NULL;
}

int pci_driver_load(struct pci_slot_dev *dev) {
	int ret;
	const struct pci_driver *drv;
	const struct mod *dep;

	assert(dev != NULL);

	drv = pci_driver_find(dev->vendor, dev->device);

	if (NULL == drv) {
		return -ENOENT;
	}

	/* Enable mod (and dependencies) without cyclic detection error generating.
	 * This introduced since some driver can be inited before pci_driver_load, then next lines
	 * just ensure that other dependecies of driver are satisfied */
	mod_foreach_requires(dep, drv->mod) {
		if ((ret = mod_enable_rec_safe(dep, true))) {
			return ret;
		}
	}

	printk("\tpci: loading %s.%s: ", drv->mod->package->name, drv->mod->name);
	ret = drv->init(dev);
	if (ret == 0) {
		printk("done\n");
	}
	else {
		printk("error: %s\n", strerror(-ret));
	}

	return ret;
}
