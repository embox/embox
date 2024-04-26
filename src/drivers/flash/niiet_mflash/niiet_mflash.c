/**
 * @file
 *
 * @brief Flash driver for NIIET mflash (inchip)
 *
 * @author  Anton Bondarev
 * @date    26.04.2024
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <drivers/block_dev/flash/in_chip_flash.h>
#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>

#include <util/macro.h>
#include <util/math.h>

#define FLASH_NAME          inchip_flash
#define FLASH_WORD_SIZE     (4)
#define FLASH_ADDR_SECTOR_0 (0x80000000UL)

IN_CHIP_FLASH_DECL();

FLASH_CACHE_DEF(FLASH_NAME, FLASH_WORD_SIZE, IN_CHIP_FLASH_SECTOR_SIZE);

#define FLASH_START_SECTOR \
	((IN_CHIP_FLASH_START - FLASH_ADDR_SECTOR_0) / IN_CHIP_FLASH_SECTOR_SIZE)


static const struct flash_dev_drv niiet_mflash_drv;

static int niiet_mflash_init(struct flash_dev *dev, void *arg) {
	struct flash_dev *flash;

	assert((IN_CHIP_FLASH_SIZE % IN_CHIP_FLASH_SECTOR_SIZE) == 0);

	flash = flash_create(MACRO_STRING(FLASH_NAME), IN_CHIP_FLASH_SIZE);
	if (flash == NULL) {
		log_error("Failed to create flash device!");
		return -1;
	}
	flash->drv = &niiet_mflash_drv;
	flash->size = IN_CHIP_FLASH_END - IN_CHIP_FLASH_START;
	flash->num_block_infos = 1;
	flash->block_info[0] = (struct flash_block_info){
	    .fbi_start_id = IN_CHIP_FLASH_START,
	    .block_size = IN_CHIP_FLASH_SECTOR_SIZE,
	    .blocks = IN_CHIP_FLASH_SIZE / IN_CHIP_FLASH_SECTOR_SIZE,
	};
	flash->fld_aligned_word = NULL;
	flash->fld_word_size = FLASH_WORD_SIZE;

	flash->fld_cache = FLASH_CACHE_GET(flash, FLASH_NAME);

	log_debug("");
	log_debug("Flash info:");
	log_debug("  Flash start address = 0x%08x", IN_CHIP_FLASH_START);
	log_debug("  Flash start sector  = %d", FLASH_START_SECTOR);
	log_debug("  Flash size   = 0x%x", flash->size);
	log_debug("  Block size   = 0x%x", flash->block_info[0].block_size);
	log_debug("  Blocks count = 0x%x", flash->block_info[0].blocks);
	log_debug("");

	return 0;
}

static int niiet_mflash_erase_block(struct flash_dev *dev, uint32_t block) {
	return 0;
}

static int niiet_mflash_read(struct flash_dev *dev, uint32_t base, void *data,
    size_t len) {
		return 0;
}

static int niiet_mflash_program(struct flash_dev *dev, uint32_t base,
    const void *data, size_t len) {
		return 0;
}

static const struct flash_dev_drv niiet_mflash_drv = {
    .flash_init = niiet_mflash_init,
    .flash_read = niiet_mflash_read,
    .flash_erase_block = niiet_mflash_erase_block,
    .flash_program = niiet_mflash_program,
};

FLASH_DEV_DEF(MACRO_STRING(FLASH_NAME), &niiet_mflash_drv);
