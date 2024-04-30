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

#include <hal/reg.h>

#include <drivers/block_dev/flash/in_chip_flash.h>
#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>

#include <util/macro.h>
#include <util/math.h>

#define FLASH_NAME          inchip_flash
#define FLASH_WORD_SIZE     (4*4) /* 4 32bit words*/
#define FLASH_ADDR_SECTOR_0 (0x80000000UL)

IN_CHIP_FLASH_DECL();

FLASH_CACHE_DEF(FLASH_NAME, FLASH_WORD_SIZE, IN_CHIP_FLASH_SECTOR_SIZE);

#define FLASH_START_SECTOR \
	((IN_CHIP_FLASH_START - FLASH_ADDR_SECTOR_0) / IN_CHIP_FLASH_SECTOR_SIZE)

static uint32_t niiet_mflash_fld_aligned_word[4];
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
	flash->fld_aligned_word = &niiet_mflash_fld_aligned_word[0];
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

#define MAIN_REGION             0
#define NVR_REGION              1
/*-- MFLASH ------------------------------------------------------------------*/
#define MFLASH_PAGE_SIZE        4096
#define MFLASH_PAGE_TOTAL       256
#define MFLASH_WORD_WIDTH       4
#define MFLASH_BASE             ((void*)0x3000D000)
#define MFLASH_BANK_ADDR        0x80000000

#if 0
#define MFLASH_ADDR             (*(volatile uint32_t*)(0x3000D000u))
#define MFLASH_DATA0            (*(volatile uint32_t*)(0x3000D004u))
#define MFLASH_DATA1            (*(volatile uint32_t*)(0x3000D008u))
#define MFLASH_DATA2            (*(volatile uint32_t*)(0x3000D00Cu))
#define MFLASH_DATA3            (*(volatile uint32_t*)(0x3000D010u))
#define MFLASH_CMD              (*(volatile uint32_t*)(0x3000D044u))
#define MFLASH_STAT             (*(volatile uint32_t*)(0x3000D048u))
#endif

#define MFLASH_ADDR             ((0x3000D000u))
#define MFLASH_DATA0            ((0x3000D004u))
#define MFLASH_DATA1            ((0x3000D008u))
#define MFLASH_DATA2            ((0x3000D00Cu))
#define MFLASH_DATA3            ((0x3000D010u))
#define MFLASH_CMD              ((0x3000D044u))
#define MFLASH_STAT             ((0x3000D048u))

/*---- MFLASH->CMD: Command register */
#define MFLASH_CMD_RD           (1<<0)              /* Read data in region */
#define MFLASH_CMD_WR           (1<<1)              /* Write data in region */
#define MFLASH_CMD_ERSEC        (1<<2)              /* Sector erase in region */
#define MFLASH_CMD_ERALL        (1<<3)              /* Erase all sectors in region */
#define MFLASH_CMD_NVRON        (1<<8)              /* Select NVR region for command operation */
#define MFLASH_CMD_KEY          (0xC0DE<<16)        /* Command enable key */
/*---- MFLASH->STAT: Status register */
#define MFLASH_STAT_BUSY        (1<<0)              /* Flag operation busy */

static inline int niiet_mflash_get_start_sector(struct flash_dev *dev) {
	return ((dev->block_info[0].fbi_start_id - MFLASH_BANK_ADDR)
	        / dev->block_info[0].block_size);
}

/**
 * Wait while operation with flash being performed
 */
static int niiet_mflash_waitdone(struct flash_dev *dev) {
	uint32_t flash_status;

	do {
		flash_status = REG32_LOAD(MFLASH_STAT);
	} while(flash_status & MFLASH_STAT_BUSY);

	return 0;
}

static int niiet_mflash_erase_block(struct flash_dev *dev, uint32_t block) {
	uint32_t addr;

	addr = dev->block_info[0].fbi_start_id + block * MFLASH_PAGE_SIZE;

	niiet_mflash_waitdone(dev);
	REG32_STORE(MFLASH_ADDR, addr);
	REG32_STORE(MFLASH_CMD, MFLASH_CMD_KEY | MFLASH_CMD_ERSEC);
	niiet_mflash_waitdone(dev);

	return 0;
}

static int niiet_mflash_read(struct flash_dev *dev, uint32_t base, void *data,
    size_t len) {
	
	if (base + len > dev->size) {
		log_error("Address is out of range. Base=0x%x,len=0x%x", base, len);
		return -1;
	}
	/* read can be unaligned */
	memcpy(data, (void *)(dev->block_info[0].fbi_start_id) + base, len);
	
	return len;
}

static int niiet_mflash_program(struct flash_dev *dev, uint32_t base,
    const void *data, size_t len) {
	uint32_t addr;
	uint32_t *d = (void *)data;
	int cnt;

	for(cnt = 0; cnt < len; cnt += 16) {
		addr = dev->block_info[0].fbi_start_id + base + cnt;

		niiet_mflash_waitdone(dev);
		REG32_STORE(MFLASH_ADDR, addr);

		REG32_STORE(MFLASH_DATA0, d[0]);
		REG32_STORE(MFLASH_DATA1, d[1]);
		REG32_STORE(MFLASH_DATA2, d[2]);
		REG32_STORE(MFLASH_DATA3, d[3]);

		REG32_STORE(MFLASH_CMD, MFLASH_CMD_KEY | MFLASH_CMD_WR);
		niiet_mflash_waitdone(dev);

		d += 4;
	}

	return len;
}

static const struct flash_dev_drv niiet_mflash_drv = {
    .flash_init = niiet_mflash_init,
    .flash_read = niiet_mflash_read,
    .flash_erase_block = niiet_mflash_erase_block,
    .flash_program = niiet_mflash_program,
};

FLASH_DEV_DEF(MACRO_STRING(FLASH_NAME), &niiet_mflash_drv);
