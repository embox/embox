/**
 * \file amba_pnp.c
 *
 * \date 28.01.2009
 * \author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include "types.h"
#include "common.h"
#include "amba_pnp.h"


/**
 * \struct AHB_SLOT
 */
typedef struct _AHB_SLOT {
	UINT32 id_reg;
	UINT32 user_defined[3];
	UINT32 ba_reg[4];
}AHB_SLOT;

/**
 * \struct APB_SLOT
 */
typedef struct _APB_SLOT {
	UINT32 id_reg;
	UINT32 ba_reg[1];
}APB_SLOT;


inline static int lock_ahbm_slot(UINT16 slot, AMBA_DEV *dev) {
	if (ahbm_devices[slot]) {
		return -1;
	}
	ahbm_devices[slot] = dev;
	return 0;
}

inline static int lock_ahbsl_slot(UINT16 slot, AMBA_DEV *dev) {
	if (ahbsl_devices[slot]) {
		return -1;
	}
	ahbsl_devices[slot] = dev;
	return 0;
}

inline static int lock_apb_slot(UINT16 slot, AMBA_DEV *dev) {
	if (apb_devices[slot]) {
		return -1;
	}
	apb_devices[slot] = dev;
	return 0;
}

inline static int lock_amba_slot (UINT16 slot, AMBA_DEV *dev, BOOL is_ahb, BOOL is_master) {
	if (!is_ahb) {
		return (-1 == lock_apb_slot(slot, dev)) ? -1 : 0;
	}
	if (!is_master) {
		return (-1 == lock_ahbsl_slot(slot, dev)) ? -1 : 0;
	}
	return (-1 == lock_ahbm_slot(slot, dev)) ? -1 : 0;
}

/**
 * id_reg
 *  _________________________________________
 * |31____24|23____12|11_10|9______5|4______0|
 * |vendor  |device  |00   |version |irq     |
 * |________|________|_____|________|________|
 */
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

/**
 * Find apbdev.
 * @param ven_id vendor ID
 * @param dev_id device ID
 * @return slotnumber or -1
 */
inline static int find_apbdev_slotnum(BYTE ven_id, UINT16 dev_id) {
	APB_SLOT *pslotbase = ((APB_SLOT *) APB_BASE);
	int cur_slotnum;
	for (cur_slotnum = 0; cur_slotnum < APB_QUANTITY; cur_slotnum++) {
		if ( (ven_id == get_ven(pslotbase[cur_slotnum].id_reg))
		    && (dev_id == get_dev(pslotbase[cur_slotnum].id_reg)) ) {
			return cur_slotnum;
		}
	}
	return -1;
}

/**
 * Find ahbdev.
 * @param ven_id vendor ID
 * @param dev_id device ID
 * @param is_master master/slave
 * @return slotnumber or -1
 */
inline static int find_ahbdev_slotnum(BYTE ven_id, UINT16 dev_id, BOOL is_master) {
	AHB_SLOT *pslotbase = (AHB_SLOT *) (is_master ? AHB_MASTER_BASE : AHB_SLAVE_BASE);
	int maxdevs = is_master ? AHB_MASTERS_QUANTITY : AHB_SLAVES_QUANTITY;
	int cur_slotnum;
	for (cur_slotnum = 0; cur_slotnum < maxdevs; cur_slotnum++) {
		if ( (ven_id == get_ven(pslotbase[cur_slotnum].id_reg))
		    && (dev_id == get_dev(pslotbase[cur_slotnum].id_reg)) ) {
			return cur_slotnum;
		}
	}
	return -1;
}

/**
 * Fill AHB info.
 * @param bar amba bar info
 * @param ba_reg Bank Address Register
 *  _____________________________________
 * |31_____20|19|18|17|16|15______4|3___0|
 * |address  |0 |0 |0 |0 |mask     |type |
 * |_________|__|__|__|__|_________|_____|
 * type: 0001 - APB I/O space
 *       0010 - AHB Memory space
 *       0011 - AHB I/O space
 */
inline static void fill_ahb_bar_info(AMBA_BAR_INFO *bar, UINT32 ba_reg) {
	if (ba_reg) {
		bar->start = ba_reg & 0xFFF00000;
		bar->prefetchable = (0x1 & ((ba_reg) >> 17));
		bar->cacheable = (0x1 & ((ba_reg) >> 16));
		bar->mask = (0xFFF & ((ba_reg) >> 4));
		bar->type = (0xF & (ba_reg));
		bar->used = TRUE;
	} else {
		bar->used = FALSE;
	}
}

inline static void fill_ahb_bar_infos(AMBA_DEV *dev, AHB_SLOT *ahb_slot) {
	int i;
	for (i = 0; i < array_len(dev->bar); i++) {
		fill_ahb_bar_info(&dev->bar[i], ahb_slot->ba_reg[i]);
	}
}

/**
 * Fill APB info.
 * @param bar amba bar info
 * @param ba_reg Bank Address Register
 */
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

/**
 * @return TRUE (1) if successed
 * @return FALSE (0) slot is empty
 */
BOOL fill_amba_dev(AMBA_DEV *dev, BYTE slot_number, BOOL is_ahb, BOOL is_master) {
	/* ATTENTION! DON'T USE ANY printf IN THIS FUNCTION */
	int base;
	if (!is_ahb) {
		APB_SLOT *slot = ((APB_SLOT *) APB_BASE) + slot_number;
		if (0 == slot->id_reg)
			return FALSE;
		fill_amba_dev_info((AMBA_DEV_INFO *) dev, slot->id_reg);
		dev->slot = slot_number;

		fill_apb_bar_info(&dev->bar[0], slot->ba_reg[0]);
		return;
	}
	dev->is_master = is_master;
	AHB_SLOT *slot;
	if (!is_master) {
		slot = ((AHB_SLOT *) AHB_SLAVE_BASE) + slot_number;
	} else {
		slot = ((AHB_SLOT *) AHB_MASTER_BASE) + slot_number;
	}
	if (0 == slot->id_reg)
		FALSE;

	fill_amba_dev_info((AMBA_DEV_INFO *) dev, slot->id_reg);
	dev->slot = slot_number;
	fill_ahb_bar_infos(dev, slot);
}

int capture_amba_dev(AMBA_DEV *dev, BYTE ven_id, UINT16 dev_id, BOOL is_ahb, BOOL is_master) {
	int slot_number;
	if (dev == NULL) {
		return -1;
	}
	if (is_ahb) {
		slot_number = find_ahbdev_slotnum(ven_id, dev_id, is_master);
	} else {
		slot_number = find_apbdev_slotnum(ven_id, dev_id);
	}
	if (slot_number == -1) {
		LOG_WARN("can't find slot\n");
		return -1;
	}
	if (-1 == lock_amba_slot(slot_number, dev, is_ahb, is_master)) {
		LOG_WARN("a device can be opened only once\n");
		return -1;
	}
	fill_amba_dev(dev, slot_number, is_ahb, is_master);
	return slot_number;
}

int free_amba_dev(AMBA_DEV *dev) {
	//TODO:
	return 0;
}
