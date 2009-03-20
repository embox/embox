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
}AHB_SLOT;

typedef struct _APB_SLOT {
	UINT32 id_reg;
	UINT32 ba_reg[1];
}APB_SLOT;

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
static AMBA_DEV *ahbm_devices[AHB_MASTERS_QUANTITY];
static AMBA_DEV *ahbsl_devices[AHB_SLAVES_QUANTITY];
static AMBA_DEV *apb_devices[APB_QUANTITY];

inline static int lock_ahbm_slot(UINT16 slot, AMBA_DEV *dev) {
	if (ahbm_devices[slot])
		return -1;
	ahbm_devices[slot] = dev;
	return 0;
}
inline static int lock_ahbsl_slot(UINT16 slot, AMBA_DEV *dev) {
	if (ahbsl_devices[slot])
		return -1;
	ahbsl_devices[slot] = dev;
	return 0;
}

inline static int lock_apb_slot(UINT16 slot, AMBA_DEV *dev) {
	if (apb_devices[slot])
		return -1;
	apb_devices[slot] = dev;
	return 0;
}

inline static int lock_amba_slot (UINT16 slot, AMBA_DEV *dev, BOOL is_ahb, BOOL is_master) {
	if (!is_ahb) {
		if (-1 == lock_apb_slot(slot, dev)) 	 return -1;
		return 0;
	}
	if (!is_master) {
		if (-1 == lock_ahbsl_slot(slot, dev)) return -1;
		return 0;
	}

	if (-1 == lock_ahbm_slot(slot, dev)) 	return -1;
	return 0;
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
inline static int find_apbdev(BYTE ven_id, UINT16 dev_id)
{
	int cur_slot;
	APB_SLOT *pslot = ((APB_SLOT *) APB_BASE);
	for (cur_slot = 0; cur_slot < APB_QUANTITY; cur_slot++) {
		if ((ven_id == get_ven(pslot[cur_slot].id_reg)) && (dev_id == get_dev(
			pslot[cur_slot].id_reg)))
			return cur_slot;
	}
}

/* return slotnumber or -1*/
inline static int find_amba_dev(BYTE ven_id, UINT16 dev_id, BOOL is_ahb, BOOL is_master) {
	int cur_slot;
	int base;
	int quantity;

	if (!is_ahb)
		return find_apbdev(ven_id, dev_id);
	if (is_master)
	{
		base = AHB_MASTER_BASE;
		quantity = AHB_MASTERS_QUANTITY;
	}
	else
	{
		base = AHB_SLAVE_BASE;
		quantity = AHB_SLAVES_QUANTITY;
	}
	AHB_SLOT *pslot = ((AHB_SLOT *) base);
	for (cur_slot = 0; cur_slot < quantity; cur_slot++) {
		if ((ven_id == get_ven(pslot[cur_slot].id_reg)) && (dev_id == get_dev(
				pslot[cur_slot].id_reg)))
			return cur_slot;
	}

	return -1;
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

inline static void fill_ahb_bar_infos(AMBA_DEV *dev, AHB_SLOT *ahb_slot) {
	int i;
	for (i = 0; i < sizeof(dev->bar) / sizeof(dev->bar[0]); i++)
		fill_ahb_bar_info(&dev->bar[i], ahb_slot->ba_reg[i]);
}

inline static void fill_apb_bar_info(AMBA_BAR_INFO *bar, UINT32 ba_reg) {
	bar->start = 0x80000000;
	bar->prefetchable = 0;
	bar->cacheable = 0;
	bar->mask = (0xFFF & ((ba_reg) >> 4));
	bar->type = (0xF & (ba_reg));
	bar->start |= ((((0xFFF & (ba_reg) >> 20)) & bar->mask) << 8);
}

inline static void fill_amba_dev_info(AMBA_DEV_INFO *dev_info, UINT32 id_reg) {
	dev_info->venID = get_ven(id_reg);
	dev_info->devID = get_dev(id_reg);
	dev_info->irq = get_irq(id_reg);
	dev_info->version = get_version(id_reg);
}

inline static void fill_amba_dev(AMBA_DEV *dev, BYTE slot_number, BOOL is_ahb, BOOL is_master) {
	int base;

	if (!is_ahb) {
		APB_SLOT *slot = ((APB_SLOT *) APB_BASE) + slot_number;
		fill_amba_dev_info((AMBA_DEV_INFO *) dev, slot->id_reg);
		dev->slot = slot_number;

		fill_apb_bar_info(&dev->bar[0], slot->ba_reg[0]);
		return;
	}
	AHB_SLOT *slot = ((AHB_SLOT *) base) + slot_number;
	if (!is_master) {
		dev->is_master = FALSE;
		slot = ((AHB_SLOT *) AHB_SLAVE_BASE) + slot_number;
	} else {
		dev->is_master = TRUE;
		slot = ((AHB_SLOT *) AHB_MASTER_BASE) + slot_number;
	}
	fill_amba_dev_info((AMBA_DEV_INFO *) dev, slot->id_reg);
	dev->slot = slot_number;
	fill_ahb_bar_infos(dev, slot);

}

int capture_amba_dev(AMBA_DEV *dev, BYTE ven_id, UINT16 dev_id, BOOL is_ahb, BOOL is_master) {
	int slot_number;

	if (NULL == dev) {
		return -1;
	}

	if (-1 == (slot_number = find_amba_dev(ven_id, dev_id, is_ahb, is_master))) {
		return -1;
	}

	if (-1 == lock_amba_slot(slot_number, dev, is_ahb, is_master)) {
		return -1;//a device can be opened only once
	}
	fill_amba_dev(dev, slot_number, is_ahb, is_master);
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

void show_bars_infos(AMBA_DEV *dev) {
	int i;

	for (i = 0; i < sizeof(dev->bar) / sizeof(dev->bar[0]); i++) {
		if (dev->bar[i].used) {
			show_bars_type(&dev->bar[i]);
			TRACE("%X\n", dev->bar[i].start);
		}
	}
}

const char UNKNOWN[] = "unknown";

static void show_dev(AMBA_DEV *dev, BOOL show_user) {

	if (NULL == dev) return ;

	char *ven_name;
	char *dev_name;

	if ((!show_user)||(NULL == dev->handler_data )) {
		//standard out
		if (NULL != (ven_name = (char *) get_ven_name(dev->dev_info.venID))) {
			if (NULL == (dev_name = (char *) get_dev_name(dev->dev_info.venID,
					dev->dev_info.devID))) {
				dev_name = (char *) UNKNOWN;
			}
		} else {
			ven_name = (char *) UNKNOWN;
			dev_name = (char *) UNKNOWN;
		}
		print_table_row(0, dev->dev_info.venID, dev->dev_info.devID, ven_name,
				dev_name, dev->dev_info.version, dev->dev_info.irq);
		show_bar_info(dev->bar);
		return;
	}
	//all info out using handler
	(HANDLER_DATA_FUNC *) dev->handler_data;
	return;

}

static int print_amba_entries(UINT32 *base_addr, int amount, BOOL is_ahb, BOOL is_master) {
	int i, count = 0;
	AMBA_DEV dev;

	if (!is_ahb) {
		APB_SLOT *pslot = (APB_SLOT *)base_addr;
		for (i = 0; i < amount; i++) {
			if (0 != pslot[i].id_reg) {
				fill_amba_dev(&dev, pslot[i].id_reg, is_ahb, is_master);
				show_dev(&dev, FALSE);
				count++;
			}
		}
		return;
	}
	AHB_SLOT *pslot = (AHB_SLOT *)base_addr;
			for (i = 0; i < amount; i++) {
				if (0 != pslot[i].id_reg) {
					fill_amba_dev(&dev, pslot[i].id_reg, is_ahb, is_master);
					show_dev(&dev, FALSE);
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
	+= print_amba_entries((UINT32 *) AHB_MASTER_BASE,
			AHB_MASTERS_QUANTITY, TRUE, TRUE);
	return count;
}

int print_ahbsl_pnp_devs() {
	int count = 0;
	TRACE("\nAHB slaves..\n");
	print_table_head();
	count
	+= print_amba_entries((UINT32 *) AHB_SLAVE_BASE,
			AHB_SLAVES_QUANTITY, TRUE, FALSE);
	return count;
}

int print_apb_pnp_devs() {
	int count = 0;
	TRACE("\nAPB slaves..\n");
		print_table_head();
		count
		+= print_amba_entries((UINT32 *) APB_BASE,
				APB_QUANTITY, FALSE, FALSE);
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

void print_ahbm_pnp_dev(UINT32 slot) {

	if (slot >AHB_MASTERS_QUANTITY) {
		TRACE("ERROR: print_ahbm_pnp_dev: Too big arg. The quantity of AHB masters is %d\n",AHB_MASTERS_QUANTITY);
		return;
	}
	AMBA_DEV dev;
	fill_amba_dev(&dev, slot, TRUE, TRUE);
	show_dev(&dev , TRUE);
}

void print_ahbsl_pnp_dev(UINT32 slot) {

	if (slot >AHB_SLAVES_QUANTITY) {
		TRACE("ERROR: print_ahbsl_pnp_dev: Too big arg. The quantity of AHB slaves is %d\n",AHB_SLAVES_QUANTITY);
		return;
	}
	AMBA_DEV dev;
	fill_amba_dev(&dev, slot, TRUE, FALSE);
	show_dev(&dev , TRUE);
}

void print_apb_pnp_dev(UINT32 slot) {


	if (slot >APB_QUANTITY) {
		TRACE("ERROR: print_apb_pnp_dev: Too big arg. The quantity of APB devices is %d\n",APB_QUANTITY);
		return;
	}
	AMBA_DEV dev;
	fill_amba_dev(&dev, slot, FALSE, FALSE);
	show_dev(&dev , TRUE);
}
