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
	const struct mod mod;
	int  (*init)(struct pci_slot_dev *pci_dev);
	const char *name;
	uint16_t ven_id;
	uint16_t dev_id;
};

#define __PCI_DRIVER(drv_name,init_func,vid,did,driver_struct_nm,mod_ptr) \
		MOD_SELF_INIT_DECLS(__EMBUILD_MOD__);                      \
		static int init_func(struct pci_slot_dev *pci_dev);        \
		extern const struct mod_ops __pci_mod_ops;                 \
		const struct pci_driver mod_self = {                       \
			.mod = MOD_SELF_INIT(__EMBUILD_MOD__, &__pci_mod_ops), \
			.init = init_func,                                     \
			.name = "" drv_name,                                   \
			.ven_id = vid,                                         \
			.dev_id = did                                          \
		};

/* Expands vid and did if it is macros */
#define _PCI_DRIVER(drv_name,init_func,vid,did,mod) __PCI_DRIVER(drv_name,init_func,vid,did,drv_ ## vid ## _ ## did,mod)

#define PCI_DRIVER(drv_name,init_func,vid,did) _PCI_DRIVER(drv_name,init_func,vid,did,mod_self)

#endif /* PCI_DRIVER_H_ */
