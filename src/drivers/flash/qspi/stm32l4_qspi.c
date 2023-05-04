/**
 * @file stm32l4_qspi.c
 * @brief QSPI flash driver (currently just for STM32L4Discovery)
 * @author Andrew Bursian
 * @date 04.05.2023
 */

#include <embox/unit.h>

#if defined USE_BL475EIOT01A
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_qspi.h" // includes "../Components/mx25r6435f/mx25r6435f.h"
#else
#error Unsupported platform
#endif

EMBOX_UNIT_INIT(stm32l4_qspi_init);

static int stm32l4_qspi_init(void) {
	BSP_QSPI_Init();

	BSP_QSPI_EnableMemoryMappedMode();

	return 0;
}

// ================================================
// Adding flash device functionality for MX25R6435F

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <util/math.h>
#include <util/log.h>

#include <drivers/flash/flash.h>
#include <stm32l4xx_hal.h>

/* reduce flash volume
#undef MX25R6435F_FLASH_SIZE
#define MX25R6435F_FLASH_SIZE	0x8000
*/

static int qspi_flash_read(struct flash_dev *dev, uint32_t addr, void *data, size_t len) {
	if (addr + len > MX25R6435F_FLASH_SIZE) {
		log_error("End address is out of range. addr=0x%x,len=0x%x", addr, len);
		return -1;
	}
	/* read can be unaligned */
	memcpy(data, (void *) QSPI_BASE + addr, len);
	return len;
}

static int qspi_flash_erase_sector(struct flash_dev *dev, uint32_t sector) {
	int res;

	BSP_QSPI_Init(); // exit memory mapped mode

	res = BSP_QSPI_Erase_Sector(sector);
	if(res != QSPI_OK) {
		log_error("Fail erasing sector %d", sector);
		}

	log_debug("Erasing sector %d ", sector);

	/* check BUSY flags
	if (BSP_QSPI_GetStatus == QSPI_BUSY) printf("QSPI_BUSY\n"); // check MX25R6435F
	if (*(uint32_t*)0xa0001008 & 0x20) printf("QUADSPI_SR BUSY!\n"); // check STM32L475
	*/

	usleep(100000);

	BSP_QSPI_EnableMemoryMappedMode();
	/* check entire sector
	for (int a=0; a < MX25R6435F_SECTOR_SIZE; a++) {
		if (*(uint8_t*)(QSPI_BASE + sector * MX25R6435F_SECTOR_SIZE + a) != 0xff) {
			printf("Erase fail, sector %d, offset %d\n", sector, a); break;
		}
	}
	*/
	return res;
}

static int qspi_flash_write(struct flash_dev *dev, uint32_t addr, const void *data, size_t len) {
	int res;
	
	if (addr + len > MX25R6435F_FLASH_SIZE) {
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
	.flash_erase_block = qspi_flash_erase_sector,
	.flash_program = qspi_flash_write,
	.flash_copy = qspi_flash_copy,
};

static int qspi_flash_init(void *arg) {
	struct flash_dev *flash;

	flash = flash_create("qspiflash", MX25R6435F_FLASH_SIZE);
	if (flash == NULL) {
		log_error("Failed to create flash device!");
		return -1;
	}
	flash->drv = &qspi_flash_drv;
	flash->size = MX25R6435F_FLASH_SIZE;
	flash->num_block_infos = 1;
	flash->block_info[0] = (flash_block_info_t) {
		.block_size = MX25R6435F_SECTOR_SIZE,
		.blocks = MX25R6435F_FLASH_SIZE / MX25R6435F_SECTOR_SIZE,
	};

	return 0;
}

FLASH_DEV_DEF("qspiflash", &qspi_flash_drv, qspi_flash_init);
