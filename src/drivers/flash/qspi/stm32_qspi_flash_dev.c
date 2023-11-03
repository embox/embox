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

//#include <arm/cpu_cache.h>
#include <drivers/flash/flash.h>
#include <framework/mod/options.h>

extern uintptr_t flash_cache_addr(struct flash_dev *dev);

#if defined USE_STM32L475E_IOT01
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_qspi.h" // includes "../Components/mx25r6435f/mx25r6435f.h"
#define QSPI_FLASH_SIZE MX25R6435F_FLASH_SIZE
#define QSPI_BLOCK_SIZE MX25R6435F_SECTOR_SIZE
#define QSPI_ERASE_TIME MX25R6435F_SECTOR_ERASE_MAX_TIME

#elif defined USE_STM32F769I_DISCOVERY
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_qspi.h" // includes "../Components/mx25l512/mx25l512.h"
#define QSPI_FLASH_SIZE MX25L512_FLASH_SIZE
#define QSPI_BLOCK_SIZE MX25L512_SUBSECTOR_SIZE
#define QSPI_ERASE_TIME MX25L512_SUBSECTOR_ERASE_MAX_TIME

#error Unsupported platform // not tested properly yet

#else
#error Unsupported platform
#endif // defined USE_XXX

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

static inline int poll_flash_busy(int maxms) {
	clock_t t0 = clock();
	while ((clock()-t0 < CLOCKS_PER_SEC*maxms/1000) && (BSP_QSPI_GetStatus() == QSPI_BUSY)) {
		usleep(1000);
	}
	return (clock()-t0)*1000/CLOCKS_PER_SEC;
}

static int qspi_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	int res;
log_debug("Block %d", block);
	BSP_QSPI_Init(); // exit memory mapped mode
	if(poll_flash_busy(100) >= 100) {
		log_error("Fail after BSP_QSPI_Init: flash chip busy > 100 ms");
		BSP_QSPI_EnableMemoryMappedMode();
		return QSPI_BUSY;
	}

#if defined USE_STM32L475E_IOT01
	res = BSP_QSPI_Erase_Sector(block);
#elif defined USE_STM32F769I_DISCOVERY
	res = BSP_QSPI_Erase_Block(block * QSPI_BLOCK_SIZE);
#else
#error Unsupported platform
#endif // defined USE_XXX
	if(res != QSPI_OK) {
		log_error("Fail erasing block %d", block);
		}
	if(poll_flash_busy(QSPI_ERASE_TIME) >= QSPI_ERASE_TIME) {
		log_error("Fail erasing block: flash chip busy > %d ms", QSPI_ERASE_TIME);
		res = QSPI_BUSY;
	}

	BSP_QSPI_EnableMemoryMappedMode();
	//dcache_flush_all();	// or use dcache_flush(const void *p, size_t size);

#if QSPI_ERASE_CHECK
	/* check entire block */
	for (int a=0; a < QSPI_BLOCK_SIZE; a++) {
		if (*(uint8_t*)(QSPI_BASE + block * QSPI_BLOCK_SIZE + a) != 0xff) {
			log_error("Erase fail, block %d, offset %d", block, a);
			res = QSPI_ERROR;
			break;
		}
	}
#endif	
log_debug("Ok");
	return res;
}

static int qspi_flash_write(struct flash_dev *dev, uint32_t addr, const void *data, size_t len) {
	int res;
int t0=clock();
log_debug("Addr %d, length %d", addr, len);
	if (addr + len > QSPI_FLASH_SIZE) {
		log_error("End address is out of range. addr=0x%x,len=0x%x", addr, len);
		return -1;
	}

	BSP_QSPI_Init(); // exit memory mapped mode
log_debug("BSP_QSPI_Init(): %d", clock()-t0);t0=clock();
	if(poll_flash_busy(100) >= 100) {
		log_error("Fail after BSP_QSPI_Init: flash chip busy > 100 ms");
		BSP_QSPI_EnableMemoryMappedMode();
		return QSPI_BUSY;
	}
log_debug("poll_flash_busy(): %d", clock()-t0);t0=clock();

	res = BSP_QSPI_Write((uint8_t*) data, addr, len);
log_debug("BSP_QSPI_Write(): %d", clock()-t0);t0=clock();
	if(res != QSPI_OK) {
		log_error("QSPI write failed. addr=0x%x,len=0x%x", addr, len);
		BSP_QSPI_EnableMemoryMappedMode();
		return res;
		}
	if(poll_flash_busy(10) >= 10) {
		log_error("Fail writing block: flash chip busy > 10 ms");
		res = QSPI_BUSY;
	}
log_debug("poll_flash_busy(): %d", clock()-t0);t0=clock();

	BSP_QSPI_EnableMemoryMappedMode();
log_debug("BSP_QSPI_EnableMemoryMappedMode(): %d", clock()-t0);
	//dcache_flush_all();	// or use dcache_flush(const void *p, size_t size);
log_debug("Ok");
	return len;
}

static const struct flash_dev_drv qspi_flash_drv;

static int qspi_flash_init(struct flash_dev *dev, void *arg) {
	static uint32_t qspi_flash_aligned_word;
	struct flash_dev *flash;

	flash = flash_create("qspiflash", QSPI_FLASH_SIZE);
	if (flash == NULL) {
		log_error("Failed to create flash device!");
		return -1;
	}
	flash->drv = &qspi_flash_drv;
	flash->size = QSPI_FLASH_SIZE;
	flash->num_block_infos = 1;
	flash->block_info[0] = (struct flash_block_info) {
		.block_size = QSPI_BLOCK_SIZE,
		.blocks = QSPI_FLASH_SIZE / QSPI_BLOCK_SIZE,
	};
	flash->fld_aligned_word = &qspi_flash_aligned_word;
	flash->fld_word_size = 4;

	flash->fld_cache = flash_cache_addr(flash);

	return 0;
}

static const struct flash_dev_drv qspi_flash_drv = {
	.flash_init = qspi_flash_init,
	.flash_read = qspi_flash_read,
	.flash_erase_block = qspi_flash_erase_block,
	.flash_program = qspi_flash_write,
};

FLASH_DEV_DEF("qspiflash", &qspi_flash_drv);
