/*
 * @file
 *
 * @date Sep 7, 2012
 * @author: Anton Bondarev
 */

#ifndef PCI_DRIVER_H_
#define PCI_DRIVER_H_

struct pci_slot_dev;

struct pci_driver {
	int  (*init)(struct pci_slot_dev *pci_dev);
	const char *name;
	uint16_t ven_id;
	uint16_t dev_id;
};

#define PCI_DRIVER(drv_name,init_func,vid,did)      \
		extern const struct pci_driver *__pci_driver_registry[]; \
		static int init_func(struct pci_slot_dev *pci_dev);      \
		static const struct pci_driver drv_ ## vid ## _ ## pid = { \
			.init = init_func,                                   \
			.name = "" drv_name,                                 \
			.ven_id = vid,                                       \
			.dev_id = did                                        \
		};                                                       \
		ARRAY_SPREAD_ADD(__pci_driver_registry, &drv_ ## vid ## _ ## pid)

extern int pci_driver_load(struct pci_slot_dev *dev);

#endif /* PCI_DRIVER_H_ */
