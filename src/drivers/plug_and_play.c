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

typedef struct _AHB_SLOT {
	UINT32 id_reg;
	UINT32 user_defined[3];
	UINT32 ba_reg[4];
} AHB_SLOT;

typedef struct _APB_SLOT {
	UINT32 id_reg;
	UINT32 ba_reg;
} APB_SLOT;

typedef struct _PNP_DEVICE_INFO {
	const BYTE vendor_id;
	const UINT16 device_id;
	const char *name;
} PNP_DEVICE_INFO;

typedef struct _PNP_VENDOR_INFO {
	const BYTE vendor_id;
	const char *name;
} PNP_VENDOR_INFO;

static PNP_VENDOR_INFO const vendors_table[] = {
#include "pnp_vendors_table.inc"
		};

static PNP_DEVICE_INFO const devs_table[] = {
#include "pnp_devices_table.inc"
		};

#define VENDORS_TABLE_LEN (sizeof(vendors_table) / sizeof(vendors_table[0]))
#define DEVICES_TABLE_LEN (sizeof(devs_table) / sizeof(devs_table[0]))

#define AHB_MASTER_BASE	0xFFFFF000
#define AHB_SLAVE_BASE	0xFFFFF800
#define APB_BASE		0x800FF000

#define AHB_MASTERS_QUANTITY	0x10
#define AHB_SLAVES_QUANTITY		0x40
#define APB_QUANTITY			0x40
static AHB_DEV *ahb_devices[AHB_MASTERS_QUANTITY+ AHB_SLAVES_QUANTITY];
static APB_DEV *apb_devices[APB_QUANTITY];
inline static lock_ahbm_slot(UINT16 slot, AHB_DEV *dev) {
	if (ahb_devices[slot])
		return -1;
	ahb_devices[slot] = dev;
	return 0;
}
inline static lock_ahbsl_slot(UINT16 slot, AHB_DEV *dev) {
	if (ahb_devices[AHB_MASTERS_QUANTITY + slot])
		return -1;
	ahb_devices[AHB_MASTERS_QUANTITY + slot] = dev;
	return 0;
}

inline static lock_apb_slot(UINT16 slot, APB_DEV *dev) {
	if (apb_devices[slot])
		return -1;
	apb_devices[slot] = dev;
	return TRUE;
}

inline static BYTE get_ven(UINT32 id_reg) {
	return (0xFF & ((id_reg) >> 24));
}

inline static UINT16 get_dev(UINT32 id_reg) {
	return (0xFFF & ((id_reg) >> 12));
}
inline static BYTE get_irq(UINT32 id_reg) {
	return (0x1F & (id_reg));
}
inline static BYTE get_version(UINT32 id_reg) {
	return (0x1F & ((id_reg)) >> 5);
}
/* return slotnumber or -1*/
inline static int find_ahbm_dev(BYTE ven_id, UINT16 dev_id) {
	int cur_slot;
	AHB_SLOT *pslot = ((AHB_SLOT *) AHB_MASTER_BASE);
	for (cur_slot = 0; cur_slot < AHB_MASTERS_QUANTITY; cur_slot++) {
		if ((ven_id == get_ven(pslot[cur_slot].id_reg)) && (dev_id == get_dev(
				pslot[cur_slot].id_reg)))
			return cur_slot;
	}
	return -1;
}
/* return slotnumber or -1*/
inline static int find_ahbsl_dev(BYTE ven_id, UINT16 dev_id) {
	int cur_slot;
	AHB_SLOT *pslot = ((AHB_SLOT *) AHB_SLAVE_BASE);
	for (cur_slot = 0; cur_slot < AHB_SLAVES_QUANTITY; cur_slot++) {
		if ((ven_id == get_ven(pslot[cur_slot].id_reg)) && (dev_id == get_dev(
				pslot[cur_slot].id_reg)))
			return cur_slot;
	}
	return -1;
}

inline static void fill_amba_dev_info(AMBA_DEV_INFO *dev, UINT32 id_reg) {
	dev->venID = get_ven(id_reg);
	dev->devID = get_dev(id_reg);
	dev->irq = get_irq(id_reg);
	dev->version = get_version(id_reg);
}

inline static void fill_ahb_bar_info(AMBA_BAR_INFO *bar, UINT32 ba_reg) {
	if (ba_reg) {
		bar->start = ba_reg & 0xFFF00000;
		bar->prefetchable = (0x1 & ((ba_reg) >> 17));
		bar->cacheable = (0x1 & ((ba_reg) >> 16));
		bar->mask = (0xFFF & ((ba_reg) >> 4));
		bar->type = (0xF & (ba_reg));
		bar->used = TRUE;
	} else
		bar->used = FALSE;

}

inline static void fill_ahb_bar_infos(AHB_DEV *dev, AHB_SLOT *ahb_slot) {
	int i;
	for (i = 0; i < sizeof(dev->bar) / sizeof(dev->bar[0]); i++)
		fill_ahb_bar_info(&dev->bar[i], ahb_slot->ba_reg[i]);
}

inline static void fill_ahbm_dev_info(AHB_DEV *dev, BYTE slot_number) {
	AHB_SLOT *ahb_slot = ((AHB_SLOT *) AHB_MASTER_BASE) + slot_number;
	fill_amba_dev_info((AMBA_DEV_INFO *) dev, ahb_slot->id_reg);
	fill_ahb_bar_infos(dev, ahb_slot);

	dev->is_master = TRUE;
	dev->slot = slot_number;
}

inline static void fill_ahbsl_dev_info(AHB_DEV *dev, BYTE slot_number) {
	AHB_SLOT *ahb_slot = ((AHB_SLOT *) AHB_SLAVE_BASE) + slot_number;
	fill_amba_dev_info((AMBA_DEV_INFO *) dev, ahb_slot->id_reg);
	fill_ahb_bar_infos(dev, ahb_slot);

	dev->is_master = TRUE;
	dev->slot = slot_number;
}

int capture_ahbm_dev(AHB_DEV *dev, BYTE ven_id, UINT16 dev_id) {
	int slot_number;
	AHB_SLOT *pslot = (AHB_SLOT *) AHB_MASTER_BASE;

	if (NULL == dev) {
		return -1;
	}

	if (-1 == find_ahbm_dev(ven_id, dev_id)) {
		return -1;
	}

	if (-1 == lock_ahbm_slot(slot_number, dev)) {
		return -1;//a device can be opened only once
	}

	fill_ahbm_dev_info(dev, slot_number);

	return slot_number;
}

int capture_ahbsl_dev(AHB_DEV *dev, BYTE ven_id, UINT16 dev_id) {
	int slot_number;
	//AHB_SLOT *pslot = (AHB_SLOT *) AHB_SLAVE_BASE;

	if (NULL == dev) {
		return -1;
	}

	if (-1 == (slot_number = find_ahbsl_dev(ven_id, dev_id))) {
		return -1;
	}

	if (-1 == lock_ahbsl_slot(slot_number, dev)) {
		return -1;//a device can be opened only once
	}

	fill_ahbsl_dev_info(dev, slot_number);

	return slot_number;
}

inline static void fill_apb_bar_info(AMBA_BAR_INFO *bar, UINT32 ba_reg) {
	bar->start = 0x80000000;
	bar->prefetchable = 0;
	bar->cacheable = 0;
	bar->mask = (0xFFF & ((ba_reg) >> 4));
	bar->type = (0xF & (ba_reg));
	bar->start |= ((((0xFFF & (ba_reg) >> 20)) & bar->mask) << 8);
}

inline static void fill_apb_dev_info(APB_DEV *dev, BYTE slot_number) {
	APB_SLOT *apb_slot = ((APB_SLOT *) APB_BASE) + slot_number;
	fill_amba_dev_info((AMBA_DEV_INFO *) dev, apb_slot->id_reg);
	fill_apb_bar_info(&dev->bar, apb_slot->ba_reg);

	dev->slot = slot_number;
}

/* return slotnumber or -1*/
inline static int find_apb_dev(BYTE ven_id, UINT16 dev_id) {
	int cur_slot;
	APB_SLOT *pslot = ((APB_SLOT *) APB_BASE);
	for (cur_slot = 0; cur_slot < APB_QUANTITY; cur_slot++) {
		if ((ven_id == get_ven(pslot[cur_slot].id_reg)) && (dev_id == get_dev(
				pslot[cur_slot].id_reg)))
			return cur_slot;
	}
	return -1;
}

int capture_apb_dev(APB_DEV *dev, BYTE ven_id, UINT16 dev_id) {
	int slot_number;
	//APB_SLOT *pslot = (APB_SLOT *) APB_BASE;

	if (NULL == dev) {
		return -1;
	}

	if (-1 == (slot_number = find_apb_dev(ven_id, dev_id))) {
		return -1;
	}

	if (-1 == lock_apb_slot(slot_number, dev)) {
		return -1;//a device can be opened only once
	}

	fill_apb_dev_info(dev, slot_number);

	return slot_number;
}

inline static const char *get_ven_name(BYTE ven_id) {
	int i;
	for (i = 0; i < VENDORS_TABLE_LEN; i++) {
		if (vendors_table[i].vendor_id == ven_id) {
			return vendors_table[i].name;
		}
	}
	return NULL;
}

inline static const char* get_dev_name(BYTE ven_id, UINT16 dev_id) {
	int i;
	for (i = 0; i < DEVICES_TABLE_LEN; i++) {
		if ((devs_table[i].vendor_id == ven_id) && (devs_table[i].device_id
				== dev_id)) {
			return devs_table[i].name;
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
inline static void show_bars_type(AMBA_BAR_INFO *bar) {
	switch (bar->type) {
	case 1:
		TRACE("\tapb:");
	case 2:
		TRACE("\tahb:");
		break;

	default:
		TRACE("\t%X", bar->type);
	}
}
inline static void show_bar_info(AMBA_BAR_INFO *bar) {
	if (bar->used) {
		show_bars_type(bar);
		TRACE("%X\n", bar->start);
	}
}
void show_bars_infos(AHB_DEV *dev) {
	int i;

	for (i = 0; i < sizeof(dev->bar) / sizeof(dev->bar[0]); i++) {
		if (dev->bar[i].used) {
			show_bars_type(&dev->bar[i]);
			TRACE("%X\n", dev->bar[i].start);
		}
	}
}
const char UNKNOWN[] = "unknown";
static int print_ahb_entries(AHB_SLOT *base_addr, int amount) {
	int i, count = 0;
	AHB_SLOT *pslot = base_addr;

	char* ven_name;
	char* dev_name;
	//AMBA_DEV_INFO dev_info;
	AHB_DEV dev;

	for (i = 0; i < amount; i++) {
		if (0 != pslot[i].id_reg) {
			fill_amba_dev_info(&dev.dev_info, pslot[i].id_reg);
			if (NULL != (ven_name = (char *) get_ven_name(dev.dev_info.venID))) {
				if (NULL == (dev_name = (char *) get_dev_name(
						dev.dev_info.venID, dev.dev_info.devID))) {
					dev_name = (char *) UNKNOWN;
				}
			} else {
				ven_name = (char *) UNKNOWN;
				dev_name = (char *) UNKNOWN;
			}

			fill_ahb_bar_infos(&dev, &pslot[i]);
			print_table_row(i, dev.dev_info.venID, dev.dev_info.devID,
					ven_name, dev_name, dev.dev_info.version, dev.dev_info.irq);
			show_bars_infos(&dev);
			count++;
		}
	}
	return count;
}
static int print_apb_entries(APB_SLOT *base_addr, int amount) {
	int i, count = 0;
	APB_SLOT *pslot = base_addr;

	char* ven_name;
	char* dev_name;

	APB_DEV dev;

	for (i = 0; i < amount; i++) {
		if (0 != pslot[i].id_reg) {
			fill_amba_dev_info(&dev.dev_info, pslot[i].id_reg);
			if (NULL != (ven_name = (char *) get_ven_name(dev.dev_info.venID))) {
				if (NULL == (dev_name = (char *) get_dev_name(
						dev.dev_info.venID, dev.dev_info.devID))) {
					dev_name = (char *) UNKNOWN;
				}
			} else {
				ven_name = (char *) UNKNOWN;
				dev_name = (char *) UNKNOWN;
			}

			//fill_ahb_bar_infos(&dev, &pslot[i]);
			fill_apb_bar_info(&dev.bar, pslot[i].ba_reg);
			print_table_row(i, dev.dev_info.venID, dev.dev_info.devID,
					ven_name, dev_name, dev.dev_info.version, dev.dev_info.irq);
			show_bar_info(&dev.bar);
			count++;
		}
	}
	return count;
}

int print_ahbm_pnp_devs() {
	int count = 0;
	TRACE("\nAHB masters..\n");
	print_table_head();
	count
	+= print_ahb_entries((AHB_SLOT *) AHB_MASTER_BASE,
			AHB_MASTERS_QUANTITY);
	return count;
}

int print_ahbsl_pnp_devs() {
	int count = 0;
	TRACE("\nAHB slaves..\n");
	print_table_head();
	count
	+= print_ahb_entries((AHB_SLOT *) AHB_SLAVE_BASE,
			AHB_SLAVES_QUANTITY);
	return count;
}

int print_apb_pnp_devs() {
	int count = 0;
	TRACE("\nAPB slaves..\n");
		print_table_head();
		count
		+= print_apb_entries((APB_SLOT *) APB_BASE,
				APB_QUANTITY);
		return count;
}

void print_all_pnp_devs() {
#ifdef RELEASE
	return;
#else

	int count = 0;
	TRACE("\nListing Plug'n'Play devices..\n");

	count +=print_ahbm_pnp_devs();
	count +=print_ahbsl_pnp_devs();
	TRACE("\n..Total: %d\n\n", count);

	count = 0;
	count += print_apb_pnp_devs();
	TRACE("\n..Total: %d\n\n", count);

#endif
}

void print_ahbm_pnp_dev(UINT32 id_reg) {

	if (id_reg >AHB_MASTERS_QUANTITY) {
		TRACE("ERROR: print_ahbm_pnp_dev: Too big arg. The quantity of AHB masters is %d\n",AHB_MASTERS_QUANTITY);
		return;
	}
	print_table_head();
	print_ahb_entries((AHB_SLOT *) AHB_MASTER_BASE + id_reg, 1);
//	printf("print_ahbm_pnp_dev\n");
}

void print_ahbsl_pnp_dev(UINT32 id_reg) {

	if (id_reg >AHB_SLAVES_QUANTITY) {
		TRACE("ERROR: print_ahbm_pnp_dev: Too big arg. The quantity of AHB slaves is %d\n",AHB_SLAVES_QUANTITY);
		return;
	}
	print_table_head();
	print_ahb_entries((AHB_SLOT *) AHB_SLAVE_BASE + id_reg, 1);
//	printf("print_ahbsl_pnp_dev\n");
}

void print_apb_pnp_dev(UINT32 id_reg) {

	if (id_reg >AHB_MASTERS_QUANTITY) {
		TRACE("ERROR: print_ahbm_pnp_dev: Too big arg. The quantity of apb devices is %d\n",APB_QUANTITY);
		return;
	}
	print_table_head();
	print_apb_entries((APB_SLOT *) APB_BASE + id_reg, 1);
//	printf("print_apb_pnp_dev\n");
}
