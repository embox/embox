/**
 * @file
 *
 * @date 06.09.12
 * @author Anton Bondarev
 */

#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/api.h>
#include <lib/libds/array.h>
#include <util/log.h>

static int pci_drv_probe(const struct pci_driver *drv,
    struct pci_slot_dev *dev) {
	int i;

	for (i = 0; i < drv->id_table_n; i++) {
		if (dev->vendor == drv->id_table[i].ven_id
		    && dev->device == drv->id_table[i].dev_id && !drv->init(dev)) {
			return 0;
		}
	}

	return -ENOSYS;
}

static int pci_mod_enable(const struct mod *self) {
	const struct pci_driver *pci_drv = (const struct pci_driver *)self;
	struct pci_slot_dev *dev = NULL;

	log_info("%s driver inserted", pci_drv->name);

	pci_foreach_dev(dev) {
		if (!dev->pci_drv) {
			if (!pci_drv_probe(pci_drv, dev)) {
				log_info("%s handles %04x:%04x bus %" PRId32 " slot %" PRId8
				         " func %" PRId8,
				    pci_drv->name, dev->vendor, dev->device, dev->busn,
				    dev->slot, dev->func);

				dev->pci_drv = pci_drv;
			}
		}
	}

	return 0;
}

static int pci_mod_disable(const struct mod *self) {
	const struct pci_driver *pci_drv = (const struct pci_driver *)self;
	struct pci_slot_dev *dev;

	pci_foreach_dev(dev) {
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
