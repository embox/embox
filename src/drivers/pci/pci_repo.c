/**
 * @file
 * @brief TODO only for lspci command
 *
 * @date 28.06.2011
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_id.h>
#include <drivers/pci/pci_repo.h>
#include <lib/libds/array.h>

typedef struct pci_vendor {
	uint16_t ven_id;
	const char *ven_name;
} pci_vendor_t;

typedef struct pci_device {
	uint16_t ven_id;
	uint16_t dev_id;
	const char *dev_name;
} pci_device_t;

typedef struct pci_baseclass {
	uint8_t baseclass;
	const char *name;
} pci_baseclass_t;

typedef struct pci_subclass {
	uint8_t baseclass;
	uint8_t subclass;
	const char *name;
} pci_subclass_t;

static pci_vendor_t const pci_vendors[] = {
#include "pci_vendors_table.inc"
};

static pci_device_t const pci_devices[] = {
#include "pci_devices_table.inc"
};

static pci_baseclass_t const pci_baseclasses[] = {
#include "pci_baseclass_table.inc"
};

static pci_subclass_t const pci_subclasses[] = {
#include "pci_subclass_table.inc"
};

const char *find_vendor_name(uint16_t ven_id) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_vendors); i++) {
		if (pci_vendors[i].ven_id == ven_id) {
			return pci_vendors[i].ven_name;
		}
	}
	return NULL;
}

const char *find_device_name(uint16_t dev_id) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_devices); i++) {
		if (pci_devices[i].dev_id == dev_id) {
			return pci_devices[i].dev_name;
		}
	}
	return NULL;
}

const char *find_class_name(uint8_t base, uint8_t sub) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pci_subclasses); i++) {
		if (pci_subclasses[i].baseclass == base
		    && pci_subclasses[i].subclass == sub) {
			return pci_subclasses[i].name;
		}
	}
	for (i = 0; i < ARRAY_SIZE(pci_baseclasses); i++) {
		if (pci_baseclasses[i].baseclass == base) {
			return pci_baseclasses[i].name;
		}
	}
	return NULL;
}
