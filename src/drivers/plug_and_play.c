/*
 * test_availability_irq.c
 *
 *  Created on: 28.01.2009
 *      Authors: Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include "types.h"
#include "common.h"
#include "pnp_id.h"
#include "plug_and_play.h"

#define COMPUTE_PNP_VENDOR_ID(id_reg)	(0xFF & ((id_reg) >> 24))
#define COMPUTE_PNP_DEVICE_ID(id_reg)	(0xFFF & ((id_reg) >> 12))
#define COMPUTE_PNP_VERSION(id_reg)		(0x1F & ((id_reg)) >> 5)
#define COMPUTE_PNP_IRQ(id_reg)			(0x1F & (id_reg))

#define COMPUTE_PNP_BAR_ADDR(bar)		(0xFFF & ((bar) >> 20))
#define COMPUTE_PNP_BAR_P(bar)			(0x1 & ((bar) >> 17))
#define COMPUTE_PNP_BAR_C(bar)			(0x1 & ((bar) >> 16))
#define COMPUTE_PNP_BAR_MASK(bar)		(0xFFF & ((bar) >> 4))
#define COMPUTE_PNP_BAR_TYPE(bar)		(0xF & (bar))

typedef struct {
	WORD id_reg;
	WORD user_defined[3];
	WORD bar[4];
} AHB_ENTRY;

typedef struct {
	WORD id_reg;
	WORD bar;
} APB_ENTRY;

typedef struct {
	const PNP_VENDOR_ID vendor_id;
	const PNP_DEVICE_ID device_id;
	const char *name;
} PNP_DEVICE_INFO;

typedef struct {
	const PNP_VENDOR_ID vendor_id;
	const char *name;
} PNP_VENDOR_INFO;

PNP_VENDOR_INFO const vendors_table[] = {
#include "pnp_vendors_table.inc"
		};

PNP_DEVICE_INFO const devices_table[] = {
#include "pnp_devices_table.inc"
		};

#define AHB_MASTER_BASE	0xFFFFF000
#define AHB_SLAVE_BASE	0xFFFFF800
#define APB_BASE		0x800FF000

#define AHB_MASTERS_COUNT	128
#define AHB_SLAVES_COUNT	128
static AHB_DEV * ahb_devices[AHB_MASTERS_COUNT+ AHB_SLAVES_COUNT];

#define APB_COUNT		512
static APB_DEV * apb_devices[APB_COUNT];

inline static void fill_ahb_dev(AHB_DEV *ahb_dev, AHB_ENTRY *ahb_entry) {
	int i;

	// Identification Register
	ahb_dev->id_reg.vendor_id = COMPUTE_PNP_VENDOR_ID(ahb_entry->id_reg);
	ahb_dev->id_reg.device_id = COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg);
	ahb_dev->id_reg.version = COMPUTE_PNP_VERSION(ahb_entry->id_reg);
	ahb_dev->id_reg.irq = COMPUTE_PNP_IRQ(ahb_entry->id_reg);

	// User defined x3
	for (i = 0; i < 3; i++) {
		ahb_dev->user_defined[i] = ahb_entry->user_defined[i];
	}

	// Bank Address Registers x4
	for (i = 0; i < 4; i++) {
		ahb_dev->bar[i].addr = COMPUTE_PNP_BAR_ADDR(ahb_entry->bar[i]);
		ahb_dev->bar[i].prefetchable = COMPUTE_PNP_BAR_P(ahb_entry->bar[i]);
		ahb_dev->bar[i].cacheable = COMPUTE_PNP_BAR_C(ahb_entry->bar[i]);
		ahb_dev->bar[i].mask = COMPUTE_PNP_BAR_MASK(ahb_entry->bar[i]);
		ahb_dev->bar[i].type = COMPUTE_PNP_BAR_TYPE(ahb_entry->bar[i]);
	}

}

inline static void fill_apb_dev(APB_DEV *apb_dev, APB_ENTRY *apb_entry) {
	// Identification Register
	apb_dev->id_reg.vendor_id = COMPUTE_PNP_VENDOR_ID(apb_entry->id_reg);
	apb_dev->id_reg.device_id = COMPUTE_PNP_DEVICE_ID(apb_entry->id_reg);
	apb_dev->id_reg.version = COMPUTE_PNP_VERSION(apb_entry->id_reg);
	apb_dev->id_reg.irq = COMPUTE_PNP_IRQ(apb_entry->id_reg);

	// Bank Address Registers x4
	apb_dev->bar.addr = COMPUTE_PNP_BAR_ADDR(apb_entry->bar);
	apb_dev->bar.prefetchable = COMPUTE_PNP_BAR_P(apb_entry->bar);
	apb_dev->bar.cacheable = COMPUTE_PNP_BAR_C(apb_entry->bar);
	apb_dev->bar.mask = COMPUTE_PNP_BAR_MASK(apb_entry->bar);
	apb_dev->bar.type = COMPUTE_PNP_BAR_TYPE(apb_entry->bar);

}

int capture_ahb_dev(AHB_DEV *ahb_dev, BYTE vendor_id, WORD device_id) {
	int i;
	AHB_ENTRY *ahb_entry;

	if (NULL == ahb_dev) {
		return -1;
	}

	ahb_entry = (AHB_ENTRY *) AHB_MASTER_BASE;
	for (i = 0; i < AHB_MASTERS_COUNT; i++) {
		if ((COMPUTE_PNP_VENDOR_ID(ahb_entry->id_reg) == vendor_id)
				&& (COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg) == device_id)
				&& (NULL == ahb_devices[AHB_MASTERS_COUNT + i])) {

			fill_ahb_dev(ahb_dev, ahb_entry);
			ahb_devices[i] = ahb_dev;

			return 0;
		}
		ahb_entry++;
	}

	ahb_entry = (AHB_ENTRY *) AHB_SLAVE_BASE;
	for (i = 0; i < AHB_SLAVES_COUNT; i++) {
		if ((COMPUTE_PNP_VENDOR_ID(ahb_entry->id_reg) == vendor_id)
				&& (COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg) == device_id)
				&& (NULL == ahb_devices[AHB_MASTERS_COUNT + i])) {

			fill_ahb_dev(ahb_dev, ahb_entry);
			ahb_devices[AHB_MASTERS_COUNT + i] = ahb_dev;

			return 0;
		}
		ahb_entry++;
	}

	return -1;
}

int capture_apb_dev(APB_DEV *apb_dev, BYTE vendor_id, WORD device_id) {
	int i, j;
	APB_ENTRY *apb_entry = (APB_ENTRY *) APB_BASE;

	if (NULL == apb_dev) {
		return -1;
	}

	for (i = 0; i < APB_COUNT; i++) {
		if ((COMPUTE_PNP_VENDOR_ID(apb_entry->id_reg) == vendor_id)
				&& (COMPUTE_PNP_DEVICE_ID(apb_entry->id_reg) == device_id)
				&& (NULL == apb_devices[i])) {

			fill_apb_dev(apb_dev, apb_entry);
			apb_devices[i] = apb_dev;

			return 0;
		}
		apb_entry++;
	}
	return -1;
}

static const int vendors_len = sizeof(vendors_table) / sizeof(vendors_table[0]);
static const int devices_len = sizeof(devices_table) / sizeof(devices_table[0]);

inline static PNP_VENDOR_INFO* get_vendor_info(PNP_VENDOR_ID vendor_id) {
	int i;
	for (i = 0; i < vendors_len; i++) {
		if (vendors_table[i].vendor_id == vendor_id) {
			return (PNP_VENDOR_INFO*) &(vendors_table[i]);
		}
	}
	return NULL;
}

inline static PNP_DEVICE_INFO* get_device_info(PNP_VENDOR_ID vendor_id,
		PNP_DEVICE_ID device_id) {
	int i;
	for (i = 0; i < devices_len; i++) {
		if (devices_table[i].vendor_id == vendor_id
				&& devices_table[i].device_id == device_id) {
			return (PNP_DEVICE_INFO*) &(devices_table[i]);
		}
	}
	return NULL;
}

inline static void print_table_head() {
	//	TRACE("%c",);
}

void print_ahb_dev() {
	int i, j, k;
	WORD vendor_id, device_id, irq;
	PNP_VENDOR_INFO* vendor_info;
	PNP_DEVICE_INFO* device_info;
	AHB_ENTRY *pnp_entry = (AHB_ENTRY *) AHB_MASTER_BASE;

	for (i = 0; i < 128; i++) {

		if (0 != COMPUTE_PNP_DEVICE_ID(pnp_entry->id_reg)) {
			vendor_id = COMPUTE_PNP_VENDOR_ID(pnp_entry->id_reg);
			device_id = COMPUTE_PNP_DEVICE_ID(pnp_entry->id_reg);
			irq = COMPUTE_PNP_IRQ(pnp_entry->id_reg);

			// first line
			TRACE("\n%02x:%03x:%02x\n", vendor_id, device_id, irq);
			// Second line
			//TRACE("Vendor: ");
			if ((vendor_info = get_vendor_info(vendor_id)) != NULL) {
				TRACE("Vendor: %16s\t", vendor_info->name);
				if ((device_info = get_device_info(vendor_id, device_id))
						!= NULL) {
					TRACE("Device: %24s\n", device_info->name);
				} else {
					TRACE("Device: unknown\n");
				}
				break;
			} else {
				TRACE("Vendor: unknown         \tDevice: unknown\n");
			}

			TRACE("Version: %d", COMPUTE_PNP_VERSION(pnp_entry->id_reg));TRACE("\tIRQ: 0x%02x\n", irq);
			for (j = 0; j < 4; j++) {

			}

		}
		pnp_entry++;
	}

}
