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
} PNP_TABLE_ENTRY;

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

static void fill_pnp_bar(BANK_ADDRESS_REG *bar, WORD *addr) {
	bar->addr = COMPUTE_PNP_BAR_ADDR(*addr);
	bar->prefetchable = COMPUTE_PNP_BAR_P(*addr);
	bar->cacheable = COMPUTE_PNP_BAR_C(*addr);
	bar->mask = COMPUTE_PNP_BAR_MASK(*addr);
	bar->type = COMPUTE_PNP_BAR_TYPE(*addr);
}

int capture_ahb_dev(PNP_DEV *pnp_dev, BYTE vendor_id, WORD device_id) {
	int i, j;
	PNP_TABLE_ENTRY *pnp_entry_ptr;

	if (NULL == pnp_dev) {
		return 1;
	}

	pnp_entry_ptr = (PNP_TABLE_ENTRY *) PNP_MASTER_BASE;
	for (i = 0; i < 128; i++) {

		if ((COMPUTE_PNP_VENDOR_ID(pnp_entry_ptr->id_reg) == vendor_id) && (COMPUTE_PNP_DEVICE_ID(pnp_entry_ptr->id_reg) == device_id) && (FALSE
		== pnp_dev_not_available[i])) {

			// Mark as not available (already determined)
			pnp_dev_not_available[i] = TRUE;

			// fill plug and play device structure
			// Identification Register
			pnp_dev->id_reg.vendor_id = COMPUTE_PNP_DEVICE_ID(pnp_entry_ptr->id_reg);
			pnp_dev->id_reg.device_id = COMPUTE_PNP_VENDOR_ID(pnp_entry_ptr->id_reg);
			pnp_dev->id_reg.version = COMPUTE_PNP_VERSION(pnp_entry_ptr->id_reg);
			pnp_dev->id_reg.irq = COMPUTE_PNP_IRQ(pnp_entry_ptr->id_reg);
			// User defined x3
			pnp_dev->user_defined[0] = pnp_entry_ptr->user_defined[0];
			pnp_dev->user_defined[1] = pnp_entry_ptr->user_defined[1];
			pnp_dev->user_defined[2] = pnp_entry_ptr->user_defined[2];

			// Bank Address Registers x4
			for (j = 0; j < 4; j++) {
				fill_pnp_bar(&(pnp_dev->bar[j]), &(pnp_entry_ptr->bar[j]));
			}

			return 0;
		}
		pnp_entry_ptr++;
	}
	return 1;
}

static const int vendors_len = sizeof(vendors_table) / sizeof(vendors_table[0]);
static const int devices_len = sizeof(devices_table) / sizeof(devices_table[0]);

void print_ahb_dev() {
	int i, j, k;
	WORD ven_id, dev_id, irq;
	PNP_TABLE_ENTRY *pnp_entry;

	pnp_entry = (PNP_TABLE_ENTRY *) PNP_MASTER_BASE;

	for (i = 0; i < 128; i++) {

		if (0 != COMPUTE_PNP_DEVICE_ID(pnp_entry->id_reg)) {
			ven_id = COMPUTE_PNP_VENDOR_ID(pnp_entry->id_reg);
			dev_id = COMPUTE_PNP_DEVICE_ID(pnp_entry->id_reg);
			irq = COMPUTE_PNP_IRQ(pnp_entry->id_reg);

			// first line
			TRACE("\n%02x:%03x:%02x\n", ven_id, dev_id, irq);
			// Second line
			//TRACE("Vendor: ");
			for (j = 0; j < vendors_len; j++) {
				if (vendors_table[j].vendor_id == ven_id) {
					TRACE("Vendor: %16s\t", vendors_table[j].name);
					for (k = 0; k < devices_len; k++) {
						if (devices_table[k].vendor_id == ven_id
								&& devices_table[k].device_id == dev_id) {
							TRACE("Device: %24s\n", devices_table[k].name);
							break;
						}
					}
					if (k == devices_len) {
						TRACE("Device: unknown\n");
					}
					break;
				}
			}
			if (j == vendors_len) {
				TRACE("Vendor: unknown         \tDevice: unknown\n");
			}

			TRACE("Version: %d", COMPUTE_PNP_VERSION(pnp_entry->id_reg));TRACE("\tIRQ: 0x%02x\n", irq);
			for (j = 0; j < 4; j++) {

			}

		}
		pnp_entry++;
	}

}
