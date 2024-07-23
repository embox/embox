/*
 * @file
 *
 * @date Sep 7, 2012
 * @author: Anton Bondarev
 */

#ifndef PCI_DRIVER_H_
#define PCI_DRIVER_H_

#include <stdint.h>

#include <framework/mod/api.h>
#include <framework/mod/self.h>
#include <lib/libds/array.h>

struct pci_slot_dev;

struct pci_id {
	uint16_t ven_id;
	uint16_t dev_id;
};

struct pci_driver {
	const struct mod mod;
	int (*init)(struct pci_slot_dev *pci_dev);
	const char *name;

	const struct pci_id *id_table;
	unsigned int id_table_n;
};

#define PCI_DRIVER_TABLE(_drv_name, _init_func, _id_table) \
	static int _init_func(struct pci_slot_dev *pci_dev);   \
	extern const struct mod_ops __pci_mod_ops;             \
	const struct pci_driver __MOD(__EMBUILD_MOD__) = {     \
	    .mod = MOD_SELF_INIT(&__pci_mod_ops),              \
	    .init = _init_func,                                \
	    .name = "" _drv_name,                              \
	    .id_table = _id_table,                             \
	    .id_table_n = ARRAY_SIZE(_id_table),               \
	};                                                     \
	MOD_SELF_RUNTIME()

#define __PCI_DRIVER(_drv_name, _init_func, _id_table, _vid, _did) \
	static const struct pci_id _id_table[] = {{_vid, _did}};       \
	PCI_DRIVER_TABLE(_drv_name, _init_func, _id_table)

#define PCI_DRIVER(_drv_name, _init_func, _vid, _did)                          \
	__PCI_DRIVER(_drv_name, _init_func, __EMBUILD_MOD__##__pci_id_table, _vid, \
	    _did)

#endif /* PCI_DRIVER_H_ */
