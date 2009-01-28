#include "types.h"
#include "common.h"
#include "pnp_id.h"
#include "plug_and_play.h"


static void fill_pnp_bar(BANK_ADDRESS_REG *bar, WORD *addr) {
	bar->addr = COMPUTE_PNP_BAR_ADDR(*addr);
	bar->prefetchable = COMPUTE_PNP_BAR_P(*addr);
	bar->cacheable = COMPUTE_PNP_BAR_C(*addr);
	bar->mask = COMPUTE_PNP_BAR_MASK(*addr);
	bar->type = COMPUTE_PNP_BAR_TYPE(*addr);
}

PNP_DEV *alloc_ahb_dev(BYTE vendor_id, WORD device_id) {
	int i, j;
	PNP_DEV *pnp_dev;
	void * pnp_dev_addr;

	for (i = 0; i < 128; i++) {
		//pnp_entry = pnp_table + i;

		// Count address of the next plug and play device structure
		pnp_dev_addr = (void *)(PNP_MASTER_BASE + i * sizeof(WORD) * 8);

		if ((COMPUTE_PNP_VENDOR_ID(*((WORD *) pnp_dev_addr)) == vendor_id)
				&& (COMPUTE_PNP_DEVICE_ID(*((WORD *) pnp_dev_addr)) == device_id)
				&& (FALSE == pnp_dev_not_available[i])) {

			// Mark as not available (already determined)
			pnp_dev_not_available[i] = TRUE;

			// fill plug and play device structure
			// Identification Register
			pnp_dev->id_reg.vendor_id = COMPUTE_PNP_DEVICE_ID(
					*((WORD *) pnp_dev_addr));
			pnp_dev->id_reg.device_id = COMPUTE_PNP_VENDOR_ID(
					*((WORD *) pnp_dev_addr));
			pnp_dev->id_reg.version = COMPUTE_PNP_VERSION(
					*((WORD *) pnp_dev_addr));
			pnp_dev->id_reg.irq = COMPUTE_PNP_IRQ(*((WORD *) pnp_dev_addr));
			// User defined x3
			pnp_dev->user_defined[0] = *((WORD *) pnp_dev_addr + 1);
			pnp_dev->user_defined[1] = *((WORD *) pnp_dev_addr + 2);
			pnp_dev->user_defined[2] = *((WORD *) pnp_dev_addr + 3);
			// Bank Address Registers x4
			for (j = 0; j < 4; j++)
				fill_pnp_bar(&(pnp_dev->bar[j]), (WORD *) pnp_dev_addr + 4 + j);

			return pnp_dev;
		}

	}
	return NULL;
}

void print_ahb_dev() {
	int i;
	void *pnp_dev_addr;


	for (i = 0; i < 128; i++) {
		pnp_dev_addr = (void *)(PNP_MASTER_BASE + i * sizeof(WORD) * 8);


		if (0 != COMPUTE_PNP_DEVICE_ID(*((WORD *) pnp_dev_addr))) {
			TRACE("Vendor: ");
			switch (COMPUTE_PNP_VENDOR_ID(*((WORD *) pnp_dev_addr))) {
			case VENDOR_ID_GAISLER_RESEARCH:
				TRACE("Gaisler Research");
				TRACE("\tDevice: ");
				switch (COMPUTE_PNP_DEVICE_ID(*((WORD *) pnp_dev_addr))) {
				case DEV_ID_GAISLER_TIMER:
					TRACE("Timer\n");
					break;
				case DEV_ID_GAISLER_INTERRUPT_UNIT:
					TRACE("Interrupt Unit\n");
					break;
				default:
					TRACE("Unknown device. ID: 0x%X\n", COMPUTE_PNP_DEVICE_ID(*((WORD *) pnp_dev_addr)));
				}
				break;
			default:
				TRACE("Unknown. VenID: 0x%X DevID 0x%X\n", COMPUTE_PNP_VENDOR_ID(*((WORD *) pnp_dev_addr)), COMPUTE_PNP_DEVICE_ID(*((WORD *) pnp_dev_addr)));
			}
		}
	}

}

