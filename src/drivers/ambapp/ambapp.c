/**
 * @file
 * @brief driver for Advanced Microcontroller Bus Architecture (AMBA)
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include <stdint.h>
#include <lib/libds/array.h>
#include <drivers/amba_pnp.h>

/** AMBA High-performance Bus (AHB) slot */
typedef struct ahb_slot {
	uint32_t id_reg;
	uint32_t user_defined[3];
	uint32_t ba_reg[4];
} ahb_slot_t;

/** Advanced Peripheral Bus (APB) slot */
typedef struct apb_slot {
	uint32_t id_reg;
	uint32_t ba_reg[1];
} apb_slot_t;

amba_dev_t *ahbm_devices[AHB_MASTERS_QUANTITY];
amba_dev_t *ahbsl_devices[AHB_SLAVES_QUANTITY];
amba_dev_t *apb_devices[APB_QUANTITY];

typedef int (*LOCK_SLOT)(uint16_t slot, amba_dev_t *dev);

static inline int lock_ahbm_slot(uint16_t slot, amba_dev_t *dev) {
	if (ahbm_devices[slot]) {
		return -1;
	}
	ahbm_devices[slot] = dev;
	return 0;
}

static inline int lock_ahbsl_slot(uint16_t slot, amba_dev_t *dev) {
	if (ahbsl_devices[slot]) {
		return -1;
	}
	ahbsl_devices[slot] = dev;
	return 0;
}

static inline int lock_apb_slot(uint16_t slot, amba_dev_t *dev) {
	if (apb_devices[slot]) {
		return -1;
	}
	apb_devices[slot] = dev;
	return 0;
}

static inline int lock_amba_slot(uint16_t slot, amba_dev_t *dev,
					bool is_ahb, bool is_master) {
	LOCK_SLOT lock_handler;
	if (!is_ahb) {
		lock_handler = lock_apb_slot;
	} else if (!is_master) {
		lock_handler = lock_ahbsl_slot;
	} else {
		lock_handler = lock_ahbm_slot;
	}
	return (-1 == lock_handler(slot, dev)) ? -1 : 0;
}

/**
 * id_reg
 *  _________________________________________
 * |31____24|23____12|11_10|9______5|4______0|
 * |vendor  |device  |00   |version |irq     |
 * |________|________|_____|________|________|
 */
static inline uint8_t get_ven(uint32_t id_reg) {
	return (0xFF & ((id_reg) >> 24));
}

static inline uint16_t get_dev(uint32_t id_reg) {
	return (0xFFF & ((id_reg) >> 12));
}

static inline uint8_t get_irq(uint32_t id_reg) {
	return (0x1F & (id_reg));
}

static inline uint8_t get_version(uint32_t id_reg) {
	return (0x1F & ((id_reg)) >> 5);
}

/**
 * Find apbdev.
 * @param ven_id vendor ID
 * @param dev_id device ID
 * @return slotnumber or -1
 */
static inline int find_apbdev_slotnum(uint8_t ven_id, uint16_t dev_id) {
	apb_slot_t *pslotbase = (apb_slot_t *) APB_BASE;
	size_t cur_slotnum;
	for (cur_slotnum = 0; cur_slotnum < APB_QUANTITY; cur_slotnum++) {
		if ((ven_id == get_ven(pslotbase[cur_slotnum].id_reg))
		    && (dev_id == get_dev(pslotbase[cur_slotnum].id_reg))) {
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
static inline int find_ahbdev_slotnum(uint8_t ven_id, uint16_t dev_id, bool is_master) {
	ahb_slot_t *pslotbase = (ahb_slot_t *)
				(is_master ? AHB_MASTER_BASE : AHB_SLAVE_BASE);
	size_t maxdevs = is_master ? AHB_MASTERS_QUANTITY : AHB_SLAVES_QUANTITY;
	size_t cur_slotnum;
	for (cur_slotnum = 0; cur_slotnum < maxdevs; cur_slotnum++) {
		if ((ven_id == get_ven(pslotbase[cur_slotnum].id_reg))
		    && (dev_id == get_dev(pslotbase[cur_slotnum].id_reg))) {
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
static inline void fill_ahb_bar_info(amba_bar_info_t *bar, uint32_t ba_reg) {
	if (ba_reg) {
		bar->start = ba_reg & 0xFFF00000;
		bar->prefetchable = (0x1 & ((ba_reg) >> 17));
		bar->cacheable = (0x1 & ((ba_reg) >> 16));
		bar->mask = (0xFFF & ((ba_reg) >> 4));
		bar->type = (0xF & (ba_reg));
		bar->used = true;
	} else {
		bar->used = false;
	}
}

static inline void fill_ahb_bar_infos(amba_dev_t *dev, ahb_slot_t *ahb_slot) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(dev->bar); i++) {
		fill_ahb_bar_info(&dev->bar[i], ahb_slot->ba_reg[i]);
	}
}

/**
 * Fill APB info.
 * @param bar amba bar info
 * @param ba_reg Bank Address Register
 */
static inline void fill_apb_bar_info(amba_bar_info_t *bar, uint32_t ba_reg) {
	bar->start = 0x80000000;
	bar->prefetchable = 0;
	bar->cacheable = 0;
	bar->mask = (0xFFF & ((ba_reg) >> 4));
	bar->type = (0xF & (ba_reg));
	bar->start |= ((((0xFFF & (ba_reg) >> 20)) & bar->mask) << 8);
}

/**
 * Fill AMBA dev info.
 * @param dev_info device info struct
 * @param id_reg Id Register
 */
static inline void fill_amba_dev_info(amba_dev_info_t *dev_info, uint32_t id_reg) {
	dev_info->venID = get_ven(id_reg);
	dev_info->devID = get_dev(id_reg);
	dev_info->irq = get_irq(id_reg);
	dev_info->version = get_version(id_reg);
}

int fill_amba_dev(amba_dev_t *dev, uint8_t slot_number, bool is_ahb, bool is_master) {
	/* ATTENTION! DON'T USE ANY printf IN THIS FUNCTION */
	ahb_slot_t *slot;
	if (!is_ahb) {
		apb_slot_t *slot = ((apb_slot_t *) APB_BASE) + slot_number;
		if (0 == slot->id_reg) {
			return -1;
		}
		fill_amba_dev_info((amba_dev_info_t *) dev, slot->id_reg);
		dev->slot = slot_number;

		fill_apb_bar_info(&dev->bar[0], slot->ba_reg[0]);
		return 0;
	}
	dev->is_master = is_master;
	if (!is_master) {
		slot = ((ahb_slot_t *) AHB_SLAVE_BASE) + slot_number;
	} else {
		slot = ((ahb_slot_t *) AHB_MASTER_BASE) + slot_number;
	}
	if (0 == slot->id_reg) {
		return -1;
	}

	fill_amba_dev_info((amba_dev_info_t *) dev, slot->id_reg);
	dev->slot = slot_number;
	fill_ahb_bar_infos(dev, slot);
	return 0;
}

int capture_amba_dev(amba_dev_t *dev, uint8_t ven_id,
		uint16_t dev_id, bool is_ahb, bool is_master) {
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
		/* can't find slot */
		return -1;
	}
	if (-1 == lock_amba_slot(slot_number, dev, is_ahb, is_master)) {
		/* a device can be opened only once */
		return -1;
	}
	fill_amba_dev(dev, slot_number, is_ahb, is_master);
	return slot_number;
}

int free_amba_dev(amba_dev_t *dev) {
	//TODO: we can't free device
	return 0;
}
