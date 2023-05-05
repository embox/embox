/**
 * @file stm32_qspi_flash_dev.c
 * @brief QSPI flash device driver (BSP-based)
 * @author Andrew Bursian
 * @date 05.05.2023
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <util/math.h>
#include <util/log.h>

#include <drivers/flash/flash.h>
#include <framework/mod/options.h>

#if defined USE_STM32L475E_IOT01
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_qspi.h" // includes "../Components/mx25r6435f/mx25r6435f.h"
#define QSPI_FLASH_SIZE MX25R6435F_FLASH_SIZE
#define QSPI_BLOCK_SIZE MX25R6435F_SECTOR_SIZE
#define QSPI_ERASE_TIME MX25R6435F_SECTOR_ERASE_MAX_TIME

#else
#error Unsupported platform
#endif

/* reduce flash volume to check it fast */
#define	FLASH_CUT OPTION_GET(NUMBER,flash_cut)
#if	FLASH_CUT
#undef  QSPI_FLASH_SIZE
#define QSPI_FLASH_SIZE FLASH_CUT
#endif // FLASH_CUT

#define	QSPI_ERASE_CHECK OPTION_GET(NUMBER,erase_check)

static int qspi_flash_read(struct flash_dev *dev, uint32_t addr, void *data, size_t len) {
	if (addr + len > QSPI_FLASH_SIZE) {
		log_error("End address is out of range. addr=0x%x,len=0x%x", addr, len);
		return -1;
	}
	/* read can be unaligned */
	memcpy(data, (void *) QSPI_BASE + addr, len);
	return len;
}

static int qspi_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	int res;

	BSP_QSPI_Init(); // exit memory mapped mode

	res = BSP_QSPI_Erase_Sector(block);
	if(res != QSPI_OK) {
		log_error("Fail erasing block %d", block);
		}

	log_debug("Erasing block %d ", block);

	/* check BUSY flags
	if (BSP_QSPI_GetStatus == QSPI_BUSY) printf("QSPI_BUSY\n"); // check MX25R6435F
	if (*(uint32_t*)0xa0001008 & 0x20) printf("QUADSPI_SR BUSY!\n"); // check STM32L475
	*/

	usleep(QSPI_ERASE_TIME * 1000);

	BSP_QSPI_EnableMemoryMappedMode();

#if QSPI_ERASE_CHECK
	/* check entire block */
	for (int a=0; a < QSPI_BLOCK_SIZE; a++) {
		if (*(uint8_t*)(QSPI_BASE + block * QSPI_BLOCK_SIZE + a) != 0xff) {
			printf("Erase fail, block %d, offset %d\n", block, a);
			res = QSPI_ERROR;
			break;
		}
	}
#endif	

	return res;
}

static int qspi_flash_write(struct flash_dev *dev, uint32_t addr, const void *data, size_t len) {
	int res;
	
	if (addr + len > QSPI_FLASH_SIZE) {
		log_error("End address is out of range. addr=0x%x,len=0x%x", addr, len);
		return -1;
	}

	BSP_QSPI_Init(); // exit memory mapped mode

	res = BSP_QSPI_Write((uint8_t*) data, addr, len);
	if(res != QSPI_OK) {
		log_error("QSPI write failed. addr=0x%x,len=0x%x", addr, len);
		BSP_QSPI_EnableMemoryMappedMode();
		return res;
		}

	BSP_QSPI_EnableMemoryMappedMode();

	return len;
}

static int qspi_flash_copy(struct flash_dev *dev, uint32_t base_dst,
				uint32_t base_src, size_t len) {
	log_error("QSPI copy is not realized");
	return -1;
}

static const struct flash_dev_drv qspi_flash_drv = {
	.flash_read = qspi_flash_read,
	.flash_erase_block = qspi_flash_erase_block,
	.flash_program = qspi_flash_write,
	.flash_copy = qspi_flash_copy,
};

static int qspi_flash_init(void *arg) {
	struct flash_dev *flash;

	flash = flash_create("qspiflash", QSPI_FLASH_SIZE);
	if (flash == NULL) {
		log_error("Failed to create flash device!");
		return -1;
	}
	flash->drv = &qspi_flash_drv;
	flash->size = QSPI_FLASH_SIZE;
	flash->num_block_infos = 1;
	flash->block_info[0] = (flash_block_info_t) {
		.block_size = QSPI_BLOCK_SIZE,
		.blocks = QSPI_FLASH_SIZE / QSPI_BLOCK_SIZE,
	};

	return 0;
}

FLASH_DEV_DEF("qspiflash", &qspi_flash_drv, qspi_flash_init);
