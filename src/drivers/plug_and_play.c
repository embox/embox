/*
 * plug_and_play.c
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
	WORD ba_reg[4];
} AHB_ENTRY;

typedef struct {
	WORD id_reg;
	WORD ba_reg;
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

static PNP_VENDOR_INFO const vendors_table[] = {
#include "pnp_vendors_table.inc"
		};

static PNP_DEVICE_INFO const devices_table[] = {
#include "pnp_devices_table.inc"
		};

#define VENDORS_TABLE_LEN (sizeof(vendors_table) / sizeof(vendors_table[0]))
#define DEVICES_TABLE_LEN (sizeof(devices_table) / sizeof(devices_table[0]))

#define AHB_MASTER_BASE	0xFFFFF000
#define AHB_SLAVE_BASE	0xFFFFF800
#define APB_BASE		0x800FF000

#define AHB_MASTERS_COUNT	128
#define AHB_SLAVES_COUNT	128
static AHB_DEV *ahb_devices[AHB_MASTERS_COUNT+ AHB_SLAVES_COUNT];

#define APB_COUNT		512
static APB_DEV *apb_devices[APB_COUNT];

inline static void fill_pnp_id_reg(ID_REG *id_reg, WORD entry_id_reg) {
	id_reg->vendor_id = COMPUTE_PNP_VENDOR_ID(entry_id_reg);
	id_reg->device_id = COMPUTE_PNP_DEVICE_ID(entry_id_reg);
	id_reg->version = COMPUTE_PNP_VERSION(entry_id_reg);
	id_reg->irq = COMPUTE_PNP_IRQ(entry_id_reg);
}

inline static void fill_pnp_ba_reg(BA_REG *ba_reg, WORD entry_ba_reg) {
	ba_reg->addr = COMPUTE_PNP_BAR_ADDR(entry_ba_reg);
	ba_reg->prefetchable = COMPUTE_PNP_BAR_P(entry_ba_reg);
	ba_reg->cacheable = COMPUTE_PNP_BAR_C(entry_ba_reg);
	ba_reg->mask = COMPUTE_PNP_BAR_MASK(entry_ba_reg);
	ba_reg->type = COMPUTE_PNP_BAR_TYPE(entry_ba_reg);
}

inline static void fill_ahb_dev(AHB_DEV *ahb_dev, AHB_ENTRY *ahb_entry) {
	int i;
	// Identification Register
	fill_pnp_id_reg(&(ahb_dev->id_reg), ahb_entry->id_reg);
	// User defined x3
	for (i = 0; i < 3; i++) {
		ahb_dev->user_defined[i] = ahb_entry->user_defined[i];
	}
	// Bank Address Registers x4
	for (i = 0; i < 4; i++) {
		fill_pnp_ba_reg(&(ahb_dev->ba_reg[i]), ahb_entry->ba_reg[i]);
	}
}

inline static void fill_apb_dev(APB_DEV *apb_dev, APB_ENTRY *apb_entry) {
	// Identification Register
	fill_pnp_id_reg(&(apb_dev->id_reg), apb_entry->id_reg);
	// Bank Address Register
	fill_pnp_ba_reg(&(apb_dev->ba_reg), apb_entry->ba_reg);
}

int capture_ahb_dev(AHB_DEV *ahb_dev, BYTE vendor_id, WORD device_id) {
	int i;
	AHB_ENTRY *ahb_entry;

	if (NULL == ahb_dev) {
		return -1;
	}

	ahb_entry = (AHB_ENTRY *) AHB_MASTER_BASE;
	for (i = 0; i < AHB_MASTERS_COUNT; i++) {
		if ((COMPUTE_PNP_VENDOR_ID(ahb_entry->id_reg) == vendor_id) && (COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg) == device_id) && (NULL == ahb_devices[i])) {

			fill_ahb_dev(ahb_dev, ahb_entry);
			ahb_devices[i] = ahb_dev;

			return 0;
		}
		ahb_entry++;
	}

	ahb_entry = (AHB_ENTRY *) AHB_SLAVE_BASE;
	for (i = 0; i < AHB_SLAVES_COUNT; i++) {
		if ((COMPUTE_PNP_VENDOR_ID(ahb_entry->id_reg) == vendor_id) && (COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg) == device_id) && (NULL
		== ahb_devices[AHB_MASTERS_COUNT + i])) {

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
		if ((COMPUTE_PNP_VENDOR_ID(apb_entry->id_reg) == vendor_id) && (COMPUTE_PNP_DEVICE_ID(apb_entry->id_reg) == device_id) && (NULL == apb_devices[i])) {

			fill_apb_dev(apb_dev, apb_entry);
			apb_devices[i] = apb_dev;

			return 0;
		}
		apb_entry++;
	}
	return -1;
}

inline static PNP_VENDOR_INFO* get_vendor_info(PNP_VENDOR_ID vendor_id) {
	int i;
	for (i = 0; i < VENDORS_TABLE_LEN; i++) {
		if (vendors_table[i].vendor_id == vendor_id) {
			return (PNP_VENDOR_INFO*) &(vendors_table[i]);
		}
	}
	return NULL;
}

inline static PNP_DEVICE_INFO* get_device_info(PNP_VENDOR_ID vendor_id,
		PNP_DEVICE_ID device_id) {
	int i;
	for (i = 0; i < DEVICES_TABLE_LEN; i++) {
		if (devices_table[i].vendor_id == vendor_id
				&& devices_table[i].device_id == device_id) {
			return (PNP_DEVICE_INFO*) &(devices_table[i]);
		}
	}
	return NULL;
}

inline static void print_table_row(int n, int ven_id, int dev_id,
		const char *ven_name, const char *dev_name, int ver, int irq) {
	TRACE("%02x.%02x:%03x %20s %28s \t0x%02x %3d\n", n, ven_id, dev_id, ven_name, dev_name, ver, irq);
}

inline static void print_table_head() {
	TRACE("\n  %7s %20s %28s \t%4s %3s\n", "ven:dev", "Vendor Name", "Device Name","ver", "irq");
}

static int print_ahb_entries(AHB_ENTRY *base_addr, int amount) {
	int i, count = 0;
	AHB_ENTRY *ahb_entry = base_addr;
	WORD vendor_id, device_id, irq;
	char *vendor_name, *device_name;
	const char *UNKNOWN = "unknown";
	PNP_VENDOR_INFO* vendor_info;
	PNP_DEVICE_INFO* device_info;

	for (i = 0; i < amount; i++) {
		if (0 != COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg)) {
			vendor_id = COMPUTE_PNP_VENDOR_ID(ahb_entry->id_reg);
			device_id = COMPUTE_PNP_DEVICE_ID(ahb_entry->id_reg);
			irq = COMPUTE_PNP_IRQ(ahb_entry->id_reg);

			if ((vendor_info = get_vendor_info(vendor_id)) != NULL) {
				vendor_name = (char *) vendor_info->name;
				if ((device_info = get_device_info(vendor_id, device_id))
						!= NULL) {
					device_name = (char *) device_info->name;
				} else {
					device_name = (char *) UNKNOWN;
				}
			} else {
				vendor_name = (char *) UNKNOWN;
				device_name = (char *) UNKNOWN;
			}

			print_table_row(i, vendor_id, device_id, vendor_name, device_name,
					COMPUTE_PNP_VERSION(ahb_entry->id_reg), irq);
			count++;
		}
		ahb_entry++;
	}
	return count;
}

void print_ahb_dev() {
	int count = 0;
	TRACE("\nListing Plug'n'Play devices..\n");
	print_table_head();
	count
			+= print_ahb_entries((AHB_ENTRY *) AHB_MASTER_BASE,
					AHB_MASTERS_COUNT);
	TRACE("\n..Total: %d\n\n", count);

	//	TRACE(">>> (ahb slave)\n");
	//	print_ahb_entries((AHB_ENTRY *)AHB_SLAVE_BASE, AHB_SLAVES_COUNT);

}
