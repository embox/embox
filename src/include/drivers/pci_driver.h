/*
 * @file
 *
 * @date Sep 7, 2012
 * @author: Anton Bondarev
 */

#ifndef PCI_DRIVER_H_
#define PCI_DRIVER_H_

#include <framework/mod/api.h>
#include <framework/mod/self.h>

struct pci_slot_dev;

struct pci_driver {
	int  (*init)(struct pci_slot_dev *pci_dev);
	const struct mod *mod;
	const char *name;
	uint16_t ven_id;
	uint16_t dev_id;
};

#define __PCI_DRIVER(drv_name,init_func,vid,did,driver_struct_nm,mod_ptr)      \
		extern const struct pci_driver *__pci_driver_registry[]; \
		static int init_func(struct pci_slot_dev *pci_dev);      \
		static const struct pci_driver driver_struct_nm = { \
			.init = init_func,                                   \
			.mod = &mod_ptr,				     \
			.name = "" drv_name,                                 \
			.ven_id = vid,                                       \
			.dev_id = did                                        \
		};                                                       \
		ARRAY_SPREAD_ADD(__pci_driver_registry, &driver_struct_nm)

/* Expands vid and did if it is macros */
#define _PCI_DRIVER(drv_name,init_func,vid,did,mod) __PCI_DRIVER(drv_name,init_func,vid,did,drv_ ## vid ## _ ## did,mod)

#define PCI_DRIVER(drv_name,init_func,vid,did) _PCI_DRIVER(drv_name,init_func,vid,did,mod_self)

extern int pci_driver_load(struct pci_slot_dev *dev);

#endif /* PCI_DRIVER_H_ */
