/**
 * @file
 *
 * @brief Flash driver for stm32 F3/F4/F7
 *
 * @details F4 and F7 have sectors of different sizes.
 *   For example, STM32F4-Discovery:
 *      - 4 16Kb sectors
 *      - 1 64Kb sector
 *      - 7 128Kb sector
 *   So we use only first 4 64Kb sectors.
 *   For example, STM32F3-Discovery:
 *      - 256 2Kb sectors (they are called pages)
 *   So we use only first 256 2Kb sectors.
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 *             Fixes and adoptation to new STM32Cube
 * @date    23.10.2014
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <drivers/block_dev/flash/stm32_in_chip_flash.h>
#include <drivers/block_dev/flash/stm32flash.h>
#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>
#include <util/log.h>
#include <util/macro.h>
#include <util/math.h>

#define FLASH_NAME stm32flash

/* TODO remove this. it's only temporary */
static_assert(IN_CHIP_FLASH_SECTOR_SIZE == STM32_FLASH_SECTOR_SIZE, "");

IN_CHIP_FLASH_DECL();

FLASH_CACHE_DEF(FLASH_NAME, STM32_FLASH_WORD, IN_CHIP_FLASH_SECTOR_SIZE);

#define FLASH_START_SECTOR \
	((IN_CHIP_FLASH_START - STM32_ADDR_FLASH_SECTOR_0) / IN_CHIP_FLASH_SECTOR_SIZE)

static uint8_t stm32_flash_aligned_word[STM32_FLASH_WORD]
    __attribute__((aligned(STM32_FLASH_WORD)));

static const struct flash_dev_drv stm32_flash_drv;

static int stm32_flash_init(struct flash_dev *dev, void *arg) {
	struct flash_dev *flash;

	assert((IN_CHIP_FLASH_SIZE % IN_CHIP_FLASH_SECTOR_SIZE) == 0);

	flash = flash_create(MACRO_STRING(FLASH_NAME), IN_CHIP_FLASH_SIZE);
	if (flash == NULL) {
		log_error("Failed to create flash device!");
		return -1;
	}
	flash->drv = &stm32_flash_drv;
	flash->size = IN_CHIP_FLASH_END - IN_CHIP_FLASH_START;
	flash->num_block_infos = 1;
	flash->block_info[0] = (struct flash_block_info){
	    .fbi_start_id = IN_CHIP_FLASH_START,
	    .block_size = IN_CHIP_FLASH_SECTOR_SIZE,
	    .blocks = IN_CHIP_FLASH_SIZE / IN_CHIP_FLASH_SECTOR_SIZE,
	};
	flash->fld_aligned_word = stm32_flash_aligned_word;
	flash->fld_word_size = STM32_FLASH_WORD;

	flash->fld_cache = FLASH_CACHE_GET(flash, FLASH_NAME);

	/* We are using only first STM32_FLASH_SECTORS_COUNT sectors */
	assert((flash->block_info[0].blocks + FLASH_START_SECTOR)
	       <= (IN_CHIP_FLASH_SECTOR_SIZE * STM32_FLASH_SECTORS_COUNT));

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

extern int stm32_flash_erase_block(struct flash_dev *dev, uint32_t block);

extern int stm32_flash_read(struct flash_dev *dev, uint32_t base, void *data,
    size_t len);

extern int stm32_flash_program(struct flash_dev *dev, uint32_t base,
    const void *data, size_t len);

static const struct flash_dev_drv stm32_flash_drv = {
    .flash_init = stm32_flash_init,
    .flash_read = stm32_flash_read,
    .flash_erase_block = stm32_flash_erase_block,
    .flash_program = stm32_flash_program,
};

FLASH_DEV_DEF(MACRO_STRING(FLASH_NAME), &stm32_flash_drv);
